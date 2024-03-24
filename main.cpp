/*
	Reminder List

		1) Root -> always black
		2) All leaves -> black
		3) red node -> children are black
		4) every path from a node to its descendent NIL nodes -> same # of black nodes
		5) insertion, deletion -> invoke self balancing 
		6) left rotate, right rotate

		Comparison with AVL Trees: Both red-black trees and AVL trees are self-balancing binary search trees, 
									but they differ in their balancing criteria and operations. AVL trees are more rigidly balanced, 
									which makes them faster for lookup-intensive applications, 
									but potentially slower for insertions and deletions due to more frequent rebalances. 
									Red-black trees, on the other hand, provide a good compromise between the complexity of operations and balancing, 
									making them efficient for applications that require a balanced mix of insertions, deletions, and lookups.
*/


#include <iostream>
#include <vector>
#include <iterator>
using namespace std;


// class RedBlack implements the operations in Red Black Tree
template <class T>
class RedBlack 
{
private:

	struct Node
	{
		T data; // holds the key
		Node* parent; // pointer to the parent
		Node* left; // pointer to left child
		Node* right; // pointer to right child
		int color; // 1 -> Red, 0 -> Black
	};
	typedef Node* NodePtr;

	NodePtr root;
	NodePtr TNULL;


	void helper_pre_order(NodePtr node) 
	{
		if (node != TNULL) 
		{
			cout << node->data << " ";
			helper_pre_order(node->left);
			helper_pre_order(node->right);
		}
	}


	void helper_in_order(NodePtr node) 
	{
		if (node != TNULL) 
		{
			helper_in_order(node->left);
			cout << node->data << " ";
			helper_in_order(node->right);
		}
	}


	void helper_post_order(NodePtr node) 
	{
		if (node != TNULL) 
		{
			helper_post_order(node->left);
			helper_post_order(node->right);
			cout << node->data << " ";
		}
	}


	NodePtr helper_search_tree(NodePtr node, T key) 
	{
		if (node == TNULL || key == node->data) 
		{
			return node;
		}

		if (key < node->data) 
		{
			return helper_search_tree(node->left, key);
		}
		return helper_search_tree(node->right, key);
	}


	// fix the rb tree modified by the delete operation
	void helper_fix_delete(NodePtr x) 
	{
		NodePtr s;
		while (x != root && x->color == 0) 
		{
			if (x == x->parent->left) 
			{
				s = x->parent->right;
				if (s->color == 1) 
				{
					s->color = 0;
					x->parent->color = 1;
					rotate_left(x->parent);
					s = x->parent->right;
				}

				if (s->left->color == 0 && s->right->color == 0) 
				{
					s->color = 1;
					x = x->parent;
				}
				else 
				{
					if (s->right->color == 0) 
					{
						s->left->color = 0;
						s->color = 1;
						right_rotate(s);
						s = x->parent->right;
					}

					s->color = x->parent->color;
					x->parent->color = 0;
					s->right->color = 0;
					rotate_left(x->parent);
					x = root;
				}
			}
			else 
			{
				s = x->parent->left;

				if (s->color == 1) 
				{
					s->color = 0;
					x->parent->color = 1;
					right_rotate(x->parent);
					s = x->parent->left;
				}

				if (s->left->color == 0 && s->right->color == 0) 
				{
					s->color = 1;
					x = x->parent;
				}

				else 
				{
					if (s->left->color == 0) 
					{
						s->right->color = 0;
						s->color = 1;
						rotate_left(s);
						s = x->parent->left;
					}

					s->color = x->parent->color;
					x->parent->color = 0;
					s->left->color = 0;
					right_rotate(x->parent);
					x = root;
				}
			}
		}
		x->color = 0;
	}


	void transplant(NodePtr u, NodePtr v) 
	{
		if (u->parent == nullptr) 
		{
			root = v;
		}
		else if (u == u->parent->left) 
		{
			u->parent->left = v;
		}
		else 
		{
			u->parent->right = v;
		}
		v->parent = u->parent;
	}


	void helper_delete_node(NodePtr node, T key) 
	{
		// find the node containing key
		NodePtr z = TNULL;
		NodePtr x, y;
		while (node != TNULL) 
		{
			if (node->data == key) 
			{
				z = node;
			}

			if (node->data <= key) 
			{
				node = node->right;
			}
			else 
			{
				node = node->left;
			}
		}

		if (z == TNULL) 
		{
			cout << "Couldn't find key in the tree" << endl;
			return;
		}

		y = z;
		int y_original_color = y->color;

		if (z->left == TNULL) 
		{
			x = z->right;
			transplant(z, z->right);
		}

		else if (z->right == TNULL) 
		{
			x = z->left;
			transplant(z, z->left);
		}

		else 
		{
			y = minimum(z->right);
			y_original_color = y->color;
			x = y->right;

			if (y->parent == z) 
			{
				x->parent = y;
			}

			else 
			{
				transplant(y, y->right);
				y->right = z->right;
				y->right->parent = y;
			}

			transplant(z, y);
			y->left = z->left;
			y->left->parent = y;
			y->color = z->color;
		}

		delete z;

		if (y_original_color == 0) 
		{
			helper_fix_delete(x);
		}
	}


	// fix the red-black tree
	void helper_fix_insert(NodePtr k) 
	{
		NodePtr u;

		while (k->parent->color == 1) 
		{
			if (k->parent == k->parent->parent->right) 
			{
				u = k->parent->parent->left; // uncle
				if (u->color == 1) 
				{
					u->color = 0;
					k->parent->color = 0;
					k->parent->parent->color = 1;
					k = k->parent->parent;
				}

				else 
				{
					if (k == k->parent->left) 
					{
						k = k->parent;
						right_rotate(k);
					}

					k->parent->color = 0;
					k->parent->parent->color = 1;
					rotate_left(k->parent->parent);
				}
			}

			else 
			{
				u = k->parent->parent->right; // uncle

				if (u->color == 1) 
				{
					u->color = 0;
					k->parent->color = 0;
					k->parent->parent->color = 1;
					k = k->parent->parent;
				}

				else 
				{
					if (k == k->parent->right) 
					{
						k = k->parent;
						rotate_left(k);
					}

					k->parent->color = 0;
					k->parent->parent->color = 1;
					right_rotate(k->parent->parent);
				}
			}

			if (k == root) 
			{
				break;
			}
		}
		root->color = 0;
	}


	void helper_print(NodePtr root, string indent, bool last) 
	{
		// print the tree structure on the screen
		if (root != TNULL) 
		{
			cout << indent;
			if (last) 
			{
				cout << "R----";
				indent += "     ";
			}
			else 
			{
				cout << "L----";
				indent += "|    ";
			}

			string sColor = root->color ? "RED" : "BLACK";
			cout << root->data << "(" << sColor << ")" << endl;
			helper_print(root->left, indent, false);
			helper_print(root->right, indent, true);
		}
		// cout<<root->left->data<<endl;
	}


	// rotate left at node x
	void rotate_left(NodePtr x)
	{
		NodePtr y = x->right;
		x->right = y->left;
		if (y->left != TNULL)
		{
			y->left->parent = x;
		}
		y->parent = x->parent;

		if (x->parent == nullptr)
		{
			this->root = y;
		}

		else if (x == x->parent->left)
		{
			x->parent->left = y;
		}

		else
		{
			x->parent->right = y;
		}
		y->left = x;
		x->parent = y;
	}


	// rotate right at node x
	void right_rotate(NodePtr x)
	{
		NodePtr y = x->left;
		x->left = y->right;

		if (y->right != TNULL)
		{
			y->right->parent = x;
		}

		y->parent = x->parent;
		if (x->parent == nullptr)
		{
			this->root = y;
		}

		else if (x == x->parent->right)
		{
			x->parent->right = y;
		}

		else
		{
			x->parent->left = y;
		}

		y->right = x;
		x->parent = y;
	}





public:

	RedBlack() 
	{
		TNULL = new Node;
		TNULL->color = 0;
		TNULL->left = nullptr;
		TNULL->right = nullptr;
		root = TNULL;
	}


	// Pre-Order traversal
	// Node->Left Subtree->Right Subtree
	void preorder() 
	{
		helper_pre_order(this->root);
	}


	// In-Order traversal
	// Left Subtree -> Node -> Right Subtree
	void inorder() 
	{
		helper_in_order(this->root);
	}


	// Post-Order traversal
	// Left Subtree -> Right Subtree -> Node
	void postorder() 
	{
		helper_post_order(this->root);
	}


	// search the tree for the key k
	// and return the corresponding node
	NodePtr searchTree(T k) 
	{
		return helper_search_tree(this->root, k);
	}


	// find the node with the minimum key
	NodePtr minimum(NodePtr node) 
	{
		while (node->left != TNULL) 
		{
			node = node->left;
		}
		return node;
	}


	// find the node with the maximum key
	NodePtr maximum(NodePtr node) 
	{
		while (node->right != TNULL) 
		{
			node = node->right;
		}
		return node;
	}


	// find the successor of a given node
	NodePtr find_successor(NodePtr x) 
	{
		// if the right subtree is not null,
		// the successor is the leftmost node in the
		// right subtree
		if (x->right != TNULL) 
		{
			return minimum(x->right);
		}

		// else it is the lowest ancestor of x whose
		// left child is also an ancestor of x.
		NodePtr y = x->parent;
		while (y != TNULL && x == y->right) 
		{
			x = y;
			y = y->parent;
		}
		return y;
	}


	// find the predecessor of a given node
	NodePtr find_predecessor(NodePtr x) 
	{
		// if the left subtree is not null,
		// the predecessor is the rightmost node in the 
		// left subtree
		if (x->left != TNULL) 
		{
			return maximum(x->left);
		}

		NodePtr y = x->parent;
		while (y != TNULL && x == y->left) 
		{
			x = y;
			y = y->parent;
		}

		return y;
	}


	// insert the key to the tree in its appropriate position
	// and fix the tree
	void insert(T key) 
	{
		// Ordinary Binary Search Insertion
		NodePtr node = new Node;
		node->parent = nullptr;
		node->data = key;
		node->left = TNULL;
		node->right = TNULL;
		node->color = 1; // new node must be red

		NodePtr y = nullptr;
		NodePtr x = this->root;

		while (x != TNULL) 
		{
			y = x;
			if (node->data < x->data) 
			{
				x = x->left;
			}
			else 
			{
				x = x->right;
			}
		}

		// y is parent of x
		node->parent = y;
		if (y == nullptr) 
		{
			root = node;
		}

		else if (node->data < y->data) 
		{
			y->left = node;
		}

		else 
		{
			y->right = node;
		}

		// if new node is a root node, simply return
		if (node->parent == nullptr) 
		{
			node->color = 0;
			return;
		}

		// if the grandparent is null, simply return
		if (node->parent->parent == nullptr) 
		{
			return;
		}

		// Fix the tree
		helper_fix_insert(node);
	}


	NodePtr getRoot() const
	{
		return this->root;
	}


	// delete the node from the tree
	void deleteNode(T data) 
	{
		helper_delete_node(this->root, data);
	}


	T search_parent_data(const T& data)
	{
		NodePtr result = searchTree(data);
		return result->parent->data;
	}


	int search_node_depth(T& data)
	{
		NodePtr current = root;
		int depth = 0;

		// traverse the tree starting from the root
		while (current != TNULL)
		{
			if (data < current->data)
			{
				current = current->left;
				depth++;
			}

			else if (data > current->data)
			{
				current = current->right;
				depth++;
			}

			else
				return depth;
		}

		return -1;
	}


	// print the tree structure on the screen
	void prettyPrint() 
	{
		if (root) 
		{
			helper_print(this->root, "", true);
		}
	}

};


template <typename T>
void PopulateTree(std::vector<T>& array, RedBlack<T>& tree)
{
	for (auto itr = array.begin(); itr != array.end(); itr++)
	{
		tree.insert(*itr);
	}
}


int main() 
{
	std::vector<int> array_int = { 95,301,501,801,90,70,80,25,67,89,11 };
	RedBlack<int> tree_int;

	std::vector<float> array_float = { 84.8,103.5,67.8,90.3,23.5,67.1,44.5,89.2,100.5,300.2 };
	RedBlack<float> tree_float;

	std::vector<std::string> array_string = { "Milton", "Ajax", "Clarington", "Brock", "Oshawa", "Pickering", "Uxbridge", "Whitby", "Burlington", "Oakville", "Brampton", "Caledon", "Mississauga","Aurora","Georgina" };
	RedBlack<std::string> tree_string;

	PopulateTree(array_int, tree_int);
	PopulateTree(array_float, tree_float);
	PopulateTree(array_string, tree_string);

	tree_int.prettyPrint(); puts("");
	tree_float.prettyPrint(); puts("");
	tree_string.prettyPrint(); puts("");

	std::cout << "\nAdding Stouffville and then print...\n";
	tree_string.prettyPrint(); puts("");
	std::string Stouffville = "Stouffville";
	tree_string.insert(Stouffville);
	tree_string.prettyPrint(); puts("");

	std::cout << "\nAdding 506 and then print...\n";
	tree_int.prettyPrint(); puts("");
	int FiveOhSix = 506;
	tree_int.insert(FiveOhSix);
	tree_int.prettyPrint(); puts("");

	std::cout << "\nAdding 88.5 and then print...\n";
	tree_float.prettyPrint(); puts("");
	float number = 88.5;
	tree_float.insert(number);
	tree_float.prettyPrint(); puts("");

	std::cout << "\nHere we delete Caledon...\n";
	tree_string.prettyPrint(); puts("");
	std::string delete_string = "Caledon";
	tree_string.deleteNode(delete_string);
	tree_string.prettyPrint(); puts("");

	std::cout << "\nHere we delete 89...\n";
	tree_int.prettyPrint(); puts("");
	int delete_int = 89;
	tree_int.deleteNode(delete_int);
	tree_int.prettyPrint(); puts("");

	std::cout << "\nHere we delete 23.5...\n";
	tree_float.prettyPrint(); puts("");
	float delete_float = 23.5;
	tree_float.deleteNode(delete_float);
	tree_float.prettyPrint(); puts("");

	std::string search_string = "Whitby";
	std::cout << "Let's search for " << search_string; puts("");
	std::cout << "The height of the node is -> " << tree_string.search_node_depth(search_string); puts("");
	std::cout << "The parent data is -> " << tree_string.search_parent_data(search_string); puts(""); puts("");

	int search_int = 11;
	std::cout << "Let's search for " << search_int; puts("");
	std::cout << "The height of the node is -> " << tree_string.search_node_depth(search_string); puts("");
	std::cout << "The parent data is -> " << tree_string.search_parent_data(search_string); puts(""); puts("");

	float search_float = 90.3;
	std::cout << "Let's search for " << search_float; puts("");
	std::cout << "The height of the node is -> " << tree_string.search_node_depth(search_string); puts("");
	std::cout << "The parent data is -> " << tree_string.search_parent_data(search_string); puts(""); puts("");

	return 0;
}