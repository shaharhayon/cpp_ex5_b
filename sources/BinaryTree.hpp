#pragma once
#include "Node.hpp"
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <queue>

namespace ariel
{
    template <typename T>
    class BinaryTree
    {
    private:
        Node<T> *_root = nullptr;

        Node<T> *find(T val)
        {
            if (this->_root == nullptr)
            {
                throw std::invalid_argument("root is null");
            }
            return find(val, this->_root);
        }

        Node<T> *find(T val, Node<T> *currentNode)
        {
            Node<T> *node1 = nullptr;
            Node<T> *node2 = nullptr;
            if (val == currentNode->_value)
            {
                return currentNode;
            }
            if ((currentNode->_left == nullptr) && (currentNode->_right == nullptr))
            {
                return nullptr;
            }
            if (currentNode->_left != nullptr)
            {
                node1 = find(val, currentNode->_left);
            }
            if (node1 != nullptr)
            {
                return node1;
            }
            if (currentNode->_right != nullptr)
            {
                node2 = find(val, currentNode->_right);
            }
            if (node2 != nullptr)
            {
                return node2;
            }
            return nullptr;
        }

        void printTree(std::ostream &os, const std::string &prefix, const Node<T> *node) const
        {
            if (node != nullptr)
            {
                bool hasLeft = node->_left != nullptr;
                bool hasRight = node->_right != nullptr;
                if (!hasLeft && !hasRight)
                {
                    return;
                }

                os << prefix;
                os << ((hasLeft && hasRight) ? "├──" : "");
                os << ((!hasLeft && hasRight) ? "└── " : "");

                if (hasRight)
                {
                    bool printStrand = (hasLeft && hasRight && (node->_right->_right != nullptr || node->_right->_left != nullptr));
                    std::string newPrefix = prefix + (printStrand ? "│   " : "    ");
                    os << "R " << node->_right->_value << std::endl;
                    printTree(os, newPrefix, node->_right);
                }

                if (hasLeft)
                {
                    std::cout << (hasRight ? prefix : "") << "└──L " << node->_left->_value << std::endl;
                    printTree(os, prefix + "    ", node->_left);
                }
            }
        }

    public:
        BinaryTree<T>() : _root(nullptr) {}
        BinaryTree<T>(BinaryTree<T> &tree)
        {
            this->add_root(tree._root->_value);
            for (auto it = tree.begin_preorder(); it != tree.end_preorder(); ++it)
            {
                if (it.has_left())
                {
                    this->add_left(*it, it.Node()._left->_value);
                }
                if (it.has_right())
                {
                    this->add_right(*it, it.Node()._right->_value);
                }
            }
        }
        BinaryTree<T>(BinaryTree<T> &&tree) noexcept
        {
            this->_root = tree._root;
            tree._root = nullptr;
        }
        ~BinaryTree<T>()
        {
            std::queue<Node<T> *> toRemove;
            for (auto it = this->begin_postorder(); it != this->end_postorder(); ++it)
            {
                Node<T> *n = &it.Node();
                toRemove.push(n);
            }
            while (!toRemove.empty())
            {
                delete toRemove.front();
                toRemove.pop();
            }
        }
        BinaryTree<T> &operator=(BinaryTree<T> tree)
        {
            if (this == &tree)
            {
                return *this;
            }
            std::queue<Node<T> *> toRemove;
            for (auto it = this->begin_postorder(); it != this->end_postorder(); ++it)
            {
                Node<T> *n = &it.Node();
                toRemove.push(n);
            }
            while (!toRemove.empty())
            {
                delete toRemove.front();
                toRemove.pop();
            }
            this->_root = nullptr;
            this->add_root(tree._root->_value);
            for (auto it = tree.begin_preorder(); it != tree.end_preorder(); ++it)
            {
                if (it.has_left())
                {
                    this->add_left(*it, it.Node()._left->_value);
                }
                if (it.has_right())
                {
                    this->add_right(*it, it.Node()._right->_value);
                }
            }
            return *this;
        }
        BinaryTree &operator=(BinaryTree<T> &&tree) noexcept
        {
            if (this == &tree)
            {
                return *this;
            }
            std::queue<Node<T> *> toRemove;
            for (auto it = this->begin_postorder(); it != this->end_postorder(); ++it)
            {
                toRemove.push(&(it.Node()));
            }
            while (!toRemove.empty())
            {
                delete toRemove.front();
            }

            this->add_root(tree._root->_value);
            for (auto it = tree.begin_preorder(); it != tree.end_preorder(); ++it)
            {
                if (it.has_left())
                {
                    this->add_left(*it, it.Node()._left->_value);
                }
                if (it.has_right())
                {
                    this->add_right(*it, it.Node()._right->_value);
                }
            }
        }

        BinaryTree<T> &add_root(T val)
        {
            if (this->_root == nullptr)
            {
                this->_root = new Node<T>(val);
            }
            this->_root->_value = val;
            return *this;
        }

        BinaryTree<T> &add_left(T parent, T child)
        {
            Node<T> *parentNode = find(parent);
            if (parentNode == nullptr)
            {
                throw std::invalid_argument("First value is not in the tree.");
            }
            if (parentNode->_left == nullptr)
            {
                parentNode->add_left(child);
            }
            else
            {
                parentNode->_left->_value = child;
            }
            return *this;
        }

        BinaryTree<T> &add_right(T parent, T child)
        {
            Node<T> *parentNode = find(parent);
            if (parentNode == nullptr)
            {
                throw std::invalid_argument("First value is not in the tree.");
            }
            if (parentNode->_right == nullptr)
            {
                parentNode->add_right(child);
            }
            else
            {
                parentNode->_right->_value = child;
            }
            return *this;
        }

        friend std::ostream &operator<<(std::ostream &os, const BinaryTree<T> &tree)
        {
            os << tree._root->_value << std::endl;
            tree.printTree(os, "", tree._root);
            os << std::endl;
            return os;
        }

        class iterator
        {
        public:
            enum Order
            {
                PREORDER,
                INORDER,
                POSTORDER
            };

        private:
            Node<T> *ptr_current;
            Node<T> *_root;
            Node<T> *prev = nullptr;
            Node<T> *last;
            Order _type; // 0 = preorder, 1 = inorder, 2 = postorder

            void left()
            {
                this->prev = this->ptr_current;
                this->ptr_current = this->ptr_current->_left;
            }

            void right()
            {
                this->prev = this->ptr_current;
                this->ptr_current = this->ptr_current->_right;
            }

            void up()
            {
                this->prev = this->ptr_current;
                this->ptr_current = this->ptr_current->_parent;
            }

        public:
            iterator(Node<T> *root, Node<T> *curr, Order type) : _type(type), _root(root), ptr_current(curr)
            {
                this->last = this->_root;
                if (this->_root != nullptr)
                {
                    if (this->_type != Order::POSTORDER)
                    {
                        {
                            while (this->last->_right != nullptr)
                            {
                                this->last = this->last->_right;
                            }
                        }
                    }
                }
            }
            iterator(Node<T> *root, Order type) : _type(type), _root(root)
            {
                this->ptr_current = root;
                this->last = this->_root;
                if (this->_root != nullptr)
                {
                    if (this->_type == Order::INORDER)
                    {
                        {
                            while (this->last->_right != nullptr)
                            {
                                this->last = this->last->_right;
                            }
                        }
                    }
                    else if (this->_type == Order::PREORDER)
                    {
                        while (this->last->_right != nullptr || this->last->_left != nullptr)
                        {
                            if (this->last->_right != nullptr)
                            {
                                this->last = this->last->_right;
                            }
                            else if (this->last->_left != nullptr)
                            {
                                this->last = this->last->_left;
                            }
                        }
                    }
                }
            }

            Node<T> &Node()
            {
                return *(this->ptr_current);
            }

            bool has_left()
            {
                return (this->ptr_current->_left != nullptr);
            }

            bool has_right()
            {
                return (this->ptr_current->_right != nullptr);
            }

            T &operator*() const
            {
                return ptr_current->_value;
            }

            T *operator->() const
            {
                return &(ptr_current->_value);
            }

            bool operator==(const iterator &rhs) const
            {
                return (ptr_current == rhs.ptr_current);
            }

            bool operator!=(const iterator &rhs) const
            {
                return (ptr_current != rhs.ptr_current);
            }

            void try_left()
            {
                while (this->has_left() || this->has_right())
                {
                    if (this->has_left())
                    {
                        this->left();
                        continue;
                    }
                    this->right();
                }
            }

            void try_right()
            {
                if (this->has_right())
                {
                    this->right();
                    while (this->has_left() || this->has_right())
                    {
                        if (this->has_left())
                        {
                            this->left();
                            continue;
                        }
                        this->right();
                    }
                }
            }

            iterator &operator++()
            {
                if (this->ptr_current == this->last)
                {
                    this->ptr_current = nullptr;
                    return *this;
                }

                switch (this->_type)
                {
                case Order::PREORDER:
                    if (this->prev == nullptr)
                    {
                        if (this->has_left())
                        {
                            this->left();
                            break;
                        }
                        if (this->has_right())
                        {
                            this->right();
                            break;
                        }
                        this->ptr_current = nullptr;
                    }

                    if (this->prev == this->ptr_current->_parent)
                    {
                        if (this->has_left())
                        {
                            this->left();
                            break;
                        }
                        if (this->has_right())
                        {
                            this->right();
                            break;
                        }
                        while (this->ptr_current->_parent != nullptr)
                        {
                            this->up();
                            while (this->prev == this->ptr_current->_right)
                            {
                                this->up();
                            }
                            if (this->has_right())
                            {
                                break;
                            }
                        }
                        if (this->has_right())
                        {
                            this->right();
                            break;
                        }
                        this->ptr_current = nullptr;
                    }
                    break;
                case Order::INORDER:
                    if (this->prev == this->ptr_current->_left)
                    {
                        if (this->has_right())
                        {
                            this->right();
                            while (this->has_left())
                            {
                                this->left();
                            }
                            break;
                        }
                        this->up();
                        while (this->prev == this->ptr_current->_right)
                        {
                            this->up();
                        }
                        break;
                    }
                    else if (this->prev == this->ptr_current->_parent)
                    {
                        if (this->has_left())
                        {
                            while (this->has_left())
                            {
                                this->left();
                            }
                            break;
                        }
                        if (this->has_right())
                        {
                            this->right();
                            while (this->has_left())
                            {
                                this->left();
                            }
                            break;
                        }
                        this->up();
                        while (this->prev == this->ptr_current->_right)
                        {
                            this->up();
                        }
                    }
                    else if (this->prev == this->ptr_current->_right)
                    {
                        while (this->prev == this->ptr_current->_right)
                        {
                            this->up();
                        }
                    }
                    break;
                case Order::POSTORDER:
                    if (this->prev == nullptr)
                    {
                        up();
                        if (this->ptr_current == nullptr)
                        {
                            break;
                        }
                        if (this->prev == this->ptr_current->_left)
                        {
                            this->try_right();
                        }
                        break;
                    }

                    if (this->prev == this->ptr_current->_parent)
                    {
                        this->up();
                        if (this->prev == this->ptr_current->_left)
                        {
                            this->try_right();
                        }
                        break;
                    }

                    if (this->prev == this->ptr_current->_left)
                    {
                        auto tmp = this->ptr_current;
                        this->try_right();
                        if (this->ptr_current == tmp)
                        {
                            this->up();
                            if (this->prev == this->ptr_current->_left)
                            {
                                this->try_right();
                            }
                        }
                        break;
                    }

                    if (this->prev == this->ptr_current->_right)
                    {
                        up();
                        if (this->prev == this->ptr_current->_left)
                        {
                            this->try_right();
                        }

                    }
                    break;
                }
                return *this;
            }
            iterator operator++(int)
            {
                iterator temp {this->_root, this->ptr_current, this->_type};
                this->operator++();
                return temp;
            }
        };

        iterator begin_preorder()
        {
            if (this->_root == nullptr)
            {
                return iterator(nullptr, iterator::Order::PREORDER);
            }
            return iterator(this->_root, iterator::Order::PREORDER);
        }
        iterator end_preorder()
        {
            return iterator(nullptr, iterator::Order::PREORDER);
        }

        iterator begin_inorder()
        {
            if (this->_root == nullptr)
            {
                return iterator(nullptr, iterator::Order::INORDER);
            }
            Node<T> *p = this->_root;
            while (p->_left != nullptr)
            {
                p = p->_left;
            }
            return iterator(this->_root, p, iterator::Order::INORDER);
        }
        iterator end_inorder()
        {
            return iterator(nullptr, iterator::Order::INORDER);
        }

        iterator begin_postorder()
        {
            if (this->_root == nullptr)
            {
                return iterator(nullptr, iterator::Order::POSTORDER);
            }
            Node<T> *p = this->_root;
            while (p->_left != nullptr || p->_right != nullptr)
            {
                if (p->_left != nullptr)
                {
                    p = p->_left;
                }
                else if (p->_right != nullptr)
                {
                    p = p->_right;
                }
            }
            return iterator(this->_root, p, iterator::Order::POSTORDER);
        }
        iterator end_postorder()
        {
            return iterator(nullptr, iterator::Order::POSTORDER);
        }

        iterator begin()
        {
            return this->begin_inorder();
        }
        iterator end()
        {
            return this->end_inorder();
        }
    };
}