#include "sequential_brtree.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <functional>
#include <random>
bool test_small_add();
bool test_large_add();

void run_test(std::string message,std::function<bool()>test_func) {
    std::cout << "Starting Test: " << message << "\t";
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