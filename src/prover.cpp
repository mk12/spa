// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "prover.hpp"

#include "sentence.hpp"

#include <iostream>

// ...
struct TheoremProver::Node {
	Sentence* given;
	Sentence* goal;
	Node* child;
	Node* child2;
};

TheoremProver::TheoremProver() : _theorem(nullptr) {}

TheoremProver::~TheoremProver() {
	delete _theorem;
}

void TheoremProver::setTheorem(Sentence* s) {
	delete _theorem;
	_theorem = s;
}

bool TheoremProver::hasTheorem() const {
	return _theorem != nullptr;
}

void TheoremProver::printTheorem() const {
	if (hasTheorem()) {
		std::cout << *_theorem << std::endl;
	}
}
