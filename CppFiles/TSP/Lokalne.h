#define LINIOWY 0.8

class KomLok //problem komiwojazera algorytmami metaheurystycznego przeszukiwania lokalnego
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
	bool wynik()
	{
		if (macierz) return true;
		return false;
	}

	Wyniki* TS(bool wyswietl) //algorytm przeszukiwania z zakazami
	{
		bool tab;
		bool* zaliczony;
		int *obecny, *najlepszy, *sasiad, *temp;
		int c, i, j, k, l, m, n, tmp, koszt, dystans, min, best, kryterium;
		Wyniki* zwrot;
		if (!macierz)
		{
			printf("Nie mozna wykonac algorytmu bez problemu!\n");
			return (Wyniki*)NULL;
		}
		int krytyk = (int)log2(wierzcholki) + 1;
		kryterium = krytyk * wierzcholki;
		std::vector<int*> tabu;
		obecny = new int[wierzcholki + 1];
		najlepszy = new int[wierzcholki + 1];
		sasiad = new int[wierzcholki + 1];
		dystans = 0; //poczatkowe rozwiazanie wyznaczamy z algorytmu NN
		zaliczony = (bool*)calloc(wierzcholki, sizeof(bool));
		zaliczony[0] = true;
		najlepszy[0] = 0;
		k = 0;
		for (i = 1; i < wierzcholki; i++)
		{
			j = 1;
			while (zaliczony[j]) j++;
			l = j;
			min = macierz[k][l];
			for (j; j < wierzcholki; j++)
			{
				if (min > macierz[k][j] && !zaliczony[j])
				{
					min = macierz[k][j];
					l = j;
				}
			}
			najlepszy[i] = l;
			dystans += min;
			zaliczony[l] = true;
			k = l;
		}
		free(zaliczony);
		najlepszy[wierzcholki] = 0;
		memcpy(obecny, najlepszy, (wierzcholki + 1) * sizeof(int));
		memcpy(sasiad, najlepszy, (wierzcholki + 1) * sizeof(int));
		dystans += macierz[k][0];
		best = dystans;
		if (wyswietl) printf("START %d (%.2f%%)\n", best, 100.00 * best / optimum);

		c = 0;
		j = 0;
		while (j++ < kryterium && c++ < krytyk)
		{
			for (i = 0; i < wierzcholki; i++)
			{
				min = INT_MAX;
				//wyznaczenie sasiedztwa zamiane miast 
				for (k = 1; k < wierzcholki - 1; k++) // pierwszy i zarazem ostatni wierzcholek nie jest wazny ze wzgledu na cyklicznosc, a obsluga go jest problematyczna 
				{
					for (l = k + 1; l < wierzcholki; l++)
					{
						koszt = dystans;
						koszt = koszt - macierz[obecny[k - 1]][obecny[k]] - macierz[obecny[k]][obecny[k + 1]] - macierz[obecny[l]][obecny[l + 1]];
						if (l != k + 1) koszt -= macierz[obecny[l - 1]][obecny[l]];
						sasiad[k] = obecny[l];
						sasiad[l] = obecny[k];
						koszt += macierz[sasiad[k - 1]][sasiad[k]] + macierz[sasiad[k]][sasiad[k + 1]] + macierz[sasiad[l]][sasiad[l + 1]];
						if (l != k + 1) koszt += macierz[sasiad[l - 1]][sasiad[l]];
						if (koszt < min)
						{
							if (koszt > best) //kryterium aspiracji
							{
								tab = false;
								for (auto x : tabu) //sprawdzenie tabu jest bardziej kosztowne niz sprawdzenie dystansu
									if ((x[0] == k && x[1] == l) || (x[0] == l && x[1] == k))
									{
										tab = true;
										sasiad[k] = obecny[k];
										sasiad[l] = obecny[l];
										break;
									}
								if (tab) continue;
							}
							min = koszt;
							if (koszt < best)
							{
								c = 0;
								best = koszt;
								memcpy(najlepszy, sasiad, wierzcholki * sizeof(int)); // nie wiemy jak wiele zmian wprowadzono od ostatniego
								if (wyswietl) printf("%d %d (%.2f%%)\n", (j * wierzcholki) + i + 1, best, 100.00 * best / optimum);
							}
							n = l;
							m = k;
						}
						sasiad[k] = obecny[k];
						sasiad[l] = obecny[l];
					}
				}
				for (auto x : tabu)
				{
					if (--x[2] <= 0)
					{
						auto y = std::find(tabu.begin(), tabu.end(), x);
						tabu.erase(y);
						delete x;
					}
				}

				if (min == INT_MAX) continue;
				sasiad[n] = obecny[m];
				sasiad[m] = obecny[n];
				obecny[n] = sasiad[n];
				obecny[m] = sasiad[m];
				temp = new int[3];
				temp[0] = obecny[n];
				temp[1] = obecny[m];
				temp[2] = min / dystans;
				tabu.push_back(temp);
				dystans = min;
			}
		}

		delete[]obecny;
		delete[]sasiad;
		for (auto x : tabu)
		{
			delete x;
		}
		tabu.clear();

		zwrot = new Wyniki();
		zwrot->dlugosc = best;
		zwrot->optimum = optimum;
		zwrot->trasa = najlepszy;
		zwrot->wierzcholki = wierzcholki;
		return zwrot;
	}
	Wyniki* SA(bool wyswietl) //algorytm symulowanego wyrza¿ania
	{
		srand(time(NULL));
		bool *zaliczony;
		int *kolejnosc, *obecny, *najlepszy, *sasiad;
		int i, j, x, y, z, s, dystans, min, koszt, tmp;
		int dl_epoki = wierzcholki; //dla kazdego problemu jest wyznaczana eksperymentalnie, wiec to dobry poczatek
		double temperatura, szansa, rzut, dzielna = 0.85; //zmiana temperatury schematem liniowym Caushy'ego
		Wyniki* zwrot;
		if (!macierz)
		{
			printf("Nie mozna wykonac algorytmu bez problemu!\n");
			return (Wyniki*)NULL;
		}
		int kryterium = (int)log2(dl_epoki) + 1;
		kryterium *= dl_epoki; //kryterium stopu - tyle epok w stagnacji musi min¹æ
		kolejnosc = new int[wierzcholki + 1];
		obecny = new int[wierzcholki];
		najlepszy = new int[wierzcholki];
		sasiad = new int[wierzcholki];
		koszt = 0; //koszt wyznaczamy z algorytmu NN
		zaliczony = (bool*)calloc(wierzcholki, sizeof(bool));
		zaliczony[0] = true;
		x = 0;
		for (i = 1; i < wierzcholki; i++)
		{
			j = 0;
			while (zaliczony[j]) j++;
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
			najlepszy[x] = y;
			koszt += min;
			zaliczony[y] = true;
			x = y;
		}
		free(zaliczony);
		najlepszy[x] = 0;
		memcpy(obecny, najlepszy, wierzcholki * sizeof(int));
		memcpy(sasiad, obecny, wierzcholki * sizeof(int));
		koszt += macierz[x][0];
		//przyjetym wspolczynnikiem dla temperatury poczatkowej jest 1, wiec nie wykonujemy dzialan
		dystans = koszt;
		min = koszt;
		if (wyswietl) printf("START %d (%.2f%%)\n", min, 100.00 * min / optimum);

		j = 0;
		s = 0;
		temperatura = (double)koszt;
		while (j++ < kryterium)
		{
			for (i = 0; i < dl_epoki; i++)
			{
				//wyznaczenie sasiedztwa zmieniajac trzy krawedzie, z wiedza jak je zmieniamy 
				x = rand() % wierzcholki;
				koszt -= macierz[x][obecny[x]];
				tmp = rand() % (wierzcholki - 2) + 1;
				z = rand() % (wierzcholki - tmp - 1) + 1;
				y = x;
				while (tmp--) y = obecny[y];
				koszt -= macierz[y][obecny[y]];
				tmp = z;
				z = y;
				while (tmp--) z = obecny[z];
				koszt -= macierz[z][obecny[z]];
				
				sasiad[x] = obecny[y];
				koszt += macierz[x][sasiad[x]];
				sasiad[y] = obecny[z];
				koszt += macierz[y][sasiad[y]];
				sasiad[z] = obecny[x];
				koszt += macierz[z][sasiad[z]];

				if (koszt < dystans)
				{
					obecny[x] = sasiad[x];
					obecny[y] = sasiad[y];
					obecny[z] = sasiad[z];
					if (koszt < min)
					{
						min = koszt;
						memcpy(najlepszy, sasiad, wierzcholki * sizeof(int)); // nie wiemy jak wiele zmian wprowadzono od ostatniego
						if (wyswietl) printf("%d %d (%.2f%%)\n", s + 1, min, 100.00 * min / optimum);
					}
					j = 0;
					dystans = koszt;
				}
				else
				{
					szansa = exp((dystans - koszt) / temperatura);
					rzut = (double)rand() / RAND_MAX;
					if (rzut < szansa)
					{
						obecny[x] = sasiad[x];
						obecny[y] = sasiad[y];
						obecny[z] = sasiad[z];
						j = 0;
						dystans = koszt;
					}
					else
					{
						sasiad[x] = obecny[x];
						sasiad[y] = obecny[y];
						sasiad[z] = obecny[z];
						koszt = dystans;
					}
				}
			}
			s++;
			temperatura /= dzielna;
			dzielna += LINIOWY;
		}
		
		delete []obecny;
		delete []sasiad;
		
		kolejnosc[0] = 0;
		x = najlepszy[0];
		kolejnosc[1] = x;
		for (i = 2; i < wierzcholki+1; i++)
		{
			x = najlepszy[x];
			kolejnosc[i] = x;
		}
		delete[]najlepszy;

		zwrot = new Wyniki();
		zwrot->dlugosc = min;
		zwrot->optimum = optimum;
		zwrot->trasa = kolejnosc;
		zwrot->wierzcholki = wierzcholki;

		return zwrot;
	}
};

void menuLok()
{
	printf("--------------MENU ALGORYTMOW LOKALNYCH--------------\n");
	printf("1.Zaladuj macierz sasiedztwa z pliku\n");
	printf("2.Wyswietl obecny problem\n");
	printf("3.Podaj wlasna trase\n");
	printf("4.Algorytm poszukiwania z zakazami\n");
	printf("5.Algorytm symulowanego wyzarzania\n");
	printf("6.Tryb pomiaru czasu\n");
	printf("0.Powrot do menu typow algorytmow\n");
	printf("Podaj opcje: ");
}

void lok()
{
	KomLok* problem = new KomLok();
	Wyniki* X;
	double Czas;
	short opcja = -1;
	int i;
	std::string plik;
	while (opcja)
	{
		menuLok();
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
			X = problem->TS(true);
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
			X = problem->SA(true);
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
			if (problem->wynik() == false)
			{
				printf("Potrzeba problemu do rozwiazania!\n");
				system("Pause");
				break;
			}
			printf("1.TS\n2.SA\nDajesz: ");
			scanf_s("%hd", &opcja);
			Czas = 0;
			switch (opcja)
			{
			case 1:
				for (i = 0; i < POPULACJA; i++)
				{
					StartCounter();
					problem->TS(false);
					Czas += GetCounter();
				}
				printf("Algorytm srednio zajmuje %.6f ms\n", Czas / POPULACJA);
				system("Pause");
				break;
			case 2:
				for (i = 0; i < POPULACJA; i++)
				{
					StartCounter();
					problem->SA(false);
					Czas += GetCounter();
				}
				printf("Algorytm srednio zajmuje %.6f ms\n", Czas / POPULACJA);
				system("Pause");
				break;
			}
			opcja = 6;
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
