//#define POPULUS 5000 //populacja poczatkowa
#define MUTACJA 10 //poczatkowa szansa na mutacje (w procentach) - min. 3
#define PAROWANIE 50 //wyparowywujacy feromon w czasie (w procentach) 
#define FEROMON 100

class KomEwol //Problem Komiwojazera algorytmami metaheurystcznymi ewolucyjnymi
{
	std::string nazwa;
	int wierzcholki, optimum;
	int** macierz;

	struct Jednostka //pomocnicza struktura wykorzystywana w algorytmach
	{
		int dlugosc;
		int* jednostka;
		Jednostka* nastepny = (Jednostka*)NULL;
		Jednostka* poprzedni = (Jednostka*)NULL;
	};

	class ListaJednostek //pomocnicza klasa wykorzystywana w algorytmach
	{
		Jednostka* glowa;
		Jednostka* ogon;
	public:
		ListaJednostek()
		{
			glowa = (Jednostka*)NULL;
			ogon = (Jednostka*)NULL;
		}
		void usun(Jednostka* sprawdzana)
		{
			if (sprawdzana == glowa)
			{
				glowa = sprawdzana->nastepny;
				if(glowa) glowa->poprzedni = (Jednostka*)NULL;
			}
			else { sprawdzana->poprzedni->nastepny = sprawdzana->nastepny; }
			if (sprawdzana == ogon)
			{
				ogon = sprawdzana->poprzedni;
				if(ogon) ogon->nastepny = (Jednostka*)NULL;
			}
			else { sprawdzana->nastepny->poprzedni = sprawdzana->poprzedni; }
			sprawdzana->poprzedni = sprawdzana->nastepny = (Jednostka*)NULL;
		}
		void dodaj(Jednostka* dodawana)
		{
			if (glowa == NULL) { glowa = ogon = dodawana; }
			else
			{
				ogon->nastepny = dodawana;
				dodawana->poprzedni = ogon;
				ogon = dodawana;
			}
		}
		Jednostka* start() { return glowa; }
		Jednostka* koniec() { return ogon; }
	};

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
		delete[]trasa;
		printf("Dlugosc trasy: %d \t", dlugosc);
		printf("Percentage Relative Deviation: %.2f\n", dlugosc * 100.00 / optimum);
	}
	bool wynik()
	{
		if (macierz) return true;
		return false;
	}

	Wyniki* GA(int POPULUS, bool wyswietl) //algorytm genetyczny
	{
		bool *zaliczony;
		int *kolejnosc, *drugi, *potomek, *najlepszy;
		Jednostka *temp, *amp, *pierwszy;
		int i, j, k, l, m, populacja, min, best, dlugosc, mut, alert, stop;
		if (!macierz)
		{
			printf("Nie mozna wykonac algorytmu bez problemu!\n");
			return (Wyniki*)NULL;
		}
		
		srand(time(NULL));
		ListaJednostek* lista = new ListaJednostek();
		ListaJednostek* skopulowani = new ListaJednostek();
		ListaJednostek* turniejowcy = new ListaJednostek();
		int** EX = new int*[wierzcholki];
		for (i = 0; i < wierzcholki; i++) EX[i] = new int[5];
		populacja = POPULUS;
		best = INT_MAX;
		mut = MUTACJA;
		stop = wierzcholki * POPULUS / 10; //kryterium stopu - brak poprawy przez N*p iteracji
		alert = (int)log(stop) + 1; //warunek geometrycznego przyrostu szansy na mutacje
		najlepszy = new int[wierzcholki + 1];

		//czesc populacji wyznaczana losowo-zachlannie
		for (i = 0; i < populacja / 100 ; i++)
		{
			temp = new Jednostka();
			zaliczony = (bool*)calloc(wierzcholki, sizeof(bool));
			kolejnosc = new int[wierzcholki + 1];
			dlugosc = 0;
			l = rand() % wierzcholki;
			kolejnosc[0] = l;
			zaliczony[l] = true;
			for (j = 0; j < wierzcholki / 3; j++) //trzecia czesc wierzcholkow wyznaczana metoda najblizszego sasiada
			{
				min = INT_MAX;
				for (k = 0; k < wierzcholki; k++)
				{
					if (macierz[kolejnosc[j]][k] < min && !zaliczony[k])
					{
						min = macierz[kolejnosc[j]][k];
						l = k;
					}
				}
				zaliczony[l] = true;
				kolejnosc[j + 1] = l;
				dlugosc += min;
			}
			for (j; j < wierzcholki - 1; j++) 
			{
				l = rand() % wierzcholki;
				while (zaliczony[l])
				{
					l = (l + 1) % wierzcholki;
				}
				zaliczony[l] = true;
				kolejnosc[j + 1] = l;
				dlugosc += macierz[kolejnosc[j]][l];
			}
			dlugosc += macierz[kolejnosc[j]][kolejnosc[0]];
			temp->dlugosc = dlugosc;
			temp->jednostka = kolejnosc;
			if (best > dlugosc)
			{
				memcpy(najlepszy, kolejnosc, sizeof(int) * (wierzcholki + 1));
				best = dlugosc;
			}
			lista->dodaj(temp);
			free(zaliczony);
		}
		
		//reszta populacji wyznaczana losowo
		for (i; i < populacja; i++)
		{
			temp = new Jednostka();
			zaliczony = (bool*)calloc(wierzcholki, sizeof(bool));
			kolejnosc = new int[wierzcholki + 1];
			dlugosc = 0;
			l = rand() % wierzcholki;
			kolejnosc[0] = l;
			zaliczony[l] = true;
			for (j = 0; j < wierzcholki - 1; j++)
			{
				l = rand() % wierzcholki;
				while (zaliczony[l])
				{
					l = (l + 1) % wierzcholki;
				}
				zaliczony[l] = true;
				kolejnosc[j + 1] = l;
				dlugosc += macierz[kolejnosc[j]][l];
			}
			dlugosc += macierz[kolejnosc[j]][kolejnosc[0]];
			temp->dlugosc = dlugosc;
			temp->jednostka = kolejnosc;
			if (best > dlugosc)
			{
				memcpy(najlepszy, kolejnosc, sizeof(int) * (wierzcholki + 1));
				best = dlugosc;
			}
			lista->dodaj(temp);
			free(zaliczony);
		}

		//wlasciwy algorytm genetyczny
		{
			i = 0;
			m = 0;
			if (wyswietl) printf("START %d (%.2f%%)\n", best, 100.00 * best / optimum);
			while(m++ < stop)
			{
				int kopulacja = populacja / 10; //ilosc krzyzowan w iteracji
				if (kopulacja == 0) kopulacja = 1;
				int turniej = populacja / kopulacja;
				if (turniej < 2) break; //populacja wymiera badz wymarla
				for (j = 0; j < kopulacja; j++)
				{
					//wyznaczanie osobnikow - selekcja turniejowa
					{
						for (k = 0; k < turniej; k++) //zbieramy jednostki do turnieju
						{
							temp = lista->start();
							l = rand() % populacja--; //nie uwzgledniamy juz wyciagnietych jednostek
							while (l)
							{
								if(temp->nastepny) temp = temp->nastepny;
								l--;
							}
							lista->usun(temp);
							turniejowcy->dodaj(temp);
						}
						amp = temp = turniejowcy->start();
						min = temp->dlugosc;
						while (temp)
						{
							if (temp->dlugosc < min)
							{
								amp = temp;
								min = temp->dlugosc;
							}
							temp = temp->nastepny;
						}
						pierwszy = amp;
						turniejowcy->usun(amp);
						skopulowani->dodaj(amp);
						temp = turniejowcy->start();
						while (temp) //zwracamy jednostki do populacji
						{
							turniejowcy->usun(temp);
							lista->dodaj(temp);
							populacja++;
							temp = turniejowcy->start();
						}
						if (turniej > populacja) turniej--;
						for (k = 0; k < turniej; k++)
						{
							temp = lista->start();
							l = rand() % populacja--;
							while (l)
							{
								if(temp->nastepny) temp = temp->nastepny;
								l--;
							}
							lista->usun(temp);
							turniejowcy->dodaj(temp);
						}
						amp = temp = turniejowcy->start();
						min = temp->dlugosc;
						while (temp)
						{
							if (temp->dlugosc < min)
							{
								amp = temp;
								min = temp->dlugosc;
							}
							temp = temp->nastepny;
						}
						turniejowcy->usun(amp);
						skopulowani->dodaj(amp);
						while (temp)
						{
							turniejowcy->usun(temp);
							lista->dodaj(temp);
							populacja++;
							temp = turniejowcy->start();
						}
					}

					//krzyzowanie osobnikow - krzyzowanie EX
					{
						kolejnosc = new int[wierzcholki + 1];
						temp = pierwszy;
						//tworzenie listy sasiedztwa
						for (k = 0; k < wierzcholki; k++)
						{
							EX[k][0] = 4;
							if (k)
							{
								EX[k][1] = temp->jednostka[k - 1];
								EX[k][2] = temp->jednostka[(k + 1) % wierzcholki];
							}
							else
							{
								EX[k][1] = temp->jednostka[1];
								EX[k][2] = temp->jednostka[wierzcholki - 1];
							}
							l = 0;
							while (amp->jednostka[l] != temp->jednostka[k]) l++;
							
							if (l)
							{
								if (EX[k][1] == amp->jednostka[l - 1] || EX[k][1] == amp->jednostka[(l + 1)%wierzcholki]) //jesli pierwszy wierzcholek jest osiagalny z drugiego rodzica
								{
									EX[k][3] = EX[k][2];
									EX[k][2] = -1;
									if (EX[k][2] == amp->jednostka[l - 1] || EX[k][2] == amp->jednostka[(l + 1)%wierzcholki])	EX[k][4] = -1; //jesli drugi wierzcholek rowniez jest osiagalny z obu
									else
									{
										if (EX[k][1] == amp->jednostka[l - 1]) EX[k][4] = amp->jednostka[(l + 1)%wierzcholki]; //dodajemy niepowtarzajacy sie wierzcholek
										else EX[k][4] = amp->jednostka[l - 1];
									}
								}
								else if (EX[k][2] == amp->jednostka[l - 1] || EX[k][2] == amp->jednostka[(l + 1)%wierzcholki]) //jesli drugi wierzcholek jest osiagalny z drugiego rodzica
								{
									EX[k][3] = -1;
									if (EX[k][2] == amp->jednostka[l - 1]) EX[k][4] = amp->jednostka[(l + 1)%wierzcholki];
									else EX[k][4] = amp->jednostka[l - 1];
								}
								else //zaden z wierzcholkow sie nie powtarza w obu rodzicach
								{
									EX[k][3] = amp->jednostka[l - 1];
									EX[k][4] = amp->jednostka[(l + 1)%wierzcholki];
								}
							}
							else //dla zerowej pozycji drugiego rodzica, parafraza powyzszej czynnosci
							{
								if (EX[k][1] == amp->jednostka[1] || EX[k][1] == amp->jednostka[wierzcholki - 1])
								{
									EX[k][3] = EX[k][2];
									EX[k][2] = -1;
									if (EX[k][2] == amp->jednostka[1] || EX[k][2] == amp->jednostka[wierzcholki - 1]) EX[k][4] = -1;
									else
									{
										if (EX[k][1] == amp->jednostka[1]) EX[k][4] = amp->jednostka[wierzcholki - 1];
										else EX[k][4] = amp->jednostka[1];
									}
								}
								else if (EX[k][2] == amp->jednostka[1] || EX[k][2] == amp->jednostka[wierzcholki - 1])
								{
									EX[k][3] = -1;
									if (EX[k][2] == amp->jednostka[1]) EX[k][4] = amp->jednostka[wierzcholki - 1];
									else EX[k][4] = amp->jednostka[1];
								}
								else
								{
									EX[k][3] = amp->jednostka[1];
									EX[k][4] = amp->jednostka[wierzcholki - 1];
								}
							}
							
						}

						//tworzenie osobnika
						bool candidates[4], dobble[4];
						int wierz = rand() % wierzcholki;
						int x, y, z, a;
						kolejnosc[0] = wierz;
						for (k = 1; k < wierzcholki;)
						{
							if (wierz == -1) //wybor wierzcholka losowego w przypadku braku
							{
								wierz = rand() % wierzcholki;
								while (EX[wierz][0] == -1) wierz = (wierz + 1) % wierzcholki;
								kolejnosc[k] = wierz;
								k++;
								continue;
							}
							for (l = 0; l < 4; l++)
							{
								candidates[l] = false;
								dobble[l] = false;
								if (EX[wierz][l + 1] == -1) continue;
								if (EX[EX[wierz][l + 1]][0] == -1) continue; //juz zaliczony wierzcholek
								EX[EX[wierz][l + 1]][0] = 4; //wyznaczanie licznosci w sasiadujacych wierzcholkach
								for (x = 1; x < 5; x++)
								{
									if (EX[EX[wierz][l + 1]][x] == -1)
									{
										EX[EX[wierz][l + 1]][0]--;
										continue;
									}
									if (EX[EX[EX[wierz][l + 1]][x]][0] == -1) EX[EX[wierz][l + 1]][0]--;
								}
							}

							EX[wierz][0] = -1; //wylaczenie wierzcholka obecnego z obiegu
							min = 5;
							for (l = 1; l < 5; l++) //szukanie potencjalnych kandydatow
							{
								x = EX[wierz][l];
								if (x == -1) //wierzcholek osiagalny z obu
								{
									dobble[l - 2] = true;
									continue;
								}
								if (EX[x][0] < min && EX[x][0] != -1) //wierzcholek o najmniejszej ilosci sasiadow
								{
									min = EX[x][0];
									for (y = 0; y < 4; y++) candidates[y] = false;
									candidates[l - 1] = true;
								}
								else if (EX[x][0] == min) candidates[l - 1] = true;
							}
							x = z = 0;
							y = a = -1;
							for (l = 0; l < 4; l++) //sprawdzanie kandydatow, najmniejsza ilosc ma priorytet nad osiagalnoscia z obu rodzicow
							{
								if (candidates[l])
								{
									x++;
									y = l;
								}
								if (dobble[l])
								{
									z++;
									a = l;
								}
							}
							if (x > 1) //wiecej niz jeden osiagalny wierzcholek o minimalnej dlugosci
							{
								if (z == 1) //wiele minimalnych, jesli ktorys jest podwojny, to zwycieza, jesli nie, to losujemy
								{
									if (candidates[a])
									{
										kolejnosc[k] = EX[wierz][a + 1];
										wierz = kolejnosc[k];
										k++;
										continue;
									}
								}
								z = rand() % 4;
								while (!candidates[z]) z = (z + 1) % 4;
								kolejnosc[k] = EX[wierz][z + 1];
								wierz = kolejnosc[k];
								k++;
							}
							else if (x == 1) //dokladnie jeden osiagalny wierzcholek z obu rodzicow
							{
								kolejnosc[k] = EX[wierz][y + 1];
								wierz = kolejnosc[k];
								k++;
							}
							else wierz = -1; //kazdy z zapisanych wierzcholkow zostal juz raz odwiedzony
						}
					}

					//mutacja nowego osobnika, kontrola najlepszej jednostki i dodanie osobnika do wylaczonych z iteracji
					{
						if(1.0*rand() / RAND_MAX < mut / 100.0)
						{ 
							k = rand() % (wierzcholki - 1);
							l = rand() % (wierzcholki - k) + k; //zapewnienie roznych granic
							if (k == l) l++;
							//mutacja typu inversion
							while (k < l)
							{
								int x = kolejnosc[k];
								kolejnosc[k++] = kolejnosc[l];
								kolejnosc[l--] = x;
							}
						}

						temp = new Jednostka();
						dlugosc = 0;
						for (k = 1; k < wierzcholki; k++) { dlugosc += macierz[kolejnosc[k - 1]][kolejnosc[k]]; }
						dlugosc += macierz[kolejnosc[k - 1]][kolejnosc[0]];
						if (best > dlugosc)
						{
							memcpy(najlepszy, kolejnosc, (wierzcholki + 1) * sizeof(int));
							best = dlugosc;
							if (wyswietl) printf("%d %d (%.2f%%)\n", i, best, 100.00 * best / optimum);
							m = 0;
							mut = MUTACJA;
						}
						temp->dlugosc = dlugosc;
						temp->jednostka = kolejnosc;
						skopulowani->dodaj(temp);
					}
				}
				
				//zwracamy osobnikow do kolejnych turniejow
				{
					temp = skopulowani->start();
					while (temp)
					{
						skopulowani->usun(temp);
						lista->dodaj(temp);
						temp = skopulowani->start();
					}
					populacja += 3*kopulacja; //aktualizujemy licznosc populacji
				}

				//usuwamy czesc populacji
				{
					j = populacja / 10;
					while (j--)
					{
						amp = temp = lista->start();
						while (temp)
						{
							if (amp->dlugosc < temp->dlugosc) amp = temp;
							temp = temp->nastepny;
						}
						lista->usun(amp);
						delete[] amp->jednostka;
						delete amp;
						populacja--;
					}
					if (populacja < 3) break; //populacja nie ma szans przetrwac (i dac istotnych wynikow)
				}

				if (m > alert && mut < 100)
				{
					mut = (int)mut * 1.2;
					if (mut > 100) mut = 100;
				}
				i++;
			}
			najlepszy[wierzcholki] = najlepszy[0]; //rzutujemy do modelu zwracanych danych
		}

		//czyszczenie pamieci
		{
			temp = lista->start();
			while (temp)
			{
				amp = temp;
				temp = temp->nastepny;
				delete[] amp->jednostka;
				delete amp;
			}
			delete lista;
			temp = skopulowani->start();
			while (temp)
			{
				amp = temp;
				temp = temp->nastepny;
				delete[] amp->jednostka;
				delete amp;
			}
			delete skopulowani;
			for (i = 0; i < wierzcholki; i++) delete[] EX[i];
			delete[] EX;
		}

		//zwracanie wynikow
		{
			Wyniki* rezultat = new Wyniki();
			rezultat->dlugosc = best;
			rezultat->optimum = optimum;
			rezultat->trasa = najlepszy;
			rezultat->wierzcholki = wierzcholki;
			return rezultat;
		}
	}
	Wyniki* ACO(int iteracje, bool wyswietl) //algorytm mrowkowy
	{
		bool poprawa = false;
		int i, j, k, l, min, temp, curr, dlugosc, best, mrowki, stop;
		float widocznosc, szansa, sumP, zachlanna;
		if (!macierz)
		{
			printf("Nie mozna wykonac algorytmu bez problemu!\n");
			return (Wyniki*)NULL;
		}
		mrowki = wierzcholki;
		
		srand(time(NULL));
		best = INT_MAX;
		int feromon = FEROMON * wierzcholki;
		int** szlak = new int*[mrowki];
		int** feromony = new int*[wierzcholki];
		bool** zaliczone = new bool*[mrowki];
		int* najlepszy = new int[wierzcholki + 1];
		for (i = 0; i < wierzcholki; i++)
		{
			feromony[i] = new int[wierzcholki];
			for (j = 0; j < wierzcholki; j++)
			{
				feromony[i][j] = wierzcholki * 10; //mala ilosc startowa feromonu
			}
		}
		for (i = 0; i < mrowki; i++)
		{
			szlak[i] = new int[wierzcholki];
			zaliczone[i] = (bool*)calloc(wierzcholki, sizeof(bool));
		}

		for (i = 0; i < iteracje; i++)
		{
			for (j = 0; j < mrowki; j++) //inicjowanie cyklu dla kazdej mrowki
			{
				curr = rand() % wierzcholki;
				szlak[j][0] = curr;
				zaliczone[j][curr] = true;
			}
			for (j = 1; j < wierzcholki; j++) //cykl wedrowek dla kazdej mrowki
			{
				for (k = 0; k < mrowki; k++)
				{
					sumP = 0.0;
					zachlanna = 0;
					curr = szlak[k][j - 1];
					for (l = 0; l < wierzcholki; l++)
					{
						if (zaliczone[k][l] || curr == l) continue;
						widocznosc = 1.0 / macierz[curr][l];
						szansa = feromony[curr][l] * pow(widocznosc, 3); // wplyw feromonu = 1, wplyw zachlannosci = 3
						sumP += szansa;
						if (szansa > zachlanna || !zachlanna) //na wypadek nie wylosowania zadnego miasta korzystamy z metody zachlannej
						{
							zachlanna = szansa;
							temp = l;
						}
					}
					for (l = 0; l < wierzcholki; l++)
					{
						if (zaliczone[k][l] || curr == l) continue;
						widocznosc = 1.0 / macierz[curr][l];
						szansa = feromony[curr][l] * pow(widocznosc, 3);
						if (szansa / sumP > 1.0 * rand() / RAND_MAX)
						{
							temp = l;
							break;
						}
					}
					szlak[k][j] = temp;
					zaliczone[k][temp] = true;
				}
			}
			for (j = 0; j < wierzcholki; j++) //parowanie feromonow pod koniec cyklu
			{
				for (k = 0; k < wierzcholki; k++)
				{
					if (feromony[j][k] && feromony[j][k] < 10) feromony[j][k]--; //aby feromon parowal rowniez jak zostalo mniej niz 10
					feromony[j][k] -= feromony[j][k] * PAROWANIE / 100; //parowanie feromonu
				}
			}
			stop = 0; //warunek stopu
			for (j = 0; j < mrowki; j++) //feromony uaktualniane cyklicznie, przy okazji jest liczona potrebna do tego dlugosc trasy, wiec nastepuje kontrola jakosci
			{
				dlugosc = macierz[szlak[j][wierzcholki - 1]][szlak[j][0]];
				for (k = 0; k < wierzcholki - 1; k++)	dlugosc += macierz[szlak[j][k]][szlak[j][k + 1]];
				for (k = 0; k < wierzcholki - 1; k++)	feromony[szlak[j][k]][szlak[j][k + 1]] += feromon / dlugosc;
				feromony[szlak[j][wierzcholki - 1]][szlak[j][0]] += feromon / dlugosc;
				if (dlugosc < best)
				{
					memcpy(najlepszy, szlak[j], wierzcholki * sizeof(int));
					best = dlugosc;
					poprawa = true;
					stop = 0;
				}
				if (dlugosc == best) stop++;
				free(zaliczone[j]);
				zaliczone[j] = (bool*)calloc(mrowki, sizeof(bool)); //tworzymy na nowo liste zakazanych wierzcholkow dla kazdej mrowki
			}
			if (i == 0) // na poczatku wyswietlana jest wartosc poczatkowa
			{
				if (wyswietl) printf("START %d (%.2f%%)\n", best, 100.00 * best / optimum);
				poprawa = false;
			}
			if(poprawa) if (wyswietl) printf("%d %d (%.2f%%)\n", i, best, 100.00 * best / optimum);
			poprawa = false;
			if (stop == mrowki) break;
		}

		//czyszczenie pamieci
		for (i = 0; i < wierzcholki; i++)
		{
			delete[] feromony[i];
		}
		for (i = 0; i < mrowki; i++)
		{
			delete[] szlak[i];
			delete[] zaliczone[i];
		}
		delete[] feromony;
		delete[] szlak;
		delete[] zaliczone;

		najlepszy[wierzcholki] = najlepszy[0];
		Wyniki* rezultat = new Wyniki();
		rezultat->dlugosc = best;
		rezultat->optimum = optimum;
		rezultat->trasa = najlepszy;
		rezultat->wierzcholki = wierzcholki;
		return rezultat;
	}
};

void menuEwol()
{
	printf("--------------MENU ALGORYTMOW EWOLUCYJNYCH--------------\n");
	printf("1.Zaladuj macierz sasiedztwa z pliku\n");
	printf("2.Wyswietl obecny problem\n");
	printf("3.Podaj wlasna trase\n");
	printf("4.Algorytm genetyczny\n");
	printf("5.Algorytm mrowkowy\n");
	printf("6.Tryb pomiaru czasu\n");
	printf("0.Powrot do menu typow algorytmow\n");
	printf("Podaj opcje: ");
}

void ewol()
{
	KomEwol* problem = new KomEwol();
	Wyniki* X;
	double Czas;
	short opcja = -1;
	int i, rozmiar;
	std::string plik;
	while (opcja)
	{
		menuEwol();
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
			if (problem->wynik())
			{
				printf("Podaj rozmiar populacji algorytmu: ");
				scanf_s("%d", &rozmiar);
			}
			X = problem->GA(rozmiar, true);
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
				delete [] X->trasa;
				delete X;
			}
			system("Pause");
			break;
		case 5:
			if (problem->wynik())
			{
				printf("Podaj ilosc iteracji dzialania algorytmu: ");
				scanf_s("%d", &rozmiar);
			}
			X = problem->ACO(rozmiar, true);
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
				delete [] X->trasa;
				delete X;
			}
			opcja = 5;
			system("Pause");
			break;
		case 6:
			if (problem->wynik() == false)
			{
				printf("Potrzeba problemu do rozwiazania!\n");
				system("Pause");
				break;
			}
			printf("1.GA\n2.ACO\nDajesz: ");
			scanf_s("%hd", &opcja);
			Czas = 0;
			switch (opcja)
			{
			case 1:
				if (problem->wynik())
				{
					printf("Podaj rozmiar populacji algorytmu: ");
					scanf_s("%d", &rozmiar);
				}
				for (i = 0; i < POPULACJA; i++)
				{
					StartCounter();
					problem->GA(rozmiar, false);
					Czas += GetCounter();
				}
				printf("Algorytm srednio zajmuje %.6f ms\n", Czas / POPULACJA);
				system("Pause");
				break;
			case 2:
				printf("Podaj ilosc iteracji dzialania algorytmu: ");
				scanf_s("%d", &rozmiar);
				for (i = 0; i < POPULACJA; i++)
				{
					StartCounter();
					problem->ACO(rozmiar, false);
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