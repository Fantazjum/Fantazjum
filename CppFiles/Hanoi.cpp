#include <iostream>
#include <vector>

class tower
{
	int limit;
  public:
	std::vector<int> discs;
	
	tower(int num, int lim)
	{
		discs = std::vector<int>();
		discs.reserve(lim);
		for(int i=0; i<num; i++) discs.push_back(num - i);
	}
	
	void move(tower* other)
	{
		other->discs.push_back(discs.back());
		discs.pop_back();
	}
	
	void show(int height)
	{
		if(discs.size() <= height) return;
		std::cout << discs[height];
	}
};

tower *X, *Y, *Z; 
int lim;

void hanoi(int num, tower* A, tower* B, tower* C)
{
	if(0 == num) return;
	hanoi(num - 1, A, C, B);
	A->move(C);
	for(int i=lim; i>-1; i--)
	{
		X->show(i);
		std::cout << "\t";
		Y->show(i);
		std::cout << "\t";
		Z->show(i);
		std::cout << std::endl;
	}
	std::cout << std::endl;
	hanoi(num - 1, B, A, C);
}

int main()
{
	lim = 0;
	while(lim <= 0)
	{
		std::cout << "Input number of discs on Hanoi towers: ";
		std::cin >> lim; 
		if(lim <= 0) std::cout << "Invalid number of discs!" << std::endl;
	}
	std::cout.flush();
	X = new tower(lim, lim);
	Y = new tower(0, lim);
	Z = new tower(0, lim);
	lim--;
	for(int i=lim; i>-1; i--)
	{
		X->show(i);
		std::cout << "\t";
		Y->show(i);
		std::cout << "\t";
		Z->show(i);
		std::cout << std::endl;
	}
	std::cout << std::endl;
	hanoi(lim + 1, X, Y, Z);
	system("pause");
	return 0;
}
