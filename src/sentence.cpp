// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "sentence.hpp"

#include "object.hpp"

#include <algorithm>

#include <cassert>

// =============================================================================
//            Sentence
// =============================================================================

Sentence::~Sentence() {}

// =============================================================================
//            Logical
// =============================================================================

std::ostream& operator<<(std::ostream& stream, const Sentence& s) {
	return s.print(stream);
}

// =============================================================================
//            Logical
// =============================================================================

Logical::Logical(Type t, Sentence* a, Sentence* b) : _type(t), _a(a), _b(b) {}

Logical::~Logical() {
	delete _a;
	delete _b;
}

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
		return static_cast<Value>(va && vb);
	case OR:
		return static_cast<Value>(va || vb);
	case IMPLIES:
		return static_cast<Value>(!va || vb);
	case IFF:
		return static_cast<Value>(va == vb);
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

std::ostream& Logical::print(std::ostream& s) const {
	s << '(';
	switch (_type) {
	case AND: s << "and"; break;
	case OR: s << "or"; break;
	case IMPLIES: s << "=>"; break;
	case IFF: s << "iff"; break;
	}
	s << ' ';
	_a->print(s);
	s << ' ';
	_b->print(s);
	return s << ')';
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

Relation::Relation(Type t, Object* a, Object* b) : _type(t), _a(a), _b(b) {}

Relation::~Relation() {
	delete _a;
	delete _b;
}

Sentence* Relation::clone() const {
	return new Relation(_type, _a->clone(), _b->clone());
}

Sentence::Value Relation::value() const {
	return Sentence::MU;
}

void Relation::negate() {
	switch (_type) {
	case EQ: _type = NEQ; break;
	case NEQ: _type = EQ; break;
	case LT: _type = GTE; break;
	case GT: _type = LTE; break;
	case LTE: _type = GT; break;
	case GTE: _type = LT; break;
	case IN: _type = NOTIN; break;
	case NOTIN: _type = IN; break;
	case SUBSET:
		expandSubset();
		negate();
		break;
	}
}

std::ostream& Relation::print(std::ostream& s) const {
	s << '(';
	switch (_type) {
	case EQ: s << '='; break;
	case NEQ: s << "!="; break;
	case LT: s << '<'; break;
	case GT: s << '>'; break;
	case LTE: s << "<="; break;
	case GTE: s << ">="; break;
	case IN: s << "in"; break;
	case NOTIN: s << "notin"; break;
	case SUBSET: s << "subset"; break;
	}
	s << ' ';
	_a->print(s);
	s << ' ';
	_b->print(s);
	return s << ')';
}

Sentence* Relation::expandSubset() {
	assert(_type == SUBSET);
	Symbol* var = new Symbol('x');
	return new Quantified(
		Quantified::FORALL,
		var,
		dynamic_cast<Set*>(_a->clone()),
		new Relation(Relation::IN, var->cloneSelf(), _b->clone())
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

// The bound variable _var doesn't really need to be a pointer, since it could
// be embedded directly in the Quantified object. However, using a pointer
// simplifies things and makes everything more consistent.

Quantified::Quantified(Type t, Symbol* var, Sentence* body)
	: _type(t), _var(var), _body(body) {}

Quantified::Quantified(Type t, Symbol* var, Set* domain, Sentence* body)
		: _type(t), _var(var) {
	_body = new Logical(
		(_type == FORALL) ? Logical::IMPLIES : Logical::AND,
		new Relation(Relation::IN, var->clone(), domain),
		body
	);
}

Quantified::~Quantified() {
	delete _var;
	delete _body;
}

Sentence* Quantified::clone() const {
	return new Quantified(_type, _var->cloneSelf(), _body->clone());
}

Sentence::Value Quantified::value() const {
	return Sentence::MU;
}

void Quantified::negate() {
	_type = static_cast<Type>(!_type);
	_body->negate();
}

std::ostream& Quantified::print(std::ostream& s) const {
	s << '(';
	switch (_type) {
	case FORALL: s << "forall"; break;
	case EXISTS: s << "exists"; break;
	}
	s << ' ';
	_var->print(s);
	s << ' ';
	_body->print(s);
	return s << ')';
}

int Quantified::getType(const std::string& s) {
	if (s == "forall") return FORALL;
	if (s == "exists") return EXISTS;
	return -1;
}
