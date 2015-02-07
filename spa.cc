// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include <iostream>
#include <stack>
#include <vector>

#include <readline/readline.h>
#include <readline/history.h>

// =============================================================================
//            String constants
// =============================================================================

const char* spa_prompt = "spa> ";

const char* spa_header =
" _____   _____   _____\n"
"|  ___| |  _  | |  _  |  |  Simple Proof Assisant\n"
"| |___  | |_| | | |_| |  |  (c) 2015 Mitchell Kember\n"
"|___  | |  ___| |  _  |  |  Version 0.1\n"
" ___| | | |     | | | |  |\n"
"|_____| |_|     |_| |_|  |  Type \"help\" to get started\n";

const char* spa_help = "\n"
"help   -  show this help message\n"
"thm    -  set or show the theorem\n"
"go     -  go to the next step\n"
"triv   -  prove a trivial goal\n"
"goal   -  show the current goal\n"
"given  -  show the given truths\n"
"stack  -  show the entire proof stack\n"
"print  -  print the format proof\n";

// =============================================================================
//            Sentences
// =============================================================================

struct Sentence {
	// bool want = true;
	virtual bool evaluate(std::map<???>) = 0;
	virtual ~Sentence() = 0;
	void negate() { want = !want; }
}

// integer, set, rational, real
struct Object {
}

struct OpenSentence {
	Variable* x;
	Sentence* body;
}

struct Quantified : Sentence {
}

struct Logical : Sentence {
}

struct Relation : Sentence {
}

struct Symbol : Object {
}

struct Set : Object {
}

// =============================================================================
//            Provers
// =============================================================================

class GoalProver {
	Sentence* goal;
	std::vector<Sentence*> givens;
public:

};

class TheoremProver {
	std::stack<GoalProver> stack;
	Sentence* theorem;
public:
	void dispatch(const std::vector<std::string>& tokens) {
		string cmd = tokens[0];
		if (cmd == "thm") {
			if (theorem == nullptr) {
				theorem = Sentence.parse(tokens);
			} else {
				std::cout << theorem;
			}
		} else if (cmd == "stack") {
			// print stack
		} else if (cmd == "print") {
			// print English proof
		} else {
			stack.top().dispatch(tokens);
		}
	}
};

// =============================================================================
//            Tokenize
// =============================================================================

// Returns a vector of string tokens by splitting on whitespace.
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

int main() {
	char* line;
	TheoremProver tp;
	std::cout << spa_header << std::endl;
	for (;;) {
		line = readline(spa_prompt);
		if (line == nullptr) {
			std::cout << std::endl;
			break;
		}
		std::vector<std::string> tokens = tokenize(line);
		if (tokens.size() != 0) {
			add_history(line);
			if (tokens[0] == "help") {
				std::cout << spa_help;
			} else {
				tp.dispatch(tokens);
			}
		}
		free(line);
	}
	return 0;
}
