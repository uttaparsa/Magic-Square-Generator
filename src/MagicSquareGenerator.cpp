#include "MagicSquareGenerator.h"
#include <cmath>
#include <time.h>
#include <iostream>
#include <algorithm>
#include <omp.h>

using namespace std;

MagicSquareGenerator::MagicSquareGenerator(int n, int populationSize, int eliteSize, int mutationChance)
{

	this->mutationChance = mutationChance;
	this->n = n;
	this->populationSize = populationSize;
	this->eliteSize = eliteSize;
	this->matingPoolSize = populationSize / 2;

	elite = new MagicSquare *[eliteSize];
	matingPool = new MagicSquare *[matingPoolSize];
	population = new MagicSquare *[populationSize];

	for (int i = 0; i < populationSize; ++i)
	{
		population[i] = new MagicSquare(n);
		if (CheckDuplicate(population, population[i], i))
		{
			delete population[i];
			i--;
		}
	}

	for (int i = 0; i < populationSize; ++i)
	{
		randomizedIndexes.push_back(i);
	}
}

MagicSquareGenerator::~MagicSquareGenerator()
{
	for (int i = 0; i < populationSize; ++i)
	{
		delete population[i];
	}
	delete[] population;
	delete[] matingPool;
	delete[] elite;
}

void MagicSquareGenerator::Run(int maxGenerations)
{
	omp_lock_t locks[populationSize], eliteLock[eliteSize], matingPoolLock[matingPoolSize], populationLock;
	omp_init_lock(&populationLock);
	for (int j = 0; j < populationSize; ++j)
	{
		omp_init_lock(locks + j);
	}
	for (int j = 0; j < eliteSize; ++j)
	{
		omp_init_lock(eliteLock + j);
	}
	for (int j = 0; j < matingPoolSize; ++j)
	{
		omp_init_lock(matingPoolLock + j);
	}
	bool found = false;


	omp_set_num_threads(32);

	
#pragma omp parallel shared(population, matingPool, randomizedIndexes, locks, eliteLock, matingPoolLock, elite, found, maxGenerations, populationSize, populationLock)
	{

		for (int i = 0; i <= maxGenerations; ++i)
		{
			if (!found)
			{
#pragma omp single
				{
					random_shuffle(randomizedIndexes.begin(), randomizedIndexes.end());
					//Determine the group of elite squares
					GetElite();
				}

				//Generate the mating pool, and delete unused squares from memory
#pragma omp for schedule(static, 4)
				for (int j = 0; j < populationSize - 1; j += 2)
				{

					if (population[randomizedIndexes[j]]->Fitness() <= population[randomizedIndexes[j + 1]]->Fitness())
					{
						omp_set_lock(matingPoolLock + j / 2);
						matingPool[j / 2] = population[randomizedIndexes[j]];
						omp_unset_lock(matingPoolLock + j / 2);
						omp_set_lock(locks + randomizedIndexes[j + 1]);
						delete population[randomizedIndexes[j + 1]];
						omp_unset_lock(locks + randomizedIndexes[j + 1]);
					}
					else
					{
						omp_set_lock(matingPoolLock + j / 2);
						matingPool[j / 2] = population[randomizedIndexes[j + 1]];
						omp_unset_lock(matingPoolLock + j / 2);
						omp_set_lock(locks + randomizedIndexes[j]);
						delete population[randomizedIndexes[j]];
						omp_unset_lock(locks + randomizedIndexes[j]);
					}
				}

//Move the elite squares into the next generation
#pragma omp for schedule(static)
				for (int j = 0; j < eliteSize; ++j)
				{
					omp_set_lock(locks + j);

					population[j] = elite[j];
					omp_unset_lock(locks + j);
				}

//Fill the rest of the new population with squares produced by cross-over within the mating pool
#pragma omp for schedule(static)
				for (int j = eliteSize; j < populationSize; ++j)
				{

					omp_set_lock(locks + j);

					int index1 = rand() % (populationSize / 2);
					int index2 = index1;
					while (index1 == index2)
					{
						index2 = rand() % (populationSize / 2);
					}

					population[j] = new MagicSquare(matingPool[index1], matingPool[index2], mutationChance, rand() % 2);

					omp_unset_lock(locks + j);
				}

#pragma omp single
				{
					for (int j = eliteSize; j < populationSize; ++j)
					{

						bool duplicate = CheckDuplicate(population, population[j], j);
						while (duplicate)
						{
							int index1 = rand() % (populationSize / 2);
							int index2 = index1;
							while (index1 == index2)
							{
								index2 = rand() % (populationSize / 2);
							}

							population[j] = new MagicSquare(matingPool[index1], matingPool[index2], mutationChance, rand() % 2);
							duplicate = CheckDuplicate(population, population[j], j);
						}
					}
				}

#pragma omp for schedule(static)
				for (int i = 0; i < populationSize / 2; ++i)
				{
					omp_set_lock(matingPoolLock + i);
					delete matingPool[i];
					omp_unset_lock(matingPoolLock + i);
				}

#pragma omp single
				{

					//Find the square with minimal fitness within the new population
					MagicSquare *best = BestFitness();

					//Check if a magic square has been found
					if (best->Fitness() == 0.0)
					{
						cout << "Generation: " << i << "\n";
						cout << "Found Magic Square: " << endl;
						best->print();
						found = true;
					}

					//Print the current status every 100 generations
					if (i % 100 == 0)
					{
						cout << "Generation: " << i << "\n";
						cout << "Fitness (Average): " << AverageFitness() << "\n";
						cout << "Fitness (Best): " << best->Fitness() << "\n"
							 << endl;
					}
				}

#pragma omp barrier
			}
		}

#pragma omp for
		for (int j = 0; j < populationSize; ++j)
		{
			omp_destroy_lock(locks + j);
		}
#pragma omp for
		for (int j = 0; j < matingPoolSize; ++j)
		{
			omp_destroy_lock(matingPoolLock + j);
		}
#pragma omp for
		for (int j = 0; j < eliteSize; ++j)
		{
			omp_destroy_lock(eliteLock + j);
		}
	}

	std::cout << "No Magic Square Found." << endl;
}

bool MagicSquareGenerator::CheckDuplicate(MagicSquare **population, MagicSquare *square, int end) const
{
	for (int i = 0; i < end; ++i)
	{

		if ((*square) == *(population[i])) //
		{
			return true;
		}
	}

	return false;
}

double MagicSquareGenerator::AverageFitness()
{
	long double total = 0;
	for (int i = 0; i < populationSize; ++i)
	{
		total += population[i]->Fitness();
	}
	return total / (long double)populationSize;
}

MagicSquare *MagicSquareGenerator::BestFitness()
{
	MagicSquare *bestsq = population[0];
	double best = population[0]->Fitness();
	for (int i = 1; i < populationSize; ++i)
	{
		if (population[i]->Fitness() < best)
		{
			best = population[i]->Fitness();
			bestsq = population[i];
		}
	}

	return bestsq;
}

void MagicSquareGenerator::GetElite()
{

	double *minFitness = new double[eliteSize];
	int *index = new int[eliteSize];

	for (int i = 0; i < eliteSize; ++i)
	{
		index[i] = -1;
		minFitness[i] = 999999;
	}

	for (int i = 0; i < populationSize; ++i)
	{

		for (int j = 0; j < eliteSize; ++j)
		{
			if (population[i]->Fitness() < minFitness[j] && population[i]->Generations() < 1000)
			{
				for (int k = eliteSize - 2; k >= j; k--)
				{
					index[k + 1] = index[k];
					minFitness[k + 1] = minFitness[k];
				}

				minFitness[j] = population[i]->Fitness();
				index[j] = i;
				break;
			}
		}
	}

	for (int i = 0; i < eliteSize; ++i)
	{

		elite[i] = new MagicSquare(population[index[i]]);
		elite[i]->Inc_Generations();
	}

	delete[] minFitness;
	delete[] index;
}
