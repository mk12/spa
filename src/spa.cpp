// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "parse.hpp"
#include "prover.hpp"

#include <readline/readline.h>
#include <readline/history.h>

#include <iostream>

#include <cassert>

namespace {
	const char* prompt = "\x1b[1;35mspa>\x1b[0m ";

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
	"prove  -  set the theorem to prove\n"
	"dec    -  decompose the current goal\n"
	"ded    -  deduce from the current goal\n"
	"triv   -  prove a trivial goal\n"
	"just   -  prove a goal with justification\n"
	"stat   -  show the overall status\n"
	"thm    -  show the current theorem\n"
	"given  -  show the current givens\n"
	"goal   -  show the current goal\n"
	"tree   -  show the entire proof tree\n\n";

	const char* bad_cmd = "invalid command";
	const char* no_thm = "no theorem loaded";
}

// Prints an error message to stderr.
static void error(const char* s) {
	std::cerr << "error: " << s << '\n';
}

// Performs the appropriate action for the given tokenized user input. Does
// nothing for empty input. Returns true if the program should quit.
static bool dispatch(const StrVec& tokens, TheoremProver& tp) {
	auto size = tokens.size();
	if (size == 0) {
		return false;
	}
	const std::string cmd = tokens[0];
	if (size == 1) {
		if (cmd == "quit" || cmd == "exit") {
			return true;
		}
		TheoremProver::Mode m = tp.mode();
		if (cmd == "prove") {
			error("expecting theorem");
		} else if (cmd == "help") {
			std::cout << help;
		} else if (cmd == "dec" || cmd == "ded" || cmd == "triv"
				|| cmd == "just" || cmd == "given" || cmd == "givens"
				|| cmd == "goal") {
			if (m == TheoremProver::NOTHM) {
				error(no_thm);
				return false;
			}
			if (m == TheoremProver::DONE) {
				error("the proof is complete");
				return false;
			}
			if (cmd == "dec") {
				tp.decompose();
			} else if (cmd == "ded") {
				tp.deduce();
			} else if (cmd == "triv") {
				tp.trivial();
			} else if (cmd == "just") {
				tp.justify();
			} else if (cmd == "given" || cmd == "givens") {
				tp.printGivens();
			} else if (cmd == "goal") {
				tp.printGoal();
			}
		} else if (cmd == "stat" || cmd == "thm" || cmd == "tree") {
			if (m == TheoremProver::NOTHM) {
				error(no_thm);
				return false;
			}
			if (cmd == "stat") {
				tp.printStatus();
			} else if (cmd == "thm") {
				tp.printTheorem();
			} else if (cmd == "tree") {
				tp.printTree();
			}
		} else {
			error(bad_cmd);
		}
	} else if (cmd == "prove") {
		Index i = 1;
		Sentence* thm = parseSentence(tokens, i);
		if (thm == nullptr) {
			error(parseError);
		} else {
			tp.setTheorem(thm);
		}
	} else {
		error(bad_cmd);
	}
	return false;
}

// Runs the interactive proof assistant loop, using the GNU Readline library for
// user input. Commands are handled by the dispatch function.
int main() {
	char* line;
	TheoremProver tp;
	std::cout << header << '\n';
	for (;;) {
		line = readline(prompt);
		if (line == nullptr) {
			// This occurs on EOF, so print a newline before quitting.
			// Otherwise, the shell prompt will appear on the same line.
			std::cout << '\n';
			break;
		}
		StrVec tokens = tokenize(line);
		if (tokens.size() != 0) {
			add_history(line);
			if (dispatch(tokens, tp)) {
				break;
			}
		}
		free(line);
	}
	return 0;
}
