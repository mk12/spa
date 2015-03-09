// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "object.hpp"
#include "sentence.hpp"

namespace err {
	const char* no_err_msg = "no error message";
	const char* unexpected_eoi = "unexpected end of input";
}

// Returns a vector of string tokens by splitting on whitespace. Left and right
// parentheses are always treated as separate tokens.
std::vector<std::string> tokenize(char* line) {
	std::vector<std::string> tokens;
	char c;
	std::string s;
	while ((c = *line++)) {
		bool paren = (c == '(' || c == ')');
		if (paren || c == ' ' || c == '\t' || c == '\n' || c == '\r') {
			if (!s.empty()) {
				tokens.push_back(s);
				s.clear();
			}
			if (paren) {
				tokens.push_back(std::string(1, c));
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

#define CHECK_EOI() do { if (i >= tokens.size()) { \
	_parseError = strings::unexpected_eoi; \
	return nullptr; \
} while (0)

#define EXPECT(c) do { if (tokens[i++] != c) { \
	_parseError = "expected '" c "'"; \
	return nullptr; \
} while (0)

Object* Object::parse(
		const std::vector<std::string>& tokens,
		int& i,
		std::map<char, unsigned int>& symbols) {
	_parseError = strings::no_err_msg;
}

Sentence* Sentence::parse(
		const std::vector<std::string>& tokens,
		int& i
		std::map<char, unsigned int>& symbols) {
	_parseError = strings::no_err_msg;
	CHECK_EOI();
	EXPECT("(");
	CHECK_EOI();
	const string tok = tokens[i++];
	int type = Logical::getType(tok);
	if (type != -1) {
		Sentence* a = parse(tokens, i, symbols);
		if (a == nullptr) return nullptr;
		Sentence* b = parse(tokens, i, symbols);
		if (b == nullptr) return nullptr;
		return new Logical(type, a, b);
	}
	type = Quantified::getType(tok);
	if (type != -1) {
		CHECK_EOI();
		const string tok2 = tokens[i++];
		if (tok2.length() > 1) {
			_parseError = "symbols can only be one character long";
			return nullptr;
		}
		Symbol x(tok2[0]);
		x.insertInMap(symbols);
		CHECK_EOI();
		const string tok3 = tokens[i++];
		if (tok3 == "in") {
			Object *set = Object::parse(tokens, i, symbols);
			if (dynamic_cast<Set*>(set) == nullptr) {
				_parseError = "expected domain to be a set";
				return nullptr;
			}
			Sentence *body = parse(tokens, i, symbols);
			if (body == nullptr) return nullptr;
			return new Quantified(type, x, set, body);
		} else {
			i--;
			Sentence *body = parse(tokens, i, symbols);
			if (body == nullptr) return nullptr;
			return new Quantified(type, x, body);
		}
	}
	return nullptr;
}
