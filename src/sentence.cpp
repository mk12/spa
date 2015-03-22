// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "sentence.hpp"

#include "object.hpp"

#include <algorithm>
#include <utility>

#include <cassert>

// =============================================================================
//            Macros
// =============================================================================

// Adds a decomposition that only uses one child.
#define DEC1(vec, s, gi, go) do { \
	vec.emplace_back(s, gi, go); \
} while (0)

// Adds a decomposition to the vector.
#define DEC2(vec, s, gi1, go1, gi2, go2) do { \
	vec.emplace_back(s, gi1, go1, gi2, go2); \
} while (0)

// Add a deduction to the vector.
#define DED(vec, hyp, conc) do { vec.emplace_back(hyp, conc); } while (0)

// =============================================================================
//            Decomp
// =============================================================================

Decomp::Decomp(std::string name, Sentence* givenA, Sentence* goalA,
	Sentence* givenB, Sentence* goalB)
	: _name(name), _givenA(givenA), _goalA(goalA), _givenB(givenB), _goalB(goalB) {}

Decomp::Decomp(std::string name, Sentence* givenA, Sentence* goalA)
	: _name(name), _givenA(givenA), _goalA(goalA), _givenB(nullptr), _goalB(nullptr)
	{}

void Decomp::print() const {
	std::cout << _name;
}

void Decomp::free() {
	delete _givenA;
	delete _goalA;
	delete _givenB;
	delete _goalB;
}

// =============================================================================
//            Deduct
// =============================================================================

Deduct::Deduct(Sentence* hyp, Sentence* conc) : _hyp(hyp), _conc(conc) {}

Deduct::~Deduct() {
	delete _hyp;
	delete _conc;
}

// =============================================================================
//            Sentence
// =============================================================================

Sentence::~Sentence() {}

std::ostream& operator<<(std::ostream& stream, const Sentence& s) {
	return s.print(stream);
}

// Convenience function for cloning and negating all at once.
static Sentence* negClone(Sentence* s) {
	Sentence* n = s->clone();
	n->negate();
	return n;
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
	return cloneSelf();
}

Logical* Logical::cloneSelf() const {
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
		_type = AND;
		Logical* fwd = new Logical(IMPLIES, _a, _b);
		Logical* bwd = new Logical(IMPLIES, _b->clone(), _a->clone());
		_a = fwd;
		_b = bwd;
		negate();
		break;
	}
}

std::vector<Decomp> Logical::decompose() const {
	std::vector<Decomp> vec;
	switch (_type) {
	case AND:
		DEC2(vec, "separate", nullptr, _a->clone(), nullptr, _b->clone());
		break;
	case OR:
		DEC1(vec, "first", negClone(_b), _a->clone());
		DEC1(vec, "second", negClone(_a), _b->clone());
		break;
	case IMPLIES:
		DEC1(vec, "direct", _a->clone(), _b->clone());
		DEC1(vec, "contrapositive", negClone(_b), negClone(_a));
		break;
	case IFF:
		Logical* fwd = new Logical(IMPLIES, _a->clone(), _b->clone());
		Logical* bwd = new Logical(IMPLIES, _b->clone(), _a->clone());
		DEC2(vec, "bidirectional", nullptr, fwd, nullptr, bwd);
		break;
	}
	return vec;
}

std::vector<Deduct> Logical::deduce() const {
	std::vector<Deduct> vec;
	switch (_type) {
	case AND:
		DED(vec, nullptr, _a->clone());
		DED(vec, nullptr, _b->clone());
		break;
	case OR:
		DED(vec, negClone(_a), _b->clone());
		DED(vec, negClone(_b), _a->clone());
		break;
	case IMPLIES:
		DED(vec, _a->clone(), _b->clone());
		DED(vec, negClone(_b), negClone(_a));
		break;
	case IFF:
		Logical* fwd = new Logical(IMPLIES, _a->clone(), _b->clone());
		Logical* bwd = new Logical(IMPLIES, _b->clone(), _a->clone());
		DED(vec, nullptr, fwd);
		DED(vec, nullptr, bwd);
		break;
	}
	return vec;
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

Relation::Relation(Type t, bool positive, Object* a, Object* b)
	: _type(t), _want(positive), _a(a), _b(b) {}

Relation::~Relation() {
	delete _a;
	delete _b;
}

Sentence* Relation::clone() const {
	return cloneSelf();
}

Relation* Relation::cloneSelf() const {
	return new Relation(_type, _want, _a->clone(), _b->clone());
}

Sentence::Value Relation::value() const {
	return Sentence::MU;
}

void Relation::negate() {
	_want = !_want;
}

std::vector<Decomp> Relation::decompose() const {
	std::vector<Decomp> vec;
	if (_want) {
		switch (_type) {
		case SEQ: {
			Relation* fwd = new Relation(SUB, true, _a->clone(), _b->clone());
			Relation* bwd = new Relation(SUB, true, _b->clone(), _a->clone());
			DEC2(vec, "mutual subsets", nullptr, fwd, nullptr, bwd);
			break;
		}
		case SUB: {
			Symbol* var = new Symbol('x');
			DEC1(vec, "definition", nullptr, new Quantified(
				Quantified::FORALL,
				var,
				dynamic_cast<Set*>(_a->clone()),
				new Relation(Relation::IN, true, var->cloneSelf(), _b->clone()) 
			));
			break;
		}
		case DIV: {
			Symbol* var = new Symbol('k');
			DEC1(vec, "definition", nullptr, new Quantified(
				Quantified::EXISTS,
				var,
				new SpecialSet(SpecialSet::INTEGERS),
				new Relation(
					Relation::EQ,
					true,
					new CompoundNumber(
						CompoundNumber::MUL,
						var->cloneSelf(),
						dynamic_cast<Number*>(_a->clone())
					),
					_b->clone()
				)
			));
			break;
		}
		default:
			break;
		}
	}
	return vec;
}

std::vector<Deduct> Relation::deduce() const {
	std::vector<Deduct> vec;
	Relation* r;
	if (_want) {
		switch (_type) {
		case EQ:
			r = cloneSelf();
			r->_type = LTE;
			DED(vec, nullptr, r);
			break;
		case LT:
			r = cloneSelf();
			r->_type = EQ;
			r->_want = false;
			DED(vec, nullptr, r);
			break;
		default:
			break;
		}
	}
	return vec;
}

std::ostream& Relation::print(std::ostream& s) const {
	s << '(';
	if (_want) {
		switch (_type) {
		case EQ: s << '='; break;
		case LT: s << '<'; break;
		case LTE: s << "<="; break;
		case SEQ: s << "s="; break;
		case SUB: s << "sub"; break;
		case SUBE: s << "sube"; break;
		case IN: s << "in"; break;
		case DIV: s << "div"; break;
		}
	} else {
		switch (_type) {
		case EQ: s << "!="; break;
		case LT: s << ">="; break;
		case LTE: s << ">"; break;
		case SEQ: s << "s!="; break;
		case SUB: s << "supe"; break;
		case SUBE: s << "sup"; break;
		case IN: s << "notin"; break;
		case DIV: s << "notdiv"; break;
		}
	}
	s << ' ';
	_a->print(s);
	s << ' ';
	_b->print(s);
	return s << ')';
}

std::pair<int, bool> Relation::getType(const std::string& s) {
	if (s == "=") return {EQ, true};
	if (s == "!=") return {EQ, false};
	if (s == "<") return {LT, true};
	if (s == ">=") return {LT, false};
	if (s == "<=") return {LTE, true};
	if (s == ">") return {LTE, false};
	if (s == "s=") return {SEQ, true};
	if (s == "s!=") return {SEQ, false};
	if (s == "sub") return {SUB, true};
	if (s == "supe") return {SUB, false};
	if (s == "sube") return {SUBE, true};
	if (s == "sup") return {SUBE, false};
	if (s == "in") return {IN, true};
	if (s == "notin") return {IN, false};
	if (s == "div") return {DIV, true};
	if (s == "notdiv") return {DIV, false};
	return {-1, true};
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
		new Relation(Relation::IN, true, var->clone(), domain),
		body
	);
}

Quantified::~Quantified() {
	delete _var;
	delete _body;
}

Sentence* Quantified::clone() const {
	return cloneSelf();
}

Quantified* Quantified::cloneSelf() const {
	return new Quantified(_type, _var->cloneSelf(), _body->clone());
}

Sentence::Value Quantified::value() const {
	return Sentence::MU;
}

void Quantified::negate() {
	_type = static_cast<Type>(!_type);
	_body->negate();
}

std::vector<Decomp> Quantified::decompose() const {
	std::vector<Decomp> vec;
	if (_type == FORALL) {
		DEC1(vec, "general", nullptr, _body->clone());
	}
	return vec;
}

// TODO: differentiate universal & existential instantiation
std::vector<Deduct> Quantified::deduce() const {
	std::vector<Deduct> vec;
	switch (_type) {
	case FORALL:
		DED(vec, nullptr, _body->clone());
		break;
	case EXISTS:
		DED(vec, nullptr, _body->clone());
		break;
	}
	return vec;
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
