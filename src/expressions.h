#ifndef GENETIC_MULTIPLEXER_EXPRESSIONS_H
#define GENETIC_MULTIPLEXER_EXPRESSIONS_H

#include <memory>
#include <vector>
#include <string>

/*
 * The depth and size specifies the depth and size
 * of the entire tree below the respective node.
 */
class Expr
{
protected:
public:
    virtual int computeDepth() = 0;
    virtual int computeSize() = 0;
};

/*
 * Generates a random node and children based on specified depth.
 */
std::unique_ptr<Expr>
randomNode(const std::vector<std::string>& terminalOptions, int depth);

class Not final : public Expr
{
private:
    std::unique_ptr<Expr> expr;
public:
    Not(const std::vector<std::string>& terminalOptions, int depth);
    int computeDepth() override;
    int computeSize() override;
};

class And final : public Expr
{
private:
    std::unique_ptr<Expr> first;
    std::unique_ptr<Expr> second;
public:
    And(const std::vector<std::string>& terminalOptions, int depth);
    int computeDepth() override;
    int computeSize() override;
};

class Or final : public Expr
{
private:
    std::unique_ptr<Expr> first;
    std::unique_ptr<Expr> second;
public:
    Or(const std::vector<std::string>& terminalOptions, int depth);
    int computeDepth() override;
    int computeSize() override;
};

class If final : public Expr
{
private:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> trueCase;
    std::unique_ptr<Expr> falseCase;
public:
    If(const std::vector<std::string>& terminalOptions, int depth);
    int computeDepth() override;
    int computeSize() override;
};

class Terminal final : public Expr
{
private:
    std::string terminal;
public:
    explicit Terminal(const std::vector<std::string>& terminalOptions);
    int computeSize() override;
    int computeDepth() override;
};

#endif
