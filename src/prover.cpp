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

// A theorem prover node is a node in the binary tree that decomposes a theorem.
// Each node specifies a goal; this goal is considerd proven when all its
// subgoals (goals of children nodes) are proven. Each node has a list of
// givens, which are facts it can use in the proof of its goal. This may begin
// as an empty list or a singleton list, and typically more givens are added as
// they are deduced from existing ones. The total givens of a node consist of
// its own list in addition to all givens of nodes in the chain from the node to
// the root of the tree. If a node has no children (a leaf node), then it must
// be proven directly.
class TheoremProver::Node {
public:
	// Creates a new node with the supplied goal, and optionally starting with
	// one given (otherwise it will have no givens).
	Node(Sentence* goal, Sentence* given = nullptr);

	// Deletes this node and all nodes below it. Also deletes the goal and all
	// the givens of the node.
	~Node();

	// Accessors for the given, goal, and children of the node.
	Node* primaryChild() const { return _a; }
	Node* secodaryChild() const { return _b; }

	// Assumes this is a leaf node. Adds one or two children to the node.
	void extend(Node* a, Node* b = nullptr);

	// Adds a given to the node.
	void deduce(Sentence* g);

	// Returns true if this node has any givens.
	bool hasGivens() const;

	// Prints the goal or the givens of the node to stdout, optionally including
	// the node label as well.
	void printGoal(bool label) const;
	void printGivens(bool label) const;

	// Pretty-prints the tree to stdout.
	void printTree() const;

private:
	// Prints a section of the ASCII tree, and outputs the legend information of
	// this node to the given stream.
	void printHelp(int indent, std::ostream& legend) const;

	// Calculates the maximum depth of this tree.
	int maxDepth() const;

	Sentence* _goal; // the current goal
	std::vector<Sentence*> _givens; // the givens deduced at this node
	Node* _a; // the primary child, or null
	Node* _b; // the secondary child, or null
	char _label; // used for printing
};

namespace {
	char currentLabel = 'A';
	char genUniqueLabel() { return currentLabel++; }
}

TheoremProver::Node::Node(Sentence* goal, Sentence* given)
		: _goal(goal), _a(nullptr), _b(nullptr),
		_label(genUniqueLabel()) {
	assert(goal != nullptr);
	if (given != nullptr) {
		deduce(given);
	}
}

TheoremProver::Node::~Node() {
	for (Sentence* g: _givens) {
		delete g;
	}
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

void TheoremProver::Node::deduce(Sentence* g) {
	assert(g != nullptr);
	_givens.push_back(g);
}

bool TheoremProver::Node::hasGivens() const {
	return !_givens.empty();
}

void TheoremProver::Node::printGoal(bool label) const {
	assert(_goal != nullptr);
	if (label) {
		std::cout << '[' << _label << "] ";
	}
	std::cout << *_goal << std::endl;
}

void TheoremProver::Node::printGivens(bool label) const {
	if (hasGivens()) {
		if (label) {
			std::cout << '[' << _label << "] ";
		}
		bool first = true;
		for (Sentence *g: _givens) {
			if (label) {
				if (first) {
					first = false;
				} else {
					std::cout << "    ";
				}
			}
			std::cout << *g << std::endl;
		}
	}
}

void TheoremProver::Node::printTree() const {
	int depth = maxDepth();
	// Compute the indent level for the first row. By inspection I discovered
	// the pattern to be 2^(n-2) spaces followed by 2^(n-2)-1 underscores, where
	// n is the maximum depth of the tree.
	int indent = 0;
	if (depth > 1) {
		indent = 1;
		for (int i = 0; i < depth - 2; ++i) {
			indent *= 2;
		}
	}
	// We print the legend information to a separate stream (which gets printed
	// at the end) to avoid doing two traversals.
	std::ostringstream legend;
	// Do a breadth-first traversal.
	std::queue<const Node*> queue;
	queue.push(this);
	while (!queue.empty()) {
		auto sz = queue.size();
		for (unsigned int i = 0; i < sz; ++i) {
			const Node* n = queue.front();
			queue.pop();
			n->printHelp(indent, legend);
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

void TheoremProver::Node::printHelp(int indent, std::ostream& legend) const {
	unsigned int ind = static_cast<unsigned int>(indent);
	std::string spaces(ind, ' ');
	std::string scores(std::max(0u, ind), '_');
	std::cout << spaces << scores << _label << scores << spaces;
	legend << '[' << _label << "] " << *_goal << std::endl;
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
	// This is why we are using vectors instead of stacks (clear method).
	_dfs.clear();
	_lineage.clear();
	if (s == nullptr) {
		_root = nullptr;
	} else {
		_root = new Node(s);
		_dfs.push_back(_root);
		_lineage.push_back(_root);
	}
}

bool TheoremProver::hasTheorem() const {
	return _root != nullptr;
}

bool TheoremProver::advance() const {
}

void TheoremProver::printTheorem() const {
	if (hasTheorem()) {
		_root->printGoal(false);
	}
}

void TheoremProver::printGivens() const {
	bool empty = true;
	for (const Node* n: _lineage) {
		empty = empty || n->hasGivens();
		n->printGivens(true);
	}
	if (empty) {
		std::cout << "(no givens)" << std::endl;
	}
}

void TheoremProver::printGoal() const {
	if (hasTheorem()) {
		currentNode()->printGoal(true);
	}
}

void TheoremProver::printTree() const {
	if (hasTheorem()) {
		_root->printTree();
	}
}

TheoremProver::Node* TheoremProver::currentNode() const {
	assert(_root != nullptr);
	return _dfs.back();
}
