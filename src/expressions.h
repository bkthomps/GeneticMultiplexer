#ifndef GENETIC_MULTIPLEXER_EXPRESSIONS_H
#define GENETIC_MULTIPLEXER_EXPRESSIONS_H

#include <memory>
#include <string>
#include <tuple>
#include <vector>

int uniformIntegerInclusiveBounds(int low, int high);

double uniformReal();

/*
 * The depth and size specifies the depth and size
 * of the entire tree below the respective node.
 */
class Expr
{
public:
    virtual ~Expr() = default;
    [[nodiscard]] virtual std::unique_ptr<Expr> clone() const = 0;
    [[nodiscard]] virtual int computeDepth() const = 0;
    [[nodiscard]] virtual int computeLogicSize() const = 0;
    [[nodiscard]] virtual bool evaluate(const std::vector<bool>& truthTable) const = 0;
    [[nodiscard]] virtual std::string prettyPrint() const = 0;
    [[nodiscard]] virtual Expr* retrieveArbitraryNode(double probability) = 0;
    [[nodiscard]] virtual std::unique_ptr<Expr> ownRandomChild() = 0;
    virtual void returnChildOwnership(std::unique_ptr<Expr> child) = 0;
};

/* Generates a random node with children based on specified depth. */
std::unique_ptr<Expr> randomNode(const std::vector<const std::string>& terminalOptions, int depth);

/* Performs a recombination on copies of both trees passed in. */
std::tuple<std::unique_ptr<Expr>, std::unique_ptr<Expr>>
performRecombination(Expr* firstHead, Expr* secondHead);

/* Performs a mutation on a copy of the tree passed in. */
std::unique_ptr<Expr> performMutation(Expr* head, const std::vector<const std::string>& options);

class Not final : public Expr
{
private:
    std::unique_ptr<Expr> expr;
public:
    Not(const std::vector<const std::string>& terminalOptions, int depth);
    Not(const Not& old);
    [[nodiscard]] std::unique_ptr<Expr> clone() const override;
    [[nodiscard]] int computeDepth() const override;
    [[nodiscard]] int computeLogicSize() const override;
    [[nodiscard]] bool evaluate(const std::vector<bool>& truthTable) const override;
    [[nodiscard]] std::string prettyPrint() const override;
    [[nodiscard]] Expr* retrieveArbitraryNode(double probability) override;
    [[nodiscard]] std::unique_ptr<Expr> ownRandomChild() override;
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
    [[nodiscard]] std::unique_ptr<Expr> clone() const override;
    [[nodiscard]] int computeDepth() const override;
    [[nodiscard]] int computeLogicSize() const override;
    [[nodiscard]] bool evaluate(const std::vector<bool>& truthTable) const override;
    [[nodiscard]] std::string prettyPrint() const override;
    [[nodiscard]] Expr* retrieveArbitraryNode(double probability) override;
    [[nodiscard]] std::unique_ptr<Expr> ownRandomChild() override;
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
    [[nodiscard]] std::unique_ptr<Expr> clone() const override;
    [[nodiscard]] int computeDepth() const override;
    [[nodiscard]] int computeLogicSize() const override;
    [[nodiscard]] bool evaluate(const std::vector<bool>& truthTable) const override;
    [[nodiscard]] std::string prettyPrint() const override;
    [[nodiscard]] Expr* retrieveArbitraryNode(double probability) override;
    [[nodiscard]] std::unique_ptr<Expr> ownRandomChild() override;
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
    [[nodiscard]] std::unique_ptr<Expr> clone() const override;
    [[nodiscard]] int computeDepth() const override;
    [[nodiscard]] int computeLogicSize() const override;
    [[nodiscard]] bool evaluate(const std::vector<bool>& truthTable) const override;
    [[nodiscard]] std::string prettyPrint() const override;
    [[nodiscard]] Expr* retrieveArbitraryNode(double probability) override;
    [[nodiscard]] std::unique_ptr<Expr> ownRandomChild() override;
    void returnChildOwnership(std::unique_ptr<Expr> child) override;
};

class Terminal final : public Expr
{
private:
    std::string terminal;
    int truthTableIndex;
public:
    explicit Terminal(const std::vector<const std::string>& terminalOptions);
    Terminal(const Terminal& old);
    [[nodiscard]] std::unique_ptr<Expr> clone() const override;
    [[nodiscard]] int computeDepth() const override;
    [[nodiscard]] int computeLogicSize() const override;
    [[nodiscard]] bool evaluate(const std::vector<bool>& truthTable) const override;
    [[nodiscard]] std::string prettyPrint() const override;
    [[nodiscard]] Expr* retrieveArbitraryNode(double probability) override;
    [[nodiscard]] std::unique_ptr<Expr> ownRandomChild() override;
    void returnChildOwnership(std::unique_ptr<Expr> child) override;
};

#endif
