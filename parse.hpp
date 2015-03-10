// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef PARSE_H
#define PARSE_H

#include "sentence.hpp"

#include <map>
#include <string>
#include <vector>

extern const char* parseError;

std::vector<std::string> tokenize(char* line);

Sentence* parseSentence(
	const std::vector<std::string>& tokens,
	int i,
	std::map<char, unsigned int>&);

#endif
