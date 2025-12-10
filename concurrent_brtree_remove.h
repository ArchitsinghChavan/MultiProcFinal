#ifndef BRTREE_REMOVE
#define BRTREE_REMOVE
#include "Concurrent_Node.h"

enum class Case {CASE1,CASE2,CASE3,CASE4,CASE5,CASE6};

bool acquire_remove_nodes(Node<T> * parent) {
    if()
}


Case identify_case(Node<T> * parent, Node<T> * sibling, Node<T> * close_nephew, Node<T> * distant_nephew) {
    if(parent == nullptr) {
        return Case::CASE1;
    }
    if(get_color(parent) == Color::BLACK && get_color(sibling) == Color::BLACK
        && get_color(close_nephew) == Color::BLACK && get_color(distant_nephew) == Color::BLACK) {
        return Case::CASE2;
    }
    if(get_color(parent) == Color::BLACK && get_color(sibling) == Color::RED
        && get_color(close_nephew) == Color::BLACK && get_color(distant_nephew) == Color::BLACK) {
        return Case::CASE3;
    }
    if(get_color(parent) == Color::RED && get_color(sibling) == Color::BLACK
        && get_color(close_nephew) == Color::BLACK && get_color(distant_nephew) == Color::BLACK) {
        return Case::CASE4;
    }
    if(get_color(sibling) == Color::BLACK
        && get_color(close_nephew) == Color::RED && get_color(distant_nephew) == Color::BLACK) {
        return Case::CASE5;
    }
    if(get_color(sibling) == Color::BLACK
        && get_color(distant_nephew) == Color::RED) {
        return Case::CASE6;
    }
    throw std::runtime_error("identify_case: No case matched.");


}

    void start_balance(Node<T> * parent, Direction dir) {
    if(parent == nullptr)
        return;
    Node<T>* sibling = parent->get_opposite_child(dir);
    if (!sibling) return;  // No sibling, nothing to balance
    
    Node<T>* distant_nephew = sibling->get_opposite_child(dir);
    Node<T>* close_nephew = sibling->get_child(dir);

    Case case_id = identify_case(parent, sibling, close_nephew, distant_nephew);

    switch(case_id) {
        case Case::CASE1:
        {
            return;
            break;
        }
        case Case::CASE2:
        {
            //std::cout << "Case 2 on pareent " << parent->data << std::endl << std::flush; 
            set_color(sibling, Color::RED);
            if(parent->parent == nullptr)
                return;
            Direction parent_dir = (parent->parent->left == parent) ? Direction::LEFT : Direction::RIGHT;
            start_balance(parent->parent, parent_dir);
            return;
            break;
        }
        case Case::CASE3:
        {
            rotate_tree(parent, dir);
            set_color(parent, Color::RED);
            set_color(sibling, Color::BLACK);
            sibling = close_nephew;
            distant_nephew = sibling->get_opposite_child(dir);
            start_balance(parent, dir);
            return;
            break;
        }
        case Case::CASE4:
        {
            set_color(sibling, Color::RED);
            set_color(parent, Color::BLACK);
            return;
            break;
        }
        case Case::CASE5:
        {
            //std::cout << "Start Balance Test: Case 5 identified." << std::endl << std::flush;
            rotate_tree(sibling, (dir == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT);
            set_color(sibling, Color::RED);
            set_color(close_nephew, Color::BLACK);
            sibling = close_nephew;
            distant_nephew = sibling->get_opposite_child(dir);
            start_balance(parent, dir);
            return;
            break;
        }
        case Case::CASE6:
        {
            //std::cout << "Start Balance Test: Case 6 identified." << std::endl << std::flush;
            rotate_tree(parent, dir);
            set_color(sibling, get_color(parent));
            set_color(parent, Color::BLACK);
            set_color(distant_nephew, Color::BLACK);
            return;
            break;
        }
    }
}

bool simple_remove(Node<T> * node) {
    if(node->left != nullptr && node->right != nullptr) {
        //Node has two children, find in-order successor
        #if DEBUG
        std::cout << "Two Children Remove called." << std::endl << std::flush;
        #endif
        Node<T> * successor = node->right;
        while(successor->left != nullptr) {
            successor = successor->left;
        }

        //swap node data with successor data
        node->data = successor->data;
        this->remove_helper(successor);
        #if DEBUG
        print_tree();
        #endif
        return true;
    }
    else if(node->left != nullptr || node->right != nullptr) {
        //Node has one child
        Node<T> * child = (node->left != nullptr) ? node->left : node->right;
        Node<T> * parent = node->parent;
        
        child->color = Color::BLACK;

        if(parent != nullptr) {
            Direction dir = (parent->left == node) ? Direction::LEFT: Direction::RIGHT;
            parent->set_child(dir, child);
            child->parent = parent;
        } else {
            //node is root
            root = child;
            child->parent = nullptr;
        }
        delete node;
        return true;
    }
    //Node has no children
    if(node == root) {
        //node is root
        root = nullptr;
        delete node;
        return true;
    }
    if(node->color == Color::RED) {
        //Simply remove red leaf
        Node<T> * parent = node->parent;
        if(parent->left == node) {
            parent->left = nullptr;
        } else {
            parent->right = nullptr;
        }
        delete node;
        return true;
    }
    return false;
}

void remove_helper(Node<T> * node) {

    //std::cout << "Removing node with data " << node->data << std::endl << std::flush;

    if((node->color == Color::BLACK) && (node->left == nullptr) && (node->right == nullptr)) {
        //Node is black leaf
        if(node == root) {
            root = nullptr;
            delete node;
            return;
        }
        #if DEBUG
        std::cout << "Black leaf remove called." << std::endl << std::flush;
        #endif
        Node<T> * parent = node->parent;
        Direction dir = (node->parent->left == node) ? Direction::LEFT : Direction::RIGHT;

        parent->set_child(dir, nullptr);
        delete node;
        start_balance(parent, dir);
    }
    else {
        #if DEBUG
        std::cout << "Simple remove called." << std::endl << std::flush;
        #endif
        simple_remove(node);
    }
}
#endif //BRTREE_REMOVE