// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "sentence.hpp"

#include <algorithm>
#include <cassert>

// =============================================================================
//            Logical
// =============================================================================

Sentence* Logical::clone() const {
	return new Logical(_type, _a->clone(), _b->clone());
}

Sentence::Value Logical::value() const {
	Value va = _a->value();
	Value vb = _b->value();
	if (va == MU || vb == MU) {
		return MU;
	}
	switch(_type) {
	case AND:
		return (Value)(va && vb); break;
	case OR:
		return (Value)(va || vb); break;
	case IMPLIES:
		return (Value)(!va || vb); break;
	case IFF:
		return (Value)(va == vb); break;
	}
}

void Logical::negate() {
	switch (_type) {
	case AND:
		_type = OR;
		_a->negate();
		_b->negate();
		break;
	case OR:
		_type = AND;
		_a->negate();
		_b->negate();
		break;
	case IMPLIES:
		_type = AND;
		_b->negate();
		break;
	case IFF:
		expandIff();
		negate();
		break;
	}
}

void Logical::converse() {
	assert(_type == IMPLIES);
	std::swap(_a, _b);
}

void Logical::contrapositive() {
	converse();
	_a->negate();
	_b->negate();
}

void Logical::expandIff() {
	assert(_type == IFF);
	_type = AND;
	Logical* fwd = new Logical(IMPLIES, _a, _b);
	Logical* bwd = new Logical(IMPLIES, _a->clone(), _b->clone());
	_a = fwd;
	_b = bwd;
}

int Logical::getType(const std::string& s) {
	if (s == "and") return AND;
	if (s == "or") return OR;
	if (s == "=>") return IMPLIES;
	if (s == "iff") return IFF;
	return -1;
}

// =============================================================================
//            Relation
// =============================================================================

Sentence* Relation::clone() const {
	return new Relation(_type, _a->clone(), _b->clone());
}

Sentence::Value Relation::value() const {
	return Sentence::MU;
}

void Relation::negate() {
	switch (_type) {
	case EQ:    _type = NEQ;   break;
	case NEQ:   _type = EQ;    break;
	case LT:    _type = GTE;   break;
	case GT:    _type = LTE;   break;
	case LTE:   _type = GT;    break;
	case GTE:   _type = LT;    break;
	case IN:    _type = NOTIN; break;
	case NOTIN: _type = IN;    break;
	case SUBSET:
		expandSubset();
		negate();
		break;
	}
}

Sentence* Relation::expandSubset() {
	assert(_type == SUBSET);
	Symbol x('x');
	return new Quantified(
		Quantified::FORALL,
		x,
		dynamic_cast<Set*>(_a->clone()),
		new Relation(Relation::IN, x.cloneSelf(), _b->clone())
	);
}

int Relation::getType(const std::string& s) {
	if (s == "=") return EQ;
	if (s == "!=") return NEQ;
	if (s == "<") return LT;
	if (s == ">") return GT;
	if (s == "<=") return LTE;
	if (s == ">=") return GTE;
	if (s == "in") return IN;
	if (s == "notin") return NOTIN;
	if (s == "subset") return SUBSET;
	return -1;
}

// =============================================================================
//            Quantified
// =============================================================================

Quantified::Quantified(Type t, Symbol var, Set* domain, Sentence* body)
	: _type(t), _var(var) {
	_body = new Logical(
		Logical::IMPLIES,
		new Relation(Relation::IN, var.clone(), domain),
		body
	);
}

Sentence* Quantified::clone() const {
	return new Quantified(_type, _var, _body->clone());
}

Sentence::Value Quantified::value() const {
	return Sentence::MU;
}

void Quantified::negate() {
	_type = (Type)!_type;
	_body->negate();
}

int Quantified::getType(const std::string& s) {
	if (s == "forall") return FORALL;
	if (s == "exists") return EXISTS;
	return -1;
}
