// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "object.hpp"

// =============================================================================
//            Object
// =============================================================================

std::ostream& operator<<(std::ostream& stream, const Object& obj) {
	return obj.print(stream);
}

// =============================================================================
//            Number
// =============================================================================

Number* ConcreteNumber::cloneSelf() const {
	return new ConcreteNumber(_x);
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

Set* ConcreteSet::cloneSelf() const {
	std::vector<Object*> v;
	v.reserve(_items.size());
	for (Object *obj: _items) {
		v.push_back(obj->clone());
	}
	return new ConcreteSet(v);
}

ConcreteSet::~ConcreteSet() {
	for (Object *obj: _items) {
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

int SpecialSet::getType(const std::string& s) {
	if (s == "null") return EMPTY;
	if (s == "ZZ") return INTEGERS;
	if (s == "NN") return NATURALS;
	if (s == "SS") return SETS;
	return -1;
}

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

// =============================================================================
//            Symbol
// =============================================================================

unsigned int Symbol::_count = 0;

Symbol* Symbol::cloneSelf() const {
	return new Symbol(_c, _id);
}

void Symbol::insertInMap(SymMap& symbols) {
	symbols[_c] = _id;
}
