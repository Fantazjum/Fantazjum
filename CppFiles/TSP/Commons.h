//wszystkie obiekty bedace uzywane w wielu rodzajach algorytmow beda tutaj

struct Wyniki
{
	int* trasa;
	int dlugosc, wierzcholki, optimum; //przekazujemy ilosc wierzcholkow i optimum, aby byl dostep do danych klasy
};

//Funkcje pomiaru czasu zaczynaja sie tutaj

double PCFreq = 0.0; //stale globalne pozwalajace na obliczenie czasu dzialania
__int64 CounterStart = 0;
#define POPULACJA 10

void StartCounter() //funckja skopiowana za wskazowkami prowadzacego
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li)) printf("QueryPerformanceFrequency failed!\n");

	PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}
double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}