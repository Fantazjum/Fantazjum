/*#include <cstdio>
#include <string>
#include <cstdlib>
#include <conio.h>
#include <fstream>
#include <iostream>
#include "Commons.h"*/


class KomZach //Problem Komiwojazera algorytmami zach³annymi 
{
	std::string nazwa;
	int wierzcholki, optimum;
	int** macierz;

	struct Krawedz //pomocnicza struktura wykorzystywana w algorytmach
	{
		int WP, WK, Dl; //wierzcholek poczatkowy, wierzcholek koncowy, dlugosc
	};

	void kopcowanieK(Krawedz** todo, int idx, int rozmiar) //dla kopca minimalnego w algorytmie minimalnej krawedzi
	{
		int min;
		if (2 * idx > rozmiar) return;
		if (todo[2 * idx - 1]->Dl < todo[idx - 1]->Dl) min = 2 * idx; //literatura z ktorej korzystalem bazuje na naturalnym indeksowaniu
		else min = idx;
		if (2 * idx + 1 <= rozmiar) if (todo[2 * idx]->Dl < todo[min - 1]->Dl) min = 2 * idx + 1;
		if (min != idx)
		{
			Krawedz* temp = todo[idx - 1];
			todo[idx - 1] = todo[min - 1];
			todo[min - 1] = temp;
			kopcowanieK(todo, min, rozmiar);
		}
	}

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
					printf("%7d", macierz[i-1][j]);
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
		if (wierzcholki>20)
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
		checker = (bool*) calloc(wierzcholki,sizeof(bool));
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
			printf("%8d",trasa[0] + 1);
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
		printf("  --->  %8d  --->  %8d\n", trasa[j] + 1, trasa[j+1] + 1);
		dlugosc = 0;
		for (i = 1; i < wierzcholki + 1; i++)
		{
			dlugosc += macierz[trasa[i - 1]][trasa[i]];
		}
		delete []trasa;
		printf("Dlugosc trasy: %d \t", dlugosc);
		printf("Percentage Relative Deviation: %.2f\n", dlugosc * 100.00 / optimum);
	}
	bool wynik()
	{
		if (macierz) return true;
		return false;
	}
	
	Wyniki* NN(int start, int lowest) //najblizszy sasiad
	{
		bool* zaliczony;
		int* kolejnosc;
		int i, j, x, y, min, dlugosc;
		if (!macierz)
		{
			printf("Nie mozna wykonac algorytmu bez problemu!\n");
			return (Wyniki*)NULL;
		}
		zaliczony = (bool*)calloc(wierzcholki, sizeof(bool));
		kolejnosc = (int*)malloc((1 + wierzcholki) * sizeof(int));
		zaliczony[start] = true;
		kolejnosc[0] = start;
		x = start;
		dlugosc = 0;
		for (i = 1; i < wierzcholki; i++) //nie rozwazamy powrotu do startu, bo jest tylko jedna mozliwosc
		{
			j = 0;
			while (zaliczony[j])	j++; //szukamy pierwszego nieodwiedzonego wierzcholka
			y = j;
			min = macierz[x][y];
			for (j; j < wierzcholki; j++)
			{
				if (min > macierz[x][j] && !zaliczony[j])
				{
					min = macierz[x][j];
					y = j;
				}
			}
			dlugosc += min;
			if (lowest && dlugosc > lowest)
			{
				free(zaliczony);
				free(kolejnosc);
				return (Wyniki*)NULL; //przyspieszenie w wielokrotnym wywolywaniu
			}
			zaliczony[y] = true;
			kolejnosc[i] = y;
			x = y;
		}
		dlugosc += macierz[y][start];
		kolejnosc[wierzcholki] = start;

		free(zaliczony);
		Wyniki* rezultat = new Wyniki();
		rezultat->dlugosc = dlugosc;
		rezultat->optimum = optimum;
		rezultat->trasa = kolejnosc;
		rezultat->wierzcholki = wierzcholki;
		return rezultat;
	}
	Wyniki* RNN() //powtarzalny wielokrotny sasiad
	{
		int i, trasa=0;
		Wyniki* wynik=(Wyniki*)NULL;
		Wyniki* temp;
		if (!macierz)
		{
			printf("Nie mozna wykonac algorytmu bez problemu!\n");
			return (Wyniki*) NULL;
		}
		for (i = 0; i < wierzcholki; i++)
		{
			temp = NN(i, trasa);
			
			if(temp)	if (i == 0 || trasa > temp->dlugosc)
			{
				if (wynik)
				{
					free(wynik->trasa);
					delete wynik;
				}
				wynik = temp;
				trasa = wynik->dlugosc;
			}
		}
		return wynik;
	}
	Wyniki* ME() //minimalna krawedz
	{
		bool* target,* init; //na kazdym wierzcholku wchodzi i wychodzi jedna krawedz
		int *color; //krawedz nie moze tworzyc cyklu
		Krawedz** lista,** przyjete;
		int* kolejnosc;
		int i, j, k, dlugosc = 0;
		if (!macierz)
		{
			printf("Nie mozna wykonac algorytmu bez problemu!\n");
			return (Wyniki*)NULL;
		}
		target = (bool*)calloc(wierzcholki, sizeof(bool));
		init = (bool*)calloc(wierzcholki, sizeof(bool));
		color = (int*)malloc(wierzcholki*sizeof(int));
		for (i = 0; i < wierzcholki; i++) color[i] = i;
		kolejnosc = (int*)malloc((wierzcholki+1) * sizeof(int));
		lista = (Krawedz**)malloc((wierzcholki*(wierzcholki-1)) * sizeof(Krawedz*));
		przyjete = (Krawedz**)malloc(wierzcholki * sizeof(Krawedz*));
		k = 0;
		for (i = 0; i < wierzcholki; i++) //tworzenie listy krawedzi
		{
			for (j = 0; j < wierzcholki; j++)
			{
				if (i == j) continue;
				lista[k] = new Krawedz();
				lista[k]->WP = i;
				lista[k]->WK = j;
				lista[k]->Dl = macierz[i][j];
				k++;
			}
		}
		i = k/2;
		while (i) //zmiana listy w kopiec minimalny
		{
			kopcowanieK(lista, i, k);
			i--;
		}
		while (i < wierzcholki-1)
		{
			if (!target[lista[0]->WK] && !init[lista[0]->WP] && color[lista[0]->WK] != color[lista[0]->WP])
			{
				j = color[lista[0]->WK];
				if (color[color[lista[0]->WK]]!=color[lista[0]->WK]) 
				{
					while (color[j] != color[lista[0]->WK])
					{
						color[lista[0]->WK] = color[j];
						j = color[lista[0]->WK];
					}
					continue;
				}
				dlugosc += lista[0]->Dl;
				przyjete[i] = lista[0];
				target[lista[0]->WK] = true;
				init[lista[0]->WP] = true;
				color[lista[0]->WP] = j;
				i++;
			}
			else delete lista[0];
			k--;
			lista[0] = lista[k];
			kopcowanieK(lista, 1, k);
		}
		for (j = 0; j < k; j++) delete lista[j];
		free(lista);
		j = 0;
		while (target[j]) j++;
		k = 0;
		while (init[k])k++;
		free(color);
		free(init);
		free(target);
		przyjete[i] = new Krawedz();
		przyjete[i]->WP = k;
		przyjete[i]->WK = j;
		dlugosc += macierz[k][j];
		j = k = 0;
		kolejnosc[0] = przyjete[0]->WP;
		j = przyjete[0]->WK;
		kolejnosc[1] = j;
		k = 2;
		while (k < wierzcholki) //porzadkujemy trase zgodnie z kolejnoscia
		{
			for (i = 0; i < wierzcholki; i++)
			{
				if (przyjete[i]->WP == j)
				{
					j = przyjete[i]->WK;
					kolejnosc[k] = j;
					k++;
					break;
				}
			}
		}
		kolejnosc[k] = kolejnosc[0];
		for (i = 0; i < wierzcholki; i++) delete przyjete[i];
		free(przyjete);
		Wyniki* rezultat = new Wyniki();
		rezultat->dlugosc = dlugosc;
		rezultat->optimum = optimum;
		rezultat->trasa = kolejnosc;
		rezultat->wierzcholki = wierzcholki;
		return rezultat;
	}
};

void menuZach()
{
	printf("--------------MENU ALGORYTMOW ZACHLANNYCH--------------\n");
	printf("1.Zaladuj macierz sasiedztwa z pliku\n");
	printf("2.Wyswietl obecny problem\n");
	printf("3.Podaj wlasna trase\n");
	printf("4.Algorytm najblizszego sasiada\n");
	printf("5.Powtarzalny algorytm najblizszego sasiada\n");
	printf("6.Algorytm najmniejszej krawedzi\n");
	printf("7.Tryb pomiaru czasu\n");
	printf("0.Powrot do menu typow algorytmow\n");
	printf("Podaj opcje: ");
}

void zach()
{
	KomZach* problem = new KomZach();
	Wyniki* X;
	double Czas;
	short opcja=-1;
	int i;
	std::string plik;
	while (opcja)
	{
		menuZach();
		scanf_s("%hd",&opcja);
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
			X = problem->NN(0,0); //zaczynamy od zerowego wierzholka, bo zawsze istnieje
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
				free(X->trasa);
				delete X;
			}
			system("Pause");
			break;
		case 5:
			X = problem->RNN();
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
				free(X->trasa);
				delete X;
			}
			system("Pause");
			break;
		case 6:
			X = problem->ME();
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
				free(X->trasa);
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
			printf("1.NN\n2.RNN\n3.ME\nDajesz: ");
			scanf_s("%hd", &opcja);
			Czas = 0;
			switch (opcja)
			{
			case 1:
				for (i = 0; i < POPULACJA; i++)
				{
					StartCounter();
					problem->NN(0, 0);
					Czas += GetCounter();
				}
				printf("Algorytm srednio zajmuje %.6f ms\n", Czas/POPULACJA);
				system("Pause");
				break;
			case 2:
				for (i = 0; i < POPULACJA; i++)
				{
					StartCounter();
					problem->RNN();
					Czas += GetCounter();
				}
				printf("Algorytm srednio zajmuje %.6f ms\n", Czas/POPULACJA);
				system("Pause");
				break;
			case 3:
				for (i = 0; i < POPULACJA; i++)
				{
					StartCounter();
					problem->RNN();
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
