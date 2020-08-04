#ifndef GENETIC_MULTIPLEXER_EXPRESSIONS_H
#define GENETIC_MULTIPLEXER_EXPRESSIONS_H

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

/*
 * The depth and size specifies the depth and size
 * of the entire tree below the respective node.
 */
class Expr
{
public:
    virtual ~Expr() = default;
    virtual std::unique_ptr<Expr> clone() = 0;
    virtual int computeDepth() = 0;
    virtual int computeLogicSize() = 0;
    virtual bool evaluate(const std::unordered_map<std::string, bool>& truthMap) = 0;
    virtual std::string prettyPrint() = 0;
    virtual Expr* retrieveArbitraryNode(double probability) = 0;
    virtual std::unique_ptr<Expr> ownRandomChild() = 0;
    virtual void returnChildOwnership(std::unique_ptr<Expr> child) = 0;
};

/* Generates a random node with children based on specified depth. */
std::unique_ptr<Expr> randomNode(const std::vector<const std::string>& terminalOptions, int depth);

/* Performs a recombination on copies of both trees passed in. */
std::tuple<std::unique_ptr<Expr>, std::unique_ptr<Expr>>
performRecombination(const std::unique_ptr<Expr>& firstHead,
                     const std::unique_ptr<Expr>& secondHead);

class Not final : public Expr
{
private:
    std::unique_ptr<Expr> expr;
public:
    Not(const std::vector<const std::string>& terminalOptions, int depth);
    Not(const Not& old);
    std::unique_ptr<Expr> clone() override;
    int computeDepth() override;
    int computeLogicSize() override;
    bool evaluate(const std::unordered_map<std::string, bool>& truthMap) override;
    std::string prettyPrint() override;
    Expr* retrieveArbitraryNode(double probability) override;
    std::unique_ptr<Expr> ownRandomChild() override;
    void returnChildOwnership(std::unique_ptr<Expr> child) override;
};

class And final : public Expr
{
private:
    std::unique_ptr<Expr> first;
    std::unique_ptr<Expr> second;
public:
    And(const std::vector<const std::string>& terminalOptions, int depth);
    And(const And& old);
    std::unique_ptr<Expr> clone() override;
    int computeDepth() override;
    int computeLogicSize() override;
    bool evaluate(const std::unordered_map<std::string, bool>& truthMap) override;
    std::string prettyPrint() override;
    Expr* retrieveArbitraryNode(double probability) override;
    std::unique_ptr<Expr> ownRandomChild() override;
    void returnChildOwnership(std::unique_ptr<Expr> child) override;
};

class Or final : public Expr
{
private:
    std::unique_ptr<Expr> first;
    std::unique_ptr<Expr> second;
public:
    Or(const std::vector<const std::string>& terminalOptions, int depth);
    Or(const Or& old);
    std::unique_ptr<Expr> clone() override;
    int computeDepth() override;
    int computeLogicSize() override;
    bool evaluate(const std::unordered_map<std::string, bool>& truthMap) override;
    std::string prettyPrint() override;
    Expr* retrieveArbitraryNode(double probability) override;
    std::unique_ptr<Expr> ownRandomChild() override;
    void returnChildOwnership(std::unique_ptr<Expr> child) override;
};

class If final : public Expr
{
private:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> trueCase;
    std::unique_ptr<Expr> falseCase;
public:
    If(const std::vector<const std::string>& terminalOptions, int depth);
    If(const If& old);
    std::unique_ptr<Expr> clone() override;
    int computeDepth() override;
    int computeLogicSize() override;
    bool evaluate(const std::unordered_map<std::string, bool>& truthMap) override;
    std::string prettyPrint() override;
    Expr* retrieveArbitraryNode(double probability) override;
    std::unique_ptr<Expr> ownRandomChild() override;
    void returnChildOwnership(std::unique_ptr<Expr> child) override;
};

class Terminal final : public Expr
{
private:
    std::string terminal;
public:
    explicit Terminal(const std::vector<const std::string>& terminalOptions);
    Terminal(const Terminal& old);
    std::unique_ptr<Expr> clone() override;
    int computeDepth() override;
    int computeLogicSize() override;
    bool evaluate(const std::unordered_map<std::string, bool>& truthMap) override;
    std::string prettyPrint() override;
    Expr* retrieveArbitraryNode(double probability) override;
    std::unique_ptr<Expr> ownRandomChild() override;
    void returnChildOwnership(std::unique_ptr<Expr> child) override;
};

#endif
