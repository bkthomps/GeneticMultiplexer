#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <unordered_set>
#include <vector>
#include "constants.h"
#include "expressions.h"

constexpr std::size_t calculateCombinations(std::size_t length) {
    return 1u << length;
}

std::size_t correctLogicCount(Expr* head, std::size_t addressPins, std::size_t optionsCount,
                              std::size_t combinations) {
    assert(calculateCombinations(addressPins) == optionsCount - addressPins);
    std::vector<char> truthTable(optionsCount, 0);
    std::size_t correct = 0;
    for (std::size_t i = 0; i < combinations; i++) {
        for (std::size_t j = 0; j < optionsCount; j++) {
            std::size_t offset = (optionsCount - 1) - j % optionsCount;
            truthTable[j] = (i & (1U << offset)) >> offset;
        }
        std::size_t address = 0;
        for (std::size_t j = 0; j < addressPins; j++) {
            address *= 2;
            address += truthTable[j];
        }
        bool actualTruth = truthTable[addressPins + address];
        bool predictedTruth = head->evaluate(truthTable);
        if (actualTruth == predictedTruth) {
            correct++;
        }
    }
    return correct;
}

double computeFitness(Expr* head, std::size_t addressPins, std::size_t optionsCount) {
    assert(head != nullptr);
    assert(disfavorDepth < maximumDepth);
    int depth = head->computeDepth();
    if (depth > maximumDepth) {
        return 0;
    }
    std::size_t combinations = calculateCombinations(optionsCount);
    std::size_t correct = correctLogicCount(head, addressPins, optionsCount, combinations);
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
tournamentSelection(std::size_t addressPins, std::size_t optionsCount,
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
        double fitness = computeFitness(head.get(), addressPins, optionsCount);
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
computeMultiplexer(int addressPins, const std::vector<std::string>& options) {
    static_assert(crossoverProbability + mutationProbability <= 1.0);
    static_assert(populationSize % selectionPerTournament == 0);
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
            auto tuple = tournamentSelection(addressPins, options.size(), population);
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

void logComputedMultiplexer(const std::string& name, const int addressPins,
                            const std::vector<std::string>& options) {
    std::cout << "* Starting " << name << std::endl;
    auto[bestFitness, prettyTree] = computeMultiplexer(addressPins, options);
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

constexpr int getAddressPins(int dataPins) {
    dataPins--;
    int powers = 0;
    while (dataPins != 0) {
        powers++;
        dataPins /= 2;
    }
    return powers;
}

std::vector<int> dataPinsToCompute(int argc, char* argv[]) {
    std::vector<int> compute{};
    std::unordered_set<int> alreadyComputed{};
    for (int i = 1; i < argc; i++) {
        int dataPins;
        try {
            dataPins = std::stoi(argv[i]);
        } catch (const std::logic_error& e) {
            std::cout << "Error: not representable (" << argv[i] << ")" << std::endl;
            return compute;
        }
        if (dataPins < 1) {
            std::cout << "Error: data pin count must be positive (" << dataPins << ")" << std::endl;
            return compute;
        }
        if (alreadyComputed.count(dataPins)) {
            std::cout << "Warn: ignoring duplicate (" << dataPins << ")" << std::endl;
            continue;
        }
        // TODO: implement capability
        if (calculateCombinations(getAddressPins(dataPins)) != (std::size_t) dataPins) {
            std::cout << "Warn: non-power of 2 data pins not yet implemented, skipping ("
                      << dataPins << ")" << std::endl;
            continue;
        }
        alreadyComputed.insert(dataPins);
        compute.emplace_back(dataPins);
    }
    return compute;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Add data pin count as input argument" << std::endl;
        return -1;
    }
    for (int dataPins : dataPinsToCompute(argc, argv)) {
        std::string name{std::to_string(dataPins) + std::string{"_data_pins"}};
        int addressPins = getAddressPins(dataPins);
        std::vector<std::string> options{};
        options.reserve(addressPins + dataPins);
        for (int i = 0; i < addressPins; i++) {
            options.emplace_back(std::string{"a"} + "0");
        }
        for (int i = 0; i < dataPins; i++) {
            options.emplace_back(std::string{"d"} + std::to_string(i));
        }
        logComputedMultiplexer(name, addressPins, options);
    }
    return 0;
}
