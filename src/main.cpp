#include <iostream>
#include <memory>
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
    std::unique_ptr<Expr> firstTreeHead = randomNode(optionsMux6, headDepth);
    std::unique_ptr<Expr> secondTreeHead = randomNode(optionsMux6, headDepth);
    std::cout << "Depth: " << firstTreeHead->computeDepth()
              << ", Logic Size: " << firstTreeHead->computeLogicSize()
              << ", Truth: " << firstTreeHead->evaluate(mux6) << std::endl;
    std::cout << firstTreeHead->prettyPrint() << std::endl;
    std::cout << "Depth: " << secondTreeHead->computeDepth()
              << ", Logic Size: " << secondTreeHead->computeLogicSize()
              << ", Truth: " << secondTreeHead->evaluate(mux6) << std::endl;
    std::cout << secondTreeHead->prettyPrint() << std::endl;
    Expr* first = firstTreeHead->retrieveArbitraryNode(4.0 / firstTreeHead->computeLogicSize());
    Expr* second = secondTreeHead->retrieveArbitraryNode(4.0 / secondTreeHead->computeLogicSize());
    std::cout << first << std::endl;
    std::cout << second << std::endl;
    if (first != nullptr && second != nullptr) {
        // This is recombination; TODO: make it it's own function
        std::unique_ptr<Expr> firstChild = first->ownRandomChild();
        std::unique_ptr<Expr> secondChild = second->ownRandomChild();
        first->returnChildOwnership(std::move(secondChild));
        second->returnChildOwnership(std::move(firstChild));
        std::cout << firstTreeHead->prettyPrint() << std::endl;
        std::cout << secondTreeHead->prettyPrint() << std::endl;
    }
    return 0;
}
