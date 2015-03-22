// Copyright 2015 Mitchell Kember. Subject to the MIT License.

#include "prover.hpp"

#include "sentence.hpp"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <queue>
#include <sstream>
#include <string>

#include <cassert>

// =============================================================================
//            Fancy printing
// =============================================================================

// Begins printing things in red.
static void startRed(std::ostream& s) {
	s << "\x1b[31m";
}

// Stops printing things in red.
static void stopRed(std::ostream& s) {
	s << "\x1b[0m";
}

// Prints a string underlined with newlines before and after.
static void printUnderlined(const char* str) {
	std::cout << "\n\x1b[4m" << str << "\x1b[0m\n";
}

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

	// Accessors for the goal and the children of the node.
	Sentence* goal() const { return _goal; }
	Node* primaryChild() const { return _a; }
	Node* secondaryChild() const { return _b; }

	// Assumes this is a leaf node. Adds one or two children to the node via a
	// decomp object.
	void decompose(Decomp d);

	// Adds a given to the node.
	void deduce(Sentence* g);

	// Returns true if this node has any givens.
	bool hasGivens() const;

	// Prints the goal or the givens of the node to stdout, optionally including
	// the node label as well (in a different colour).
	void printGoal(bool label) const;
	void printGivens(bool label) const;

	// Pretty-prints the tree to stdout. If the current node is provided, it
	// will be made distinct by printing in a different colour.
	void printTree(Node* current = nullptr) const;

private:
	// Prints a section of the ASCII tree, and outputs the legend information of
	// this node to the given stream. Uses a different colour is col is true.
	void printHelp(int indent, std::ostream& legend, bool col) const;

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

void TheoremProver::Node::decompose(Decomp d) {
	assert(_a == nullptr);
	assert(_b == nullptr);
	_a = new Node(d._goalA, d._givenA);
	if (d._goalB != nullptr) {
		_b = new Node(d._goalB, d._givenB);
	}
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
		startRed(std::cout);
		std::cout << '[' << _label << ']';
		stopRed(std::cout);
		std::cout << ' ';
	}
	std::cout << *_goal << '\n';
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
			std::cout << *g << '\n';
		}
	}
}

void TheoremProver::Node::printTree(Node* current) const {
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
		bool allNull = true;
		auto sz = queue.size();
		std::stringstream slashes;
		for (unsigned int i = 0; i < sz; ++i) {
			const Node* n = queue.front();
			queue.pop();
			std::string spaces1(
				static_cast<unsigned int>(std::max(0, indent - 1)),
				' '
			);
			std::string spaces2(
				static_cast<unsigned int>(std::max(0, indent * 2 - 1)),
				' '
			);
			char leftp = ' ';
			char rightp = ' ';
			if (n != nullptr) {
				if (n->_a != nullptr) leftp = '/';
				if (n->_b != nullptr) rightp = '\\';
			}
			slashes << spaces1 << leftp << spaces2 << rightp << spaces1;
			if (n == nullptr) {
				std::cout << std::string(
					static_cast<unsigned int>(1 + std::max(0, 4 * indent - 2)),
					' '
				);
				// Push nulls to ensure correct spacing.
				// (This is why we need to break on allNull.)
				queue.push(nullptr);
				queue.push(nullptr);
			} else {
				n->printHelp(indent, legend, n == current);
				queue.push(n->_a);
				queue.push(n->_b);
				if (n->_a != nullptr || n->_b != nullptr) {
					allNull = false;
				}
			}
			if (i != sz - 1) {
				std::cout << ' ';
				slashes << ' ';
			}
		}
		indent /= 2;
		std::cout << '\n';
		if (allNull) {
			break;
		} else {
			std::cout << slashes.str() << '\n';
		}
	}
	std::cout << '\n' << legend.str();
}

void TheoremProver::Node::printHelp(int indent, std::ostream& legend, bool col)
		const {
	unsigned int n_sp = static_cast<unsigned int>(indent);
	unsigned int n_us = static_cast<unsigned int>(std::max(0, indent - 1));
	std::string spaces(n_sp, ' ');
	std::string scoresl(n_us, (_a == nullptr) ? ' ' : '_');
	std::string scoresr(n_us, (_b == nullptr) ? ' ' : '_');
	std::cout << spaces << scoresl;
	if (col) {
		startRed(std::cout);
		startRed(legend);
	}
	std::cout << _label;
	legend << '[' << _label << ']';
	if (col) {
		stopRed(std::cout);
		stopRed(legend);
	}
	std::cout << scoresr << spaces;
	legend << ' ' << *_goal << '\n';
}

int TheoremProver::Node::maxDepth() const {
	int da = (_a == nullptr) ? 0 : _a->maxDepth();
	int db = (_b == nullptr) ? 0 : _b->maxDepth();
	return 1 + std::max(da, db);
}

// =============================================================================
//            Index parsing
// =============================================================================

namespace {
	const char* bad_index = "Invalid index.\n";
}

// Prompts the user to enter an integer between lo and hi (inclusive). Prompts
// repeatedly until valid input is parsed. Returns the integer.
static int readIndex(int lo, int hi) {
	int option;
	for(;;) {
		std::cout << "Enter the option index: ";
		std::string line;

		// Make sure Ctrl-D is handled properly.
		if (!getline(std::cin, line)) {
			std::cout << '\n';
			exit(1);
		}

		// Try again if anything goes wrong.
		try {
			option = std::stoi(line);
		} catch (const std::invalid_argument& e) {
			(void)e;
			std::cerr << bad_index;
			continue;
		} catch (const std::out_of_range& e) {
			(void)e;
			std::cerr << bad_index;
			continue;
		}
		if (option < lo || option > hi) {
			std::cerr << bad_index;
			continue;
		}
		break;
	}
	return option;
}

// =============================================================================
//            Theorem prover
// =============================================================================

TheoremProver::TheoremProver() : _root(nullptr) {}

TheoremProver::~TheoremProver() {
	delete _root;
}

void TheoremProver::cleanUp() {
	// This is why we are using vectors instead of stacks (clear method).
	_dfs.clear();
	_lineage.clear();
}

void TheoremProver::setTheorem(Sentence* s) {
	delete _root;
	cleanUp();
	if (s == nullptr) {
		_root = nullptr;
	} else {
		_root = new Node(s);
		_dfs.push_back(_root);
		_lineage.push_back(_root);
		printGoal();
	}
}

TheoremProver::Mode TheoremProver::mode() const {
	if (_root == nullptr) return NOTHM;
	if (_dfs.empty()) return DONE;
	return PROVING;
}

void TheoremProver::decompose() {
	assert(mode() == PROVING);
	std::vector<Decomp> vec = currentNode()->goal()->decompose();

	if (vec.empty()) {
		std::cout << "This goal cannot be decomposed.\n";
		return;
	}

	// Print the indexed options.
	std::cout << "Choose a decomposition option.\n";
	std::cout << "(0) abort\n";
	int i = 1;
	for (const Decomp& d: vec) {
		std::cout << '(' << i++ << ") ";
		d.print();
		std::cout << '\n';
	}

	// FIXME: The following is pretty wasteful. It is only necessary to do it
	// this way for deduction, since each hypothesis needs to be checked and
	// each conclusion needs to be displayed.

	// Get the index, and delete the unused decomps.
	int option = readIndex(0, static_cast<int>(vec.size()));
	for (int j = 0; j < static_cast<int>(vec.size()); j++) {
		if (j != option - 1) {
			vec[static_cast<size_t>(j)].free();
		}
	}
	if (option == 0) {
		std::cout << "Decomposition aborted.\n";
		return;
	}

	// Add it to the tree.
	Node* n = currentNode();
	n->decompose(vec[static_cast<size_t>(option - 1)]);
	_dfs.pop_back();
	if (n->secondaryChild() != nullptr) {
		_dfs.push_back(n->secondaryChild());
	}
	_dfs.push_back(n->primaryChild());
	_lineage.push_back(n->primaryChild());
	std::cout << "New goal: ";
	printGoal();
}

void TheoremProver::deduce() {
	assert(mode() == PROVING);
	// list possible deductions
	// choose one, or all
	// or add your own, wiht justification/trivial?
}

void TheoremProver::trivial() {
	assert(mode() == PROVING);
	_dfs.pop_back();
	if (mode() == DONE) {
		std::cout << "Proof completed!\n";
		cleanUp();
	} else {
		updateLineage();
		std::cout << "Goal proved.\nNew goal: ";
		printGoal();
	}
}

void TheoremProver::justify() {
	assert(mode() == PROVING);
	_dfs.pop_back();
	// TODO: Implement me!
}

void TheoremProver::printStatus() const {
	Mode m = mode();
	assert(m != NOTHM);
	printUnderlined("THEOREM");
	printTheorem();
	if (m == PROVING) {
		printUnderlined("CURRENT GOAL");
		printGoal();
		printUnderlined("GIVENS");
		printGivens();
		std::cout << '\n' << _dfs.size() << " goal(s) left to prove.\n\n";
	} else if (m == DONE) {
		std::cout << "\nThe proof is complete.\n\n";
	}
}

void TheoremProver::printTheorem() const {
	assert(mode() != NOTHM);
	_root->printGoal(false);
}

void TheoremProver::printTree() const {
	Mode m = mode();
	assert(m != NOTHM);
	std::cout << '\n';
	if (m == PROVING) {
		_root->printTree(currentNode());
	} else {
		_root->printTree();
	}
	std::cout << '\n';
}

void TheoremProver::printGoal() const {
	assert(mode() == PROVING);
	currentNode()->printGoal(true);
}

void TheoremProver::printGivens() const {
	assert(mode() == PROVING);
	bool empty = true;
	for (const Node* n: _lineage) {
		empty = empty && !n->hasGivens();
		n->printGivens(true);
	}
	if (empty) {
		std::cout << "(no givens)\n";
	}
}

TheoremProver::Node* TheoremProver::currentNode() const {
	assert(mode() == PROVING);
	return _dfs.back();
}

void TheoremProver::updateLineage() {
	Node* c = currentNode();
	while (!_lineage.empty()
			&& _lineage.back()->primaryChild() != c
			&& _lineage.back()->secondaryChild() != c) {
		_lineage.pop_back();
	}
	_lineage.push_back(c);
}
