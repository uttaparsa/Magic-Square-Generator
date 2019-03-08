#include "MagicSquareGenerator.h"
#include <iostream>
#include <time.h>

using namespace std;

int main() {

	srand(time(0));

	int n;
	int populationSize;
	int eliteSize;
	int mutationChance;

	do{
		cout << "Size of square: ";
		cin.clear();
		cin >> n;
		if (cin.fail()) {
			cout << "Argument must be an integer" << endl;
			continue;
		}
		if (n < 3) {
			cout << "Argument must be greater than or equal to 3" << endl;
			continue;
		}
		break;
	} while (true);

	do {
		cout << "Size of population: ";
		cin.clear();
		cin >> populationSize;
		if (cin.fail()) {
			cout << "Argument must be an integer" << endl;
			continue;
		}
		if (populationSize % 2 != 0) {
			cout << "Argument must be an even number" << endl;
			continue;
		}
		break;
	} while (true);

	do {
		cout << "Size of elite: ";
		cin.clear();
		cin >> eliteSize;
		if (cin.fail()) {
			cout << "Argument must be an integer" << endl;
			continue;
		}
		if (eliteSize > populationSize) {
			cout << "Elite size must be smaller than population size" << endl;
			continue;
		}
		break;
	} while (true);

	do {
		cout << "Chance of mutation (0-100): ";
		cin.clear();
		cin >> mutationChance;
		if (cin.fail()) {
			cout << "Argument must be an integer" << endl;
			continue;
		}
		if (mutationChance < 0) {
			cout << "Argument must not be negative" << endl;
			continue;
		}
		if (mutationChance > 100) {
			cout << "Argument must not be greater than 100" << endl;
			continue;
		}
		break;
	} while (true);

	cout << "\n";

	double timeT = clock();

	MagicSquareGenerator ms(n, populationSize, eliteSize, mutationChance);
	ms.Run(100000);

	cout << "Execution time: " << (clock() - timeT) / (double) CLOCKS_PER_SEC << " seconds" << endl;
	system("PAUSE");
}