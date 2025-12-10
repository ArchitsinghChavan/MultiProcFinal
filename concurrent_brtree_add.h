 
#ifndef BRTREE_ADD
#define BRTREE_ADD
#include "Concurrent_Node.h"
#include <omp.h>
#include <thread>

#ifndef OMP_WORK
#define OMP_WORK 0
#endif


enum class AddCase {CASE1,CASE2,CASE3,CASE4,CASE5,CASE6};

/*
bool acquire_add_nodes(Node<T> * parent) {
    if(omp_get_thread_num() == 3) {
    //std::cout << "Thread " << omp_get_thread_num() << " acquiring " << parent->data << std::endl;
    }
    //if parent exists
    //std::cout << "parent is " << parent->data << std::endl;
    if(parent != nullptr) {
        //attempt to acquire flag
        int flag_expected = -1;
        __atomic_compare_exchange_n(&parent->flag, &flag_expected, omp_get_thread_num(), false, 5, 5);

        if(parent->flag != omp_get_thread_num()) {
            //std::cout << omp_get_thread_num << "Failed to acquire parent = " << parent->data << ", flag is " << parent->flag << std::endl;
            return false; //failed to acquire parent
        }
        //parent acquired

        //if grandparent exists
        Node<T> * grandparent = parent->parent;
        if(grandparent != nullptr) {
            flag_expected = -1;
            __atomic_compare_exchange_n(&grandparent->flag, &flag_expected, omp_get_thread_num(), false, 5, 5);
            if(grandparent->flag != omp_get_thread_num()) {
                //std::cout << omp_get_thread_num<< "Failed to acquire grandparent = " << grandparent->data << ", flag is " << grandparent->flag << std::endl;
                // Failed to acquire grandparent - must release parent before returning
                int parent_release = omp_get_thread_num();
                __atomic_compare_exchange_n(&parent->flag, &parent_release, -1, false, 5, 5);
                return false; //failed to acquire grandparent
            }
            //grandparent acquired
            Node<T> * uncle = grandparent->get_child((grandparent->left == parent) ? Direction::RIGHT: Direction::LEFT);
            if(uncle == nullptr) {
                //std::cout << omp_get_thread_num() << " Acquired" << std::endl;
                return true;
            }
            
            while(uncle->flag != omp_get_thread_num()) {
                flag_expected = -1;
                __atomic_compare_exchange_n(&uncle->flag, &flag_expected, omp_get_thread_num(), false, 5, 5);
                if(uncle->flag != omp_get_thread_num()) {
                //std::cout << omp_get_thread_num << " Failed to acquire uncle = " << uncle->data << ", flag is " << uncle->flag << std::endl;
                std::this_thread::yield();
                }
                else {
                    break;
                }
            }
        }
        //std::cout << omp_get_thread_num() << " Acquired" << std::endl;
        return true;
    }

    //std::cout << "Error: Trying to acquire root" << std::endl << std::flush;
    return false;
}

bool release_add_nodes(Node<T> * parent) {

    if(parent == nullptr) {
        //std::cout << omp_get_thread_num() << " ReleasedP" << std::endl;
        return true;
    }
    int thread_num;



    Node<T> * grandparent = parent->parent;
    if(grandparent != nullptr) {
        Node<T> * uncle = grandparent->get_child((grandparent->left == parent) ? Direction::RIGHT: Direction::LEFT);
        if(uncle != nullptr) {
            thread_num = omp_get_thread_num();
            if(uncle->flag == thread_num)
                __atomic_compare_exchange_n(&uncle->flag, &thread_num, -1, false, 5, 5);
            if(uncle->flag == omp_get_thread_num()) {
                //std::cout << omp_get_thread_num() << " Failed to release uncle" << std::endl << std::flush;
            }
            else {
                //std::cout << "Uncle released, flag = " << uncle->flag << std::endl;
            }

        }
        thread_num = omp_get_thread_num();
        if(grandparent->flag == thread_num)
            __atomic_compare_exchange_n(&grandparent->flag, &thread_num, -1, false, 5, 5);
        if(grandparent->flag == omp_get_thread_num()) {
            std::cout << omp_get_thread_num() << " Failed to release grandparent" << std::endl << std::flush;
        }
    }

    thread_num = omp_get_thread_num();
    if(parent->flag == thread_num)
        __atomic_compare_exchange_n(&parent->flag, &thread_num, -1, false, 5, 5);
    if(parent->flag == omp_get_thread_num()) {
        //std::cout << "Paraent flag = " << parent->flag << "should be" << omp_get_thread_num() << std::endl;
        //std::cout << omp_get_thread_num() << " Failed to release parent" << std::endl << std::flush;
    }
    //if(omp_get_thread_num() != 1)
    //std::cout << omp_get_thread_num() << " Released" << std::endl;
    return true;

}


*/
void fix_tree(Node<T> * parent, Direction dir) {
    std::cout << "fixing tree" << std::endl;
    //Case #1: Parent is black - Handled in call
    if(parent == nullptr) {
        std::cout << "parent is null" << std::endl;
    }
    if(parent->flag != omp_get_thread_num()) {
        std::cout << "thread doesn't own parent" << std::endl;
    }
    if(parent->color == Color::BLACK) {
        return;
    }

    Node<T> * grandparent = parent->parent;
    //case 4: grandparent is null
    if(grandparent == nullptr) {
        std::cout << "No grandpa" << std::endl;
        parent->color = Color::BLACK;
        return;
    }

    if(grandparent->flag != omp_get_thread_num()) {
        std::cout << "thread doesn't own grandparent" << std::endl;
    }

    Direction p_dir = (parent == grandparent->left) ? Direction::LEFT : Direction::RIGHT;
    Node<T> * uncle = grandparent->get_opposite_child(p_dir);
    if((uncle == nullptr) || (uncle->color == Color::BLACK)) {
        if((uncle != nullptr) && uncle->flag != omp_get_thread_num()) {
            std::cout << uncle->data << "unc" << std::endl;
            std::cout << "Thread doesn't own uncle in fix" << std::endl;
        }
        
        std::cout << "Case 5" << std::endl;
        if(p_dir != dir) {
            rotate_tree(parent, p_dir);
            parent->flag = -1;
            parent = grandparent->get_child(p_dir);
        }
        //Case #6
        std::cout << "Case 6" << std::endl;
        rotate_tree(grandparent, (p_dir == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT);
        parent->color = Color::BLACK;
        grandparent->color = Color::RED;
        parent->flag = -1;
        if(uncle)
            uncle->flag = -1;
        parent->parent->flag = -1;
        parent->parent->left->flag = -1;
        parent->parent->right->flag = -1;
        return;

    }
    // Case #2
    parent->color = Color::BLACK;
    uncle->color = Color::BLACK;
    grandparent->color = Color::RED;
    if(grandparent->parent != nullptr) {
        if(grandparent->parent->color == Color::RED) {
            while(grandparent->parent && (grandparent->parent->flag != omp_get_thread_num())) {
                int expected = -1;
                __atomic_compare_exchange_n(&grandparent->parent->flag, &expected, omp_get_thread_num(), false, 5, 5);
                std::this_thread::yield();
            }
            Direction dir = (grandparent == grandparent->parent->left) ? Direction::LEFT: Direction::RIGHT;
            //If grandparent succesfully acquired
            if(grandparent->parent && (grandparent->parent->flag == omp_get_thread_num())) {
                std::cout << "Acquired new grandparent" << std::endl;
            }
            while(grandparent->parent->get_opposite_child(dir) && (grandparent->parent->get_opposite_child(dir)->flag != omp_get_thread_num())) {
                int expected = -1;
                __atomic_compare_exchange_n(&grandparent->parent->get_opposite_child(dir)->flag, &expected, omp_get_thread_num(), false, 5, 5);
                std::this_thread::yield();
            }
            if(grandparent->parent->get_opposite_child(dir) && (grandparent->parent->get_opposite_child(dir)->flag == omp_get_thread_num())) {
                std::cout << "Uncle new acquired" << std::endl;
            }

            fix_tree(grandparent, (grandparent->parent->left == grandparent) ? Direction::LEFT: Direction::RIGHT);
            if(grandparent->parent)
            grandparent->parent->flag = -1;
            if(grandparent->parent->get_opposite_child(dir))
            grandparent->parent->get_opposite_child(dir)->flag;
        }
    }
}

bool insert_helper(Node<T> * node, Node<T> * new_node) {
    //std::cout << omp_get_thread_num() << ": Inserting " << new_node->data << std::endl;
    if(node->data == new_node->data) {
        std::cout << omp_get_thread_num() << ": Already in tree" << std::endl;
        node->data = new_node->data;
        int expected = omp_get_thread_num();
        __atomic_compare_exchange_n(&node->flag, &expected, -1, false, 5, 5);
        if(expected != omp_get_thread_num()) {
            std::cout << omp_get_thread_num() << ": Error: Insert Helper called on node that wasn't owned" << std::endl;
        }
        node->flag = -1;
        return true;
    }

    //Find direction
    Direction dir = (new_node->data < node->data) ? Direction::LEFT: Direction::RIGHT;


    Node<T> * child = node->get_child(dir);
    
    if(child == nullptr) {
        std::cout << omp_get_thread_num() << ": Found Spot" << std::endl;
        if(node->parent == nullptr) {
            std::cout << "Node is root" << std::endl;
            node->color = Color::BLACK;
            node->set_child(dir, new_node);
            new_node->parent = node;
            node->flag = -1;
            return true;
        }
        else {
            std::cout << "Node is not root" << std::endl;
            node->set_child(dir, new_node);
            new_node->parent = node;

            //Attempt to acquire grandparent
            while(node->parent && (node->parent->flag != omp_get_thread_num())) {
                int expected = -1;
                __atomic_compare_exchange_n(&node->parent->flag, &expected, omp_get_thread_num(), false, 5, 5);
                std::this_thread::yield();
            }
            Direction dir = (node == node->parent->left) ? Direction::LEFT: Direction::RIGHT;
            //If grandparent succesfully acquired
            if(node->parent && (node->parent->flag == omp_get_thread_num())) {
                std::cout << "Acquired grandparent" << std::endl;
            }
            while(node->parent->get_opposite_child(dir) && (node->parent->get_opposite_child(dir)->flag != omp_get_thread_num())) {
                int expected = -1;
                __atomic_compare_exchange_n(&node->parent->get_opposite_child(dir)->flag, &expected, omp_get_thread_num(), false, 5, 5);
                std::this_thread::yield();
            }
            if(node->parent->get_opposite_child(dir) && (node->parent->get_opposite_child(dir)->flag == omp_get_thread_num())) {
                std::cout << "Uncle acquired" << std::endl;
            }
            if(node->color == Color::RED) {
                //fix_tree(node, dir);
            }
            if(node->parent) {
                std::cout << "should release now" << std::endl;
                if(node->parent->get_opposite_child(dir)) {
                    if(node->parent->get_opposite_child(dir)->flag == omp_get_thread_num()) {
                        std::cout << "Releasing Uncle" << std::endl;
                        node->parent->get_opposite_child(dir)->flag = -1;
                    }
                }
                if(node->parent->flag == omp_get_thread_num()) {
                    std::cout << "Releasing Grandparent" << std::endl;
                    node->parent->flag = -1;
                }
            }
            node->flag = -1;
            return true;
        }
    }

    else {
        //std::cout << "Recurseive" << std::endl;
        int expected = -1;
        __atomic_compare_exchange_n(&child->flag, &expected, omp_get_thread_num(), false, 5, 5);
        if(expected != -1) {
            node->flag = -1;
            std::cout <<"Restart" << child->flag << std::endl;
            //return false;
        }

         node->flag = -1; 

        return insert_helper(child, new_node);

    }
    node->flag = -1;
    return true;
}

#endif //BRTREE_ADD