// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "sentence.hpp"

#include <cassert>

// =============================================================================
//            Sentence
// =============================================================================

Sentence::Value Sentence::evaluate() const {
	Value v = value();
	return (v == MU) ? MU : (Value)(v == _want);
}

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
	return Sentence::FALSE;
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

void Relation::negate() {
	switch (_type) {
	case SUBSET:
		expandSubset();
		negate();
	default:
		Sentence::negate();
	}
}

// =============================================================================
//            Quantified
// =============================================================================

Sentence* Quantified::clone() const {
	return new Quantified(_type, _var, _body->clone());
}

Sentence::Value Quantified::value() const {
	return _body->value();
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
