#include "sequential_brtree.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <functional>
#include <random>
bool test_small_add();
bool test_large_add();
bool test_small_remove();
bool test_tiny_remove();

void run_test(std::string message,std::function<bool()>test_func) {
    std::cout << "Starting Test: " << message << "\t" << std::flush;
    if (test_func()) {
        std::cout << "[PASS] " << std::endl;
    } else {
        std::cout << "[FAIL] " << std::endl;
    }

}

int main() {
    printf("Starting BRTree tests...\n\n");

    run_test(std::string("Small BRTree Add Test"), test_small_add);
    run_test(std::string("Large BRTree Add Test"), test_large_add);
    run_test(std::string("Tiny BRTree Remove Test"), test_tiny_remove);
    //run_test(std::string("Small BRTree Remove Test"), test_small_remove);
    printf("\nAll Tests Finished\n");
    return 0;
}

bool test_small_add() {
    sql_brtree::Seq_BRTree<int> tree;

    tree.insert(10);
    tree.insert(20);
    tree.insert(5);
    tree.insert(15);
    tree.insert(25);
    tree.insert(1);
    tree.insert(7);

    if (!tree.verify_tree()) {
        std::cout << "Tree verification failed after insertions." << std::endl;
        return false;
    }
    //tree.print_tree();
    return true;
}

bool test_large_add() {
    sql_brtree::Seq_BRTree<int> tree;
    const int NUM_ELEMENTS = 10000;
    std::vector<int> values;

    // Generate random values
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<int> dist(1, 100000);

    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        int val = dist(rng);
        values.push_back(val);
        tree.insert(val);
    }
    if (!tree.verify_tree()) {
        std::cout << "Tree verification failed after insertions." << std::endl;
        return false;
    }
    //tree.print_tree();
    return true;
}

bool test_small_remove() {
    sql_brtree::Seq_BRTree<int> tree;

    tree.insert(10);
    tree.insert(20);
    tree.insert(5);
    tree.insert(15);
    tree.insert(25);
    tree.insert(1);
    tree.insert(7);

    tree.remove(15);
    tree.remove(10);
    tree.remove(5);

    if (!tree.verify_tree()) {
        std::cout << "Tree verification failed after removals." << std::endl;
        return false;
    }

    if( tree.contains(15) || tree.contains(10) || tree.contains(5)) {
        std::cout << "Tree still contains removed elements." << std::endl;
        return false;
    }
    //tree.print_tree();
    return true;
}

bool test_tiny_remove() {
    sql_brtree::Seq_BRTree<int> tree;

    tree.insert(10);
    tree.remove(10);

    tree.insert(20);
    tree.insert(10);
    tree.print_tree();
    tree.remove(20);

    if(tree.contains(20) || !tree.contains(10)) {
        std::cout << "Tree removal or insertion failed." << std::endl;
        tree.print_tree();
        return false;
    }


    if (!tree.verify_tree()) {
        std::cout << "Tree verification failed after removal." << std::endl;
        return false;
    }

    //tree.print_tree();
    return true;
}