// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

// A symbol map is a mapping from symbol characters to their identifiers.
typedef std::map<char, unsigned int> SymMap;

// An object can represent anything. In practice, it is always an idealized
// mathematical object, like a number or set. Objects can be cloned (deep copy),
// and they can print themselves to output streams.
class Object {
public:
	virtual ~Object();

	// Creates a deep copy of the object. The subclasses of Object implement
	// this by calling a cloneSelf method, which returns a more specific
	// pointer, which is sometimes used directly to avoid dynamic casting.
	virtual Object* clone() const = 0;

	// virtual bool equal(Object* other) const = 0;

	// Prints a string representation of the object to the given stream.
	virtual std::ostream& print(std::ostream& s) const = 0;
	friend std::ostream& operator<<(std::ostream& stream, const Object& obj);

protected:
	Object() {}
	Object(const Object&) = delete;
};

// A number is some object that evaluates to a numerical value.
class Number : public virtual Object {
public:
	virtual Number* cloneSelf() const = 0;
	virtual Object* clone() const;
};

// A concrete number is simply an integer.
class ConcreteNumber : public Number {
public:
	explicit ConcreteNumber(int x);
	virtual Number* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;

private:
	int _x; // the integer this object represents
};

// A compound number is a sum, difference, or product of two numbers.
class CompoundNumber : public Number {
public:
	enum Type { ADD, SUB, MUL };

	CompoundNumber(Type t, Number* a, Number* b);
	virtual ~CompoundNumber();
	virtual Number* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;

	// Returns the operation type specified by the string, or -1 otherwise.
	static int getType(const std::string& s);

private:
	Type _type; // the operation type
	Number* _a; // the first operand
	Number* _b; // the second operand
};

// A set is a collection of objects. It may be finite or infinite.
class Set : public virtual Object {
public:
	virtual Set* cloneSelf() const = 0;
	virtual Object* clone() const;
};

// A concrete set contains a finite list of objects.
class ConcreteSet : public Set {
public:
	explicit ConcreteSet(std::vector<Object*> items);
	virtual ~ConcreteSet();
	virtual Set* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;

private:
	std::vector<Object*> _items; // the elements of the set
};

// A special set does not enumerate its elements. Instead, it is described by a
// name that indicates the types of elements it contains.
class SpecialSet : public Set {
public:
	enum Type { EMPTY, INTEGERS, NATURALS, SETS };

	explicit SpecialSet(Type t);
	virtual Set* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;

	// Returns the set type specified by the string, or -1 otherwise.
	static int getType(const std::string& s);

private:
	Type _type; // the type of special set
};

// A compound set is the union, intersection, or difference of two sets.
class CompoundSet : public Set {
public:
	enum Type { UNION, INTERSECT, DIFF };

	CompoundSet(Type t, Set* a, Set* b);
	virtual ~CompoundSet();
	virtual Set* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;

	// Returns the operation type specified by the string, or -1 otherwise.
	static int getType(const std::string& s);

private:
	Type _type; // the operation type
	Set* _a; // the first operand
	Set* _b; // the second operand
};

// A symbol is a variable which represents an object.
class Symbol : public Number, public Set {
public:
	// Creates a new symbol with a unique identifier.
	explicit Symbol(char c);

	// Creates a new symbol in the given context. Fresh symbols always get
	// unique identifiers. Non-fresh symbols (or rather, not-necessarily-fresh
	// symbols) reuse existing identifiers if their characters are already bound
	// in the symbol map; otherwise, they get unique identifiers as well. In all
	// cases, if a unique identifer is generated, it will be added to the map.
	Symbol(char c, SymMap& symbols, bool fresh);

	Symbol* cloneSelf() const;
	virtual Object* clone() const;
	virtual std::ostream& print(std::ostream& s) const;

private:
	// Creates a new symbol by reusing the given identifier.
	Symbol(char c, unsigned int id);

	char _c; // the character used when printing
	unsigned int _id; // the identifier
};

#endif
