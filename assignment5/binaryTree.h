#ifndef BINARY_TREE
#define BINARY_TREE

#include <iostream>
#include "../queue.h"

using namespace std;

enum oType { PRE_ORDER, IN_ORDER, POST_ORDER};

#ifndef TREE_NODE
#define TREE_NODE

template <class iType>
struct treeNode
{
	treeNode* left;
	treeNode* right;

	iType info;
};

#endif

template <class iType>
class binaryTree
{
public:
	// add, delete, search
	binaryTree();
	~binaryTree();
	binaryTree(const binaryTree<iType>&);
	void operator=(const binaryTree<iType>&);
	void makeEmpty();
	bool isEmpty() const;
	bool isFull() const;
	int numberOfNodes() const;
	bool retrieveItem(iType&);
	void insertItem(iType);
	void deleteItem(iType);
	void resetTree(oType);
	bool getNextItem(iType&, oType);
	void printTree(ostream&) const;
private:
	treeNode<iType>* root;
	queue<iType> preQue;
	queue<iType> inQue;
	queue<iType> postQue;
};

template <class iType>
int countNodes(treeNode<iType>*);
template <class iType>
bool retrieve(treeNode<iType>*, iType&);
template <class iType>
void insert(treeNode<iType>*&, iType);
template <class iType>
void deleteOut(treeNode<iType>*&, iType);
template <class iType>
void deleteNode(treeNode<iType>*&);
template <class iType>
void getPredecessor(treeNode<iType>*, iType&);
template <class iType>
void print(treeNode<iType>*, ostream&);
template <class iType>
void destroy(treeNode<iType>*&);
template <class iType>
void copyTree(treeNode<iType>*&, treeNode<iType>*);
template <class iType>
void countNodes(treeNode<iType>*&);
template <class iType>
void preOrder(treeNode<iType>*&, queue<iType>&);
template <class iType>
void inOrder(treeNode<iType>*&, queue<iType>&);
template <class iType>
void postOrder(treeNode<iType>*&, queue<iType>&);

template <class iType>
binaryTree<iType>::binaryTree()
{
	root = NULL;
}

template <class iType>
binaryTree<iType>::~binaryTree()
{
	destroy(root);
}

template <class iType>
binaryTree<iType>::binaryTree( const binaryTree<iType>& originalTree )
{
	copyTree(root, originalTree.root);
}

template <class iType>
int binaryTree<iType>::numberOfNodes() const
{
	return countNodes(root);
}

template <class iType>
bool binaryTree<iType>::retrieveItem(iType& item)
{
	return retrieve(root, item);
}

template <class iType>
void binaryTree<iType>::insertItem(iType item)
{
	insert(root, item);
}

template <class iType>
void binaryTree<iType>::deleteItem(iType item)
{
	deleteOut(root, item);
}

template <class iType>
void binaryTree<iType>::printTree(ostream& out) const
{
	print(root, out);
}

template <class iType>
void binaryTree<iType>::resetTree( oType order )
{
	switch ( order )
	{
		case PRE_ORDER:
			while ( !preQue.empty() )
				preQue.pop();
			preOrder(root, preQue);
			break;
		case IN_ORDER:
			while ( !inQue.empty() )
				inQue.pop();
			inOrder(root, inQue);
			break;
		case POST_ORDER:
			while ( !postQue.empty() )
				postQue.pop();
			postOrder(root, postQue);
			break;
	}
}

template <class iType>
bool binaryTree<iType>::getNextItem( iType& item, oType order )
{
	switch ( order )
	{
		case PRE_ORDER:
			item = preQue.front();
			preQue.pop();
			return preQue.empty();
			break;
		case IN_ORDER:
			item = inQue.front();
			inQue.pop();
			return inQue.empty();
			break;
		case POST_ORDER:
			item = postQue.front();
			postQue.pop();
			return postQue.empty();
			break;
	}
}

template <class iType>
bool binaryTree<iType>::isEmpty() const
{
	return ( root == NULL );
}

template <class iType>
void preOrder(treeNode<iType>*& tree, queue<iType>& preQue)
{
	if ( tree != NULL )
	{
		preQue.push(tree->info);
		preOrder(tree->left, preQue);
		preOrder(tree->right, preQue);
	}
}

template <class iType>
void inOrder(treeNode<iType>*& tree, queue<iType>& inQue)
{
	if ( tree != NULL )
	{
		inOrder(tree->left, inQue);
		inQue.push(tree->info);
		inOrder(tree->right, inQue);
	}
}

template <class iType>
void postOrder(treeNode<iType>*& tree, queue<iType>& postQue)
{
	if ( tree != NULL )
	{
		postOrder(tree->left, postQue);
		postOrder(tree->right, postQue);
		postQue.push(tree->info);
	}
}

template <class iType>
int countNodes(treeNode<iType>* tree)
{
	if ( tree == NULL )
		return 0;
	else
		return countNodes(tree->left) + countNodes(tree->right) + 1;
}

template <class iType>
bool retrieve(treeNode<iType>* tree, iType& item)
{
	if ( tree == NULL )
		return false;
	else if (item < tree->info)
		return retrieve(tree->left, item);
	else if (item > tree->info)
		return retrieve(tree->right, item);
	else
	{
		// base case 1
		item = tree->info;
		return true;
	}
}

template <class iType>
void insert(treeNode<iType>*& tree, iType item)
{
	if ( tree == NULL )
	{
		tree = new treeNode<iType>;
		tree->right = NULL;
		tree->left = NULL;
		tree->info = item;
	}
	else if ( item < tree->info )
		insert(tree->left, item);
	else	// >= goes to the right
		insert(tree->right, item);
}

template <class iType>
void deleteOut(treeNode<iType>*& tree, iType item)
{
	if ( item < tree->info )
		deleteOut(tree->left, item);
	else if ( item > tree->info )
		deleteOut(tree->right, item);
	else
		deleteNode(tree);
}

template <class iType>
void deleteNode(treeNode<iType>*& tree)
{
	iType item;
	treeNode<iType>* tempPtr;

	tempPtr = tree;
	if ( tree->left == NULL )	// right child
	{
		tree = tree->right;
		delete tempPtr;
	}
	else if ( tree->right == NULL )	// left child
	{
		tree = tree->left;
		delete tempPtr;
	}
	else
	{
		getPredecessor(tree->left, item);
		tree->info = item;
		deleteOut(tree->left, item);
	}

}

template <class iType>
void getPredecessor(treeNode<iType>* tree, iType& item)
{
	while ( tree->right != NULL )
		tree = tree->right;
	item = tree->info;
}

template <class iType>
void print(treeNode<iType>* tree, ostream& out)
{
	if ( tree != NULL )
	{
		print(tree->left, out);
		out << tree->info << endl;
		print(tree->right, out);
	}
}

template <class iType>
void destroy(treeNode<iType>*& tree)
{
	if ( tree != NULL )
	{
		destroy(tree->left);
		destroy(tree->right);
		delete tree;
	}
}

template <class iType>
void copyTree(treeNode<iType>*& copy, treeNode<iType>* originalTree)
{
	if ( originalTree == NULL )
		copy = NULL;
	else
	{
		copy = new treeNode<iType>;
		copy->info = originalTree->info;
		copyTree(copy->left, originalTree->left);
		copyTree(copy->right, originalTree->right);
	}
}

#endif

