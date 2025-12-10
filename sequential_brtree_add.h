 
#ifndef BRTREE_ADD
#define BRTREE_ADD
#include "Node.h"
 
#ifndef TEMPLATE_TYPE
template <typename T>
#endif
void fix_tree(Node<T> * node) {
    //Case #1: Parent is black - Handled in call

    Node<T> * parent = node->parent;
    //node is root
    if (parent == nullptr) {
        node->color = Color::BLACK;
        return;
    }

    Node<T> * grandparent = parent->parent;
    //case 4: grandparent is null
    if(grandparent == nullptr) {
        parent->color = Color::BLACK;
        return;
    }


    Direction p_dir = (parent == grandparent->left) ? Direction::LEFT : Direction::RIGHT;
    Node<T> * uncle = grandparent->get_opposite_child(p_dir);
    if((uncle == nullptr) || (uncle->color == Color::BLACK)) {
        //Case #5
        if(node == parent->get_opposite_child(p_dir)) {
            rotate_tree(parent, p_dir);
            node = parent;
            parent = grandparent->get_child(p_dir);
        }
        //Case #6
        rotate_tree(grandparent, (p_dir == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT);
        parent->color = Color::BLACK;
        grandparent->color = Color::RED;
        return;
    }

    // Case #2

    parent->color = Color::BLACK;
    uncle->color = Color::BLACK;
    grandparent->color = Color::RED;
    if(grandparent->parent != nullptr)
        if(grandparent->parent->color == Color::RED)
            fix_tree(grandparent);
}
#ifndef TEMPLATE_TYPE
template <typename T>
#endif
void insert_helper(Node<T> * node, T data) {
    //If data already exists, update and return
    if(data == node->data) {
        node->data = data;
        return;
    }

    //If data < node->data, go left, else go right
    Direction dir = (data < node->data) ? Direction::LEFT: Direction::RIGHT;

    // If child is null, create new node and place it there
    if (node->get_child(dir) == nullptr) {
        Node<T> * new_node = new Node<T>(data, Color::RED);
        new_node->parent = node;
        node->set_child(dir, new_node);
        if(node->color == Color::RED) {
            fix_tree(new_node);
        }
    }
    //if child is not null, go down the treed
    else {
        insert_helper(node->get_child(dir), data);
    }
}

#endif //BRTREE_ADD