// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef PROVER_H
#define PROVER_H

class GoalProver {
public:
private:
	// Sentence* _goal;
	std::vector<Sentence*> _givens;
	std::vector<Sentence*> _goals;
};

class TheoremProver {
public:
	~TheoremProver();
	void dispatch(const std::vector<std::string>& tokens);
private:
	std::stack<GoalProver> _stack;
	Sentence* _theorem;
};

#endif
