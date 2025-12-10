
#include <stdio.h>
#include <iostream>
#include <thread>
#include "KVPair.h"
#include "Concurrent_Node.h"
#include <omp.h>
#define DEBUG 0


template <typename T>
class Cur_BRTree {
private:
    Node<T>* root;
    Node<T> * flyweight;
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


    int verify_helper(Node<T> * node) {
        if (node == nullptr) {
            return 1; // Black height of null nodes is 1
        }

        if(node->flag != -1) {
            std::cout<< "Flag on " << node->data << "remains set to a thread number" << std::endl << std::flush;
            return -1;
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

//#include "concurrent_brtree_remove.h"

    Color get_color(Node<T>* n) {
        return getColor(n);
    }

    void set_color(Node<T>* n, Color c) {
        setColor(n, c);
    }

    public:
    Cur_BRTree<T>() {
        root = nullptr;
    }
    ~Cur_BRTree() {
        destructor_helper(root);
    }

    void emergency_unlock(Node<T> * node) {
        if(node == nullptr)
            return;
        emergency_unlock(node->left);
        emergency_unlock(node->right);
        if(node->flag == omp_get_thread_num()) {
            node->flag = -1;
        }

    }

#include "concurrent_brtree_add.h"
    void insert(T data) {
        //replace root with new node
        Node<T> * new_node = new Node<T>(data, Color::RED);
        if(root == nullptr) {
            void * p = nullptr;
            __atomic_compare_exchange(&root, &p, &new_node, false, 5, 5);
            if(p == nullptr) 
                return;
        }

        while(root->flag != omp_get_thread_num()) {
            int expected = -1;
            __atomic_compare_exchange_n(&root->flag, &expected, omp_get_thread_num(), false, 5, 5);
            std::this_thread::yield();
        }

        std::cout << "Acquire root" << std::endl;

        //Let insert_helper handle all locking
        while(!insert_helper(root, new_node)){
            //emergency_unlock(root);
            while(root->flag != omp_get_thread_num()) {
                //emergency_unlock(root);
                int expected = -1;
                __atomic_compare_exchange_n(&root->flag, &expected, omp_get_thread_num(), false, 5, 5);
                std::cout << "Expected: " << root->data << std::endl; 
                std::this_thread::yield();
            }
            std::this_thread::yield();
            //emergency_unlock(root);

        }
        std::cout << omp_get_thread_num() << ": Insert FInished" << std::endl;
        //emergency_unlock(root);
        
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
};
