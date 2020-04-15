#pragma once

#include <vector>

using std::vector;

template <class T>
class NodeT {
public:
	T data;
	NodeT<T>* parent;
	NodeT<T>* left;
	NodeT<T>* right;
	bool isBlack;

	NodeT(T value)
	: data(value), parent(nullptr), left(nullptr), right(nullptr), isBlack(false)
	{ }
};

template <class T>
class RedBlackTree {
public:
	// constructors, operators and destructors
	RedBlackTree(); // empty tree, root is null
	RedBlackTree(const RedBlackTree &); // deep copy
	RedBlackTree & operator=(const RedBlackTree &); // deep copy, overloaded assignment operator
	~RedBlackTree(); // deallocate nodes

	// mutators
	bool insert(T); // insert data if tree does not contain parameter, no duplicates
	bool remove(T); // remove parameter and return true, if parameter is not in tree, return false

	// accessors
	bool search(T) const; // return true if parameter is found
	vector<T> search(T, T) const; // return vector containing elements in tree from a to b, ascending order
	vector<T> dump() const; // return vector with all contents of tree
	int size() const; // return number of nodes in tree
	NodeT<T>* getRoot() const; // return pointer to tree's root
	

private:
	NodeT<T>* root;
	int currentSize;


	// helpers

	// copy data into tree
	void deepCopy(const RedBlackTree & r)
	{
		if (r.root != nullptr) {
			clearTree(root); // empty original tree
			root = copyHelper(r.root);
		} else {
			root = nullptr;
		}
		currentSize = r.currentSize;
	}

	// pre order traversal and copy nodes
	NodeT<T>* copyHelper(NodeT<T>* node)
	{
		if (node == nullptr) return nullptr;
		
		NodeT<T>* newNode = new NodeT<T>(node->data);
		newNode->left = copyHelper(node->left);
		if (newNode->left != nullptr) newNode->left->parent = newNode;
		newNode->right = copyHelper(node->right);
		if (newNode->right != nullptr) newNode->right->parent = newNode;

		return newNode;
	}

	// post order traversal and delete nodes
	// POST: deallocate memory in tree
	void clearTree(NodeT<T>* node)
	{
		if (node != nullptr) {
			clearTree(node->left);
			clearTree(node->right);
			delete node;
		} 
	}

	// PARAM: value to insert in tree
	// POST: return pointer to inserted node
	NodeT<T>* btInsert(T value)
	{
		NodeT<T>* pos = root;
		NodeT<T>* newNode = new NodeT<T>(value);
		NodeT<T>* tempParent = root;
		bool isLeft = true;
		if (root == nullptr) {
			root = newNode; // set root on empty tree
			return root;
		} else {
			while (pos != nullptr) { // tree traversal
				tempParent = pos;
				if (value < tempParent->data) {
					pos = tempParent->left;
					isLeft = true;
				} else {
					pos = tempParent->right;
					isLeft = false;
				}
			} 

			// leaf found and insertion
			if (isLeft) {
				tempParent->left = newNode;
				newNode->parent = tempParent;
			} else {
				tempParent->right = newNode;
				newNode->parent = tempParent;
			}
			return newNode;
		}
	}

	// PARAM: node to be rotated left
	void leftRotate(NodeT<T>* node) 
	{
		NodeT<T>* temp = node->right;
		node->right = temp->left;

		// node's parent references temp's left child
		if (temp->left != nullptr) {
			temp->left->parent = node;
		}
		temp->parent = node->parent;

		// set child of node's parent
		if (node->parent == nullptr) { // node is root
			root = temp;
		} else if (node == node->parent->left) { // left child
			node->parent->left = temp;
		} else { // right child
			node->parent->right = temp;
		}

		// node become temp's left child
		temp->left = node;
		node->parent = temp;
	}


	// symmetry of leftRotate()
	// PARAM: node to be rotated right
	void rightRotate(NodeT<T>* node)
	{
		NodeT<T>* temp = node->left;
		node->left = temp->right;

		if (temp->right != nullptr) {
			temp->right->parent = node;
		}
		temp->parent = node->parent;

		if (node->parent == nullptr) {
			root = temp;
		} else if (node == node->parent->right) {
			node->parent->right = temp;
		} else {
			node->parent->left = temp;
		}

		temp->right = node;
		node->parent = temp;
	}

	// PRE: target contains successor
	// PARAM: target - node to find successor of
	//		  sParent - parent of successor
	// POST: return successor of target, set parent
	NodeT<T>* getSuccessor(NodeT<T>* target, NodeT<T>* & sParent) const
	{
		// successor is the left-most node of right subtree
		NodeT<T>* temp = target->right;
		sParent = target;

		while (temp->left != nullptr) {
			sParent = temp;
			temp = temp->left;
		}
		return temp;
	}

	// PRE: value is in red black tree
	// PARAM: value - value to find in tree
	//		  node - node to start searching on, usually root
	// POST: return node containing desired value
	NodeT<T>* findNode(T value, NodeT<T>* node) const
	{
		NodeT<T>* temp = node;

		// find node based on value
		while (temp != nullptr) {
			if (value == temp->data) {
				return temp;
			} else if (value > temp->data) { // access right child
				temp = temp->right;
			} else { // left child
				temp = temp->left;
			}
		}
	}

	// PARAM: node to fix
	// POST: fixes tree to be complete, correct black height of tree
	void fix(NodeT<T>* node)
	{
		NodeT<T>* sibling = nullptr;

		while (node != root && node->isBlack) {
			if (node == node->parent->left) { // node is left child
				NodeT<T>* sibling = node->parent->right; 
				if (sibling != nullptr && !sibling->isBlack) { // set node's sibling to black
					sibling->isBlack = true;
					node->parent->isBlack = false;
					leftRotate(node->parent);
					sibling = node->parent->right;
				}

				// make sibling's subtree the same black height, sibling is now red
				if (sibling->left->isBlack && sibling->right->isBlack) {
					sibling->isBlack = false;
					node = node->parent;
				} else {
					// make sibling's right child red
					if (sibling->right->isBlack) {
						sibling->left->isBlack = true;
						sibling->isBlack = false;
						rightRotate(sibling);
						sibling = node->parent->right;
					}
					sibling->isBlack = node->parent->isBlack;
					node->parent->isBlack = true;
					sibling->right->isBlack = true;
					leftRotate(node->parent);
					node = root;
					// fixed
				}
			} else { // symmetric to if, node is right child
				NodeT<T>* sibling = node->parent->left;
				if (sibling != nullptr && !sibling->isBlack) {
					sibling->isBlack = true;
					node->parent->isBlack = false;
					rightRotate(node->parent);
					sibling = node->parent->left;
				}

				if (sibling->right->isBlack && sibling->left->isBlack) {
					sibling->isBlack = false;
					node = node->parent;
				} else {
					if (sibling->left->isBlack) {
						sibling->right->isBlack = true;
						sibling->isBlack = false;
						leftRotate(sibling);
						sibling = node->parent->left;
					} 
					sibling->isBlack = node->parent->isBlack;
					node->parent->isBlack = true;
					sibling->left->isBlack = true;
					rightRotate(node->parent);
					node = root;
				}
			} 
		}
		// root is black
		node->isBlack = true;
	}

	// PRE: start < end
	// PRAM:	start - begining of interval to check
	//			end - end of interval to check
	//			node - node to check
	//			result - vector containing correct values
	// POST: add values between start and end (inclusive) to result
	void searchVectorHelper(int start, int end, NodeT<T>* node, vector<T> & result) const
	{
		if (node != nullptr) {
			searchVectorHelper(start, end, node->left, result);
			if (node->data >= start && node->data <= end) {
				result.push_back(node->data);
			}
			searchVectorHelper(start, end, node->right, result);
		}
	}

	// PARAM: result is vector contianing data in tree
	// POST: add all data into result
	void dumpHelper(NodeT<T>* node, vector<T> & result) const
	{
		// inorder traversal
		if (node != nullptr) {
			dumpHelper(node->left, result);
			result.push_back(node->data);
			dumpHelper(node->right, result);
		}
	}
};

// method implementation

// default constructor
template <class T>
RedBlackTree<T>::RedBlackTree()
: root(nullptr), currentSize(0)
{ }

// copy constructor
template <class T>
RedBlackTree<T>::RedBlackTree(const RedBlackTree & r)
: root(nullptr), currentSize(0)
{
	deepCopy(r);
}

template <class T>
RedBlackTree<T> & RedBlackTree<T>::operator=(const RedBlackTree & r)
{
	if (this != &r) { // don't copy self
		deepCopy(r);
	}
	return *this;
}

// destructor
template <class T>
RedBlackTree<T>::~RedBlackTree()
{
	clearTree(root);
}

// PARAM: check if parameter is already in array
// return true of parameter is not in array and insert parameter
template <class T>
bool RedBlackTree<T>::insert(T value)
{
	if (!search(value)) {
		NodeT<T>* newNode = btInsert(value); // regular binary tree insertion
		currentSize++;

		// redblack balancing
		while (newNode->parent != root && newNode != root && !newNode->parent->isBlack) {
			if (newNode->parent == newNode->parent->parent->left) { // parent of newNode is a left child
				NodeT<T>* uncle = newNode->parent->parent->right;
				if (uncle != nullptr && !uncle->isBlack) { // same as parent
					newNode->parent->isBlack = true;
					uncle->isBlack = true;
					newNode->parent->parent->isBlack = false;
					newNode = newNode->parent->parent;
				} else { // uncle colour is black
					if (newNode == newNode->parent->right) {
						newNode = newNode->parent;
						leftRotate(newNode);
					}
					newNode->parent->isBlack = true;
					newNode->parent->parent->isBlack = false;
					rightRotate(newNode->parent->parent);
				}
			} else { // symmetric to if
				NodeT<T>* uncle = newNode->parent->parent->left;
				if (uncle != nullptr && !uncle->isBlack) {
					newNode->parent->isBlack = true;
					uncle->isBlack = true;
					newNode->parent->parent->isBlack = false;
					newNode = newNode->parent->parent;
				} else {
					if (newNode == newNode->parent->left) {
						newNode = newNode->parent;
						rightRotate(newNode);
					}
					newNode->parent->isBlack = true;
					newNode->parent->parent->isBlack = false;
					leftRotate(newNode->parent->parent);
				}
			}
		}
		root->isBlack = true;

		return true;
	}

	return false;
} 

// PARAM: value to remove
// POST: return true if value is removed, otherwise false
template <class T>
bool RedBlackTree<T>::remove(T value)
{
	if (search(value)) {
		NodeT<T>* target = findNode(value, root); // get pointer to node of interest
		NodeT<T>* sParent = nullptr;
		NodeT<T>* toRemove = nullptr;
		if (target->left == nullptr || target->right == nullptr) { // no children
			toRemove = target; // will delete toRemove pointer
		} else { // 2 children
			// find successor (left-most node of right subtree)
			toRemove = getSuccessor(target, sParent);
		}

		// find if toRemove's only child is right or left
		NodeT<T>* child = nullptr;
		if (toRemove->left != nullptr) {
			child = toRemove->left;
		} else {
			child = toRemove->right;
		}
		if (child != nullptr) child->parent = toRemove->parent; // detach child from toRemove

		if (toRemove->parent == nullptr) { // child is new root
			root = child;
		} else {
			if (toRemove == toRemove->parent->left) {
				toRemove->parent->left = child;
			} else {
				toRemove->parent->right = child;
			}
		}
		if (toRemove != target) { // toRemove has been moved up
			target->data = toRemove->data;
		}

		if (toRemove->isBlack && child != nullptr)
			fix(child);

		delete toRemove;
		currentSize--;

		return true;
	}

	return false;
}

// PARAM: value to search for
// POST: return true if value is found
template <class T>
bool RedBlackTree<T>::search(T value) const
{
	NodeT<T>* temp = root;
	while (temp != nullptr) {
		if (value == temp->data) {
			return true; // match found
		} else if (value > temp->data) { // access right child
			temp = temp->right;
		} else { // left child
			temp = temp->left;
		}
	}

	return false;
}

// PRE: start <= end
// PARAM: values to return inbetween two parameters (inclusive)
// POST: return vector containing values between parameters in ascending order, empty if none are found
template <class T>
vector<T> RedBlackTree<T>::search(T start, T end) const
{
	vector<T> result;

	searchVectorHelper(start, end, root, result);

	return result;
}

// POST: return vector containing all elements of tree in ascending order
//		 vector is empty if tree is empty
template <class T>
vector<T> RedBlackTree<T>::dump() const
{
	vector<T> result;

	dumpHelper(root, result);

	return result;
}

// POST: return current number of nodes
template <class T>
int RedBlackTree<T>::size() const { return currentSize; }

// POST: return root node of tree
template <class T>
NodeT<T>* RedBlackTree<T>::getRoot() const { return root; }