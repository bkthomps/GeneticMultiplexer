#include <algorithm>
#include <cassert>
#include <random>
#include <stdexcept>
#include "constants.h"
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

Expr* retrieveArbitraryNode(Expr* head) {
    double probability = arbitraryNodeSelectionAggressiveness / head->computeLogicSize();
    Expr* arbitraryNode = nullptr;
    while (arbitraryNode == nullptr) {
        arbitraryNode = head->retrieveArbitraryNode(probability);
    }
    return arbitraryNode;
}

std::tuple<std::unique_ptr<Expr>, std::unique_ptr<Expr>>
performRecombination(Expr* oldFirstHead, Expr* oldSecondHead) {
    assert(oldFirstHead != nullptr && oldSecondHead != nullptr);
    std::unique_ptr<Expr> firstHeadCopy = oldFirstHead->clone();
    std::unique_ptr<Expr> secondHeadCopy = oldSecondHead->clone();
    Expr* firstArbitraryNode = retrieveArbitraryNode(firstHeadCopy.get());
    Expr* secondArbitraryNode = retrieveArbitraryNode(secondHeadCopy.get());
    std::unique_ptr<Expr> firstChild = firstArbitraryNode->ownRandomChild();
    std::unique_ptr<Expr> secondChild = secondArbitraryNode->ownRandomChild();
    firstArbitraryNode->returnChildOwnership(std::move(secondChild));
    secondArbitraryNode->returnChildOwnership(std::move(firstChild));
    return std::make_tuple(std::move(firstHeadCopy), std::move(secondHeadCopy));
}

std::unique_ptr<Expr> performMutation(Expr* head, const std::vector<std::string>& options) {
    assert(head != nullptr);
    std::unique_ptr<Expr> headCopy = head->clone();
    Expr* arbitraryNode = retrieveArbitraryNode(headCopy.get());
    static_cast<void>(arbitraryNode->ownRandomChild());
    std::discrete_distribution<int> distribution{0, 1, 1, 2, 2, 3};
    int depth = distribution(generator);
    std::unique_ptr<Expr> mutation = randomNode(options, depth);
    arbitraryNode->returnChildOwnership(std::move(mutation));
    return headCopy;
}

Not::Not(const std::vector<std::string>& terminalOptions, int depth) {
    expr = randomNode(terminalOptions, depth - 1);
}

Not::Not(const Not& old) {
    expr = old.expr->clone();
}

std::unique_ptr<Expr> Not::clone() const {
    return std::make_unique<Not>(*this);
}

int Not::computeDepth() const {
    return 1 + expr->computeDepth();
}

int Not::computeLogicSize() const {
    return 1 + expr->computeLogicSize();
}

bool Not::evaluate(const std::vector<char>& truthTable) const {
    return !expr->evaluate(truthTable);
}

std::string Not::prettyPrint() const {
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

And::And(const And& old) {
    first = old.first->clone();
    second = old.second->clone();
}

std::unique_ptr<Expr> And::clone() const {
    return std::make_unique<And>(*this);
}

int And::computeDepth() const {
    return 1 + std::max(first->computeDepth(), second->computeDepth());
}

int And::computeLogicSize() const {
    return 1 + first->computeLogicSize() + second->computeLogicSize();
}

bool And::evaluate(const std::vector<char>& truthTable) const {
    return first->evaluate(truthTable) && second->evaluate(truthTable);
}

std::string And::prettyPrint() const {
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

Or::Or(const Or& old) {
    first = old.first->clone();
    second = old.second->clone();
}

std::unique_ptr<Expr> Or::clone() const {
    return std::make_unique<Or>(*this);
}

int Or::computeDepth() const {
    return 1 + std::max(first->computeDepth(), second->computeDepth());
}

int Or::computeLogicSize() const {
    return 1 + first->computeLogicSize() + second->computeLogicSize();
}

bool Or::evaluate(const std::vector<char>& truthTable) const {
    return first->evaluate(truthTable) || second->evaluate(truthTable);
}

std::string Or::prettyPrint() const {
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

If::If(const If& old) {
    condition = old.condition->clone();
    trueCase = old.trueCase->clone();
    falseCase = old.falseCase->clone();
}

std::unique_ptr<Expr> If::clone() const {
    return std::make_unique<If>(*this);
}

int If::computeDepth() const {
    int partialDepth = std::max(trueCase->computeDepth(), falseCase->computeDepth());
    return 1 + std::max(condition->computeDepth(), partialDepth);
}

int If::computeLogicSize() const {
    return 1 + condition->computeLogicSize() + trueCase->computeLogicSize()
           + falseCase->computeLogicSize();
}

bool If::evaluate(const std::vector<char>& truthTable) const {
    return condition->evaluate(truthTable) ? trueCase->evaluate(truthTable)
                                           : falseCase->evaluate(truthTable);
}

std::string If::prettyPrint() const {
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
    size_t high = terminalOptions.size() - 1;
    int rand = uniformIntegerInclusiveBounds(0, static_cast<int>(high));
    terminal = terminalOptions[rand];
    truthTableIndex = rand;
    truthTableSize = terminalOptions.size();
}

Terminal::Terminal(const Terminal& old) {
    terminal = old.terminal;
    truthTableIndex = old.truthTableIndex;
    truthTableSize = old.truthTableSize;
}

std::unique_ptr<Expr> Terminal::clone() const {
    return std::make_unique<Terminal>(*this);
}

int Terminal::computeDepth() const {
    return 0;
}

int Terminal::computeLogicSize() const {
    return 0;
}

bool Terminal::evaluate(const std::vector<char>& truthTable) const {
    assert(truthTable.size() == truthTableSize);
    return truthTable[truthTableIndex];
}

std::string Terminal::prettyPrint() const {
    return std::string{terminal};
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
