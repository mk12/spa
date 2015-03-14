// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef PROVER_H
#define PROVER_H

#include <vector>

class Sentence;

// A theorem prover (surprise) proves theorems. It does this by creating and
// navigating a tree of given/goal pairs which break down the proof of the
// theorem into many subgoals.
class TheoremProver {
public:
	// Creates a new theorem prover, initially with no theorem loaded.
	TheoremProver();

	~TheoremProver();

	// Changes the theorem to be proved. Deletes the old theorem.
	void setTheorem(Sentence* s);

	// Returns true if a theorem is currently loaded.
	bool hasTheorem() const;

	// Prints a string representation of the theorem (the ultimate goal being
	// proved), the givens (facts that can be used to prove the goal), the goal
	// (the current subgoal of the theorem), or the stack (structure used to
	// decompose the theorem) to stdout. Does nothing if no theorem is loaded.
	void printTheorem() const;
	void printGivens() const;
	void printGoal() const;
	void printStack() const;

private:
	class Node;

	Node* _root; // the root of the given/goal tree
	std::vector<Node*> _stack; // the stack used for depth-first traversal
	std::vector<Sentence*> _givens; // the stack of currently available givens
};

#endif
