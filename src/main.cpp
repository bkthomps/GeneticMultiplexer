#include <iostream>
#include <unordered_map>
#include <vector>
#include "expressions.h"

int main() {
    const std::unordered_map<std::string, bool> mux6 = {
            {"a0", false},
            {"a1", false},
            {"d0", false},
            {"d1", false},
            {"d2", false},
            {"d3", false},
    };
    std::vector<std::string> optionsMux6 = {"a0", "a1", "d0", "d1", "d2", "d3"};
    int headDepth = 5;
    std::unique_ptr<Expr> head = randomNode(optionsMux6, headDepth);
    int depth = head->computeDepth();
    int size = head->computeSize();
    bool truth = head->evaluate(mux6);
    std::cout << "Depth: " << depth << ", Size: " << size << ", Truth: "
              << truth << std::endl;
    return 0;
}
