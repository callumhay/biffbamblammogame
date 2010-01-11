#ifndef __TREE_H__
#define __TREE_H__

#include <vector>

template<class T> class Node {
public:
	Node() {}
	Node(T value) : value(value) {}
	~Node() {
		this->RemoveAllChildren();
	}

	size_t GetNumChildren() const { return this->children.size(); }
	Node<T>* GetChildAt(size_t i) const { return this->children[i]; }
	T GetValue() const { return this->value; }

	void RemoveAllChildren() {
		for (std::vector<Node<T>*>::iterator iter = this->children.begin(); iter != this->children.end(); ++iter) {
			Node<T>* currNode = *iter;
			delete currNode;
			currNode = NULL;
		}
	}
	void AddChild(const Node<T>& child) {
		this->children.push_back(child);
	}

private:
	T value;
	std::vector<Node<T>*> children; 
};

template<class T> class Tree {
public:
	Tree() : root(NULL) {}
	~Tree() {
		this->SetRoot(NULL);
	}

	void SetRoot(Node<T>* root) { 
		if (this->root != NULL) {
			delete this->root;
		}
		this->root = root; 
	}

	Node<T>* GetRoot() const { return this->root; }

private:
	Node<T>* root;

	// Disallow copying and assigning this class
	Tree(const Tree& t);
	Tree operator=(const Tree& t);
};

#endif