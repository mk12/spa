// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef SPA_H
#define SPA_H

#include <ostream>
#include <stack>
#include <string>
#include <vector>

// =============================================================================
//            Objects
// =============================================================================

class Object {
public:
	virtual ~Object() {}
	virtual Object* clone() const = 0;
	virtual std::ostream& print(std::ostream& stream) const = 0;
	friend std::ostream& operator<<(std::ostream& stream, const Object& obj);
};

class Number : public virtual Object {
public:
	virtual ~Number() {}
	virtual Object* clone() const { return cloneSelf(); }
	virtual Number* cloneSelf() const = 0;
};

class ConcreteNumber : public Number {
public:
	ConcreteNumber(int x) : _x(x) {}
	virtual Number* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const { return s << _x; }
	bool operator==(const ConcreteNumber& n) const { return _x == n._x; }
	bool operator!=(const ConcreteNumber& n) const { return _x != n._x; }
private:
	int _x;
};

class CompoundNumber : public Number {
public:
	enum Type { ADD, SUB, MUL, DIV };
	CompoundNumber(Type t, Number* a, Number* b) : _type(t), _a(a), _b(b) {}
	virtual ~CompoundNumber() { delete _a; delete _b; }
	virtual Number* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;
private:
	Type _type;
	Number* _a;
	Number* _b;
};

class Set : public virtual Object {
public:
	virtual ~Set() {}
	virtual Object* clone() const { return cloneSelf(); }
	virtual Set* cloneSelf() const = 0;
	virtual bool contains(Object* obj) const;
};

class ConcreteSet : public Set {
public:
	virtual ~ConcreteSet();
	ConcreteSet(std::vector<Object*> items) : _items(items) {}
	virtual Set* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;
	virtual bool contains(Object* obj) const;
private:
	std::vector<Object*> _items;
};

class InfiniteSet : public Set {
public:
	enum Type { N, Z, Q, R, S };
	InfiniteSet(Type t) : _type(t) {}
	virtual bool contains(Object* obj) const;
private:
	Type _type;
};

class CompoundSet : public Set {
public:
	enum Type { UNION, INTERSECT, DIFF };
	CompoundSet(Type t, Set* a, Set* b) : _type(t), _a(a), _b(b) {}
	virtual ~CompoundSet() { delete _a; delete _b; }
	virtual Set* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const;
	virtual bool contains(Object* obj) const;
private:
	Type _type;
	Set* _a;
	Set* _b;
};

class Symbol : public Number, public Set {
public:
	Symbol(char c) : _c(c) { _id = _count++; }
	Symbol(char c, unsigned int id) : _c(c),  _id(id) {}
	virtual Object* clone() const { return cloneSelf(); }
	Symbol* cloneSelf() const;
	virtual std::ostream& print(std::ostream& s) const { return s << _c; }
	bool operator==(const Symbol& s) const { return _id == s._id; }
	bool operator!=(const Symbol& s) const { return _id != s._id; }
private:
	char _c;
	unsigned int _id;
	static int _count;
};

// =============================================================================
//            Sentences
// =============================================================================

// A sentence, or proposition, is a Boolean-valued formula with no free
// variables. The sentence expresses something concrete which must be either
// true or false (although it might be difficult to determine which it is).
class Sentence {
public:
	enum Value { FALSE = false, TRUE = true, MU };
	virtual ~Sentence() {}
	virtual Sentence* clone() const = 0;
	// virtual ??? options() = 0;
	virtual Value value() const = 0;
	Value evaluate() const;
	virtual void negate() { _want = !_want; }
	static Sentence* parse(const std::vector<std::string>& tokens);
private:
	bool _want = true;
};

// Logical sentences are the building blocks of the propositional calculus:
// logical and, logical or, implication, and logical equivalence.
class Logical : public Sentence {
public:
	enum Type { AND, OR, IMPLIES, IFF };
	Logical(Type t, Sentence* a, Sentence* b) : _type(t), _a(a), _b(b) {}
	virtual ~Logical() { delete _a; delete _b; }
	virtual Sentence* clone() const;
	void contrapositive();
	void converse();
	void expandIff();
	virtual Value value() const;
	virtual void negate();
private:
	Type _type;
	Sentence* _a;
	Sentence* _b;
};

// A relation is a sentence about two objects.
class Relation : public Sentence {
public:
	enum Type { EQ, LT, IN, SUBSET };
	Relation(Type t, Object* a, Object* b) : _type(t), _a(a), _b(b) {}
	virtual ~Relation() { delete _a; delete _b; }
	Object* expandSubset();
	virtual Sentence* clone() const;
	virtual Value value() const;
	virtual void negate();
private:
	Type _type;
	Object* _a;
	Object* _b;
};

// A quantified statement uses either the universal quantifier (for all) or the
// existential quantifier (there exists). It binds a variable in its body, an
// open sentence, thereby creating a concrete sentence.
class Quantified : public Sentence {
public:
	enum Type { FORALL = 0, EXISTS = 1 };
	Quantified(Type t, Symbol var, Sentence* body)
		: _type(t), _var(var), _body(body) {}
	Quantified(Type t, Symbol var, Set* domain, Sentence* body);
	virtual ~Quantified() { delete _body; }
	virtual Sentence* clone() const;
	void makeUnique();
	virtual Value value() const;
	virtual void negate();
private:
	Type _type;
	Symbol _var;
	Sentence* _body;
};

// =============================================================================
//            Provers
// =============================================================================

class GoalProver {
public:
private:
	// Sentence* _goal;
	std::vector<Sentence*> _givens;
	std::vector<Sentence*> _goals;
};

class TheoremProver {
public:
	~TheoremProver();
	void dispatch(const std::vector<std::string>& tokens);
private:
	std::stack<GoalProver> _stack;
	Sentence* _theorem;
};

#endif
