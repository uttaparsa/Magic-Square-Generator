#include "MagicSquareGenerator.h"
#include <cmath>
#include <time.h>
#include <iostream>
#include <algorithm>

using namespace std;

MagicSquareGenerator::MagicSquareGenerator(int n, int populationSize, int eliteSize, int mutationChance)
{

	this->mutationChance = mutationChance;
	this->n = n;
	this->populationSize = populationSize;
	this->eliteSize = eliteSize;

	elite = new MagicSquare*[eliteSize];
	matingPool = new MagicSquare*[populationSize / 2];
	population = new MagicSquare*[populationSize];
	for (int i = 0; i < populationSize; i++) {
		population[i] = new MagicSquare(n);
		if (CheckDuplicate(population, population[i], i)) {
			delete population[i];
			i--;
		}
	}

	for (int i = 0; i < populationSize; i++) {
		randomizedIndexes.push_back(i);
	}

}

MagicSquareGenerator::~MagicSquareGenerator()
{
	for (int i = 0; i < populationSize; i++) {
		delete population[i];
	}
	delete[] population;
	delete[] matingPool;
	delete[] elite;
}

void MagicSquareGenerator::Run(int maxGenerations)
{
	
	for (int i = 0; i <= maxGenerations; i++) {

		random_shuffle(randomizedIndexes.begin(), randomizedIndexes.end());

		//Determine the group of elite squares
		GetElite();

		//Generate the mating pool, and delete unused squares from memory
		for (int j = 0; j < populationSize - 1; j += 2) {
			if (population[randomizedIndexes[j]]->Fitness() <= population[randomizedIndexes[j + 1]]->Fitness()) {
				matingPool[j/2] = population[randomizedIndexes[j]];
				delete population[randomizedIndexes[j + 1]];
			}
			else {
				matingPool[j/2] = population[randomizedIndexes[j+1]];
				delete population[randomizedIndexes[j]];
			}
		}

		//Move the elite squares into the next generation
		for (int j = 0; j < eliteSize; j++) {
			population[j] = elite[j];
		}

		//Fill the rest of the new population with squares produced by cross-over within the mating pool
		for (int j = eliteSize; j < populationSize; j++) {

			while(true){

				int index1 = rand() % (populationSize / 2);
				int index2 = index1;
				while (index1 == index2) {
					index2 = rand() % (populationSize / 2);
				}

				population[j] = new MagicSquare(matingPool[rand() % (populationSize / 2)], matingPool[rand() % (populationSize / 2)], mutationChance, rand() % 2);

				if (!CheckDuplicate(population, population[j], j)) break;

				delete population[j];

			}

		}

		//Remove all squares in mating pool from memory
		for (int i = 0; i < populationSize / 2; i++) {
			delete matingPool[i];
		}

		//Find the square with minimal fitness within the new population
		MagicSquare* best = BestFitness();

		//Check if a magic square has been found
		if (best->Fitness() == 0.0) {
			cout << "Generation: " << i << "\n";
			cout << "Found Magic Square: " << endl;
			best->print();
			return;
		}

		//Print the current status every 100 generations
		if (i % 100 == 0) {
			cout << "Generation: " << i << "\n";
			cout << "Fitness (Average): " << AverageFitness() << "\n";
			cout << "Fitness (Best): " << best->Fitness() << "\n" << endl;
		}

	}
	
	cout << "No Magic Square Found." << endl;

}

bool MagicSquareGenerator::CheckDuplicate(MagicSquare** population, MagicSquare* square, int end) const
{
	for (int i = 0; i < end; i++) {
		if ((*square) == *(population[i])) {
			return true;
		}
	}
	return false;
}

double MagicSquareGenerator::AverageFitness()
{
	long double total = 0;
	for (int i = 0; i < populationSize; i++) {
		total += population[i]->Fitness();
	}
	return total / (long double)populationSize;
}

MagicSquare* MagicSquareGenerator::BestFitness()
{
	MagicSquare* bestsq = population[0];
	double best = population[0]->Fitness();
	for (int i = 1; i < populationSize; i++) {
		if (population[i]->Fitness() < best) {
			best = population[i]->Fitness();
			bestsq = population[i];
		}
	}

	return bestsq;
}

void MagicSquareGenerator::GetElite()
{

	double* minFitness = new double[eliteSize];
	int* index = new int[eliteSize];

	for (int i = 0; i < eliteSize; i++) {
		index[i] = -1;
		minFitness[i] = 999999;
	}

	for (int i = 0; i < populationSize; i++) {

		for (int j = 0; j < eliteSize; j++) {
			if (population[i]->Fitness() < minFitness[j] && population[i]->Generations() < 1000) {

				for (int k = eliteSize - 2; k >= j; k--) {
					index[k + 1] = index[k];
					minFitness[k + 1] = minFitness[k];
				}

				minFitness[j] = population[i]->Fitness();
				index[j] = i;
				break;
			}
		}

	}

	for (int i = 0; i < eliteSize; i++) {

		elite[i] = new MagicSquare(population[index[i]]);
		elite[i]->Inc_Generations();
	}

	delete[] minFitness;
	delete[] index;

}
