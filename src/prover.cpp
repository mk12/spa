// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "prover.hpp"

#include "sentence.hpp"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <queue>
#include <sstream>

#include <cassert>

// =============================================================================
//            Node
// =============================================================================

// A node is a given/goal pair in the binary tree that decomposes a theorem.
// Each node specifies a goal; this goal is considered proven when all its
// subgoals (goals of the children nodes) are proven. Each node can use its
// given (treat it as a known fact) when proving its goal; it can also use any
// given in the chain of nodes that leads up to the root node. The given may be
// null if the node provides no additional givens, but its goal cannot be null.
// If node has only one child, then it depends only on the proof of one subgoal.
// If it has no children (a leaf node), then it must be proven directly.
class TheoremProver::Node {
public:
	// Creates a new node with the specified given and goal. Children default to
	// null if they are not specified.
	Node(Sentence* given, Sentence* goal, Node* a = nullptr, Node* b = nullptr);

	// Deletes this node and all nodes below it.
	~Node();

	// Accessors for the given, goal, and children of the node.
	Sentence* given() const { return _given; }
	Sentence* goal() const { return _goal; }
	Node* primaryChild() const { return _a; }
	Node* secodaryChild() const { return _b; }

	// Assumes this is a leaf node. Adds one or two children to the node.
	void extend(Node* a, Node* b = nullptr);

	// Pretty-prints the tree to stdout.
	void print() const;

private:
	// Prints a section of the ASCII tree, and outputs the legend information of
	// this node to the given stream.
	void printHelp(char label, unsigned int indent, std::ostream& legend) const;

	// Calculates the maximum depth of this tree.
	int maxDepth() const;

	Sentence* _given; // the new given, or null
	Sentence* _goal; // the current goal
	Node* _a; // the primary child, or null
	Node* _b; // the secondary child, or null
};

TheoremProver::Node::Node(Sentence* given, Sentence* goal, Node* a, Node* b)
	: _given(given), _goal(goal), _a(a), _b(b) {
	assert(goal != nullptr);
}

TheoremProver::Node::~Node() {
	delete _given;
	delete _goal;
	delete _a;
	delete _b;
}

void TheoremProver::Node::extend(Node* a, Node* b) {
	assert(_a == nullptr);
	assert(_b == nullptr);
	_a = a;
	_b = b;
}

void TheoremProver::Node::print() const {
	int depth = maxDepth();
	// Compute the indent level for the first row. By inspection I discovered
	// the pattern to be 2^(n-2) spaces followed by 2^(n-2)-1 underscores, where
	// n is the maximum depth of the tree.
	unsigned int indent = 0;
	if (depth > 1) {
		indent = 1;
		for (int i = 0; i < depth - 2; ++i) {
			indent *= 2;
		}
	}
	// We print the legend information to a separate stream (which gets printed
	// at the end) to avoid doing two traversals.
	std::ostringstream legend;
	// Each node has an alphabetical label in the tree.
	char label = 'A';
	// Do a breadth-first traversal.
	std::queue<const Node*> queue;
	queue.push(this);
	while (!queue.empty()) {
		auto sz = queue.size();
		for (unsigned int i = 0; i < sz; ++i) {
			const Node* n = queue.front();
			queue.pop();
			n->printHelp(label, indent, legend);
			++label;
			if (n->_a != nullptr) {
				queue.push(n->_a);
			}
			if (n->_b != nullptr) {
				queue.push(n->_b);
			}
		}
		indent /= 2;
		std::cout << std::endl;
	}
	std::cout << std::endl << legend.str();
}

void TheoremProver::Node::printHelp(
		char label, unsigned int indent, std::ostream& legend) const {
	std::string spaces(indent, ' ');
	std::string scores(std::max(0u, indent - 1), '_');
	std::cout << spaces << scores << label << scores << spaces;
	legend << '[' << label << "] " << *_goal << std::endl;
}

int TheoremProver::Node::maxDepth() const {
	int da = (_a == nullptr) ? 0 : _a->maxDepth();
	int db = (_b == nullptr) ? 0 : _b->maxDepth();
	return 1 + std::max(da, db);
}

// =============================================================================
//            Theorem prover
// =============================================================================

TheoremProver::TheoremProver() : _root(nullptr) {}

TheoremProver::~TheoremProver() {
	delete _root;
}

void TheoremProver::setTheorem(Sentence* s) {
	delete _root;
	_stack.clear();
	_givens.clear();
	_root = new Node(nullptr, s);
	_stack.push_back(_root);
}

bool TheoremProver::hasTheorem() const {
	return _root != nullptr;
}

void TheoremProver::printTheorem() const {
	if (hasTheorem()) {
		std::cout << *_root->goal() << std::endl;
	}
}

void TheoremProver::printGivens() const {
	int i = 1;
	for (const Sentence* given: _givens) {
		std::cout << '[' << i++ << "] " << *given << std::endl;
	}
}

void TheoremProver::printGoal() const {
	if (hasTheorem()) {
		std::cout << *_stack.back()->goal() << std::endl;
	}
}

void TheoremProver::printStack() const {
	if (hasTheorem()) {
		_root->print();
	}
}
