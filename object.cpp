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
	_a->print(s);
	switch (_type) {
	case ADD: s << " + "; break;
	case SUB: s << " - "; break;
	case MUL: s << " * "; break;
	}
	_b->print(s);
	return s << ')';
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
	_a->print(s);
	switch (_type) {
	case UNION: s << " (union) "; break;
	case INTERSECT: s << " (intersect) "; break;
	case DIFF: s << " \\ "; break;
	}
	return _b->print(s);
}

// =============================================================================
//            Symbol
// =============================================================================

unsigned int Symbol::_count = 0;

Symbol* Symbol::cloneSelf() const {
	return new Symbol(_c, _id);
}

void Symbol::insertInMap(std::map<char, unsigned int>& symbols) {
	symbols[_c] = _id;
}
