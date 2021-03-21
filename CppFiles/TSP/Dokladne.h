/*#include <cstdio>
#include <string>
#include <cstdlib>
#include <conio.h>
#include <fstream>
#include <iostream>
#include "Commons.h"*/

class KomDok //problem komiwojazera algorytmami dokladnymi
{
	std::string nazwa;
	int wierzcholki, optimum;
	int** macierz;

public:

	int zaladuj(std::string nazwaPliku)
	{
		std::string instancja;
		int dana, i, j;
		std::ifstream plik;
		plik.open(nazwaPliku);
		if (!plik)	return(-1); //brak uchwytu
		plik >> instancja;
		plik >> dana;
		if (dana <= 0)  //niewlasciwa ilosc wierzcholkow
		{
			plik.close();
			return 0;
		}
		zniszcz();
		nazwa.assign(instancja);
		wierzcholki = dana;
		macierz = (int**)malloc(wierzcholki * sizeof(int*));
		for (i = 0; i < wierzcholki; i++) macierz[i] = (int*)malloc(wierzcholki * sizeof(int));
		for (i = 0; i < wierzcholki; i++)
		{
			for (j = 0; j < wierzcholki; j++)
			{
				plik >> dana;
				macierz[i][j] = dana;
			}
		}
		plik >> dana;
		optimum = dana;
		plik.close();
		return 1;
	}
	void zniszcz()
	{
		if (!macierz) return;
		for (int i = 0; i < wierzcholki; i++)
		{
			if (macierz[i])free(macierz[i]);
		}
		free(macierz);
	}
	void wyswietl() //wyswietlamy w indeksowaniu naturalnym, ale program liczy w normalnym
	{
		if (macierz)
		{
			int i, j;
			printf("%s\n", nazwa.c_str());
			for (i = 0; i <= wierzcholki; i++)
			{
				if (i == 0) printf("\t");
				else	printf("%7d", i);
				for (j = 0; j < wierzcholki; j++)
				{
					if (i == 0)
					{
						printf("%7d", j + 1);
						continue;
					}
					printf("%7d", macierz[i - 1][j]);
				}
				printf("\n");
			}
			printf("Optymalny cykl Hamiltona: %d\n", optimum);
		}
		else printf("BLAD! Brak danych!\n");
	}
	void sciezka()
	{
		bool* checker;
		int* trasa;
		char determinacja = 's';
		int dlugosc, k, i, j = 0;
		if (!macierz)
		{
			printf("Brak problemu do stworzenia sciezki!\n");
			return;
		}
		if (wierzcholki > 20)
		{
			printf("Czeka cie duzo wpisywania...\n");
			printf("Czy jestes pewien? [T/N] ");
			while (true)
			{
				scanf_s("%c", &determinacja, 1);
				if (determinacja == 't' || determinacja == 'T') break;
				if (determinacja == 'n' || determinacja == 'N') return;
				printf("To nie jest prawidlowa odpowiedz!");
			}
		}
		system("cls");
		wyswietl();
		trasa = new int[wierzcholki + 1];
		checker = (bool*)calloc(wierzcholki, sizeof(bool));
		while (j < wierzcholki - 1)
		{
			printf("\nPodaj kolejny wierzcholek: ");
			scanf_s("%d", &k);
			if (checker[k - 1])
			{
				printf("Wierzcholek juz byl wybrany!\n");
				continue;
			}
			checker[k - 1] = true;
			trasa[j] = k - 1;
			j++;
			system("cls");
			wyswietl();
			printf("Obecna trasa:\n");
			printf("%8d", trasa[0] + 1);
			for (i = 1; i < j; i++)
			{
				printf("  --->  %8d", trasa[i] + 1);
				if (i % 10 == 0) printf(" --->\n");
			}
		}
		k = 0;
		while (checker[k]) k++;
		free(checker);
		trasa[j] = k;
		trasa[j + 1] = trasa[0];
		printf("  --->  %8d  --->  %8d\n", trasa[j] + 1, trasa[j + 1] + 1);
		dlugosc = 0;
		for (i = 1; i < wierzcholki + 1; i++)
		{
			dlugosc += macierz[trasa[i - 1]][trasa[i]];
		}
		delete trasa;
		printf("Dlugosc trasy: %d \t", dlugosc);
		printf("Percentage Relative Deviation: %.2f\n", dlugosc * 100.00 / optimum);
	}
	
	void psyGoncze(int* teren, int* skrot, bool* check, int* min, int* shortest, int dlugosc, int kolejnosc, bool show) //funkcja rekurencyjna realizujaca branch and bound
	{
		if (shortest[0] > *min) return; // ograniczenie dolne - najkrotsza pozostala niemozliwa trasa
		int size, temp, i, j;
		temp = macierz[teren[kolejnosc - 1]][teren[kolejnosc]]; //dlugosc krawedzi jest dodawana w kroku po przypisaniu wierzcholka
		size = temp + dlugosc;
		if (size > * min) return;	//ograniczenie górne - dotychczasowa sciezka
		if (kolejnosc == wierzcholki - 1)
		{
			teren[wierzcholki] = 0;
			size += macierz[teren[kolejnosc]][0];
			if (size < *min)
			{
				*min = size;
				for (i = 1; i < wierzcholki + 1; i++) skrot[i] = teren[i];
				if (show) printf("%d (%.2f%)\n", size, 100.00 * size / optimum);
			}
			return;
		}
		j = kolejnosc + 1;
		if (kolejnosc != 1)shortest[0] += temp - shortest[kolejnosc - 1]; //odejmujemy wartosc najkrotszej krawedzi wychodzacej z wierzcholka i dodajemy dlugosc obecnej krawedzi
		for (i = 1; i < wierzcholki; i++)
		{
			if (check[i]) continue;
			check[i] = true;
			teren[j] = i;
			psyGoncze(teren, skrot, check, min, shortest, size, j, show);
			check[i] = false;
		}
		if (kolejnosc != 1)shortest[0] += shortest[kolejnosc - 1] - temp; //tu tez sprawdzamy warunek, bo w momencie zakonczenia wywolujemy z innego wierzcholka
	}
	void detektyw(int* teren, int* skrot, bool* check, int* min, int dlugosc, int kolejnosc, bool show) //funkcja rekurencyjna realizujaca brute force
	{
		int size, i, j;
		size = dlugosc + macierz[teren[kolejnosc - 1]][teren[kolejnosc]]; 
		if (kolejnosc == wierzcholki - 1)
		{
			teren[wierzcholki] = 0;
			size += macierz[teren[kolejnosc]][0];
			if (size < *min)
			{
				*min = size;
				for (i = 1; i < wierzcholki + 1; i++) skrot[i] = teren[i];
				if (show) printf("%d (%.2f%)\n", size, 100.00 * size / optimum);
			}
			return;
		}
		j = kolejnosc + 1;
		for (i = 1; i < wierzcholki; i++)
		{
			if (check[i]) continue;
			check[i] = true;
			teren[j] = i;
			detektyw(teren, skrot, check, min, size, j, show);
			check[i] = false;
		}
	}
	bool instancja(int* wzor, int* lista, int pojemnosc)
	{
		int i, j;
		bool check;
		for (i = 1; i < pojemnosc; i++)
		{
			check = false;
			for (j = 1; j < pojemnosc; j++)
			{
				if (wzor[i] == lista[j]) check = true;
			}
			if (!check) return check;
		}
		return true;
	}
	bool wynik()
	{
		if (macierz) return true;
		return false;
	}

	Wyniki* BF(bool wyswietl)
	{
		bool* test;
		int *obecny, *kolejnosc;
		int i, dystans;
		Wyniki* zwrot;
		if (!macierz)
		{
			printf("Nie mozna wykonac algorytmu bez problemu!\n");
			return (Wyniki*)NULL;
		}
		test = (bool*)calloc(wierzcholki, sizeof(bool));
		kolejnosc = new int[wierzcholki + 1];
		obecny = new int[wierzcholki + 1];
		obecny[0] = 0; //ze wzgledu na istnienie cykli, nie ma znaczenia od ktorego zaczynamy, wiec zawsze zaczynajmy od pierwszego
		kolejnosc[0] = 0;
		dystans = INT_MAX;
		for (i = 1; i < wierzcholki; i++)
		{
			test[i] = true;
			obecny[1] = i;
			detektyw(obecny, kolejnosc, test, &dystans, 0, 1, wyswietl);
			test[i] = false;
		}

		zwrot = new Wyniki();
		zwrot->dlugosc = dystans;
		zwrot->optimum = optimum;
		zwrot->trasa = kolejnosc;
		zwrot->wierzcholki = wierzcholki;
		delete []obecny;
		free(test);
		return zwrot;
	}
	Wyniki* BnB(bool wyswietl) //algorytm Branch&Bound
	{
		bool* test;
		int *obecny, *kolejnosc, *min;
		int i, j, dystans;
		Wyniki* zwrot;
		if (!macierz)
		{
			printf("Nie mozna wykonac algorytmu bez problemu!\n");
			return (Wyniki*)NULL;
		}
		test = (bool*)calloc(wierzcholki, sizeof(bool));
		kolejnosc = new int[wierzcholki + 1];
		obecny = new int[wierzcholki + 1];
		min = new int[wierzcholki];
		
		min[0] = 0;
		for (i = 1; i < wierzcholki; i++) //inicjalizowanie tablicy dajacej ograniczenie dolne - najkrotsza niemozliwa sciezka 
		{									//suma najkrotszych krawedzi laczaca dwa wierzcholki bez uwzglednienia odleglosci
			dystans = INT_MAX;
			for (j = 0; j < wierzcholki; j++)
			{
				if (i == j) continue;
				if (macierz[i][j] < dystans) dystans = macierz[i][j];
			}
			min[0] += dystans;
			min[i] = dystans;
		}
		
		dystans = 0;
		for (i = 0; i < wierzcholki; i++)
		{
			kolejnosc[i] = i;
			dystans += macierz[i][(i + 1) % wierzcholki];
		}
		kolejnosc[i] = 0;
		obecny[0] = 0;
		if (wyswietl) printf("Wstepne gorne ograniczenie:\t%d (%.2f%)\n", dystans, 100.00 * dystans / optimum);

		for (i = 1; i < wierzcholki; i++)
		{
			test[i] = true;
			obecny[1] = i;
			min[0] += macierz[0][i];
			psyGoncze(obecny, kolejnosc, test, &dystans, min, 0, 1, wyswietl);
			min[0] -= macierz[0][i];
			test[i] = false;
		}

		zwrot = new Wyniki();
		zwrot->dlugosc = dystans;
		zwrot->optimum = optimum;
		zwrot->trasa = kolejnosc;
		zwrot->wierzcholki = wierzcholki;

		delete []obecny;
		delete []min;
		free(test);
		return zwrot;
	}
	Wyniki* HK() //algorytm Helda-Kapara (prog. dynamiczne)
	{
		int i, j, k, l, min, trasa;
		int* kolejnosc, * tmp, * amp;
		Wyniki* final;
		bool check;
		if (!macierz)
		{
			printf("Nie mozna wykonac algorytmu bez problemu!\n");
			return (Wyniki*)NULL;
		}

		std::vector<int*> nizszy, obecny, temp;
		kolejnosc = new int[wierzcholki + 1];
		kolejnosc[wierzcholki] = 0; //zawsze zaczynamy pierwszym wierzcholkiem, wiec w cyklu na nim konczymy
		nizszy.reserve(wierzcholki);
		obecny.reserve(wierzcholki * (wierzcholki - 1));
		for (i = 1; i < wierzcholki; i++) //inicjalizowanie pierwszego kroku
		{
			tmp = new int[2];
			tmp[0] = macierz[0][i];
			tmp[1] = i;
			nizszy.push_back(tmp);
		}

		l = 2; //obecna ilosc elementow podzbioru (z wliczeniem trasy w indeksie 0)
		for (i = 1; i < wierzcholki - 1; i++) //wykonanie algorytmu do uzyskania podzbioru o liczebnosci n-1
		{
			while (!nizszy.empty())
			{
				tmp = nizszy.front();
				for (auto x : nizszy) if (instancja(tmp, x, l)) temp.push_back(x); //zbieramy wszystkie instancje danego podzbioru, badamy je tylko raz

				for (j = 1; j < wierzcholki; j++)
				{
					check = false; //sprawdzamy, czy dany wierzcholek nie jest wewnatrz instancji
					min = INT_MAX;
					for (k = 1; k < l; k++) if (temp.front()[k] == j) check = true;
					if (check) continue; //jesli jest w pierwszej, jest w kazdej
					for (auto x : temp)
					{
						trasa = macierz[x[l - 1]][j] + x[0];
						if (trasa < min)
						{
							min = trasa;
							tmp = x;
						}
					}
					amp = new int[l + 1];
					memcpy(amp, tmp, l * sizeof(int));
					amp[l] = j;
					amp[0] = min;
					obecny.push_back(amp);
				}

				for (auto x : temp)
				{
					auto y = std::find(nizszy.begin(), nizszy.end(), x);
					nizszy.erase(y);
					delete x;
				}
				temp.clear();
			}

			l++;
			nizszy = std::move(obecny);
		}

		min = INT_MAX;
		tmp = nullptr; //tylko dlatego, ze kompilator nie widzi ponizszego dzialania jako inicjalizacji
		for (auto x : nizszy)
		{
			trasa = macierz[x[wierzcholki - 1]][0] + x[0];
			if (trasa < min)
			{
				min = trasa;
				tmp = x;
			}
		}
		memcpy(kolejnosc, tmp, l * sizeof(int));
		kolejnosc[0] = 0; //dlugosc trasy do tego punktu jest nam juz zbedna

		final = new Wyniki();
		final->dlugosc = min;
		final->trasa = kolejnosc;
		final->optimum = optimum;
		final->wierzcholki = wierzcholki;

		for (auto x : nizszy)
		{
			delete x;
			x = nullptr;
		}
		nizszy.clear();

		return final;
	}
};

void menuDok()
{
	printf("--------------MENU ALGORYTMOW DOKLADNYCH--------------\n");
	printf("1.Zaladuj macierz sasiedztwa z pliku\n");
	printf("2.Wyswietl obecny problem\n");
	printf("3.Podaj wlasna trase\n");
	printf("4.Algorytm Brute Force\n");
	printf("5.Algorytm Branch & Bound\n");
	printf("6.Algorytm Helda-Karpa\n");
	printf("7.Tryb pomiaru czasu\n");
	printf("0.Powrot do menu typow algorytmow\n");
	printf("Podaj opcje: ");
}

void dok()
{
	KomDok* problem = new KomDok();
	Wyniki* X;
	double Czas;
	short opcja = -1;
	int i;
	std::string plik;
	while (opcja)
	{
		menuDok();
		scanf_s("%hd", &opcja);
		switch (opcja)
		{
		case 1:
			printf("Podaj nazwe pliku: ");
			std::cin >> plik;
			switch (problem->zaladuj(plik))
			{
			case -1:
				printf("Nie udalo sie wczytac pliku...\n");
				break;
			case 0:
				printf("W pliku sa nieprawidlowe dane!\n");
				break;
			case 1:
				printf("Poprawnie zaladowano plik %s!\n", plik.c_str());
				problem->wyswietl();
				break;
			}
			system("Pause");
			break;
		case 2:
			problem->wyswietl();
			system("Pause");
			break;
		case 3:
			problem->wyswietl();
			problem->sciezka();
			system("Pause");
			break;
		case 4:
			X = problem->BF(true);
			if (X)
			{
				for (i = 0; i <= X->wierzcholki; i++)
				{
					printf("%d\t", X->trasa[i] + 1);
					if (i % 10 == 9) printf("\n");
				}
				printf("\n");
				printf("Dlugosc trasy: %d\t", X->dlugosc);
				printf("Percentage Relative Deviation: %.2f\n", X->dlugosc * 100.00 / X->optimum);
				delete X->trasa;
				delete X;
			}
			system("Pause");
			break;
		case 5:
			X = problem->BnB(true);
			if (X)
			{
				for (i = 0; i <= X->wierzcholki; i++)
				{
					printf("%d\t", X->trasa[i] + 1);
					if (i % 10 == 9) printf("\n");
				}
				printf("\n");
				printf("Dlugosc trasy: %d\t", X->dlugosc);
				printf("Percentage Relative Deviation: %.2f\n", X->dlugosc * 100.00 / X->optimum);
				delete X->trasa;
				delete X;
			}
			system("Pause");
			break;
		case 6:
			X = problem->HK();
			if (X)
			{
				for (i = 0; i <= X->wierzcholki; i++)
				{
					printf("%d\t", X->trasa[i] + 1);
					if (i % 10 == 9) printf("\n");
				}
				printf("\n");
				printf("Dlugosc trasy: %d\t", X->dlugosc);
				printf("Percentage Relative Deviation: %.2f\n", X->dlugosc * 100.00 / X->optimum);
				delete X->trasa;
				delete X;
			}
			system("Pause");
			break;
		case 7:
			if (problem->wynik() == false)
			{
				printf("Potrzeba problemu do rozwiazania!\n");
				system("Pause");
				break;
			}
			printf("1.BF\n2.BnB\n3.HK\nDajesz: ");
			scanf_s("%hd", &opcja);
			Czas = 0;
			switch (opcja)
			{
			case 1:
				for (i = 0; i < POPULACJA; i++)
				{
					StartCounter();
					problem->BF(false);
					Czas += GetCounter();
				}
				printf("Algorytm srednio zajmuje %.6f ms\n", Czas/POPULACJA);
				system("Pause");
				break;
			case 2:
				for (i = 0; i < POPULACJA; i++)
				{
					StartCounter();
					problem->BnB(false);
					Czas += GetCounter();
				}
				printf("Algorytm srednio zajmuje %.6f ms\n", Czas/POPULACJA);
				system("Pause");
				break;
			case 3:
				for (i = 0; i < POPULACJA; i++)
				{
					StartCounter();
					problem->HK();
					Czas += GetCounter();
				}
				printf("Algorytm srednio zajmuje %.6f ms\n", Czas/POPULACJA);
				system("Pause");
				break;
			}
			opcja = 7;
			break;
		case 0:
			problem->zniszcz();
			delete problem;
			break;
		default:
			printf("Nieobslugiwana opcja! Sprobuj ponownie!\n");
			system("Pause");
		}
		system("cls");
	}
}
