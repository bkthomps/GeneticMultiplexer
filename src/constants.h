#ifndef GENETIC_MULTIPLEXER_CONSTANTS_H
#define GENETIC_MULTIPLEXER_CONSTANTS_H

/*
 * Setting this too high will result in higher nodes being selection more
 * frequently than deeper nodes. However, setting it too low impacts efficiency.
 */
const double arbitraryNodeSelectionAggressiveness = 4;

const double crossoverProbability = 0.9;

const int initialDepth = 3;

/* After this depth, the tree fitness will linearly scale until the maximum depth. */
const int disfavorDepth = 6;

/* After this depth, the tree will have a fitness of exactly zero. */
const int maximumDepth = 9;

const int populationSize = 1000;

const int selectionPerTournament = 100;

#endif
