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
	// A value is like a Boolean, except it can also be in a third state, MU
	// (from Hofstaedter's GEB). MU means that the truth is unknown.
	enum Value { FALSE = false, TRUE = true, MU };

	virtual ~Sentence() {}

	// Creates a deep copy of the sentence.
	virtual Sentence* clone() const = 0;

	// Evaluates the sentence and returns its truth value.
	virtual Value value() const = 0;

	// Negates the meaning of the sentence, so that it becomes true where it
	// used to be false, and vice versa. The implementation should propagate the
	// negation as far as possible and try to express the result in a positive
	// form, rather than simply wrapping the whole sentence in a logical NOT.
	virtual void negate() = 0;

	// Prints a string representation of the sentence to the given stream.
	virtual std::ostream& print(std::ostream& s) const = 0;
	friend std::ostream& operator<<(std::ostream& stream, const Sentence& s);
};

// Logical sentences are the building blocks of the propositional calculus.
// There are four types: AND, OR, implication (if-then), and logical equivalence
// (if and only if).
class Logical : public Sentence {
public:
	enum Type { AND, OR, IMPLIES, IFF };

	Logical(Type t, Sentence* a, Sentence* b) : _type(t), _a(a), _b(b) {}
	virtual ~Logical() { delete _a; delete _b; }
	virtual Sentence* clone() const;
	virtual Value value() const;
	virtual void negate();
	virtual std::ostream& print(std::ostream& s) const;

	// Assumes the type is IMPLIES. Changes the implication "A implies B" to the
	// equivalent sentence "not B implies not A", known as the contrapositive.
	void contrapositive();

	// Assumes the type is IMPLIES. Swaps the hypothesis and the conclusion, so
	// "A implies B" becomes "B implies A". The converse is not equivalent to
	// the original sentence.
	void converse();

	// Assumes the type is IFF. Expands the sentence into and AND combining the
	// forward implication and its converse. This is how equivalence is defined.
	void expandIff();

	// Returns the operation type specified by the string, or -1 otherwise.
	static int getType(const std::string& s);

private:
	Type _type; // the operation type
	Sentence* _a; // the first operand
	Sentence* _b; // the second operand
};

// A relation is a sentence that asserts a particular relationship between two
// objects. The object types are constrained for most of the relationships; they
// are all in one class (sacrificing some type safety) because there would be
// far too many classes otherwise.
class Relation : public Sentence {
public:
	enum Type { EQ, NEQ, LT, GT, LTE, GTE, NOTIN, IN, SUBSET };

	Relation(Type t, Object* a, Object* b) : _type(t), _a(a), _b(b) {}
	virtual ~Relation() { delete _a; delete _b; }
	virtual Sentence* clone() const;
	virtual Value value() const;
	virtual void negate();
	virtual std::ostream& print(std::ostream& s) const;

	// Assumes the type is SUBSET. Expands the sentence into the sentence
	// "forall x in A: x in B", which is the definition of the subset relation.
	Sentence* expandSubset();

	// Returns the operation type specified by the string, or -1 otherwise.
	static int getType(const std::string& s);

private:
	Type _type; // the operation type
	Object* _a; // the first operand
	Object* _b; // the second operand
};

// A quantified statement uses either the universal quantifier (for all) or the
// existential quantifier (there exists). It binds a variable in its body, an
// open sentence, thereby creating a concrete sentence.
class Quantified : public Sentence {
public:
	enum Type { FORALL = 0, EXISTS = 1 };

	// Creates an ordinary quantified statement of the given type with the
	// supplied variable and the body, which should be an open sentence.
	Quantified(Type t, Symbol* var, Sentence* body)
		: _type(t), _var(var), _body(body) {}

	// Creates a quantified statement using the domain shorthand, which
	// restricts the values of the variable considered to a particular set.
	Quantified(Type t, Symbol* var, Set* domain, Sentence* body);

	virtual ~Quantified() { delete _body; }
	virtual Sentence* clone() const;
	virtual Value value() const;
	virtual void negate();
	virtual std::ostream& print(std::ostream& s) const;

	// Returns the quantifier type specified by the string, or -1 otherwise.
	static int getType(const std::string& s);

private:
	Type _type; // the quantifier type

	// The bound variable. This doesn't really need to be a pointer, since it
	// could be embedded directly in this object. However, using a pointer
	// simplifies things and makes everything more consistent.
	Symbol* _var;

	Sentence* _body; // the quantified open sentence
};

#endif
