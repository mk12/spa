// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "prover.hpp"

TheoremProver::~TheoremProver() {
	if (_theorem != nullptr) {
		delete _theorem;
	}
}

void TheoremProver::dispatch(const std::vector<std::string>& tokens) {
	std::string cmd = tokens[0];
	if (cmd == "thm") {
		if (_theorem == nullptr) {
			std::map<char, unsigned int> symbols;
			_theorem = Sentence::parse(tokens, 1, symbols);
			if (_theorem == nullptr) {
				std::cout << Sentence::_parseError << std::endl;
			}
		} else {
			std::cout << _theorem;
		}
	} else if (cmd == "stack") {
		// print stack
	} else if (cmd == "print") {
		// print English proof
	} else {
		// stack.top().dispatch(tokens);
	}
}
