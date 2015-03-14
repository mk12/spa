// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef PROVER_H
#define PROVER_H

#include <stack>

class Sentence;

// A theorem prover's job is (not surprisingly) to prove a single theorem. It
// maintains a stack of goal provers......
class TheoremProver {
public:
	// Creates a new theorem prover, initially with no theorem loaded.
	TheoremProver();

	~TheoremProver();

	// Changes the theorem to be proved. Deletes the old theorem.
	void setTheorem(Sentence* s);

	// Returns true if a theorem is currently loaded.
	bool hasTheorem() const;

	// Prints a string representation of the theorem to stdout.
	void printTheorem() const;

private:
	Sentence* _theorem; // the theorem being proved

	struct Node;
	Node* _root; // the root of the given/goal tree
	std::stack<Node*> _stack; // the stack used for depth-first traversal
};

#endif
