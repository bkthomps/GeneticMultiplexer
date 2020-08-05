#ifndef GENETIC_MULTIPLEXER_CONSTANTS_H
#define GENETIC_MULTIPLEXER_CONSTANTS_H

/*
 * Setting this too high will result in higher nodes being selection more
 * frequently than deeper nodes. However, setting it too low impacts efficiency.
 */
const double arbitraryNodeSelectionAggressiveness = 4;

const double crossoverProbability = 0.94;

const double mutationProbability = 0.04;

const int initialDepth = 3;

/*
 * From this depth onwards, the tree fitness will linearly scale until the maximum depth. Meaning,
 * at this point, the fitness remains as the full unscaled factor, but afterwards, it is scaled.
 */
const int disfavorDepth = 5;

/*
 * At this depth and beyond, the tree will have a fitness of exactly zero. Meaning, at this point,
 * the fitness will be zero regardless of any other factors.
 */
const int maximumDepth = 9;

const int populationSize = 2000;

const int selectionPerTournament = 200;

#endif
