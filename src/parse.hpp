// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#ifndef PARSE_H
#define PARSE_H

#include "sentence.hpp"

#include <string>
#include <vector>

typedef std::vector<std::string> StrVec;
typedef StrVec::size_type Index;

// The parsing functions always store an error message in this string when they
// fail, before returning null.
extern const char* parseError;

// Parses a complete sentence in prefix notation. Returns null on failure and
// stores an error message in parseError.
Sentence* parseSentence(const StrVec& tokens, Index& i);

// Returns a vector of string tokens by splitting on whitespace. Left and right
// parentheses/braces and commas are always treated as separate tokens.
StrVec tokenize(char* line);

#endif
