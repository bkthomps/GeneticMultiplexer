#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include "constants.h"
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
    std::vector<const std::string> optionsMux6 = {"a0", "a1", "d0", "d1", "d2", "d3"};
    std::unique_ptr<Expr> first = randomNode(optionsMux6, initialDepth);
    std::unique_ptr<Expr> second = randomNode(optionsMux6, initialDepth);
    auto tuple = performRecombination(first.get(), second.get());
    std::cout << first->prettyPrint() << std::endl;
    std::cout << second->prettyPrint() << std::endl;
    std::cout << std::get<0>(tuple)->prettyPrint() << std::endl;
    std::cout << std::get<1>(tuple)->prettyPrint() << std::endl;
    return 0;
}
