#include "MagicSquareGenerator.h"
#include <iostream>
#include <time.h>
#include <omp.h>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{

	srand(time(0));

	int n;
	int populationSize;
	int eliteSize;
	int mutationChance;

	ifstream inputFile;
	inputFile.open(argv[1]);


	inputFile>> n >> populationSize >> eliteSize >> mutationChance;
	inputFile.close();

	cout << "read data from file : n="<< n << 
	", populationSize="<< populationSize << 
	", eliteSize="<< eliteSize << 
	", mutationChance="<< mutationChance  
	<<"\n";

	double timeT = clock();

	MagicSquareGenerator ms(n, populationSize, eliteSize, mutationChance);
	ms.Run(20);

	cout << "Execution time: " << (clock() - timeT) / (double)CLOCKS_PER_SEC << " seconds" << endl;
	system("PAUSE");
}