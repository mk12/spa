// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef PARSE_H
#define PARSE_H

#include "sentence.hpp"

#include <map>
#include <vector>

Sentence* parse(
	const std::vector<std::string>& tokens,
	int i,
	std::map<char, unsigned int>&);

#endif
