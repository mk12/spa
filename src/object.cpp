// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "object.hpp"

// =============================================================================
//            Object
// =============================================================================

Object::~Object() {}

std::ostream& operator<<(std::ostream& stream, const Object& obj) {
	return obj.print(stream);
}

// =============================================================================
//            Number
// =============================================================================

Object* Number::clone() const {
	return cloneSelf();
}

ConcreteNumber::ConcreteNumber(int x) : _x(x) {}

std::ostream& ConcreteNumber::print(std::ostream& s) const {
	return s << _x;
}

Number* ConcreteNumber::cloneSelf() const {
	return new ConcreteNumber(_x);
}

CompoundNumber::CompoundNumber(Type t, Number* a, Number* b)
	: _type(t), _a(a), _b(b) {}

CompoundNumber::~CompoundNumber() {
	delete _a;
	delete _b;
}

Number* CompoundNumber::cloneSelf() const {
	return new CompoundNumber(_type, _a->cloneSelf(), _b->cloneSelf());
}

std::ostream& CompoundNumber::print(std::ostream& s) const {
	s << '(';
	switch (_type) {
	case ADD: s << '+'; break;
	case SUB: s << '-'; break;
	case MUL: s << '*'; break;
	}
	s << ' ';
	_a->print(s);
	s << ' ';
	_b->print(s);
	return s << ')';
}

int CompoundNumber::getType(const std::string& s) {
	if (s == "+") return ADD;
	if (s == "-") return SUB;
	if (s == "*") return MUL;
	return -1;
}

// =============================================================================
//            Set
// =============================================================================

Object* Set::clone() const {
	return cloneSelf();
}

ConcreteSet::ConcreteSet(std::vector<Object*> items) : _items(items) {}

Set* ConcreteSet::cloneSelf() const {
	std::vector<Object*> v;
	v.reserve(_items.size());
	for (Object* obj: _items) {
		v.push_back(obj->clone());
	}
	return new ConcreteSet(v);
}

ConcreteSet::~ConcreteSet() {
	for (Object* obj: _items) {
		delete obj;
	}
}

std::ostream& ConcreteSet::print(std::ostream& s) const {
	s << '{';
	bool first = true;
	for (const Object* obj: _items) {
		if (first) {
			first = false;
		} else {
			s << ", ";
		}
		s << *obj;
	}
	return s << '}';
}

SpecialSet::SpecialSet(Type t) : _type(t) {}

Set* SpecialSet::cloneSelf() const {
	return new SpecialSet(_type);
}

std::ostream& SpecialSet::print(std::ostream& s) const {
	switch(_type) {
	case EMPTY: return s << "null";
	case INTEGERS: return s << "ZZ";
	case NATURALS: return s << "NN";
	case SETS: return s << "SS";
	}
}

int SpecialSet::getType(const std::string& s) {
	if (s == "null") return EMPTY;
	if (s == "ZZ") return INTEGERS;
	if (s == "NN") return NATURALS;
	if (s == "SS") return SETS;
	return -1;
}

CompoundSet::CompoundSet(Type t, Set* a, Set* b) : _type(t), _a(a), _b(b) {}

CompoundSet::~CompoundSet() {
	delete _a;
	delete _b;
}

Set* CompoundSet::cloneSelf() const {
	return new CompoundSet(_type, _a->cloneSelf(), _b->cloneSelf());
}

std::ostream& CompoundSet::print(std::ostream& s) const {
	s << '(';
	switch (_type) {
	case UNION: s << "union"; break;
	case INTERSECT: s << "intersect"; break;
	case DIFF: s << "diff"; break;
	}
	s << ' ';
	_a->print(s);
	s << ' ';
	_b->print(s);
	return s << ')';
}

int CompoundSet::getType(const std::string& s) {
	if (s == "union") return UNION;
	if (s == "intersect") return INTERSECT;
	if (s == "diff") return DIFF;
	return -1;
}

// =============================================================================
//            Symbol
// =============================================================================

namespace {
	unsigned int symbolCount = 0;
	unsigned int genUniqueId() { return symbolCount++; }
}

Symbol::Symbol(char c) : _c(c), _id(genUniqueId()) {}

Symbol::Symbol(char c, SymMap& symbols, bool fresh) : _c(c) {
	if (!fresh) {
		auto iter = symbols.find(_c);
		if (iter != symbols.end()) {
			_id = iter->second;
			return;
		}
	}
	_id = genUniqueId();
	symbols[_c] = _id;
}

Symbol::Symbol(char c, unsigned int id) : _c(c),  _id(id) {}

Symbol* Symbol::cloneSelf() const {
	return new Symbol(_c, _id);
}

Object* Symbol::clone() const {
	return cloneSelf();
}

std::ostream& Symbol::print(std::ostream& s) const {
	return s << _c;
}
