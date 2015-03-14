// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "parse.hpp"

#include "object.hpp"
#include "sentence.hpp"

namespace {
	const char* err_default = "invalid input";
	const char* err_eoi = "unexpected end of input";
	const char* err_nan = "expected object to be a number";
	const char* err_range = "integer out of range";
	const char* err_nas = "expected object to be a set";
	const char* err_comma = "expected comma in set";
	const char* err_long = "symbols can only be one character long";
	const char* err_char = "invalid symbol character";
}

const char* parseError = nullptr;

// =============================================================================
//            Parse sentence
// =============================================================================

// Checks to make sure there are more tokens (as opposed to End Of Input).
#define CHECK_EOI() do { \
	if (i >= tokens.size()) { \
		parseError = err_eoi; \
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
#define RET_PAREN(x) do { \
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
Sentence* parseSentenceIC(const StrVec&, Index&, SymMap&);
Object* parseObjectIC(const StrVec&, Index&, SymMap&);
Object* parseCompoundObjIC(const StrVec&, Index&, SymMap&);
Number* parseNumberIC(const StrVec&, Index&, SymMap&);
Set* parseSetIC(const StrVec&, Index&, SymMap&);
Symbol* parseSymbolIC(const std::string&, SymMap&, bool);

Sentence* parseSentence(const StrVec& tokens, Index& i) {
	parseError = err_default;
	SymMap symbols;
	return parseSentenceIC(tokens, i, symbols);
}

Sentence* parseSentenceIC(const StrVec& tokens, Index& i, SymMap& symbols) {
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
		RET_PAREN(new Logical(static_cast<Logical::Type>(type), a, b));
	}
	type = Relation::getType(tok);
	if (type != -1) {
		Object* a = parseObjectIC(tokens, i, symbols);
		if (a == nullptr) return nullptr;
		Object* b = parseObjectIC(tokens, i, symbols);
		if (b == nullptr) { delete a; return nullptr; }
		RET_PAREN(new Relation(static_cast<Relation::Type>(type), a, b));
	}
	type = Quantified::getType(tok);
	if (type != -1) {
		CHECK_EOI();
		const std::string tok2 = tokens[i++];
		Symbol* var = parseSymbolIC(tok2, symbols, true);
		CHECK_EOI();
		const std::string tok3 = tokens[i++];
		auto qt = static_cast<Quantified::Type>(type);
		if (tok3 == "in") {
			Set* set = parseSetIC(tokens, i, symbols);
			if (set == nullptr) { delete var; return nullptr; }
			Sentence* body = parseSentenceIC(tokens, i, symbols);
			if (body == nullptr) { delete var; delete set; return nullptr; }
			RET_PAREN(new Quantified(qt, var, set, body));
		} else {
			i--;
			Sentence* body = parseSentenceIC(tokens, i, symbols);
			if (body == nullptr) { delete var; return nullptr; }
			RET_PAREN(new Quantified(qt, var, body));
		}
	}
	return nullptr;
}

// =============================================================================
//            Parse object
// =============================================================================

Object* parseObjectIC(const StrVec& tokens, Index& i, SymMap& symbols) {
	CHECK_EOI();
	const std::string tok = tokens[i++];
	if (tok == "(") {
		RET_PAREN(parseCompoundObjIC(tokens, i, symbols));
	}
	if (tok == "{") {
		std::vector<Object*> items;
		bool success = true;
		for (;;) {
			if (i >= tokens.size()) {
				parseError = err_eoi;
				success = false;
				break;
			}
			const std::string tok2 = tokens[i++];
			if (tok2 == "}") {
				break;
			}
			i--;
			Object* obj = parseObjectIC(tokens, i, symbols);
			if (obj == nullptr) {
				success = false;
				break;
			}
			items.push_back(obj);
			if (i >= tokens.size()) {
				parseError = err_eoi;
				success = false;
				break;
			}
			const std::string tok3 = tokens[i++];
			if (tok3 == "}") {
				break;
			}
			if (tok3 != ",") {
				parseError = err_comma;
				success = false;
				break;
			}
		}
		if (success) {
			return new ConcreteSet(items);
		}
		for (Object* obj: items) {
			delete obj;
		}
		return nullptr;
	}
	int type = SpecialSet::getType(tok);
	if (type != -1) {
		return new SpecialSet(static_cast<SpecialSet::Type>(type));
	}
	try {
		int num = std::stoi(tok);
		return new ConcreteNumber(num);
	} catch (const std::invalid_argument& e) {
		(void)e;
	} catch (const std::out_of_range& e) {
		(void)e;
		parseError = err_range;
		return nullptr;
	}
	return parseSymbolIC(tok, symbols, false);
}

Object* parseCompoundObjIC(const StrVec& tokens, Index& i, SymMap& symbols) {
	CHECK_EOI();
	const std::string tok = tokens[i++];
	int type = CompoundNumber::getType(tok);
	if (type != -1) {
		Number* a = parseNumberIC(tokens, i, symbols);
		if (a == nullptr) return nullptr;
		Number* b = parseNumberIC(tokens, i, symbols);
		if (b == nullptr) { delete a; return nullptr; }
		auto t = static_cast<CompoundNumber::Type>(type);
		return new CompoundNumber(t, a, b);
	}
	type = CompoundSet::getType(tok);
	if (type != -1) {
		Set* a = parseSetIC(tokens, i, symbols);
		if (a == nullptr) return nullptr;
		Set* b = parseSetIC(tokens, i, symbols);
		if (b == nullptr) { delete a; return nullptr; }
		return new CompoundSet(static_cast<CompoundSet::Type>(type), a, b);
	}
	return nullptr;
}

Number* parseNumberIC(const StrVec& tokens, Index& i, SymMap& symbols) {
	Object* obj = parseObjectIC(tokens, i, symbols);
	if (obj == nullptr) return nullptr;
	Number* num = dynamic_cast<Number*>(obj);
	if (num == nullptr) {
		delete obj;
		parseError = err_nan;
		return nullptr;
	}
	return num;
}

Set* parseSetIC(const StrVec& tokens, Index& i, SymMap& symbols) {
	Object* obj = parseObjectIC(tokens, i, symbols);
	if (obj == nullptr) return nullptr;
	Set* set = dynamic_cast<Set*>(obj);
	if (set == nullptr) {
		delete obj;
		parseError = err_nas;
		return nullptr;
	}
	return set;
}

Symbol* parseSymbolIC(const std::string& str, SymMap& symbols, bool fresh) {
	if (str.length() > 1) {
		parseError = err_long;
		return nullptr;
	}
	char c = str[0];
	if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z')) {
		parseError = err_char;
		return nullptr;
	}
	return new Symbol(c, symbols, fresh);
}

// =============================================================================
//            Tokenize
// =============================================================================

StrVec tokenize(char* line) {
	StrVec tokens;
	char c;
	std::string s;
	while ((c = *line++)) {
		bool punct = (c == '(' || c == ')' || c == '{' || c == '}' || c == ',');
		if (punct || c == ' ' || c == '\t' || c == '\n' || c == '\r') {
			if (!s.empty()) {
				tokens.push_back(s);
				s.clear();
			}
			if (punct) {
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
