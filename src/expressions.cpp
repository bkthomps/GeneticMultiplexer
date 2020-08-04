#include <algorithm>
#include <random>
#include <stdexcept>
#include "expressions.h"

std::random_device seed;
std::mt19937 generator(seed());

int uniformIntegerInclusiveBounds(int low, int high) {
    std::uniform_int_distribution<int> distribution(low, high);
    return distribution(generator);
}

double uniformReal() {
    std::uniform_real_distribution<double> distribution(0, 1);
    return distribution(generator);
}

std::unique_ptr<Expr> randomNode(const std::vector<std::string>& terminalOptions, int depth) {
    if (depth == 0) {
        return std::make_unique<Terminal>(terminalOptions);
    }
    int rand = uniformIntegerInclusiveBounds(0, 3);
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
            assert(false);
    }
}

Not::Not(const std::vector<std::string>& terminalOptions, int depth) {
    expr = randomNode(terminalOptions, depth - 1);
}

int Not::computeDepth() {
    return 1 + expr->computeDepth();
}

int Not::computeLogicSize() {
    return 1 + expr->computeLogicSize();
}

bool Not::evaluate(const std::unordered_map<std::string, bool>& truthMap) {
    return !expr->evaluate(truthMap);
}

std::string Not::prettyPrint() {
    return "( NOT " + expr->prettyPrint() + " )";
}

Expr* Not::retrieveArbitraryNode(double probability) {
    double rand = uniformReal();
    if (rand < probability) {
        return this;
    }
    return expr->retrieveArbitraryNode(probability);
}

std::unique_ptr<Expr> Not::ownRandomChild() {
    return std::move(expr);
}

void Not::returnChildOwnership(std::unique_ptr<Expr> child) {
    expr = std::move(child);
}

And::And(const std::vector<std::string>& terminalOptions, int depth) {
    first = randomNode(terminalOptions, depth - 1);
    second = randomNode(terminalOptions, depth - 1);
}

int And::computeDepth() {
    return 1 + std::max(first->computeDepth(), second->computeDepth());
}

int And::computeLogicSize() {
    return 1 + first->computeLogicSize() + second->computeLogicSize();
}

bool And::evaluate(const std::unordered_map<std::string, bool>& truthMap) {
    return first->evaluate(truthMap) && second->evaluate(truthMap);
}

std::string And::prettyPrint() {
    return "( " + first->prettyPrint() + " AND " + second->prettyPrint() + " )";
}

Expr* And::retrieveArbitraryNode(double probability) {
    double rand = uniformReal();
    if (rand < probability) {
        return this;
    }
    int choice = uniformIntegerInclusiveBounds(0, 1);
    switch (choice) {
        case 0:
            return first->retrieveArbitraryNode(probability);
        case 1:
            return second->retrieveArbitraryNode(probability);
        default:
            assert(false);
    }
}

std::unique_ptr<Expr> And::ownRandomChild() {
    int choice = uniformIntegerInclusiveBounds(0, 1);
    switch (choice) {
        case 0:
            return std::move(first);
        case 1:
            return std::move(second);
        default:
            assert(false);
    }
}

void And::returnChildOwnership(std::unique_ptr<Expr> child) {
    assert(first == nullptr ^ second == nullptr);
    if (first == nullptr) {
        first = std::move(child);
    } else {
        second = std::move(child);
    }
}

Or::Or(const std::vector<std::string>& terminalOptions, int depth) {
    first = randomNode(terminalOptions, depth - 1);
    second = randomNode(terminalOptions, depth - 1);
}

int Or::computeDepth() {
    return 1 + std::max(first->computeDepth(), second->computeDepth());
}

int Or::computeLogicSize() {
    return 1 + first->computeLogicSize() + second->computeLogicSize();
}

bool Or::evaluate(const std::unordered_map<std::string, bool>& truthMap) {
    return first->evaluate(truthMap) || second->evaluate(truthMap);
}

std::string Or::prettyPrint() {
    return "( " + first->prettyPrint() + " OR " + second->prettyPrint() + " )";
}

Expr* Or::retrieveArbitraryNode(double probability) {
    double rand = uniformReal();
    if (rand < probability) {
        return this;
    }
    int choice = uniformIntegerInclusiveBounds(0, 1);
    switch (choice) {
        case 0:
            return first->retrieveArbitraryNode(probability);
        case 1:
            return second->retrieveArbitraryNode(probability);
        default:
            assert(false);
    }
}

std::unique_ptr<Expr> Or::ownRandomChild() {
    int choice = uniformIntegerInclusiveBounds(0, 1);
    switch (choice) {
        case 0:
            return std::move(first);
        case 1:
            return std::move(second);
        default:
            assert(false);
    }
}

void Or::returnChildOwnership(std::unique_ptr<Expr> child) {
    assert(first == nullptr ^ second == nullptr);
    if (first == nullptr) {
        first = std::move(child);
    } else {
        second = std::move(child);
    }
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

int If::computeLogicSize() {
    return 1 + condition->computeLogicSize() + trueCase->computeLogicSize()
           + falseCase->computeLogicSize();
}

bool If::evaluate(const std::unordered_map<std::string, bool>& truthMap) {
    return condition->evaluate(truthMap) ? trueCase->evaluate(truthMap)
                                         : falseCase->evaluate(truthMap);
}

std::string If::prettyPrint() {
    return "( IF " + condition->prettyPrint() + " THEN " + trueCase->prettyPrint()
           + " ELSE " + falseCase->prettyPrint() + " )";
}

Expr* If::retrieveArbitraryNode(double probability) {
    double rand = uniformReal();
    if (rand < probability) {
        return this;
    }
    int choice = uniformIntegerInclusiveBounds(0, 2);
    switch (choice) {
        case 0:
            return condition->retrieveArbitraryNode(probability);
        case 1:
            return trueCase->retrieveArbitraryNode(probability);
        case 2:
            return falseCase->retrieveArbitraryNode(probability);
        default:
            assert(false);
    }
}

std::unique_ptr<Expr> If::ownRandomChild() {
    int choice = uniformIntegerInclusiveBounds(0, 2);
    switch (choice) {
        case 0:
            return std::move(condition);
        case 1:
            return std::move(trueCase);
        case 2:
            return std::move(falseCase);
        default:
            assert(false);
    }
}

void If::returnChildOwnership(std::unique_ptr<Expr> child) {
    if (condition == nullptr) {
        assert(trueCase != nullptr && falseCase != nullptr);
        condition = std::move(child);
    } else if (trueCase == nullptr) {
        assert(condition != nullptr && falseCase != nullptr);
        trueCase = std::move(child);
    } else if (falseCase == nullptr) {
        assert(condition != nullptr && trueCase != nullptr);
        falseCase = std::move(child);
    } else {
        assert(false);
    }
}

Terminal::Terminal(const std::vector<std::string>& terminalOptions) {
    unsigned long high = terminalOptions.size() - 1;
    int rand = uniformIntegerInclusiveBounds(0, static_cast<int>(high));
    terminal = terminalOptions[rand];
}

int Terminal::computeDepth() {
    return 0;
}

int Terminal::computeLogicSize() {
    return 0;
}

bool Terminal::evaluate(const std::unordered_map<std::string, bool>& truthMap) {
    return truthMap.at(terminal);
}

std::string Terminal::prettyPrint() {
    return terminal;
}

Expr* Terminal::retrieveArbitraryNode(double) {
    return nullptr;
}

std::unique_ptr<Expr> Terminal::ownRandomChild() {
    throw std::runtime_error{"Cannot own a terminal"};
}

void Terminal::returnChildOwnership(std::unique_ptr<Expr>) {
    throw std::runtime_error{"Cannot return ownership to a terminal"};
}
