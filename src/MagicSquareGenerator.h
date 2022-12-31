#pragma once
#include <vector>
#include "MagicSquare.h"

class MagicSquareGenerator
{
public:

	//Constructor
	//	n: Size of square; n > 2
	//	populationSize: Number of squares in the population; Must be an even number
	//	eliteSize: Number of elite squares carried over into next generation; Must be an even number
	//	mutationChance: Chance of mutation during cross-over; [0, 100]
	MagicSquareGenerator(int n, int populationSize, int eliteSize, int mutationChance);

	//Destructor
	~MagicSquareGenerator();

	//Begin running the genetic algorithm
	//	maxGenerations: Maximum generations before stopping
	void Run(int maxGenerations);

private:

	//Checks if a square is equal to population[i] for all i in [0, end)
	bool CheckDuplicate(MagicSquare** population, MagicSquare* square, int end) const;

	//Returns the average fitness of all squares in the population
	double AverageFitness();

	//Returns the square with the lowest fitness in the population
	MagicSquare* BestFitness();

	//Determines the squares with the lowest fitness, and copies them into 'elite'
	void GetElite();


	//Fields
	int eliteSize;
	int mutationChance;
	int populationSize;
	int n;
	int matingPoolSize;
	std::vector<int> randomizedIndexes;
	MagicSquare** population;
	MagicSquare** matingPool;
	MagicSquare** elite;

};

