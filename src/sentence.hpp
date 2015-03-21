// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef SENTENCE_H
#define SENTENCE_H

#include <string>

class Object;
class Sentence;
class Set;
class Symbol;

// A decomp stores information about sentence decomposition. It breaks down a
// parent sentence into one equivalent goal (A) or two subgoals (A and B). Each
// subgoal can optionally include a given (a fact to be used in the proof).
struct Decomp {
	std::string name; // the type of decomposition
	Sentence* givenA; // first given, or null
	Sentence* goalA; // first goal
	Sentence* givenB; // second given, or null
	Sentence* goalB; // second goal, or null
};

// A deduct stores information about sentence deduction. It consists of
// conclusion (the thing being deduced) and an optional hypothesis, which is
// required to be proved before assuming the conclusion.
struct Deduct {
	Sentence* hypothesis; // the requirement, or null
	Sentence* conclusion; // the deduced sentence
};

// A sentence, or proposition, is a Boolean-valued formula with no free
// variables. The sentence expresses something concrete which must be either
// true or false (although it might be difficult to determine which it is).
class Sentence {
public:
	// A value is like a Boolean, except it can also be in a third state, MU
	// (from Hofstaedter's GEB). MU means that the truth is unknown.
	enum Value { FALSE = false, TRUE = true, MU };

	virtual ~Sentence();

	// Creates a deep copy of the sentence.
	virtual Sentence* clone() const = 0;

	// Evaluates the sentence and returns its truth value.
	virtual Value value() const = 0;

	// Negates the meaning of the sentence, so that it becomes true where it
	// used to be false, and vice versa. The implementation should propagate the
	// negation as far as possible and try to express the result in a positive
	// form, rather than simply wrapping the whole sentence in a logical NOT.
	virtual void negate() = 0;

	// Returns a the possible decompositions of the sentence (possibly none).
	// virtual std::vector<Decomp> decompose() const = 0;

	// Returns the possible deductions from this sentence (possible none).
	// virtual std::vector<Deduct> deduce() const = 0;

	// Prints a string representation of the sentence to the given stream.
	virtual std::ostream& print(std::ostream& s) const = 0;
	friend std::ostream& operator<<(std::ostream& stream, const Sentence& s);

protected:
	Sentence() {}
	Sentence(const Sentence&) = delete;
};

// Logical sentences are the building blocks of the propositional calculus.
// There are four types: AND, OR, implication (if-then), and logical equivalence
// (if and only if).
class Logical : public Sentence {
public:
	enum Type { AND, OR, IMPLIES, IFF };

	// Creates a new logical sentence that connects two propositions using the
	// given logical operator type.
	Logical(Type t, Sentence* a, Sentence* b);

	virtual ~Logical();
	virtual Sentence* clone() const;
	virtual Value value() const;
	virtual void negate();
	virtual std::ostream& print(std::ostream& s) const;

	// FIXME: These three don't need to be public. They might not even need to
	// be separate methods. Although contrapostive is both a decomposition and a
	// deduction, so maybe it should stay separate.
	// TODO: consider the visibility of all things currently declared public --
	// especially the expand* methods.

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
	enum Type { EQ, LT, LTE, SEQ, SUB, SUBE, IN, DIV };

	// Creates a new relation sentence that relates two objects by the given
	// relation operator type. If positive is false, then the sentence states
	// the negative relation (for example, <= becomes >).
	Relation(Type t, bool positive, Object* a, Object* b);

	virtual ~Relation();
	virtual Sentence* clone() const;
	virtual Value value() const;
	virtual void negate();
	virtual std::ostream& print(std::ostream& s) const;

	// TODO: not so simple anymore
	// Assumes the type is SUBSET. Expands the sentence into the sentence
	// "forall x in A: x in B", which is the definition of the subset relation.
	// Sentence* expandSubset();

	// Returns the operation type specified by the string, or -1 otherwise.
	static std::pair<int, bool> getType(const std::string& s);

private:
	Type _type; // the operation type
	bool _want; // negation toggles this
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
	Quantified(Type t, Symbol* var, Sentence* body);

	// Creates a quantified statement using the domain shorthand, which
	// restricts the values of the variable considered to a particular set.
	Quantified(Type t, Symbol* var, Set* domain, Sentence* body);

	virtual ~Quantified();
	virtual Sentence* clone() const;
	virtual Value value() const;
	virtual void negate();
	virtual std::ostream& print(std::ostream& s) const;

	// Returns the quantifier type specified by the string, or -1 otherwise.
	static int getType(const std::string& s);

private:
	Type _type; // the quantifier type
	Symbol* _var; // the bound variable
	Sentence* _body; // the quantified open sentence
};

#endif
