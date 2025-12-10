#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <functional>
#include <random>
#include <iomanip>
#include <algorithm>
#include <omp.h>

#include "concurrent_brtree.h"

bool test_single_add();
bool test_medium_add();

void run_test(std::string message,std::function<bool()>test_func) {
    std::cout <<  "Starting Test: " << std::left << std::setw(40) << message << std::flush;
    if (test_func()) {
        std::cout << "[PASS] " << std::endl;
    } else {
        std::cout << "[FAIL] " << std::endl;
    }

}

int main() {
    printf("Starting BRTree tests...\n\n");

    //run_test(std::string("Single BRTree Add Test"), test_single_add);
    run_test(std::string("Single BRTree Add Test"), test_medium_add);
    printf("\nAll Tests Finished\n");
    return 0;
}


bool test_single_add() {

    omp_set_num_threads(2);
    Cur_BRTree<int> tree;

    std::vector<int> v;

    v.push_back(2);
    v.push_back(4);
    v.push_back(3);
    v.push_back(6);

    int size = v.size();
    #pragma omp parallel for shared(tree)
    for(int i = 0; i < size; i++) {
        //std::cout << "Thread " << omp_get_thread_num() << " inserting " << v.at(i) << std::endl;
        tree.insert(v.at(i));
        tree.print_tree();
    }
    tree.verify_tree();
    //tree.print_tree();
    for(int i = 0; i< v.size(); i++) {
        if(!tree.contains(v.at(i))) {
            std::cout << "Tree does not contain " << v.at(i) << std::endl << std::flush;
            return false;
        }
    }

    if(!tree.verify_tree()) {
        std::cout << "Tree does not verify" << std::endl << std::flush;
        return false;
    }

   // tree.print_tree();
    return true;
}

bool test_medium_add() {

    bool pass = true;
    Cur_BRTree<int> tree;
    std::vector<int> v;

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, 1000000);

    int NUM_ELEMENTS = 100;
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        int val;
        while(true) {
            val = dist(rng);
            if(!std::count(v.begin(),v.begin()+v.size(),val)) {
                break;
            }
        }
        v.push_back(val);
    }

    omp_set_num_threads(2);
#pragma omp parallel for
    for(int i = 0; i < NUM_ELEMENTS; i++) {
        std::cout << "Thread " << omp_get_thread_num() << " inserting " << v.at(i) << std::endl;
        tree.insert(v.at(i));
        //tree.print_tree();
    }

    tree.print_tree();
    for(int i = 0; i< v.size(); i++) {
        if(!tree.contains(v.at(i))) {
            std::cout << "Tree does not contain " << v.at(i) << std::endl << std::flush;
            pass = false;
        }
    }

    if(!tree.verify_tree()) {
        std::cout << "Tree does not verify" << std::endl << std::flush;
        pass = false;
    }

   // tree.print_tree();
    return pass;
}