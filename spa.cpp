// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "spa.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

#include <readline/readline.h>
#include <readline/history.h>

// =============================================================================
//            String constants
// =============================================================================

namespace strings {

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
"thm    -  set or show the theorem\n"
"go     -  go to the next step\n"
"given  -  show the current givens\n"
"goal   -  show the current goal\n"
"stack  -  show the entire proof stack\n"
"print  -  print the formal proof\n\n";

}

// =============================================================================
//            Objects
// =============================================================================

int Symbol::_count = 0;

std::ostream& operator<<(std::ostream& stream, const Object& obj) {
	return obj.print(stream);
}

Number* ConcreteNumber::cloneSelf() const {
	return new ConcreteNumber(_x);
}

Number* CompoundNumber::cloneSelf() const {
	return new CompoundNumber(_type, _a->cloneSelf(), _b->cloneSelf());
}

std::ostream& CompoundNumber::print(std::ostream& s) const {
	s << '(';
	_a->print(s);
	switch (_type) {
	case ADD: s << " + "; break;
	case SUB: s << " - "; break;
	case MUL: s << " * "; break;
	case DIV: s << " / "; break;
	}
	_b->print(s);
	return s << ')';
}

bool ConcreteSet::contains(Object *obj) const {
	for (Object* x: _items) {
		if (*x == *obj) {
			return true;
		}
	}
	return false;
}

Set* ConcreteSet::cloneSelf() const {
	std::vector<Object*> v;
	v.reserve(_items.size());
	for (Object *obj: _items) {
		v.push_back(obj->clone());
	}
	return new ConcreteSet(v);
}

ConcreteSet::~ConcreteSet() {
	for (Object *obj: _items) {
		delete obj;
	}
}

std::ostream& ConcreteSet::print(std::ostream& s) const {
	s << '{';
	bool first = true;
	for (const Object* obj: _items) {
		if (first) {
			first = false;
		} else {
			s << ", ";
		}
		s << *obj;
	}
	return s << '}';
}

bool CompoundSet::contains(Object *obj) const {
	switch (_type) {
	case UNION: return _a->contains(obj) || _b->contains(obj);
	case INTERSECT: return _a->contains(obj) && _b->contains(obj);
	case DIFF: return _a->contais(obj) && !_b->contains(obj);
	}
}

Set* CompoundSet::cloneSelf() const {
	return new CompoundSet(_type, _a->cloneSelf(), _b->cloneSelf());
}

std::ostream& CompoundSet::print(std::ostream& s) const {
	_a->print(s);
	switch (_type) {
	case UNION: s << " (union) "; break;
	case INTERSECT: s << " (intersect) "; break;
	case DIFF: s << " \\ "; break;
	}
	return _b->print(s);
}

bool InfiniteSet::contains(Object *obj) const {
	return Sentence::FALSE;
}

Symbol* Symbol::cloneSelf() const {
	return new Symbol(_c, _id);
}

// =============================================================================
//            Sentences
// =============================================================================

Sentence* Logical::clone() const {
	return new Logical(_type, _a->clone(), _b->clone());
}

Sentence* Relation::clone() const {
	return new Relation(_type, _a->clone(), _b->clone());
}

Sentence* Quantified::clone() const {
	return new Quantified(_type, _var, _body->clone());
}

Sentence::Value Sentence::evaluate() const {
	Value v = value();
	return (v == MU) ? MU : (Value)(v == _want);
}

Sentence::Value Logical::value() const {
	Value va = _a->value();
	Value vb = _b->value();
	if (va == MU || vb == MU) {
		return MU;
	}
	switch(_type) {
	case AND:
		return (Value)(va && vb); break;
	case OR:
		return (Value)(va || vb); break;
	case IMPLIES:
		return (Value)(!va || vb); break;
	case IFF:
		return (Value)(va == vb); break;
	}
}

void Logical::converse() {
	assert(_type == IMPLIES);
	std::swap(_a, _b);
}

void Logical::contrapositive() {
	converse();
	_a->negate();
	_b->negate();
}

void Logical::expandIff() {
	assert(_type == IFF);
	_type = AND;
	Logical* fwd = new Logical(IMPLIES, _a, _b);
	// TODO: deep copy
	Logical* bwd = new Logical(*fwd);
	_a = fwd;
	_b = bwd;
	bwd->converse();
}

void Logical::negate() {
	switch (_type) {
	case AND:
		_type = OR;
		_a->negate();
		_b->negate();
		break;
	case OR:
		_type = AND;
		_a->negate();
		_b->negate();
		break;
	case IMPLIES:
		_type = AND;
		_b->negate();
		break;
	case IFF:
		expandIff();
		negate();
		break;
	}
}

Sentence::Value Relation::value() const {
	return Sentence::FALSE;
}

Object* Relation::expandSubset() {
	assert(_type == SUBSET);
	Symbol *x = new Symbol('x');
	return new Quantified(
		Quantified::FORALL,
		x,
		_a->clone(),
		new Relation(Relation::IN, x->cloneSelf(), _b->clone())
	);
}

void Relation::negate() {
	switch (_type) {
	case SUBSET:
		expandSubset();
		negate();
	default:
		Sentence::negate();
	}
}

Sentence::Value Quantified::value() const {
	return _body->value();
}

void Quantified::negate() {
	_type = (Type)!_type;
	_body->negate();
}

// =============================================================================
//            Provers
// =============================================================================

TheoremProver::~TheoremProver() {
	if (_theorem != nullptr) {
		delete _theorem;
	}
}

void TheoremProver::dispatch(const std::vector<std::string>& tokens) {
	std::string cmd = tokens[0];
	if (cmd == "thm") {
		if (_theorem == nullptr) {
			_theorem = Sentence::parse(tokens);
		} else {
			std::cout << _theorem;
		}
	} else if (cmd == "stack") {
		// print stack
	} else if (cmd == "print") {
		// print English proof
	} else {
		// stack.top().dispatch(tokens);
	}
}

// =============================================================================
//            Parsing
// =============================================================================

Sentence* Sentence::parse(const std::vector<std::string>& tokens) {
	bool first = true;
	for (const string& tok: tokens) {
		if (first) {
			first = false;
			continue;
		}
	}
}

// Returns a vector of string tokens by splitting on whitespace.
// TODO: Make this more efficient.
std::vector<std::string> tokenize(char* line) {
	std::vector<std::string> tokens;
	char c;
	std::string s;
	while ((c = *line++)) {
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
			if (!s.empty()) {
				tokens.push_back(s);
				s.clear();
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
//            Main
// =============================================================================

// Executes the interactive proof assistant loop. Uses the GNU Readline library
// for user input.
int lmain() {
	char* line;
	TheoremProver tp;
	std::cout << strings::header << std::endl;
	for (;;) {
		line = readline(strings::prompt);
		if (line == nullptr) {
			std::cout << std::endl;
			break;
		}
		std::vector<std::string> tokens = tokenize(line);
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

int main() {
	Object* obj = new CompoundNumber(
		CompoundNumber::MUL,
		new CompoundNumber(
			CompoundNumber::ADD,
			new ConcreteNumber(1),
			new ConcreteNumber(1)
		),
		new CompoundNumber(
			CompoundNumber::DIV,
			new ConcreteNumber(1),
			new ConcreteNumber(1)
		)
	);
	std::vector<Object*> v;
	v.push_back(obj->clone());
	v.push_back(new ConcreteNumber(6));
	v.push_back(new ConcreteNumber(3));
	Object *s = new ConcreteSet(v);
	std::cout << *obj << std::endl;
	// obj2->_type = CompoundNumber::SUB;
	std::cout << *s << std::endl;
	delete obj;
	delete s;
	return 0;
}
