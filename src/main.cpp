#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "constants.h"
#include "expressions.h"

constexpr std::size_t calculateCombinations(std::size_t length) {
    return 1u << length;
}

std::size_t correctMultiplexerLogicCount(Expr* head, std::size_t addressCount,
                                         std::size_t optionsCount, std::size_t combinations) {
    assert(calculateCombinations(addressCount) == optionsCount - addressCount);
    std::vector<char> truthTable(optionsCount, 0);
    std::size_t correct = 0;
    for (std::size_t i = 0; i < combinations; i++) {
        for (std::size_t j = 0; j < optionsCount; j++) {
            std::size_t offset = (optionsCount - 1) - j % optionsCount;
            truthTable[j] = (i & (1U << offset)) >> offset;
        }
        std::size_t address = 0;
        for (std::size_t j = 0; j < addressCount; j++) {
            address *= 2;
            address += truthTable[j];
        }
        bool actualTruth = truthTable[addressCount + address];
        bool predictedTruth = head->evaluate(truthTable);
        if (actualTruth == predictedTruth) {
            correct++;
        }
    }
    return correct;
}

std::size_t correctMiddleLogicCount(Expr* head, std::size_t optionsCount,
                                    std::size_t combinations) {
    std::vector<char> truthTable(optionsCount, 0);
    std::size_t correct = 0;
    for (std::size_t i = 0; i < combinations; i++) {
        int onesCount = 0;
        for (std::size_t j = 0; j < optionsCount; j++) {
            std::size_t offset = (optionsCount - 1) - j % optionsCount;
            truthTable[j] = (i & (1U << offset)) >> offset;
            onesCount += truthTable[j];
        }
        bool actualTruth = 7 <= onesCount && onesCount <= 9;
        bool predictedTruth = head->evaluate(truthTable);
        if (actualTruth == predictedTruth) {
            correct++;
        }
    }
    return correct;
}

double computeFitness(Expr* head, std::size_t addressCount, std::size_t optionsCount) {
    assert(head != nullptr);
    assert(disfavorDepth < maximumDepth);
    int depth = head->computeDepth();
    if (depth > maximumDepth) {
        return 0;
    }
    std::size_t combinations = calculateCombinations(optionsCount);
    std::size_t correct;
    if (addressCount == 0) {
        correct = correctMiddleLogicCount(head, optionsCount, combinations);
    } else {
        correct = correctMultiplexerLogicCount(head, addressCount, optionsCount, combinations);
    }
    if (correct == combinations) {
        return 1;
    }
    double baseFitness = static_cast<double>(correct) / combinations;
    if (depth > disfavorDepth) {
        double factor = static_cast<double>(maximumDepth - depth) / (maximumDepth - disfavorDepth);
        assert(0.0 <= factor && factor <= 1.0);
        baseFitness *= factor;
    }
    return baseFitness;
}

std::tuple<std::unique_ptr<Expr>, std::unique_ptr<Expr>, double>
tournamentSelection(std::size_t addressCount, std::size_t optionsCount,
                    std::vector<std::unique_ptr<Expr>>& samples) {
    std::unique_ptr<Expr> firstHead = nullptr;
    std::unique_ptr<Expr> secondHead = nullptr;
    double firstFitness = 0;
    double secondFitness = 0;
    for (int i = 0; i < selectionPerTournament; i++) {
        assert(!samples.empty());
        int index = uniformIntegerInclusiveBounds(0, static_cast<int>(samples.size()) - 1);
        std::unique_ptr<Expr> head = std::move(samples[index]);
        samples.erase(samples.begin() + index);
        double fitness = computeFitness(head.get(), addressCount, optionsCount);
        if (fitness > firstFitness) {
            firstHead = std::move(head);
            firstFitness = fitness;
        } else if (fitness > secondFitness) {
            secondHead = std::move(head);
            secondFitness = fitness;
        }
    }
    if (secondFitness > firstFitness) {
        std::swap(firstFitness, secondFitness);
        std::swap(firstHead, secondHead);
    }
    assert(firstFitness >= secondFitness);
    assert(firstHead.get() != secondHead.get());
    return std::make_tuple(std::move(firstHead), std::move(secondHead), firstFitness);
}

std::tuple<std::vector<double>, std::string>
computeMultiplexer(int addressCount, const std::vector<std::string>& options) {
    assert(crossoverProbability + mutationProbability <= 1.0);
    assert(populationSize % selectionPerTournament == 0);
    std::vector<double> bestFitness{};
    std::string prettyTree{};
    std::vector<std::unique_ptr<Expr>> population{};
    population.reserve(populationSize);
    for (int i = 0; i < populationSize; i++) {
        population.emplace_back(randomNode(options, initialDepth));
    }
    int tournaments = populationSize / selectionPerTournament;
    do {
        std::vector<std::unique_ptr<Expr>> updatedPopulation{};
        updatedPopulation.reserve(populationSize);
        double bestFitnessIteration = 0;
        for (int j = 0; j < tournaments; j++) {
            auto tuple = tournamentSelection(addressCount, options.size(), population);
            auto[parentOne, parentTwo, bestParentFitness] = std::move(tuple);
            if (bestParentFitness > bestFitnessIteration) {
                bestFitnessIteration = bestParentFitness;
                prettyTree = parentOne->prettyPrint();
            }
            for (int k = 0; k < selectionPerTournament / 2; k++) {
                if (uniformReal() < crossoverProbability) {
                    auto[childOne, childTwo] = performRecombination(parentOne.get(),
                                                                    parentTwo.get());
                    updatedPopulation.emplace_back(std::move(childOne));
                    updatedPopulation.emplace_back(std::move(childTwo));
                } else if (uniformReal() < mutationProbability / (1 - crossoverProbability)) {
                    auto childOne = performMutation(parentOne.get(), options);
                    auto childTwo = performMutation(parentTwo.get(), options);
                    updatedPopulation.emplace_back(std::move(childOne));
                    updatedPopulation.emplace_back(std::move(childTwo));
                } else {
                    updatedPopulation.emplace_back(parentOne->clone());
                    updatedPopulation.emplace_back(parentTwo->clone());
                }
            }
        }
        assert(population.empty());
        assert(updatedPopulation.size() == populationSize);
        bestFitness.emplace_back(bestFitnessIteration);
        population = std::move(updatedPopulation);
        std::cout << bestFitnessIteration << std::endl;
    } while (bestFitness.back() < 1.0 - std::numeric_limits<double>::epsilon());
    return std::make_tuple(std::move(bestFitness), prettyTree);
}

void logComputedMultiplexer(const std::string& name, int addressCount,
                            const std::vector<std::string>& options) {
    std::cout << "* Starting " << name << std::endl;
    auto[bestFitness, prettyTree] = computeMultiplexer(addressCount, options);
    std::ofstream fitnessFile;
    fitnessFile.open(name + "_fitness.csv", std::ios::out);
    if (fitnessFile.fail()) {
        throw std::runtime_error{"Could not open file: " + name};
    }
    fitnessFile << "sep=," << std::endl;
    for (std::size_t i = 0; i < bestFitness.size(); i++) {
        fitnessFile << i << ',' << bestFitness[i] << std::endl;
    }
    fitnessFile.close();
    std::ofstream treeFile;
    treeFile.open(name + "_tree.csv", std::ios::out);
    if (treeFile.fail()) {
        throw std::runtime_error{"Could not open file: " + name};
    }
    treeFile << prettyTree << std::endl;
    treeFile.close();
    std::cout << "* Done with " << name << std::endl;
}

int main(int argc, char* argv[]) {
    std::string name6mux = std::string{"6mux"};
    std::string name11mux = std::string{"11mux"};
    std::string name16middle3 = std::string{"16middle3"};
    bool run6mux = false;
    bool run11mux = false;
    bool run16middle3 = false;
    for (int i = 1; i < argc; i++) {
        run6mux = run6mux || argv[i] == name6mux;
        run11mux = run11mux || argv[i] == name11mux;
        run16middle3 = run16middle3 || argv[i] == name16middle3;
    }
    if (!run6mux && !run11mux && !run16middle3) {
        std::cout << "Run with arguments " << name6mux << " or " << name11mux << " or "
                  << name16middle3 << std::endl;
    }
    if (run6mux) {
        const std::vector<std::string> mux6 = {"a0", "a1", "d0", "d1", "d2", "d3"};
        logComputedMultiplexer(name6mux, 2, mux6);
    }
    if (run11mux) {
        const std::vector<std::string> mux11 =
                {"a0", "a1", "a2", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7"};
        logComputedMultiplexer(name11mux, 3, mux11);
    }
    if (run16middle3) {
        std::vector<std::string> pins{};
        pins.reserve(16);
        for (int i = 0; i < 16; i++) {
            pins.emplace_back("e" + std::to_string(i));
        }
        logComputedMultiplexer(name16middle3, 0, pins);
    }
    return 0;
}
