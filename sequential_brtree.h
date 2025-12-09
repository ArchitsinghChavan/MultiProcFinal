#ifndef SEQUENTIAL_BRTREE_H
#define SEQUENTIAL_BRTREE_H

#include <stdio.h>
#include <iostream>
#include "KVPair.h"
#include "Node.h"

#define DEBUG 0
namespace sql_brtree {
  
template <typename T>
class Seq_BRTree {

private:

    enum class Case {
        CASE1,
        CASE2,
        CASE3,
        CASE4,
        CASE5,
        CASE6
    };

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
    
    void destructor_helper(Node<T> * node) {
        if (node != nullptr) {
            destructor_helper(node->left);
            destructor_helper(node->right);
            delete node;
        }
    }

    void rotate_tree(Node<T> * node, Direction dir) {
        Node<T> * parent = node->parent;


        Node<T> * new_root = node->get_opposite_child(dir); 
        Node<T> * new_child = new_root->get_child(dir);
        node->set_opposite_child(dir, new_child);
        if (new_child) {
            new_child->parent = node;
        }

        new_root->set_child(dir, node);
        new_root->parent = parent;
        node->parent = new_root;

        if(parent) {
            if(parent->left == node)
                parent->left = new_root;
            else if(parent->right == node)
                parent->right = new_root;
            else
                throw std::runtime_error("rotate_tree: parent does not point to node");
        } else {
            root = new_root;
        }
    }

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
        //if child is not null, go down the tree
        else {
            insert_helper(node->get_child(dir), data);
        }
    }

    int verify_helper(Node<T> * node) {
        if (node == nullptr) {
            return 1; // Black height of null nodes is 1
        }

        Node<T> * left = node->left;
        Node<T> * right = node->right;

        // Check red-black properties
        if (node->color == Color::RED) {
            if ((left != nullptr && left->color == Color::RED) ||
                (right != nullptr && right->color == Color::RED)) {
                printf("Red violation at node with data %d\n", node->data);
                return -1; // Red violation
            }
        }

        if(left != nullptr) {
            if(left->data >= node->data) {
                printf("Left violation at node with data %d\n", node->data);
                return -1;
            }
        }
        if(right != nullptr) {
            if(right->data <= node->data) {
                printf("Right violation at node with data %d\n", node->data);
                return -1;
            }
        }

        int left_black_height = verify_helper(left);
        int right_black_height = verify_helper(right);

        if (left_black_height == -1 || right_black_height == -1 ||
            left_black_height != right_black_height) {
                printf("Black height violation at node with data %d\n", node->data);
            return -1; // Black height violation
        }

        return left_black_height + (node->color == Color::BLACK ? 1 : 0);
    }
    
    void print_helper(Node<T> * node, int depth = 0) {
        if (node != nullptr) {
           for(int i = 0; i < depth; i++) {
               std::cout << "   ";
           }
           std::cout << node->data << "(" << (node->color == Color::RED ? "R" : "B") << ")" << std::endl;
           print_helper(node->left, depth + 1);
           print_helper(node->right, depth + 1);    
        }
    }

    Node<T> * find_node(T data) {
        Node<T> * current = root;
        while(current != nullptr) {
            if(data == current->data) {
                return current;
            } else if(data < current->data) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return nullptr;
    }

    void setColor(Node<T> * n, Color c) {
        if(n != nullptr) {
            n->color = c;
        }
    }

    Color getColor(Node<T> * n) {
        if(n == nullptr) {
            return Color::BLACK;
        } else {
            return n->color;
        }
    }

    void start_balance_test(Node<T> * parent, Direction dir) {
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
                start_balance_test(parent->parent, parent_dir);
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
                start_balance_test(parent, dir);
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
                start_balance_test(parent, dir);
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
            start_balance_test(parent, dir);
        }
        else {
            #if DEBUG
            std::cout << "Simple remove called." << std::endl << std::flush;
            #endif
            simple_remove(node);
        }
    }

    Color get_color(Node<T>* n) {
        return getColor(n);
    }

    void set_color(Node<T>* n, Color c) {
        setColor(n, c);
    }

    public:
    Seq_BRTree<T>() {
        root = nullptr;
    }
    ~Seq_BRTree() {
        destructor_helper(root);
    }

    void insert(T data) {
        if(root == nullptr) {
            root = new Node<T>(data, Color::BLACK);
            return;
        }   
        insert_helper(root, data);
        if(root != nullptr) {
            set_color(root, Color::BLACK);
        }
    }
    bool contains(T data) {
        return (find_node(data) != nullptr) ? true : false;
    }
    void remove(T data) {
        Node<T> * node = find_node(data);
        if(node == nullptr) {
            std::cout << "Node with data " << data << " not found." << std::endl;
            return;  
        }
        remove_helper(node);
        if(root != nullptr) {
            set_color(root, Color::BLACK);
        }
    }


    bool verify_tree() {
        if (verify_helper(root) < 1) {
            return false;
        };
        return true;
    }

    void print_tree() {
        // In-order traversal to print the tree
        if(root == nullptr) {
            std::cout << "Tree is empty." << std::endl;
            return;
        }
        print_helper(root,0);
    }

private:
    Node<T>* root;

};
} // namespace sql_brtree
#endif // SEQUENTIAL_BRTREE_H