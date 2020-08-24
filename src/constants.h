#ifndef GENETIC_MULTIPLEXER_CONSTANTS_H
#define GENETIC_MULTIPLEXER_CONSTANTS_H

/*
 * Setting this too high will result in higher nodes being selection more
 * frequently than deeper nodes. However, setting it too low impacts efficiency.
 */
constexpr double arbitraryNodeSelectionAggressiveness{4.0};

constexpr double crossoverProbability{0.94};

constexpr double mutationProbability{0.04};

constexpr int initialDepth{3};

/*
 * From this depth onwards, the tree fitness will linearly scale until the maximum depth. Meaning,
 * at this point, the fitness remains as the full unscaled factor, but afterwards, it is scaled.
 */
constexpr int disfavorDepth{5};

/*
 * At this depth and beyond, the tree will have a fitness of exactly zero. Meaning, at this point,
 * the fitness will be zero regardless of any other factors.
 */
constexpr int maximumDepth{6};

constexpr int populationSize{10'000};

constexpr int selectionPerTournament{100};

#endif
