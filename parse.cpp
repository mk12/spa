// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "object.hpp"
#include "sentence.hpp"

// =============================================================================
//            Tokenize
// =============================================================================

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

// =============================================================================
//            Parse
// =============================================================================

namespace err {
	const char* no_err_msg = "no error message";
	const char* unexpected_eoi = "unexpected end of input";
	const char* long_symbol = "symbols can only be one character long";
	const char* domain_set = "expected domain to be a set";
}

const char* parseError = err::no_err_msg;

#define CHECK_EOI() do { \
	if (i >= tokens.size()) { \
		parseError = err::unexpected_eoi; \
		return nullptr; \
	} \
} while (0)

#define EXPECT(c) do { \
	if (tokens[i++] != c) { \
		parseError = "expected '" c "'"; \
		return nullptr; \
	} \
} while (0)

Object* parseObject(
		const std::vector<std::string>& tokens,
		int& i,
		std::map<char, unsigned int>& symbols) {
	parseError = err::no_err_msg;
	return nullptr;
}

Sentence* parseSentenceHelp(
		const std::vector<std::string>& tokens,
		int& i,
		std::map<char, unsigned int>& symbols);

Sentence* parseSentence(
		const std::vector<std::string>& tokens,
		int& i,
		std::map<char, unsigned int>& symbols) {
	parseError = err::no_err_msg;
	CHECK_EOI();
	EXPECT("(");
	CHECK_EOI();
	Sentence* s = parseSentenceHelp(tokens, i, symbols);
	if (s != nullptr) {
		EXPECT(")");
	}
	return s;
}

Sentence* parseSentenceHelp(
		const std::vector<std::string>& tokens,
		int& i,
		std::map<char, unsigned int>& symbols) {
	const std::string tok = tokens[i++];
	int type = Logical::getType(tok);
	if (type != -1) {
		Sentence* a = parseSentence(tokens, i, symbols);
		if (a == nullptr) return nullptr;
		Sentence* b = parseSentence(tokens, i, symbols);
		if (b == nullptr) { delete a; return nullptr; }
		return new Logical((Logical::Type)type, a, b);
	}
	type = Relation::getType(tok);
	if (type != -1) {
		Object *a = parseObject(tokens, i, symbols);
		if (a == nullptr) return nullptr;
		Object *b = parseObject(tokens, i, symbols);
		if (b == nullptr) { delete a; return nullptr; }
		return new Relation((Relation::Type)type, a, b);
	}
	type = Quantified::getType(tok);
	if (type != -1) {
		CHECK_EOI();
		const std::string tok2 = tokens[i++];
		if (tok2.length() > 1) {
			parseError = err::long_symbol;
			return nullptr;
		}
		Symbol x(tok2[0]);
		x.insertInMap(symbols);
		CHECK_EOI();
		const std::string tok3 = tokens[i++];
		if (tok3 == "in") {
			Object *obj = parseObject(tokens, i, symbols);
			if (obj == nullptr) return nullptr;
			Set *set = dynamic_cast<Set*>(obj);
			if (set == nullptr) {
				delete obj;
				parseError = err::domain_set;
				return nullptr;
			}
			Sentence *body = parseSentence(tokens, i, symbols);
			if (body == nullptr) { delete set; return nullptr; }
			return new Quantified((Quantified::Type)type, x, set, body);
		} else {
			i--;
			Sentence *body = parseSentence(tokens, i, symbols);
			if (body == nullptr) return nullptr;
			return new Quantified((Quantified::Type)type, x, body);
		}
	}
	return nullptr;
}
