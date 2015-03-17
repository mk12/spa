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

	// Changes the theorem to be proved. Deletes the old theorem. Passing null
	// has the effect of clearing the theorem.
	void setTheorem(Sentence* s);

	// Returns true if a theorem is currently loaded.
	bool hasTheorem() const;

	// Attemps to decompose the current goal into subgoals. Prompts the user if
	// there are multiple options. If successful, moves to the primary subgoal
	// and returns true. If not possible, returns false.
	bool advance() const;

	// Prints a string representation of the theorem (the ultimate goal being
	// proved), the givens (facts that can be used to prove the goal), the goal
	// (the current subgoal of the theorem), or the tree (structure used to
	// decompose the theorem) to stdout. Does nothing if no theorem is loaded.
	void printTheorem() const;
	void printGivens() const;
	void printGoal() const;
	void printTree() const;

private:
	class Node;

	// Returns the node that we are currently "at" in the traversal.
	Node* currentNode() const;

	Node* _root; // the root of the given/goal tree
	std::vector<Node*> _dfs; // the stack used for depth-first traversal
	std::vector<Node*> _lineage; // goes from root to the current node
};

#endif
