#pragma once
namespace ariel
{
    template <typename T>
    class Node
    {
    public:
        T _value;
        Node<T> *_left;
        Node<T> *_right;
        Node<T> *_parent;

        void add_right(T val)
        {
            this->_right = new Node<T>(val);
            this->_right->_parent = this;
        }
        void add_left(T val)
        {
            this->_left = new Node<T>(val);
            this->_left->_parent = this;
        }
        Node() : _left(nullptr), _right(nullptr), _parent(nullptr) {}
        Node(T val) : _value(val), _left(nullptr), _right(nullptr), _parent(nullptr) {}
        // ~Node()
        // {
        //     if (this->_left != nullptr){
        //         delete this->_right;
        //     }
        //     if (this->_right != nullptr){
        //         delete this->_left;
        //     }
        //     // delete this;
        // }
    };
}