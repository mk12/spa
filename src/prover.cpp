// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "prover.hpp"

#include <iostream>

// ...
struct Node {
	Sentence* given;
	Sentence* goal;
	Node* child;
	Node* child2;
};

TheoremProver::~TheoremProver() {
	delete _theorem;
}

static void error(const std::string& s) {
	std::cerr << "error: " << s << std::endl;
}

void TheoremProver::dispatch(const StrVec& tokens) {
	std::string cmd = tokens[0];
	if (cmd == "prove") {
		if (tokens.size() <= 1) {
			error("expecting theorem");
		} else {
			Index i = 1;
			Sentence* thm = parseSentence(tokens, i);
			if (thm == nullptr) {
				error(parseError);
			} else {
				if (_theorem != nullptr) {
					delete _theorem;
				}
				_theorem = thm;
			}
		}
	} else if (cmd == "thm") {
		if (_theorem == nullptr) {
			error("no theorem is loaded");
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
