#include "MagicSquareGenerator.h"
#include <iostream>
#include <time.h>

using namespace std;

int main() {

	srand(time(0));
	double timeT = clock();

	MagicSquareGenerator ms(5, 200, 4, 50);
	ms.Run(100000);

	cout << "Execution time: " << (clock() - timeT) / (double) CLOCKS_PER_SEC << " seconds" << endl;
	system("PAUSE");
}