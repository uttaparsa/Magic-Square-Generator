#pragma once

class MagicSquare
{
public:

	//Generates a random square of size n*n
	MagicSquare(int n);

	//Creates a copy of another square
	MagicSquare(const MagicSquare* sq);

	//Generates a square through cross-over
	//	square1, square2: Parent squares
	//	mutationChance: Chance of mutation; [0, 100]
	//	childNum: Direction of crossover; [0, 1]
	MagicSquare(const MagicSquare* square1, const MagicSquare* square2, int mutationChance, int childNum);

	//Destructor
	~MagicSquare();

	//Returns the number at square[i][j]
	int operator()(int i, int j) const;

	//Returns true if two squares are the same
	bool operator==(const MagicSquare& sq) const;

	//Prints the square to the console
	void print() const;

	//Returns 'n'
	int size() const;

	//Returns 'fitness'
	double Fitness() const;

	//Mutates the square, randomly swapping two numbers
	void Mutate();

	//Increases 'generations' by one
	void Inc_Generations();

	//Returns 'generations'
	int Generations() const;


private:

	//Returns the sum of all numbers in a row
	//	row: [0, n)
	int RowSum(int row) const;

	//Returns the sum of all numbers in a column
	//	column: [0, n)
	int ColumnSum(int column) const;

	//Returns the sum of all numbers in a diagonal
	//	diagonal: [0, 1]
	int DiagonalSum(int diagonal) const;

	//Calculates the standard deviation of the sums of all rows, columns, and diagonals
	void CalculateFitness();


	//Fields
	int generations;
	double fitness;
	int** square;
	int n;
};

