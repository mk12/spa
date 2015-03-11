// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "parse.hpp"

#include "object.hpp"
#include "sentence.hpp"

namespace err {
	const char* no_err_msg = "no error message";
	const char* unexpected_eoi = "unexpected end of input";
	const char* long_symbol = "symbols can only be one character long";
	const char* bad_symbol = "invalid symbol character";
	const char* obj_num = "expected object to be a number";
	const char* obj_set = "expected object to be a set";
	const char* out_of_range = "integer out of range";
}

const char* parseError = err::no_err_msg;

// =============================================================================
//            Parse sentence
// =============================================================================

// Checks to make sure there are more tokens (as opposed to End Of Input).
#define CHECK_EOI() do { \
	if (i >= tokens.size()) { \
		parseError = err::unexpected_eoi; \
		return nullptr; \
	} \
} while (0)

// Checks to make sure the next token is a particular string.
#define EXPECT(c) do { \
	if (tokens[i++] != c) { \
		parseError = "expected '" c "'"; \
		return nullptr; \
	} \
} while (0)

// Returns a value after checking for a right parenthesis character. If it is
// not found, deletes the created return value before returning null.
#define RETURN_PAREN(x) do { \
	auto _ret = (x); \
	if (tokens[i++] != ")") { \
		parseError = "expected ')'"; \
		delete _ret; \
		return nullptr; \
	} \
	return _ret; \
} while (0)

// I vow never to write a parser in C++ again. I will go running back to Parsec.
// IC means that symbols are resolved In Context, using the SymMap.
Sentence* parseSentenceIC(const StrVec&, int&, SymMap&);
Object* parseObjectIC(const StrVec&, int&, SymMap&);
Object* parseCompoundObjIC(const StrVec&, int&, SymMap&);
Number* parseNumberIC(const StrVec&, int&, SymMap&);
Set* parseSetIC(const StrVec&, int&, SymMap&);
Symbol* parseSymbolIC(const std::string&, SymMap&);

Sentence* parseSentence(const StrVec& tokens, int& i) {
	parseError = err::no_err_msg;
	SymMap symbols;
	return parseSentenceIC(tokens, i, symbols);
}

Sentence* parseSentenceIC(const StrVec& tokens, int& i, SymMap& symbols) {
	CHECK_EOI();
	EXPECT("(");
	CHECK_EOI();
	const std::string tok = tokens[i++];
	int type = Logical::getType(tok);
	if (type != -1) {
		Sentence* a = parseSentenceIC(tokens, i, symbols);
		if (a == nullptr) return nullptr;
		Sentence* b = parseSentenceIC(tokens, i, symbols);
		if (b == nullptr) { delete a; return nullptr; }
		RETURN_PAREN(new Logical((Logical::Type)type, a, b));
	}
	type = Relation::getType(tok);
	if (type != -1) {
		Object* a = parseObjectIC(tokens, i, symbols);
		if (a == nullptr) return nullptr;
		Object* b = parseObjectIC(tokens, i, symbols);
		if (b == nullptr) { delete a; return nullptr; }
		RETURN_PAREN(new Relation((Relation::Type)type, a, b));
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
			Set* set = parseSetIC(tokens, i, symbols);
			if (set == nullptr) return nullptr;
			Sentence* body = parseSentenceIC(tokens, i, symbols);
			if (body == nullptr) { delete set; return nullptr; }
			RETURN_PAREN(new Quantified((Quantified::Type)type, x, set, body));
		} else {
			i--;
			Sentence* body = parseSentenceIC(tokens, i, symbols);
			if (body == nullptr) return nullptr;
			RETURN_PAREN(new Quantified((Quantified::Type)type, x, body));
		}
	}
	return nullptr;
}

// =============================================================================
//            Parse object
// =============================================================================

Object* parseObjectIC(const StrVec& tokens, int& i, SymMap& symbols) {
	CHECK_EOI();
	const std::string tok = tokens[i++];
	if (tok == "(") {
		RETURN_PAREN(parseCompoundObjIC(tokens, i, symbols));
	}
	if (tok == "{") {
		return nullptr;
		std::vector<Object*> items;
		for (;;) {

		}
	}
	int type = SpecialSet::getType(tok);
	if (type != -1) {
		return new SpecialSet((SpecialSet::Type)type);
	}
	try {
		int num = std::stoi(tok);
		return new ConcreteNumber(num);
	} catch (const std::invalid_argument& e) {
	} catch (const std::out_of_range& e) {
		parseError = err::out_of_range;
		return nullptr;
	}
	return parseSymbolIC(tok, symbols);
}

Object* parseCompoundObjIC(const StrVec& tokens, int& i, SymMap& symbols) {
	CHECK_EOI();
	const std::string tok = tokens[i++];
	int type = CompoundNumber::getType(tok);
	if (type != -1) {
		Number* a = parseNumberIC(tokens, i, symbols);
		if (a == nullptr) return nullptr;
		Number* b = parseNumberIC(tokens, i, symbols);
		if (b == nullptr) { delete a; return nullptr; }
		return new CompoundNumber((CompoundNumber::Type)type, a, b);
	}
	type = CompoundSet::getType(tok);
	if (type != -1) {
		Set* a = parseSetIC(tokens, i, symbols);
		if (a == nullptr) return nullptr;
		Set* b = parseSetIC(tokens, i, symbols);
		if (b == nullptr) { delete a; return nullptr; }
		return new CompoundSet((CompoundSet::Type)type, a, b);
	}
	return nullptr;
}

Number* parseNumberIC(const StrVec& tokens, int& i, SymMap& symbols) {
	Object* obj = parseObjectIC(tokens, i, symbols);
	if (obj == nullptr) return nullptr;
	Number* num = dynamic_cast<Number*>(obj);
	if (num == nullptr) {
		delete obj;
		parseError = err::obj_num;
		return nullptr;
	}
	return num;
}

Set* parseSetIC(const StrVec& tokens, int& i, SymMap& symbols) {
	Object* obj = parseObjectIC(tokens, i, symbols);
	if (obj == nullptr) return nullptr;
	Set* set = dynamic_cast<Set*>(obj);
	if (set == nullptr) {
		delete obj;
		parseError = err::obj_set;
		return nullptr;
	}
	return set;
}

Symbol* parseSymbolIC(const std::string& str, SymMap& symbols) {
	if (str.length() > 1) {
		parseError = err::long_symbol;
		return nullptr;
	}
	char c = str[0];
	if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z')) {
		parseError = err::bad_symbol;
		return nullptr;
	}
	auto iter = symbols.find(c);
	if (iter != symbols.end()) {
		return new Symbol(c, iter->second);
	}
	Symbol* s = new Symbol(c);
	s->insertInMap(symbols);
	return s;
}

// =============================================================================
//            Tokenize
// =============================================================================

StrVec tokenize(char* line) {
	StrVec tokens;
	char c;
	std::string s;
	while ((c = *line++)) {
		bool special = (c == '(' || c == ')' || c == ',');
		if (special || c == ' ' || c == '\t' || c == '\n' || c == '\r') {
			if (!s.empty()) {
				tokens.push_back(s);
				s.clear();
			}
			if (special) {
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
