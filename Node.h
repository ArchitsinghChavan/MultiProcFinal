#ifndef NODE_H
#define NODE_H

#include <iostream>

enum class Color { RED, BLACK };

enum class Direction { LEFT, RIGHT };

namespace sql_brtree {

template <typename T>
class Node {
public:
    T data;
    Node* left;
    Node* right;
    Node* parent;

    Color color;

    inline Node * get_child(Direction dir) {
        return (dir == Direction::LEFT) ? left : right;
    }

    inline Node * get_opposite_child(Direction dir) {
        return (dir == Direction::LEFT) ? right : left;
    }

    inline void set_child(Direction dir, Node * child) {
        if (dir == Direction::LEFT) {
            left = child;
        } else {
            right = child;
        }
    }

    inline void set_opposite_child(Direction dir, Node * child) {
        if (dir == Direction::LEFT) {
            right = child;
        } else {
            left = child;
        }
    }

    Node(T d, Color c) : data(d), left(nullptr), right(nullptr), parent(nullptr), color(c) {}

    Node() : data(), left(nullptr), right(nullptr), parent(nullptr), color(Color::BLACK) {}

    void node_print() {
        std::cout << data << " (" << (color == Color::RED ? "R" : "B") << ") ";
        std::cout << "\nleft: " << (left ? std::to_string(left->data) : "null");
        std::cout << "\nright: " << (right ? std::to_string(right->data) : "null");
        std::cout << "\nparent: " << (parent ? std::to_string(parent->data) : "null") << std::endl;
    }

    bool operator==(const Node<T>& other) const {
        return data == other.data;
    }
    bool operator!=(const Node<T>& other) const {
        return data != other.data;
    }
    bool operator<(const Node<T>& other) const {
        return data < other.data;
    }
    bool operator>(const Node<T>& other) const {
        return data > other.data;
    }
    bool operator<=(const Node<T>& other) const {
        return data <= other.data;
    }
    bool operator>=(const Node<T>& other) const {
        return data >= other.data;
    }
    bool operator=(const Node<T>& other) const {
        data = other.data;
        color = other.color;
        left = other.left;
        right = other.right;
        parent = other.parent;
        return true;
    }

    static void setColor(Node * n, Color c) {
        if(n != nullptr) {
            n->color = c;
        }
    }

    static Color getColor(Node * n) {
        if(n == nullptr) {
            return Color::BLACK;
        } else {
            return n->color;
        }
    }

    static void print_node(Node * n) {
        if(n != nullptr) {
            n->node_print();
        } else {
            std::cout << "Node is null." << std::endl;
        }
    }
};
}
#endif // NODE_H