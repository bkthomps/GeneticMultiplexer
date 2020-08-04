#include <bitset>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include "constants.h"
#include "expressions.h"

int calculateCombinations(int length) {
    int ret = 1;
    for (int i = 0; i < length; i++) {
        ret *= 2;
    }
    return ret;
}

double fitness(Expr* head, int addressCount, const std::vector<const std::string>& options) {
    assert(options.size() <= 16);
    unsigned long startIndex = 16 - options.size();
    int combinations = calculateCombinations(options.size());
    std::unordered_map<std::string, bool> mux{};
    double correct = 0;
    for (int i = 0; i < combinations; i++) {
        std::string str = std::bitset<16>(i).to_string();
        for (unsigned long j = 0; j < options.size(); j++) {
            mux.emplace(options[j], str[startIndex + j] - '0');
        }
        auto currentAddress = str.substr(startIndex, addressCount);
        unsigned long address = std::bitset<16>(currentAddress).to_ulong();
        bool actualTruth = str[startIndex + addressCount + address] == '1';
        bool predictedTruth = head->evaluate(mux);
        if (actualTruth == predictedTruth) {
            correct++;
        }
        mux.clear();
    }
    return correct / combinations;
}

int main() {
    std::vector<const std::string> optionsMux6 = {"a0", "a1", "d0", "d1", "d2", "d3"};
    std::unique_ptr<Expr> first = randomNode(optionsMux6, initialDepth);
    std::cout << fitness(first.get(), 2, optionsMux6) << std::endl;
    std::unique_ptr<Expr> second = randomNode(optionsMux6, initialDepth);
    auto tuple = performRecombination(first.get(), second.get());
    std::cout << first->prettyPrint() << std::endl;
    std::cout << second->prettyPrint() << std::endl;
    std::cout << std::get<0>(tuple)->prettyPrint() << std::endl;
    std::cout << std::get<1>(tuple)->prettyPrint() << std::endl;
    return 0;
}
