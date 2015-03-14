// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "parse.hpp"
#include "prover.hpp"

#include <readline/readline.h>
#include <readline/history.h>

#include <iostream>

#include <cassert>

namespace {
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
	"prove  -  set the theorem to prove\n"
	"thm    -  show the current theorem\n"
	"go     -  go to the next step\n"
	"given  -  show the current givens\n"
	"goal   -  show the current goal\n"
	"stack  -  show the entire proof stack\n"
	"print  -  print the formal proof\n\n";

	const char* bad_cmd = "invalid command";
}

// Prints an error message to stderr.
static void error(const char* s) {
	std::cerr << "error: " << s << std::endl;
}

// Performs the appropriate action for the given tokenized user input. Does
// nothing for empty input. Returns true if the program should quit.
static bool dispatch(const StrVec& tokens, TheoremProver& tp) {
	auto size = tokens.size();
	if (size == 0) {
		return false;
	}
	const std::string cmd = tokens[0];
	bool prove = cmd == "prove";
	if (size == 1) {
		if (cmd == "quit" || cmd == "exit") {
			return true;
		}
		if (prove) {
			error("expecting theorem");
		} else if (cmd == "help") {
			std::cout << help;
		} else if (cmd == "thm") {
			if (tp.hasTheorem()) {
				tp.printTheorem();
			} else {
				error("no theorem loaded");
			}
		} else if (cmd == "stack") {
		} else if (cmd == "print") {
		} else {
			error(bad_cmd);
		}
	} else if (prove) {
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
	std::cout << header << std::endl;
	for (;;) {
		line = readline(prompt);
		if (line == nullptr) {
			// This occurs on EOF, so print a newline before quitting.
			// Otherwise, the shell prompt will appear on the same line.
			std::cout << std::endl;
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
