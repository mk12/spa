// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "prover.hpp"

TheoremProver::~TheoremProver() {
	if (_theorem != nullptr) {
		delete _theorem;
	}
}

void TheoremProver::dispatch(const StrVec& tokens) {
	std::string cmd = tokens[0];
	if (cmd == "prove") {
		if (tokens.size() <= 1) {
			std::cerr << "expecting theorem" << std::endl;
		} else {
			int i = 1;
			Sentence* thm = parseSentence(tokens, i);
			if (thm == nullptr) {
				std::cerr << parseError << std::endl;
			} else {
				if (_theorem != nullptr) {
					delete _theorem;
				}
				_theorem = thm;
			}
		}
	} else if (cmd == "thm") {
		if (_theorem == nullptr) {
			std::cerr << "no theorem is loaded" << std::endl;
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
