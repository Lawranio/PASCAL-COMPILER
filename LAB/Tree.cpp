#include "Tree.h"

Tree::Tree()
{
	left	= nullptr;
	right   = nullptr;
	parent  = nullptr;
	value   = "";
}
Tree::Tree(const string& val)
{
	left	= nullptr;
	right   = nullptr;
	parent  = nullptr;
	value   = val;
}
Tree::~Tree()
{
	FreeTree();
}


void Tree::AddLeftNode(const string& val)
{
	this->left = CreateNode(this, val);
}
void Tree::AddRightNode(const string& val)
{
	this->right = CreateNode(this, val);
}


void Tree::AddLeftTree(Tree* tree)
{
	tree->parent = this;
	this->left = tree;
}
void Tree::AddRightTree(Tree* tree)
{
	tree->parent = this;
	this->right = tree;
}


Tree* Tree::CreateNode(const string& val)
{
	auto* node = new Tree(val);
	return node;
}
Tree* Tree::CreateNode(Tree* parent_tree, const string& val)
{
	auto* node = new Tree(val);
	node->parent = addressof(*parent_tree);
	return node;
}


void Tree::ChangeValue(const string& val)
{
	value = val;
}
string Tree::GetValue()
{
	return this->value;
}


Tree* Tree::GetLeftNode()
{
	return this->left;
}
Tree* Tree::GetRightNode()
{
	return this->right;
}
Tree* Tree::GetParentNode()
{
	return this->parent;
}


void Tree::FreeLeftNode()
{
	FreeTree(this->left);
}
void Tree::FreeRightNode()
{
	FreeTree(this->right);
}
void Tree::FreeTree(Tree*& t_tree)
{
	try
	{
		if (t_tree->left != nullptr) FreeTree(t_tree->left);
		if (t_tree->right != nullptr) FreeTree(t_tree->right);
		delete t_tree;
		t_tree = nullptr;
	}
	catch (const std::exception& exp)
	{
		cerr << "<E> Tree: Catch exception in " << __func__ << ": "
			<< exp.what() << endl;
	}
}
void Tree::FreeTree()
{
	if (this->left != nullptr) this->left->FreeTree();
	if (this->right != nullptr) this->right->FreeTree();
	parent = nullptr;
	value = "";
}


void Tree::PrintTree(int iter)
{
	for (auto i = 0; i < iter; i++) {
		std::cout << "    ";
	}
	std::cout << this->value << std::endl;

	if (this->left != nullptr) { this->left->PrintTree(iter + 1); }
	else {
		for (auto i = 0; i < iter + 1; i++) {
			std::cout << "    ";
		}
		std::cout << "NULL" << std::endl;
	};
	if (this->right != nullptr) { this->right->PrintTree(iter + 1); }
	else {
		for (auto i = 0; i < iter + 1; i++) {
			std::cout << "    ";
		}
		std::cout << "NULL" << std::endl;
	}
}