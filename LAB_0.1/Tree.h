#ifndef LECS_PARS_TREE_H
#define LECS_PARS_TREE_H

#include <cstdlib>
#include <iostream>
#include <typeinfo>
#include <memory>
#include <string>
#include <queue>
#include <utility>
#include <iomanip>

class Tree {
public:
    Tree() {
        left   = nullptr;
        right  = nullptr;
        parent = nullptr;
        value  = "";
        alloc  = true;
    }

    explicit Tree(const std::string &val) {
        left   = nullptr;
        right  = nullptr;
        parent = nullptr;
        value  = val;
        alloc  = true;
    }


    void AddLeftNode(const std::string &val) {
        this->left = CreateNode(this, val);
    }

    void AddRightNode(const std::string &val) {
        this->right = CreateNode(this, val);
    }


    Tree *GetLeftNode() const {
        return this->left;
    }


    void AddLeftTree(Tree* tree) {
        tree->parent = this;
        this->left = tree;
    }


    void AddRightTree(Tree * tree) {
        tree->parent = this;
        this->right = tree;
    }


    void ChangeValue(const std::string &val) {
        value = val;
    }


    Tree *GetRightNode() const {
        return this->right;
    }


    Tree *GetParentNode() const {
        return this->parent;
    }


    std::string GetValue() {
        return this->value;
    }


    void PrintTree() {
        std::cout << this << "\t" << this->value
                  << ((this->value.size() >= 4) ? "\t " : "\t\t ")
                  << this->left
                  << ((this->left == nullptr) ? "\t\t\t " : "\t ")
                  << this->right << std::endl;
        if (this->left != nullptr) this->left->PrintTree();
        if (this->right != nullptr) this->right->PrintTree();
    }



    /**
     * @brief Create like a root node of syntax tree
     * @param[in] node_name - name of the node (value)
     *
     * @return node of tree
     */
    static Tree *CreateNode(const std::string &val) {
        auto *node = new Tree(val);
        return node;
    }


    /**
     * @brief Create node of syntax tree
     * @param[in] t_tree    - parent node
     * @param[in] node_name - name of the created node (value)
     *
     * @return node of tree
     */
    static Tree *CreateNode(Tree *parent_tree, const std::string &val) {
        auto *node = new Tree(val);
        node->parent = std::addressof(*parent_tree);
        return node;
    }



    void FreeLeftNode() {
        FreeTree(this->left);
    }


    void FreeRightNode() {
        FreeTree(this->right);
    }


    static void FreeTree(Tree *&t_tree) {
        try {
            if (t_tree->left  != nullptr) FreeTree(t_tree->left);  // ERROR
            if (t_tree->right != nullptr) FreeTree(t_tree->right);
            delete t_tree;
            t_tree = nullptr;
        } catch (const std::exception &exp) {
            std::cerr << "<E> Tree: Catch exception in " << __func__ << ": "
                      << exp.what() << std::endl;
        }
    }


    virtual ~Tree() {
        FreeTree();
    } // = default;
private:
    Tree *left   { nullptr };
    Tree *right  { nullptr };
    Tree *parent { nullptr };
    std::string value;
    bool alloc { false };

    void FreeTree() {
        if (this->left  != nullptr) this->left->FreeTree();
        if (this->right != nullptr) this->right->FreeTree();
        parent = nullptr;
        value  = "";
        alloc  = false;
    }
};


#endif //LECS_PARS_TREE_H
