#include "MagicSquare.h"
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

MagicSquare::MagicSquare(int n)
{
	generations = 0;

	this->n = n;
	square = new int*[n];
	for (int i = 0; i < n; i++) {
		square[i] = new int[n];
	}

	vector<int> numbers;

	for (int i = 1; i <= n * n; i++) {
		numbers.push_back(i);
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			int index = rand() % numbers.size();
			square[i][j] = numbers[index];
			numbers.erase(numbers.begin() + index);
		}
	}

	CalculateFitness();
}

MagicSquare::MagicSquare(const MagicSquare * sq)
{

	generations = sq->generations;
	n = sq->size();
	square = new int*[n];
	for (int i = 0; i < n; i++) {
		square[i] = new int[n];
		for (int j = 0; j < n; j++) {
			square[i][j] = (*sq)(i, j);
		}
	}

	fitness = sq->Fitness();

}

//Based on the special crossover algorithm found in: http://user.ceng.metu.edu.tr/~ucoluk/research/publications/tspnew.pdf
//This paper was used as a reference, but the code below is my own implementation.
MagicSquare::MagicSquare(const MagicSquare* square1, const MagicSquare* square2, int mutationChance, int childNum)
{

	
	generations = 0;

	this->n = square1->size();

	square = new int*[n];
	for (int i = 0; i < n; i++) {
		square[i] = new int[n];
	}
	
	int* square1Inverse = new int[n*n];
	int* square2Inverse = new int[n*n];

	int* childPosition = new int[n*n];
	int* childInverse = new int[n*n];

	for (int i = 1; i <= n * n; i++) {

		int larger = 0;
		bool br = false;
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < n; k++) {

				if ((*square1)(j, k) > i) {
					larger++;
				}
				else if ((*square1)(j, k) == i) {
					br = true;
					break;
				}

			}
			if (br) break;
		}
		square1Inverse[i - 1] = larger;

		larger = 0;
		br = false;
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < n; k++) {

				if ((*square2)(j, k) > i) {
					larger++;
				}
				else if ((*square2)(j, k) == i) {
					br = true;
					break;
				}

			}
			if (br) break;
		}
		square2Inverse[i - 1] = larger;

	}

	int crossoverPoint = (rand() % (n * n - 1)) + 1;

	for (int i = 0; i < n*n; i++) {
		if (childNum == 1) {
			if (i < crossoverPoint) {
				childInverse[i] = square1Inverse[i];
			}
			else {
				childInverse[i] = square2Inverse[i];
			}
		}
		else {
			if (i >= crossoverPoint) {
				childInverse[i] = square1Inverse[i];
			}
			else {
				childInverse[i] = square2Inverse[i];
			}
		}
	}

	for (int i = n * n - 1; i >= 0; i--) {

		childPosition[i] = childInverse[i];
		for (int j = i + 1; j < n*n; j++) {
			if (childPosition[j] >= childPosition[i]) childPosition[j]++;
		}

	}

	for (int i = 0; i < n*n; i++) {
		int position = childPosition[i];
		square[position / n][position%n] = i + 1;
	}
	
	if (rand() % 100 < mutationChance) {
		Mutate();
	}

	delete[] square1Inverse;
	delete[] square2Inverse;
	delete[] childPosition;
	delete[] childInverse;

	CalculateFitness();
	


}

MagicSquare::~MagicSquare()
{
	for (int i = 0; i < n; i++) {
		delete[] square[i];
	}
	delete[] square;
}

int MagicSquare::operator()(int i, int j) const
{
	return square[i][j];
}

bool MagicSquare::operator==(const MagicSquare& sq) const
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (square[i][j] != sq(i, j)) return false;
		}
	}

	return true;
}

void MagicSquare::print() const
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {

			cout << square[i][j];
			if (square[i][j] < 10) {
				cout << " ";
			}
			if (j != n - 1) {
				cout << "|";
			}

		}
		cout << endl;
	}
}

int MagicSquare::size() const
{
	return n;
}

double MagicSquare::Fitness() const
{
	return fitness;
}

int MagicSquare::RowSum(int row) const
{
	int sum = 0;
	for (int i = 0; i < n; i++) {
		sum += square[row][i];
	}
	return sum;
}

int MagicSquare::ColumnSum(int column) const
{
	int sum = 0;
	for (int i = 0; i < n; i++) {
		sum += square[i][column];
	}
	return sum;
}

int MagicSquare::DiagonalSum(int diagonal) const
{
	int sum = 0;
	for (int i = 0; i < n; i++) {
		if (diagonal == 0) {
			sum += square[i][i];
		}
		else {
			sum += square[n - i - 1][i];
		}
	}
	return sum;
}

void MagicSquare::CalculateFitness()
{

	vector<int> sums;
	sums.reserve(n * 2 + 2);

	for (int i = 0; i < n; i++) {
		sums.push_back(RowSum(i));
		sums.push_back(ColumnSum(i));
	}
	sums.push_back(DiagonalSum(0));
	sums.push_back(DiagonalSum(1));

	int total = 0;
	for (int i = 0; i < sums.size(); i++) {
		total += sums[i];
	}
	double average = (double)total / (double)sums.size();

	double temp = 0;
	for (int i = 0; i < sums.size(); i++) {
		temp += ((double)sums[i] - average) * ((double)sums[i] - average);
	}

	fitness = sqrt(temp / (double)sums.size());

}

void MagicSquare::Mutate()
{
	int x1 = rand() % n;
	int y1 = rand() % n;
	int x2 = x1;
	int y2 = y1;

	while (x1 == x2 && y1 == y2) {
		x2 = rand() % n;
		y2 = rand() % n;
	}

	int temp = square[y1][x1];
	square[y1][x1] = square[y2][x2];
	square[y2][x2] = temp;
}

void MagicSquare::Inc_Generations()
{
	generations++;
}

int MagicSquare::Generations() const
{
	return generations;
}
