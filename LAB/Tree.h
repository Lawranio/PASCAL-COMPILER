#ifndef TREE_H
#define TREE_H
#include <cstdlib>
#include <iostream>
#include <typeinfo>
#include <memory>
#include <string>
#include <queue>
#include <utility>
#include <iomanip>
using namespace std;

class Tree
{
public:
	Tree();
	explicit Tree(const string& val);
	virtual ~Tree();

	void		 SetPriority(int priority_);
	int		     GetPriority();

	void		 AddLeftNode(const string& val);
	void		 AddRightNode(const string& val);

	void		 AddLeftNode(const string& val, int priority_);
	void		 AddRightNode(const string& val, int priority_);

	void		 AddLeftTree(Tree* tree);
	void		 AddRightTree(Tree* tree);

	void		 ChangeValue(const string& val);
	string		 GetValue();

	static Tree* CreateNode(const string& val);
	static Tree* CreateNode(Tree* parent_tree, const string& val);
	static Tree* CreateNode(Tree* parent_tree, const string& val, int& priority_);

	Tree*		 GetLeftNode();
	Tree*		 GetRightNode();
	Tree*		 GetParentNode();

	void		 FreeLeftNode();
	void		 FreeRightNode();
	static void  FreeTree(Tree*& t_tree);

	void		 PrintTree(int tab);
	void		 PrintTree_2();


private:
	Tree* left;
	Tree* right;
	Tree* parent;
	string value;
	int priority;

	void FreeTree();
};



#endif TREE_H