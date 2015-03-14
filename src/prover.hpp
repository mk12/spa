// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef PROVER_H
#define PROVER_H

#include "parse.hpp"

#include <stack>
#include <vector>

struct Node;
class Sentence;

// A theorem prover's job is (not surprisingly) to prove a single theorem. It
// maintains a stack of goal provers......
class TheoremProver {
public:
	TheoremProver() : _theorem(nullptr) {}
	~TheoremProver();

	// Performs the appropriate action for the given tokenized user input.
	void dispatch(const StrVec& tokens);

private:
	Sentence* _theorem; // the theorem being proved
	Node* _root;
	std::stack<Node*> _stack;
};

// stack for traversal


#endif
