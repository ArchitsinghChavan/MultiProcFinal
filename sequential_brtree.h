#ifndef SEQUENTIAL_BRTREE_H
#define SEQUENTIAL_BRTREE_H

#include <stdio.h>
#include <iostream>
#include "KVPair.h"
#include "Node.h"
namespace sql_brtree {
  
template <typename T>
class Seq_BRTree {

private:
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

    void start_balance(Direction dir, Node<T> * node) {
        Node<T> * parent = node->parent;
        Node<T>* sibling, * close_nephew, * distant_nephew;

        sibling = parent->get_opposite_child(dir);
        distant_nephew = sibling->get_opposite_child(dir);
        close_nephew = sibling->get_child(dir);
        if(sibling->color == Color::RED) {
            //Case 3:
            rotate_tree(parent, dir);
            parent->color = Color::RED;
            sibling->color = Color::BLACK;
            sibling = close_nephew;
            
            distant_nephew = sibling->get_opposite_child(dir);

            if(distant_nephew && distant_nephew->color == Color::RED) {
                //Case 6:
                rotate_tree(parent, dir);
                sibling->color = parent->color;
                parent->color = Color::BLACK;
                distant_nephew->color = Color::BLACK;
                return;
            }
            close_nephew = sibling->get_opposite_child(dir);
            
            if(close_nephew && close_nephew->color == Color::RED) {
                //Case 5:
                rotate_tree(sibling, (dir == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT);
                close_nephew->color = Color::BLACK;
                sibling->color = Color::RED;
                sibling = close_nephew;

                rotate_tree(parent, dir);
                sibling->color = parent->color;
                parent->color = Color::BLACK;
                distant_nephew->color = Color::BLACK;
                return;
            }

            sibling->color = Color::RED;
            parent->color = Color::BLACK;
            return;
        }

        if(distant_nephew && distant_nephew->color == Color::RED) {
            //Case 6:
            rotate_tree(parent, dir);
            sibling->color = parent->color;
            parent->color = Color::BLACK;
            distant_nephew->color = Color::BLACK;
            return;
        }

        if(close_nephew && close_nephew->color == Color::RED) {
            //Case 5:
            rotate_tree(sibling, (dir == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT);
            close_nephew->color = Color::BLACK;
            sibling->color = Color::RED;
            sibling = close_nephew;

            rotate_tree(parent, dir);
            sibling->color = parent->color;
            parent->color = Color::BLACK;
            distant_nephew->color = Color::BLACK;
            return;
        }

        //Case 1:
        if(!parent)
            return;

        if(parent->color == Color::RED) {
            sibling->color = Color::RED;
            parent->color = Color::BLACK;
            return;
        }

        sibling->color = Color::RED;
        node = parent;
        if(node->parent != nullptr) {
            start_balance((node->parent->left == node) ? Direction::LEFT : Direction::RIGHT, node);
        }
    }

    bool simple_remove(Node<T> * node) {
        if(node->left != nullptr && node->right != nullptr) {
            //Node has two children, find in-order successor
            Node<T> * successor = node->right;
            while(successor->left != nullptr) {
                successor = successor->left;
            }
            if(successor->right != nullptr) {
                successor->right->color = Color::BLACK;
            }
            successor->parent->left = successor->right;
            node->data = successor->data;
            delete successor;
            return true;
        }
        if(node->left != nullptr || node->right != nullptr) {
            //Node has one child
            Node<T> * child = (node->left != nullptr) ? node->left : node->right;
            child->color = Color::BLACK;
            Node<T> * parent = node->parent;
            if(parent != nullptr) {
                if(parent->left == node) {
                    parent->left = child;
                } else {
                    parent->right = child;
                }
                child->parent = parent;
            } else {
                //node is root
                root = child;
                child->parent = nullptr;
            }
            delete node;
            return true;
        }
        return false;
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
        if (simple_remove(node)) {return;}

        
        Node<T> * parent = node->parent;
        if(parent == nullptr) {
            //node is root
            ;
            root = nullptr;
            return;
        }
        Direction dir = (parent->left == node) ? Direction::LEFT : Direction::RIGHT;

        parent->set_child(dir, nullptr);

        start_balance(dir, parent);


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