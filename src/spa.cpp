// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "parse.hpp"
#include "prover.hpp"

#include <readline/readline.h>
#include <readline/history.h>

#include <iostream>

namespace strings {
	namespace {
	static const char* prompt = "spa> ";

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
}}

// Runs the interactive proof assistant loop, using the GNU Readline library for
// user input. Most commands are passed on to the theorem prover.
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
		StrVec tokens = tokenize(line);
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
