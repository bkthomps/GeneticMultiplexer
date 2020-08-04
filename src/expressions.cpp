#include <random>
#include <stdexcept>
#include <algorithm>
#include "expressions.h"

int uniformRoundedInclusiveBounds(int low, int high) {
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_int_distribution<int> distribution(low, high);
    return distribution(generator);
}

std::unique_ptr<Expr> randomNode(const std::vector<std::string>& terminalOptions, int depth) {
    if (depth == 0) {
        return std::make_unique<Terminal>(terminalOptions);
    }
    int rand = uniformRoundedInclusiveBounds(0, 3);
    switch (rand) {
        case 0:
            return std::make_unique<Not>(terminalOptions, depth);
        case 1:
            return std::make_unique<And>(terminalOptions, depth);
        case 2:
            return std::make_unique<Or>(terminalOptions, depth);
        case 3:
            return std::make_unique<If>(terminalOptions, depth);
        default:
            throw std::runtime_error{"Invalid distribution"};
    }
}

Not::Not(const std::vector<std::string>& terminalOptions, int depth) {
    expr = randomNode(terminalOptions, depth - 1);
}

int Not::computeDepth() {
    return 1 + expr->computeDepth();
}

int Not::computeSize() {
    return 1 + expr->computeSize();
}

bool Not::evaluate(const std::unordered_map<std::string, bool>& truthMap) {
    return !expr->evaluate(truthMap);
}

std::string Not::prettyPrint() {
    return "( NOT " + expr->prettyPrint() + " )";
}

And::And(const std::vector<std::string>& terminalOptions, int depth) {
    first = randomNode(terminalOptions, depth - 1);
    second = randomNode(terminalOptions, depth - 1);
}

int And::computeDepth() {
    return 1 + std::max(first->computeDepth(), second->computeDepth());
}

int And::computeSize() {
    return 1 + first->computeSize() + second->computeSize();
}

bool And::evaluate(const std::unordered_map<std::string, bool>& truthMap) {
    return first->evaluate(truthMap) && second->evaluate(truthMap);
}

std::string And::prettyPrint() {
    return "( " + first->prettyPrint() + " AND " + second->prettyPrint() + " )";
}

Or::Or(const std::vector<std::string>& terminalOptions, int depth) {
    first = randomNode(terminalOptions, depth - 1);
    second = randomNode(terminalOptions, depth - 1);
}

int Or::computeDepth() {
    return 1 + std::max(first->computeDepth(), second->computeDepth());
}

int Or::computeSize() {
    return 1 + first->computeSize() + second->computeSize();
}

bool Or::evaluate(const std::unordered_map<std::string, bool>& truthMap) {
    return first->evaluate(truthMap) || second->evaluate(truthMap);
}

std::string Or::prettyPrint() {
    return "( " + first->prettyPrint() + " OR " + second->prettyPrint() + " )";
}

If::If(const std::vector<std::string>& terminalOptions, int depth) {
    condition = randomNode(terminalOptions, depth - 1);
    trueCase = randomNode(terminalOptions, depth - 1);
    falseCase = randomNode(terminalOptions, depth - 1);
}

int If::computeDepth() {
    int partialDepth = std::max(trueCase->computeDepth(), falseCase->computeDepth());
    return 1 + std::max(condition->computeDepth(), partialDepth);
}

int If::computeSize() {
    return 1 + condition->computeSize() + trueCase->computeSize() + falseCase->computeSize();
}

bool If::evaluate(const std::unordered_map<std::string, bool>& truthMap) {
    return condition->evaluate(truthMap) ? trueCase->evaluate(truthMap)
                                         : falseCase->evaluate(truthMap);
}

std::string If::prettyPrint() {
    return "( IF " + condition->prettyPrint() + " THEN " + trueCase->prettyPrint()
           + " ELSE " + falseCase->prettyPrint() + " )";
}

Terminal::Terminal(const std::vector<std::string>& terminalOptions) {
    unsigned long high = terminalOptions.size() - 1;
    int rand = uniformRoundedInclusiveBounds(0, static_cast<int>(high));
    terminal = terminalOptions[rand];
}

int Terminal::computeDepth() {
    return 0;
}

int Terminal::computeSize() {
    return 1;
}

bool Terminal::evaluate(const std::unordered_map<std::string, bool>& truthMap) {
    return truthMap.at(terminal);
}

std::string Terminal::prettyPrint() {
    return terminal;
}
