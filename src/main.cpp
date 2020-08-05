#include <bitset>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <tuple>
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

double computeFitness(Expr* head, int addressCount, int optionsCount) {
    assert(head != nullptr);
    assert(optionsCount <= 16);
    assert(calculateCombinations(addressCount) == optionsCount - addressCount);
    int depth = head->computeDepth();
    if (depth > maximumDepth) {
        return 0;
    }
    unsigned long startIndex = 16 - optionsCount;
    int combinations = calculateCombinations(optionsCount);
    std::vector<bool> truthTable{};
    truthTable.reserve(combinations);
    double correct = 0;
    for (int i = 0; i < combinations; i++) {
        std::string str = std::bitset<16>(i).to_string();
        for (int j = 0; j < optionsCount; j++) {
            truthTable[j] = str[startIndex + j] - '0';
        }
        auto currentAddress = str.substr(startIndex, addressCount);
        unsigned long address = std::bitset<16>(currentAddress).to_ulong();
        bool actualTruth = str[startIndex + addressCount + address] == '1';
        bool predictedTruth = head->evaluate(truthTable);
        if (actualTruth == predictedTruth) {
            correct++;
        }
    }
    double baseFitness = correct / combinations;
    if (depth > disfavorDepth) {
        double factor = static_cast<double>(maximumDepth - depth) / (maximumDepth - disfavorDepth);
        assert(0.0 <= factor && factor <= 1.0);
        baseFitness *= factor;
    }
    return baseFitness;
}

std::tuple<std::unique_ptr<Expr>, std::unique_ptr<Expr>, double>
tournamentSelection(int addressCount, int optionsCount,
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
computeMultiplexer(int addressCount, const std::vector<const std::string>& options) {
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
            int optionsCount = options.size();
            auto tuple = tournamentSelection(addressCount, optionsCount, population);
            auto[parentOne, parentTwo, bestParentFitness] = std::move(tuple);
            if (bestParentFitness > bestFitnessIteration) {
                bestFitnessIteration = bestParentFitness;
                prettyTree = parentOne->prettyPrint();
            }
            for (int k = 0; k < selectionPerTournament / 2; k++) {
                auto[childOne, childTwo] = performRecombination(parentOne.get(), parentTwo.get());
                updatedPopulation.emplace_back(std::move(childOne));
                updatedPopulation.emplace_back(std::move(childTwo));
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
                            const std::vector<const std::string>& options) {
    std::cout << "* Starting " << name << std::endl;
    auto[bestFitness, prettyTree] = computeMultiplexer(addressCount, options);
    std::ofstream fitnessFile;
    fitnessFile.open(name + "_fitness.csv", std::ios::out);
    if (fitnessFile.fail()) {
        throw std::runtime_error{"Could not open file: " + name};
    }
    fitnessFile << "sep=," << std::endl;
    for (unsigned int i = 0; i < bestFitness.size(); i++) {
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

int main() {
    std::vector<const std::string> mux6 = {"a0", "a1", "d0", "d1", "d2", "d3"};
    logComputedMultiplexer("6mux", 2, mux6);
    std::vector<const std::string> mux11 =
            {"a0", "a1", "a2", "d0", "d1", "d2", "d3", "d7", "d8", "d9", "d10"};
    logComputedMultiplexer("11mux", 3, mux11);
    return 0;
}
