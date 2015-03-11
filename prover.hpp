// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef PROVER_H
#define PROVER_H

#include "parse.hpp"
#include "sentence.hpp"

#include <stack>
#include <vector>

/*
class GoalProver {
public:

private:
	// Sentence* _goal;
	std::vector<Sentence*> _givens; // ...
	std::vector<Sentence*> _goals; // ...
};
*/

class TheoremProver {
public:
	TheoremProver() : _theorem(nullptr) {}
	~TheoremProver();

	// ...
	void dispatch(const StrVec& tokens);

private:
	Sentence* _theorem; // ...
	// std::stack<GoalProver> _stack; // ...
};

#endif
