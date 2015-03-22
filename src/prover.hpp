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

	// Returns the current mode of the theorem prover. It begins at NOTHM,
	// becomes PROVING once a theorem is loaded, and switches to DONE once the
	// proof is complete.
	enum Mode { NOTHM, PROVING, DONE };
	Mode mode() const;

	// Assumes PROVING mode. Attemps to decompose the current goal into
	// subgoals, prompting the user to choose an option.
	void decompose();

	// Assume PROVING mode. Attempts to deduce a new given from the current
	// givens, prompting the user to choose a possible deduction (or all).
	void deduce();

	// Prove the current goal by assuming it is trivial.
	void trivial();

	// Prompt the user to provide reasoning in words to prove the current goal.
	void justify();

	// Assumes PROVING mode or DONE mode. Prints the status of the theorem
	// prover (mode, theorem, goal, givens, subgoals left), the theorem being
	// proved, or the tree structure used to decompose the theorem.
	void printStatus() const;
	void printTheorem() const;
	void printTree() const;

	// Assumes PROVING mode. Prints the goal (the current subgoal of the
	// theorem) or the givens (facts that can be used to prove the goal).
	void printGoal() const;
	void printGivens() const;

private:
	class Node;

	// Returns the node that we are currently "at" in the traversal.
	Node* currentNode() const;

	Node* _root; // the root of the given/goal tree
	std::vector<Node*> _dfs; // the stack used for depth-first traversal
	std::vector<Node*> _lineage; // goes from root to the current node
};

#endif
