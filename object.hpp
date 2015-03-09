// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <map>
#include <vector>

// An object can be anything, but in practice it is always an idealized
// mathematical object, like a number or set. Objects can be cloned (deep copy),
// and they can print themselves to output streams.
class Object {
public:
	virtual ~Object() {}
	virtual Object* clone() const = 0;
	virtual std::ostream& print(std::ostream& stream) const = 0;
	friend std::ostream& operator<<(std::ostream& stream, const Object& obj);
};

// A number is some object that evaluates to a numerical value.
class Number : public virtual Object {
public:
	virtual ~Number() {}
	virtual Object* clone() const { return cloneSelf(); }
	virtual Number* cloneSelf() const = 0;
};

// A concrete number is simply an integer.
class ConcreteNumber : public Number {
public:
	ConcreteNumber(int x) : _x(x) {}
	virtual Number* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const { return s << _x; }
private:
	int _x;
};

// A compound number is a sum, difference, or product of two numbers.
class CompoundNumber : public Number {
public:
	enum Type { ADD, SUB, MUL };
	CompoundNumber(Type t, Number* a, Number* b) : _type(t), _a(a), _b(b) {}
	virtual ~CompoundNumber() { delete _a; delete _b; }
	virtual Number* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;
private:
	Type _type;
	Number* _a;
	Number* _b;
};

// A set is a collection of objects. It may be finite or infinite.
class Set : public virtual Object {
public:
	virtual ~Set() {}
	virtual Object* clone() const { return cloneSelf(); }
	virtual Set* cloneSelf() const = 0;
};

// A concrete set contains a finite list of objects.
class ConcreteSet : public Set {
public:
	virtual ~ConcreteSet();
	ConcreteSet(std::vector<Object*> items) : _items(items) {}
	virtual Set* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;
private:
	std::vector<Object*> _items;
};

// A special set does not enumerate its elements. Instead, it is described by a
// name that indicates the types of elements it contains.
class SpecialSet : public Set {
public:
	enum Type { EMPTY, INTEGERS, NATURALS, SETS };
	SpecialSet(Type t) : _type(t) {}
private:
	Type _type;
};

// A compound set is the union, intersection, or difference of two sets.
class CompoundSet : public Set {
public:
	enum Type { UNION, INTERSECT, DIFF };
	CompoundSet(Type t, Set* a, Set* b) : _type(t), _a(a), _b(b) {}
	virtual ~CompoundSet() { delete _a; delete _b; }
	virtual Set* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;
private:
	Type _type;
	Set* _a;
	Set* _b;
};

// A symbol is a variable which represents an object.
class Symbol : public Number, public Set {
public:
	Symbol(char c) : _c(c) { _id = _count++; }
	Symbol(char c, unsigned int id) : _c(c),  _id(id) {}
	virtual Object* clone() const { return cloneSelf(); }
	Symbol* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const { return s << _c; }
	void insertInMap(std::map<char, unsigned int>& symbols);
	bool operator==(const Symbol& s) const { return _id == s._id; }
private:
	char _c;
	unsigned int _id;
	static unsigned int _count;
};

#endif
