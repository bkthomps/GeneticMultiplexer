#include <bitset>
#include <iostream>
#include <memory>
#include <random>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "constants.h"
#include "expressions.h"

std::random_device mainSeed;
std::mt19937 mainGenerator(mainSeed());

int mainUniformIntegerInclusiveBounds(int low, int high) {
    std::uniform_int_distribution<int> distribution(low, high);
    return distribution(mainGenerator);
}

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

std::tuple<std::unique_ptr<Expr>, std::unique_ptr<Expr>>
tournamentSelection(int addressCount, const std::vector<const std::string>& options,
                    std::vector<std::unique_ptr<Expr>>& samples) {
    std::unique_ptr<Expr> firstHead = nullptr;
    std::unique_ptr<Expr> secondHead = nullptr;
    double firstFitness = 0;
    double secondFitness = 0;
    for (int i = 0; i < selectionPerTournament; i++) {
        assert(!samples.empty());
        int index = mainUniformIntegerInclusiveBounds(0, static_cast<int>(samples.size()) - 1);
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
    return std::make_tuple(std::move(firstHead), std::move(secondHead));
}

int main() {
    std::vector<const std::string> optionsMux6 = {"a0", "a1", "d0", "d1", "d2", "d3"};
    int addressLinesMux6 = 2;
    std::vector<std::unique_ptr<Expr>> population{};
    population.reserve(populationSize);
    for (int i = 0; i < populationSize; i++) {
        population.emplace_back(randomNode(optionsMux6, initialDepth));
    }
    int tournaments = populationSize / selectionPerTournament;
    for (int i = 0; i < iterations; i++) {
        std::vector<std::unique_ptr<Expr>> updatedPopulation{};
        updatedPopulation.reserve(populationSize);
        for (int j = 0; j < tournaments; j++) {
            auto parents = tournamentSelection(addressLinesMux6, optionsMux6, population);
            std::unique_ptr<Expr> father = std::move(std::get<0>(parents));
            std::unique_ptr<Expr> mother = std::move(std::get<1>(parents));
            for (int k = 0; k < selectionPerTournament / 2; k++) {
                auto children = performRecombination(father.get(), mother.get());
                std::unique_ptr<Expr> son = std::move(std::get<0>(children));
                std::unique_ptr<Expr> daughter = std::move(std::get<1>(children));
                updatedPopulation.emplace_back(std::move(son));
                updatedPopulation.emplace_back(std::move(daughter));
            }
        }
        assert(population.empty());
        assert(updatedPopulation.size() == populationSize);
        population = std::move(updatedPopulation);
    }
    double bestFitness = 0;
    for (int i = 0; i < populationSize; i++) {
        double fitness = computeFitness(population[i].get(), addressLinesMux6, optionsMux6);
        if (fitness > bestFitness) {
            bestFitness = fitness;
        }
    }
    std::cout << bestFitness << std::endl;
    return 0;
}
