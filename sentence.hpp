// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef SENTENCE_H
#define SENTENCE_H

#include "object.hpp"

#include <string>

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
private:
	bool _want = true;
};

// Logical sentences are the building blocks of the propositional calculus:
// AND, OR, implication, and logical equivalence (if and only if).
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
	static int getType(const std::string& s);
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
	Sentence* expandSubset();
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
	static int getType(const std::string& s);
private:
	Type _type;
	Symbol _var;
	Sentence* _body;
};

#endif
