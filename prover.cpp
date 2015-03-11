// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "prover.hpp"

TheoremProver::~TheoremProver() {
	if (_theorem != nullptr) {
		delete _theorem;
	}
}

void TheoremProver::dispatch(const StrVec& tokens) {
	std::string cmd = tokens[0];
	if (cmd == "thm") {
		if (_theorem == nullptr) {
			int i = 1;
			_theorem = parseSentence(tokens, i);
			if (_theorem == nullptr) {
				std::cout << parseError << std::endl;
			}
		} else {
			std::cout << *_theorem << std::endl;
		}
	} else if (cmd == "stack") {
		// print stack
	} else if (cmd == "print") {
		// print English proof
	} else {
		// stack.top().dispatch(tokens);
	}
}
