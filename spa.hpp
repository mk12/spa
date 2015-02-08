// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef SPA_H
#define SPA_H

#include <stack>
#include <string>
#include <vector>

// =============================================================================
//            Objects
// =============================================================================

// An object is any mathematical object that sentences can talk about.
class Object {
public:
	virtual ~Object() {}
};

class Number : Object {
public:
	virtual ~Number() {}
};

class ConcreteNumber : Number {
public:
	ConcreteNumber(int x) : _x(x) {}
private:
	int _x;
};

class CompoundNumber : Number {
public:
	enum Type { ADD, SUB, MUL, DIV };
	CompoundNumber(Type t, Number* a, Number* b) : _type(t), _a(a), _b(b) {}
	~CompoundNumber() { delete _a; delete _b; }
private:
	Type _type;
	Number* _a;
	Number* _b;
};

// reals, integers... just use predefined symbols?
class Set {
public:
	virtual ~Set() {}
};

class ConcreteSet : Set {
public:
	~ConcreteSet() {}
};

class CompoundSet : Set {
public:
	enum Type { UNION, INTERSECTION, DIFFERENCE };
	CompoundSet(Type t, Set* a, Set* b) : _type(t), _a(a), _b(b) {}
	~CompoundSet() { delete _a; delete _b; }
private:
	Type _type;
	Set* _a;
	Set* _b;
};

// A symbol is a variable that represents a concrete object.
class Symbol : Number, Set {
public:
	Symbol(std::string s) : _s(s) { _id = _count++; }
private:
	static int _count;
	int _id;
	std::string _s;
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
	// virtual ??? options() = 0;
	virtual Value value() = 0;
	Value evaluate();
	virtual void negate() { _want = !_want; }
private:
	bool _want = true;
};

// Logical sentences are the building blocks of the propositional calculus:
// logical and, logical or, implication, and logical equivalence.
class Logical : Sentence {
public:
	enum Type { AND, OR, IMPLIES, IFF };
	Logical(Type t, Sentence* a, Sentence* b) : _type(t), _a(a), _b(b) {}
	// TODO: use deep copy instead
	Logical(const Logical& s) : _type(s._type), _a(s._a), _b(s._b) {}
	~Logical() { delete _a; delete _b; }
	void contrapositive();
	void converse();
	void expandIff();
	virtual Value value();
	virtual void negate();
private:
	Type _type;
	Sentence* _a;
	Sentence* _b;
};

// A relation is a sentence about two objects.
class Relation : Sentence {
public:
	enum Type { EQ, LT, IN, SUBSET };
	Relation(Type t, Object* a, Object* b) : _type(t), _a(a), _b(b) {}
	~Relation() { delete _a; delete _b; }
	virtual void negate();
private:
	Type _type;
	Object* _a;
	Object* _b;
};

// A quantified statement uses either the universal quantifier (for all) or the
// existential quantifier (there exists). It binds a variable in its body, an
// open sentence, thereby creating a concrete sentence.
class Quantified : Sentence {
public:
	enum Type { FORALL = 0, EXISTS = 1 };
	Quantified(Type t, Symbol x, Sentence* b) : _type(t), _x(x), _body(b) {}
	Quantified(Type t, Symbol x, Object* domain, Sentence* body);
	~Quantified() { delete _body; }
	void makeUnique();
	virtual Value value();
	virtual void negate();
private:
	Type _type;
	Symbol _x;
	Sentence* _body;
};

// =============================================================================
//            Provers
// =============================================================================

class GoalProver {
public:
private:
	Sentence* _goal;
	std::vector<Sentence*> _givens;
};

class TheoremProver {
public:
	void dispatch(const std::vector<std::string>& tokens);
private:
	std::stack<GoalProver> _stack;
	Sentence* _theorem;
};

#endif
