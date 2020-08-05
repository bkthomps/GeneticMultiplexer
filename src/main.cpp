#include <algorithm>
#include <bitset>
#include <iostream>
#include <limits>
#include <memory>
#include <tuple>
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

double computeFitness(Expr* head, int addressCount, const std::vector<const std::string>& options) {
    assert(head != nullptr);
    assert(options.size() <= 16);
    int depth = head->computeDepth();
    if (depth > maximumDepth) {
        return 0;
    }
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
    double baseFitness = correct / combinations;
    if (depth > disfavorDepth) {
        double factor = 1 / (depth - disfavorDepth + 0.5);
        assert(0.0 <= factor && factor <= 1.0);
        baseFitness *= factor;
    }
    return baseFitness;
}

std::tuple<std::unique_ptr<Expr>, std::unique_ptr<Expr>, double>
tournamentSelection(int addressCount, const std::vector<const std::string>& options,
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
        double fitness = computeFitness(head.get(), addressCount, options);
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

int main() {
    std::vector<const std::string> optionsMux6 = {"a0", "a1", "d0", "d1", "d2", "d3"};
    int addressLinesMux6 = 2;
    std::vector<double> bestFitness{};
    std::string prettyTree{};
    std::vector<std::unique_ptr<Expr>> population{};
    population.reserve(populationSize);
    for (int i = 0; i < populationSize; i++) {
        population.emplace_back(randomNode(optionsMux6, initialDepth));
    }
    int tournaments = populationSize / selectionPerTournament;
    do {
        std::vector<std::unique_ptr<Expr>> updatedPopulation{};
        updatedPopulation.reserve(populationSize);
        double bestFitnessIteration = 0;
        for (int j = 0; j < tournaments; j++) {
            auto tuple = tournamentSelection(addressLinesMux6, optionsMux6, population);
            std::unique_ptr<Expr> parentOne = std::move(std::get<0>(tuple));
            std::unique_ptr<Expr> parentTwo = std::move(std::get<1>(tuple));
            if (std::get<2>(tuple) > bestFitnessIteration) {
                bestFitnessIteration = std::get<2>(tuple);
                prettyTree = parentOne->prettyPrint();
            }
            for (int k = 0; k < selectionPerTournament / 2; k++) {
                auto children = performRecombination(parentOne.get(), parentTwo.get());
                std::unique_ptr<Expr> childOne = std::move(std::get<0>(children));
                std::unique_ptr<Expr> childTwo = std::move(std::get<1>(children));
                updatedPopulation.emplace_back(std::move(childOne));
                updatedPopulation.emplace_back(std::move(childTwo));
            }
        }
        assert(population.empty());
        assert(updatedPopulation.size() == populationSize);
        bestFitness.emplace_back(bestFitnessIteration);
        population = std::move(updatedPopulation);
    } while (bestFitness.back() < 1.0 - std::numeric_limits<double>::epsilon());
    for (auto n : bestFitness) {
        std::cout << n << std::endl;
    }
    std::cout << prettyTree << std::endl;
    return 0;
}
