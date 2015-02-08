// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "spa.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

#include <readline/readline.h>
#include <readline/history.h>

// =============================================================================
//            String constants
// =============================================================================

namespace strings {

const char* prompt = "spa> ";

const char* header =
" _____   _____   _____\n"
"|  ___| |  _  | |  _  |  |  Simple Proof Assisant\n"
"| |___  | |_| | | |_| |  |  (c) 2015 Mitchell Kember\n"
"|___  | |  ___| |  _  |  |  Version 0.1\n"
" ___| | | |     | | | |  |\n"
"|_____| |_|     |_| |_|  |  Type \"help\" to get started\n";

const char* help = "\n"
"help   -  show this help message\n"
"quit   -  quit the program\n"
"thm    -  set or show the theorem\n"
"go     -  go to the next step\n"
"given  -  show the current givens\n"
"goal   -  show the current goal\n"
"stack  -  show the entire proof stack\n"
"print  -  print the formal proof\n\n";

}

// =============================================================================
//            Objects
// =============================================================================

int Symbol::_count = 0;

// =============================================================================
//            Sentences
// =============================================================================

Sentence::Value Sentence::evaluate() {
	Value v = value();
	return (v == MU) ? MU : (Value)(v == _want);
}

Sentence::Value Logical::value() {
	Value va = _a->value();
	Value vb = _b->value();
	if (va == MU || vb == MU) {
		return MU;
	}
	switch(_type) {
	case AND:
		return (Value)(va && vb); break;
	case OR:
		return (Value)(va || vb); break;
	case IMPLIES:
		return (Value)(!va || vb); break;
	case IFF:
		return (Value)(va == vb); break;
	}
}

void Logical::converse() {
	assert(_type == IMPLIES);
	std::swap(_a, _b);
}

void Logical::contrapositive() {
	converse();
	_a->negate();
	_b->negate();
}

void Logical::expandIff() {
	assert(_type == IFF);
	_type = AND;
	Logical* fwd = new Logical(IMPLIES, _a, _b);
	// TODO: deep copy
	Logical* bwd = new Logical(*fwd);
	_a = fwd;
	_b = bwd;
	bwd->converse();
}

void Logical::negate() {
	switch (_type) {
	case AND:
		_type = OR;
		_a->negate();
		_b->negate();
		break;
	case OR:
		_type = AND;
		_a->negate();
		_b->negate();
		break;
	case IMPLIES:
		_type = AND;
		_b->negate();
		break;
	case IFF:
		expandIff();
		negate();
		break;
	}
}

Sentence::Value Quantified::value() {
	return _body->value();
}

void Quantified::negate() {
	_type = (Type)!_type;
	_body->negate();
}

// =============================================================================
//            Provers
// =============================================================================

void TheoremProver::dispatch(const std::vector<std::string>& tokens) {
	std::string cmd = tokens[0];
	if (cmd == "thm") {
		if (_theorem == nullptr) {
			// theorem = Sentence.parse(tokens);
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

// =============================================================================
//            Tokenize
// =============================================================================

// Returns a vector of string tokens by splitting on whitespace.
// TODO: Make this more efficient.
std::vector<std::string> tokenize(char* line) {
	std::vector<std::string> tokens;
	char c;
	std::string s;
	while ((c = *line++)) {
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
			if (!s.empty()) {
				tokens.push_back(s);
				s.clear();
			}
		} else {
			s.push_back(c);
		}
	}
	if (!s.empty()) {
		tokens.push_back(s);
	}
	return tokens;
}

// =============================================================================
//            Main
// =============================================================================

// Executes the interactive proof assistant loop. Uses the GNU Readline library
// for user input.
int main() {
	char* line;
	TheoremProver tp;
	std::cout << strings::header << std::endl;
	for (;;) {
		line = readline(strings::prompt);
		if (line == nullptr) {
			std::cout << std::endl;
			break;
		}
		std::vector<std::string> tokens = tokenize(line);
		if (tokens.size() != 0) {
			add_history(line);
			if (tokens[0] == "quit" || tokens[0] == "exit") {
				break;
			} else if (tokens[0] == "help") {
				std::cout << strings::help;
			} else {
				tp.dispatch(tokens);
			}
		}
		free(line);
	}
	return 0;
}
