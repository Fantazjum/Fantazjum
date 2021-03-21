/*
Podstawowy element struktury: signed int [ 32 bit (4 bajty) ]
Zrezygnowalem z wczesniejszego niezaimplementowanego polimorfizmu po uswiadomieniu sobie jak bardzo beda sie roznic funkcje struktur i ile castowania mnie czeka
Pozostale struktury sa wynikiem
*/
#define KOPIEC 20		//wartosc stala dodawana do tablicy kopcowej
#define POSZUKIWANY 22348	//wartosc ktora bedzie poszukiwana w pomiarze czasu

#define PUSTA 0 //Kody bledow obslugiwanych w programie
#define ROZMIAR 1
#define NEGATYW 2
#define BRAK 3

#include <cstdio>
#include <string>
#include <cstdlib>
#include <ctime>
#include <windows.h>

double PCFreq = 0.0; //stale globalne pozwalajace na obliczenie czasu dzialania
__int64 CounterStart = 0;

void StartCounter() //funckja skopiowana za wskazowkami prowadzacego
{
    LARGE_INTEGER li;
    if( !QueryPerformanceFrequency( & li ) ) printf("QueryPerformanceFrequency failed!\n");
   
    PCFreq = double( li.QuadPart )/1000.0;
   
    QueryPerformanceCounter( & li );
    CounterStart = li.QuadPart;
}
double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter( & li );
    return double( li.QuadPart - CounterStart ) / PCFreq;
}

struct ListaElement //element listy, w strukturze, bo w klasie nalezaloby dopisac na poczatku public:, a jest tylko jeden typ zmiennych
{
	ListaElement* poprzedni;
	ListaElement* nastepny;
	int wart;
};

struct WezelCz //CZerwono-CZarne
{
	WezelCz* lewy;
	WezelCz* prawy;
	WezelCz* przodek;
	int klucz;
	bool kolor; //czerwony -  false, czarny - true
	
	WezelCz()
	{
		klucz=0;
		kolor=false; //dodajemy czerwone i dopiero wtedy rownowazymy
		lewy=(WezelCz*)NULL;
		prawy=(WezelCz*)NULL;
		przodek=(WezelCz*)NULL;
	}
};

void Err(int x) //Funkcja najczesciej pojawiajacych sie bledow z informacja zwrotna
{
	printf("\nBLAD!\n");
	switch(x)
	{
		case PUSTA:
			printf("Operacja niewykonywalna na pustej strukturze!\n");
			break;
		case ROZMIAR:
			printf("Odwolano sie do obszaru przekraczajacego rozmiar struktury!\n");
			break;
		case NEGATYW:
			printf("Oczekiwano wartosci dodatniej!\n");
			break;	
		case BRAK:
			printf("Operacja wymaga istnienia struktury!\n");
			break;
	}
}

class Lista
{						
	ListaElement* glowa;//prywatne wartosci calej struktury, by sie nie dalo ich zmienic
	ListaElement* ogon;
	int ilosc;
	
public:
	void zniszcz() //funkcja ta niszczy slady bytu poprzedniej struktury, ale zostawia pusty uchwyt
	{
		ListaElement* pogrzeb;
		ListaElement* temp;
		pogrzeb=ogon;
		if(pogrzeb==NULL) return;
		while(pogrzeb->poprzedni) //powtarzamy dopoki nie grozi nam siegniecie do elementu pustego elementu
		{
			temp=pogrzeb->poprzedni;
			delete pogrzeb;
			pogrzeb=temp;
		}
		delete pogrzeb;
		glowa=(ListaElement*) NULL;
		ogon=(ListaElement*) NULL;
		ilosc=0;
	}
	int zaladuj(std :: string nazwaPliku) //funkcja ta laduje strukture z pliku. Nie przewiduje sie opcji zapisu do pliku
	{
		int dana=0;
		ListaElement* temp;
		FILE* plik;
		plik=fopen(nazwaPliku.c_str(),"rt");
		if(plik==NULL)	return(-1); //informacja o braku uchwytu do pliku
		fscanf(plik,"%d",&dana);
		if(dana<0) return 0; //informacja o nieprawidlowej wartosci oznaczajacej ilosc
		zniszcz(); //porzucamy poprzednia liste na rzecz nowej
		ilosc=dana;
		if(ilosc>0)
		{
			fscanf(plik, "%d",&dana);
			glowa=new ListaElement;
			ogon=glowa;
			glowa->poprzedni=(ListaElement*) NULL;
			glowa->nastepny=(ListaElement*) NULL;
			glowa->wart=dana;
		}
		for(int i=1;i<ilosc;i++) //postanowilem zaufac, ze podajacy plik potrafi liczyc i nie sprawdzam
		{
			fscanf(plik,"%d",&dana);
			temp=new ListaElement;
			temp->wart=dana;
			temp->nastepny=(ListaElement*)NULL;
			temp->poprzedni=ogon;
			ogon->nastepny=temp;
			ogon=temp;
		}
		fclose(plik);
		return 1;
	}	
	bool znajdz(int wartosc)
	{
		if(ilosc==0) return 0; //nie ma sensu przeszukiwac pustej struktury
		ListaElement* temp=glowa;
		for(int i=0;i<ilosc;i++)
		{
			if(temp->wart==wartosc)		return(1);
			temp=temp->nastepny;
		}
		return 0;
	}
	void dodaj(int klucz, int indeks)
	{
		if(indeks>ilosc) //nie pozwalam na dodanie pustych elementow do wyrownania wielkosci
		{
			Err(ROZMIAR);
			return;
		}
		int i;
		ListaElement* temp;
		ListaElement* x = new ListaElement;
		x->wart=klucz;
		if(indeks<=ilosc/2) //decyzja, czy szukamy pozycji od poczatku listy, czy od jej konca
		{
			temp=glowa;
			if(temp==NULL)
			{
				glowa=x;
				ogon=x;
				x->nastepny=(ListaElement*)NULL;
				x->poprzedni=(ListaElement*)NULL;
				ilosc++;
				return;
			}
			if(indeks==0) //pozwala na unikniecie problemu siegania po element pustego elementu
			{
				glowa->poprzedni=x;
				x->nastepny=glowa;
				x->poprzedni=(ListaElement*)NULL;
				glowa=x;
				temp->poprzedni=x;
				ilosc++;
				return;
			}
			for(i=0;i<indeks;i++)	temp=temp->nastepny;
		}
		else
		{
			temp=ogon;
			for(i=ilosc;i>indeks;i--)	temp=temp->poprzedni;
			if (indeks==ilosc) //pozwala na unikniecie problemu siegania po element pustego elementu
			{
				x->poprzedni=ogon;
				x->nastepny=(ListaElement*)NULL;
				ogon=x;
				temp->nastepny=x;
				ilosc++;
				return;
			}
		}
		(temp->poprzedni)->nastepny=x; //domyslna operacja wstawiania elementu
		x->poprzedni=temp->poprzedni;
		x->nastepny=temp;
		temp->poprzedni=x;
		ilosc++;
	}
	void usun(int wartosc)
	{
		int i=0;
		if(ilosc==0) 
		{
			Err(PUSTA);
			return;
		}
		ListaElement* ofiara=glowa;
		while(i<ilosc) //szukamy elementu do usuniecia
		{
			if(ofiara->wart==wartosc)	break;
			ofiara=ofiara->nastepny;
			i++;
		}
		if(i==ilosc) return; //nie nastapilo przerwanie i doszlismy poza liste
		if(i==0) //naprawianie usuwania graniczny wartosci
		{
			glowa=ofiara->nastepny;
			if(glowa)	glowa->poprzedni=(ListaElement*)NULL;
		}
		else if(i==ilosc-1)
		{
			ogon=ofiara->poprzedni;
			if(ogon)	ogon->nastepny=(ListaElement*)NULL;
		}
		else //domyslne usuwanie wartosci
		{
			(ofiara->nastepny)->poprzedni=ofiara->poprzedni;
			(ofiara->poprzedni)->nastepny=ofiara->nastepny;
		}
		delete ofiara;
		ilosc--;
	}
	void wyswietl()
	{
		if(ilosc==0)
		{
			printf("Nie ma elementu do wyswietlenia!\n");
			return;
		}
		printf("\nZawartosc listy dwukierunkowej o %d elementach, od poczatku:\n", ilosc);
		int i;
		ListaElement* obecny=glowa;
		while(obecny)
		{
			printf("%d ",obecny->wart);
			obecny=obecny->nastepny;
		}
		printf("\n");
		printf("\nZawartosc tej samej listy dwukierunkowej od konca:\n");
		obecny=ogon;
		while(obecny)
		{
			printf("%d ",obecny->wart);
			obecny=obecny->poprzedni;
		}
		printf("\n");
	}
	void generuj(int rozmiar)
	{
		if(rozmiar<=0)
		{
			Err(NEGATYW);
			return;
		}
		zniszcz(); //porzucamy poprzednia liste na rzecz nowej
		ListaElement * x = new ListaElement();
		ListaElement * y;
		glowa=x;
		srand(time(NULL));
		x->poprzedni=(ListaElement*)NULL;
		x->wart=rand();
		ilosc++;
		for(int i=1;i<rozmiar;i++) //tworzenie listy alternatywnie do sposobu przy tworzeniu z pliku
		{
			y = new ListaElement();
			y->wart=rand();
			y->nastepny=(ListaElement*)NULL;
			y->poprzedni=x;
			x->nastepny=y;
			x=y;
			ilosc++;
		}
		x->nastepny=(ListaElement*)NULL;
		ogon=x;
	}
};

class Tablica
{
	int* tab;
	int ilosc;
	
public:
	void zniszcz() //funkcja istnieje tylko do zwalniania pamieci przy porzucaniu struktury z zewnatrz
	{
		if(tab!=NULL)free(tab);
	}
	int zaladuj(std :: string nazwaPliku)
	{
		int dana;
		FILE* plik;
		plik=fopen(nazwaPliku.c_str(),"rt");
		if(plik==NULL)	return(-1); //brak uchwytu
		fscanf(plik,"%d",&dana);
		if(dana<0) return 0; //rozmiar nie moze byc ujemny
		if(tab!=NULL) free(tab); //usuwamy stara tablice na rzecz nowej
		ilosc=dana;
		tab=(int*)malloc(dana*sizeof(int)); //i tak nadpisujemy wartosci, wiec wyczyszczenie wartosci jest zbedne
		for(int i=0;i<ilosc;i++) //tu wiara w umiejetnosci liczenia
		{
			fscanf(plik," %d",tab+i);
		}
		fclose(plik);
		return 1;
	}
	bool znajdz(int wartosc)
	{
		if(ilosc==0) return 0; 
		for(int i=0;i<ilosc;i++)	if(tab[i]==wartosc) return(1); //mozna sie odniesc tak samo jak do *(tab+i)
		return(0);
	}
	void dodaj(int klucz, int indeks)
	{
		if(indeks>ilosc) //nie wyrownujemy roznicy
		{
			Err(ROZMIAR);
			return;
		}
		ilosc++;
		if(ilosc==1) tab=(int*)malloc(sizeof(int)); //nie wywolujemy calloc, bo i tak nadpisujemy wartosc nadana
		else tab=(int*)realloc(tab,ilosc*sizeof(int)); //dynamiczna realokacja
		int temp;
		for(int i=indeks;i<ilosc;i++) //nadpisujemy dopiero od miejsca, gdzie dodajemy
		{
			temp=tab[i];
			tab[i]=klucz;
			klucz=temp;
		}		
	}
	void usun(int index) //usuwamy komorke na wskazanej pozycji i zmieniamy rozmiar pamieci tablicy
	{
		if(ilosc==0)
		{
			Err(PUSTA);
			return;
		}
		if(ilosc<index) return;
		ilosc--;
		for(int i=index;i<ilosc;i++) tab[i]=tab[i+1];
		tab=(int*)realloc(tab,ilosc*sizeof(int));
	}
	void wyswietl()
	{
		printf("Zawartosc tablicy o %d elementach:\n", ilosc);
		for(int i=0;i<ilosc;i++) printf("%d ",tab[i]);
		printf("\n");
	}
	void generuj(int rozmiar)
	{
		if(rozmiar<=0)
		{
			Err(NEGATYW);
			return;
		}
		ilosc=rozmiar;
		srand(time(NULL));
		if(tab!=NULL) free(tab); //usuwamy stara tablice na rzecz nowej
		tab=(int*)malloc(ilosc*sizeof(int));
		for(int i=0;i<ilosc;i++) tab[i]=rand();
	}
};

class Kopiec //pamietaj o wyswietlaniu drzewa Nie jest traktowany jak dziedziczacy po drzewie ze wzgledu na swoja budowe
{
	int wartosc;
	int rozmiar;
	int wezly;
	int * korzen;
	void kopcowanie(int index)	//funkcja przywracajaca wlasnosc kopca, wyzsza wartosc idzie na gore
	{
		if(2*index>wezly) return;
		int max;
		if(korzen[2*index-1]>korzen[index-1]) max=2*index; //taka zabawa ze wzgledu na literature na ktorej sie uczylem wlasnoscii kopca
		else max=index;
		if(2*index+1<=wezly)
		{
			if(korzen[2*index]>korzen[max-1]) max=2*index+1;
		}
		if(max!=index)
		{
			int temp=korzen[index-1];
			korzen[index-1]=korzen[max-1];
			korzen[max-1]=temp;
			kopcowanie(max);
		}
	}
	int lowy(int wart, int index) //szybkie przeszukanie z mozliwoscia zwrocenia pozycji
	{
		if (index>wezly) return (-1);
		if (korzen[index-1]==wart) return index;
		if (korzen[index-1]>wart) 
		{
			int f=lowy(wart,2*index);
			if(f>=0) return f; //przeszukiwanie od lewej ma pierwszenstwo
			f=lowy(wart,2*index+1);
			if(f>=0) return f;
		}
		return (-1);
	}
	void show(std::string prefix, int index, bool lewy) //funkcja rekurencyjnie rysujaca  drzewo
	{
		if(index<=wezly) //wiekszy indeks jest poza kopcem, indeks jest liczony jako naturalny
		{
			char x;
			printf("%s",prefix.c_str());
			if(lewy && wezly>index) x=195;
			else x=192;
			if(index==1) x=0;
			printf("%c %5d\n",x,korzen[index-1]);
			std::string roz="";
			if(lewy) x=179;
			else x=0;
			roz.append(1,x);
			if(lewy) roz=roz+"\t";
			else roz="\t"+roz+"\t";
			roz=prefix.append(roz.c_str());
			show(roz,2*index,true);
			show(roz,2*index+1,false);
		}
	}
	
	public:
	void zniszcz()
	{
		if(korzen!=NULL)	free(korzen);
		korzen=(int*)NULL;
		wezly=0;
		rozmiar=0;
	}
	int zaladuj(std::string nazwaPliku)
	{
		int dana,i;
		FILE* plik;
		plik=fopen(nazwaPliku.c_str(),"rt");
		if(plik==NULL)	return(-1); //brak uchwytu
		fscanf(plik,"%d",&dana);
		if(dana<0) return 0; //rozmiar nie moze byc ujemny
		if(korzen!=NULL) free(korzen); //usuwamy stara tablice na rzecz nowej
		rozmiar=dana;
		wezly=dana;
		korzen=(int*)malloc(dana*sizeof(int)); //i tak nadpisujemy wartosci, wiec wyczyszczenie wartosci jest zbedne
		for(i=0;i<rozmiar;i++) //tu wiara w umiejetnosci liczenia powraca
		{
			fscanf(plik," %d",korzen+i);
		}
		for(i=wezly/2;i>0;i--) //zaczynamy dopiero od ostatniego rodzica, nie liscia 
		{
			kopcowanie(i);
		}
		fclose(plik);
		return 1;
	}
	bool znajdz(int wart)
	{ 
		if(lowy(wart,1)) return true;
		return false;
	}
	void dodaj(int wart)
	{
		if(rozmiar<++wezly) //gdy dodajemy wartosc wieksza niz tablica realokujemy rozmiar
		{
			rozmiar=rozmiar+KOPIEC;
			if(wezly==1) korzen=(int*)malloc(rozmiar*sizeof(int));
			else	korzen=(int*)realloc(korzen,rozmiar*sizeof(int));
		}
		korzen[wezly-1]=wart;
		int i=wezly/2;
		while(true) //wywolujemy przywracanie wartosci dla kazdego przodka do kolei
		{
			if(i<1) break;
			kopcowanie(i);
			if(wart!=korzen[i-1]) return;
			i=i/2;
		}
	}
	void usun(int wart)
	{
		if(wezly==0)
		{
			Err(PUSTA);
			return;
		}
		int index=lowy(wart,1);
		if(index<0)	return;
		korzen[index-1]=korzen[wezly-1];
		wezly--;
		kopcowanie(index);
		while(korzen[index/2-1]<korzen[index-1])
		{
			index=index/2;
			kopcowanie(index);
		}
		if(wezly<rozmiar-KOPIEC)
		{
			rozmiar=rozmiar-KOPIEC;
		 	korzen=(int*)realloc(korzen,rozmiar*sizeof(int));
		}
		return;
	}
	void wyswietl()
	{
		int i,j,z=0,sek=1,kres=1;
		if(wezly<=0) return;
		printf("Tablicowa postac kopca:\n");
		for(i=0;i<wezly;i++) printf("%d ",korzen[i]);
		printf("\n");
		if(wezly>32768)
		{
			char x;
			printf("Wyswietlane drzewo sie rozjedzie. Czy chcesz porzucic drukowanie? (T): ");
			scanf("%c",&x);
			if(x=='T' || x=='t') return;
			
		}
		printf("Kopiec w postaci drzewa:\n");
		show("",1,false);
	}
	void generuj(int ilosc)
	{
		if(ilosc<=0)
		{
			Err(NEGATYW);
			return;
		}
		wezly=ilosc;
		rozmiar=ilosc;
		srand(time(NULL));
		if(korzen!=NULL) free(korzen); //usuwamy stary kopiec na rzecz nowego
		korzen=(int*)malloc(wezly*sizeof(int));
		for(int i=0;i<ilosc;i++) korzen[i]=rand();
		for(int i=wezly;i>0;i--) kopcowanie(i);
	}
};

class DrzewoCz
{
	WezelCz* korzen;
	void decymacja(WezelCz* przejscie)
	{
		if(przejscie==NULL) return;
		decymacja(przejscie->lewy);
		decymacja(przejscie->prawy);
		delete przejscie;
	}
	WezelCz* wybraniec(WezelCz* obecny,int wart) //zwraca poszukiwany wezel
	{
		if(obecny==NULL) return (WezelCz*)NULL;
		if(obecny->klucz==wart) return obecny;
		if(obecny->klucz>wart) return wybraniec(obecny->lewy,wart);
		else return wybraniec(obecny->prawy,wart);
	}
	void RotacjaL(WezelCz* izo) //jedna z funkcji sluzacych do rownowazenia drzewa, 'obraca' wezly w miejscu w lewo
	{
		if(izo->prawy==NULL) return; //By wykonac ta operacje potrzebujemy, by prawy przodek istnial
		WezelCz* temp=izo->prawy;
		izo->prawy=temp->lewy; //przyjmujemy lewa czesc prawego przodka w jego miejsce, zachowujemy binarnosc
		if(temp->lewy)	temp->lewy->przodek = izo;
		temp->lewy=izo;
		temp->przodek=izo->przodek; //zabieramy przodka obracanego wezla
		izo->przodek=temp;
		if(izo->prawy!=NULL) izo->prawy->przodek=izo;
		if(temp->przodek==NULL) korzen=temp;
		else if(izo==temp->przodek->lewy) temp->przodek->lewy=temp;
		else temp->przodek->prawy=temp;
	}
	void RotacjaP(WezelCz* izo) //druga z funkcji do rownowazenia drzewa, analogiczne dzialanie, ale 'obrot' w prawo
	{
		if(izo->lewy==NULL) return;
		WezelCz* temp=izo->lewy;
		izo->lewy=temp->prawy;
		if(temp->prawy)	temp->prawy->przodek = izo;
		temp->prawy=izo;
		temp->przodek=izo->przodek;
		izo->przodek=temp;
		if(izo->lewy!=NULL) izo->lewy->przodek=izo;
		if(temp->przodek==NULL) korzen=temp;
		else if(izo==temp->przodek->lewy) temp->przodek->lewy=temp;
		else temp->przodek->prawy=temp;
	}
	void zniwecz(WezelCz* we) //zapewnilismy wczesniej, ze moze miec najwyzej jednego potomka
	{
		WezelCz* temp;
		WezelCz* wart;
		bool wartownik=false;
		if(we->lewy) temp=we->lewy;
		else temp=we->prawy;
		if(!temp) //jedyne uzycie wartownika, mimo wszelkich staran, by do tego nie dopuscic
		{
			wart=new WezelCz;
			wartownik=true;
			we->lewy=wart;
			wart->kolor=true;
			temp=wart;
		}
		temp->przodek=we->przodek;
		if(temp->przodek==NULL)
		{
			korzen=temp;
		}
		else
		{
			if(we==we->przodek->prawy) we->przodek->prawy=temp;
			else we->przodek->lewy=temp;
		}
		if(we->kolor) //usuniecie czerwonego wezla nie zaburza wlasnosci drzewa
		{
			WezelCz* bump;
			while(temp!=korzen && temp->kolor) //przesuwamy podwojny czarny w gore drzewa i obracamy
			{
				if(temp==temp->przodek->lewy)
				{
					bump=temp->przodek->prawy;
					if(bump->kolor==false)
					{
						bump->kolor=true;
						temp->przodek->kolor=false;
						RotacjaL(temp->przodek);
						bump=temp->przodek->prawy;
					}
					if(!bump->lewy || !bump->prawy)
					{
						if(bump->lewy || bump->prawy)
						{
							if(bump->lewy)
							{
								if(!bump->lewy->kolor)
								{
									bump->lewy->kolor=true;
									bump->kolor=false;
									RotacjaP(bump);
									bump=temp->przodek->prawy;
									bump->kolor=temp->przodek->kolor;
									temp->przodek->kolor=true;
									bump->prawy->kolor=true;
									RotacjaL(temp->przodek);
									temp=korzen;
									continue;
								}
							}
							else
							{
								if(!bump->prawy->kolor)
								{
									bump->kolor=temp->przodek->kolor;
									temp->przodek->kolor=true;
									bump->prawy->kolor=true;
									RotacjaL(temp->przodek);
									temp=korzen;
									continue;
								}
							}
						}
						bump->kolor=false;
						temp=temp->przodek;
						continue;
					}
					if(bump->lewy->kolor && bump->prawy->kolor)
					{
						bump->kolor=false;
						temp=temp->przodek;
						continue;
					}
					if(bump->prawy->kolor)
					{
						bump->lewy->kolor=true;
						bump->kolor=false;
						RotacjaP(bump);
						bump=temp->przodek->prawy;
					}
					bump->kolor=temp->przodek->kolor;
					temp->przodek->kolor=true;
					bump->prawy->kolor=true;
					RotacjaL(temp->przodek);
					temp=korzen;
				}
				else //to co na gorze, ale idac z drugiej strony
				{
					bump=temp->przodek->lewy;
					if(bump->kolor==false)
					{
						bump->kolor=true;
						temp->przodek->kolor=false;
						RotacjaP(temp->przodek);
						bump=temp->przodek->lewy;
					}
					if(!bump->lewy || !bump->prawy)
					{
						if(bump->lewy || bump->prawy)
						{
							if(bump->lewy)
							{
								if(!bump->lewy->kolor)
								{
									bump->kolor=temp->przodek->kolor;
									temp->przodek->kolor=true;
									bump->lewy->kolor=true;
									RotacjaP(temp->przodek);
									temp=korzen;
									continue;
								}
							}
							else
							{
								if(!bump->prawy->kolor)
								{
									bump->prawy->kolor=true;
									bump->kolor=false;
									RotacjaL(bump);
									bump=temp->przodek->lewy;
									bump->kolor=temp->przodek->kolor;
									temp->przodek->kolor=true;
									bump->lewy->kolor=true;
									RotacjaP(temp->przodek);
									temp=korzen;
									continue;
								}
							}
						}
						bump->kolor=false;
						temp=temp->przodek;
						continue;
					}
					if(bump->lewy->kolor && bump->prawy->kolor)
					{
						bump->kolor=false;
						temp=temp->przodek;
						continue;
					}
					if(bump->lewy->kolor)
					{
						bump->prawy->kolor=true;
						bump->kolor=false;
						RotacjaL(bump);
						bump=temp->przodek->lewy;
					}
					bump->kolor=temp->przodek->kolor;
					temp->przodek->kolor=true;
					bump->lewy->kolor=true;
					RotacjaP(temp->przodek);
					temp=korzen;
				}
			}
			temp->kolor=true;
		}
		delete we;
		if(wartownik) //usuwamy znienawidzonego wartownika
		{
			if(wart==korzen) korzen=(WezelCz*)NULL;
			else if(wart==wart->przodek->prawy) wart->przodek->prawy=wart->prawy;
			else wart->przodek->lewy=wart->lewy;
			delete wart;
		}
	}
	void wsadz(WezelCz* nowy) //dodawanie do drzewa binarnego, dla czytelnosci oddzielnie
	{
		WezelCz* temp=korzen;
		WezelCz* bump=(WezelCz*)NULL;
		while(temp!=NULL)
		{
			bump=temp;
			if(nowy->klucz<temp->klucz)	temp=temp->lewy;
			else temp=temp->prawy;
		}
		nowy->przodek=bump;
		if(bump==NULL) korzen=nowy;
		else if(nowy->klucz<bump->klucz) bump->lewy=nowy;
		else bump->prawy=nowy;
	}
	void napraw(WezelCz* nowy) //przywraca wlasnosc po wstawieniu wezla
	{
		WezelCz* temp;
		WezelCz* dziad;
		if(nowy->przodek==NULL) nowy->kolor=true; //przypadek - korzen
		else if(!nowy->przodek->kolor) //jesli przodek jest czarny nie ma zaburzenia dodajac czerwony korzen
		{
			dziad=nowy->przodek->przodek; //jesli przodek nie jest czarny, nie moze byc korzeniem
			if(nowy->przodek==dziad->lewy) temp=dziad->prawy;
			else temp=dziad->lewy;
			if(temp) //jesli wuj istnieje(i jest czerwony)
			{
				if(!temp->kolor)
				{
					nowy->przodek->kolor=true;
					temp->kolor=true;
					dziad->kolor=false;
					napraw(dziad);
					return;
				}
			}
			//gdy nie istnieje czerwony wuj
			{
				temp=nowy->przodek;
				if(nowy==temp->prawy && temp==dziad->lewy) //jesli jest wewnetrznym poddrzewem po przeciwnej stronie niz rodzic... 
				{
					RotacjaL(temp);
					nowy=nowy->lewy;
					temp=nowy->przodek;
					dziad=temp->przodek;
				}
				else if(nowy==temp->lewy && temp==dziad->prawy)//...to wyrownujemy
				{
					RotacjaP(temp);
					nowy=nowy->prawy;
					temp=nowy->przodek;
					dziad=temp->przodek;
				}
				temp->kolor=true;
				dziad->kolor=false;
				if(nowy==temp->lewy) RotacjaP(dziad);
				else RotacjaL(dziad); //wystarczy przeprowadzic taka rotacje do dwoch razy, by przywrocic rownowage
			}
		}
	}
	void show(char kierunek,WezelCz* obecny,std::string prefix) //funkcja drukujaca drzewo 
	{
		int i;
		char kolor,l=179;
		std::string roz;
		if(obecny==NULL)
		{
			printf("%s%c NIL B \n",prefix.c_str(),kierunek);
			return;
		}
		if(obecny->kolor) kolor='B';
		else kolor='R';
		roz="\t";
		if(obecny!=korzen)
		{
			if(obecny==obecny->przodek->prawy) roz=l+roz;
		}
		show(218,obecny->lewy,prefix+roz);
		printf("%s%c%6d%c%c\n",prefix.c_str(),kierunek,obecny->klucz,kolor,180);
		if(obecny!=korzen)
		{
			if(obecny==obecny->przodek->lewy) roz=l+roz;
			else roz="\t";
		}
		show(192,obecny->prawy,prefix+roz);		
	}
	
	public:
	void zniszcz()
	{
		decymacja(korzen);
		korzen=(WezelCz*)NULL;
	}
	int zaladuj(std::string nazwaPliku) 
	{
		int dana,data;
		FILE* plik;
		plik=fopen(nazwaPliku.c_str(),"rt");
		if(plik==NULL)	return(-1); //brak uchwytu
		fscanf(plik,"%d",&data);
		if(data<0) return 0; //rozmiar nie moze byc ujemny, jedyna informacja co do rozmiaru jaka nas interesuje
		zniszcz();
		for(int i=0;i<data;i++)
		{
			fscanf(plik," %d",&dana);
			dodaj(dana);
		}
		fclose(plik);
		return 1;
	}
	bool znajdz(int wart)
	{
		if(wybraniec(korzen,wart)!=NULL) return true;
		return false;
	}
	void dodaj(int wart)
	{
		WezelCz* nowy=new WezelCz();
		nowy->klucz=wart;
		wsadz(nowy); //dodajemy do drzewa binarnego
		napraw(nowy); //przywracamy wlasnosc drzewa
		while(nowy->przodek) nowy=nowy->przodek; //zapewniamy, ze korzen dalej wskazuje na korzen
		korzen=nowy;
		korzen->kolor=true;
	}
	void usun(int wart)
	{
		WezelCz* temp;
		if(!korzen)	
		{
			Err(PUSTA);
			return;
		}
		WezelCz* usuwany=wybraniec(korzen,wart); //znajdujemy wezel, ktory chcemy usunac
		if(usuwany==NULL) return;
		if(usuwany->lewy && usuwany->prawy) //usuwamy tylko wezly z jednym przodkiem
		{
			temp=usuwany->prawy;
			while(temp->lewy) temp=temp->lewy;
			usuwany->klucz=temp->klucz;
			usuwany=temp;
		}
		zniwecz(usuwany);
	}
	void wyswietl()
	{
		if(!korzen)
		{
			Err(PUSTA);
			return;
		} //tu nie oferujemy nie wyswietlenia, to jedyny sposob
		show(0,korzen,"");
	}
	void generuj(int wart)
	{
		if(korzen) zniszcz();
		srand(time(NULL));
		for(int i=0;i<wart;i++) dodaj(rand());
	}
};

void menu(std::string Message) //tekst wyswietlony w menu
{
	printf("%s",Message.c_str());
	printf("1.Zaladuj dane z pliku\n");
	printf("2.Znajdz element\n");
	printf("3.Dodaj element\n");
	printf("4.Usun element\n");
	printf("5.Wyswietl zawartosc\n");
	printf("6.Generuj nowa strukture\n");
	printf("7.Test (pomiary)\n"); //Jako ze ponoc to dla nas opcja, to pozwalam sobie na nieszczegolowy opis
	printf("0.Powrot do wyboru struktry\n");
	printf("Podaj numer opcji: ");	
}

void menuTab(Tablica* operowana) //menu operacji obslugiwanych przez tablice. Nazwy mowia same za siebie
{
	unsigned short int wyb;
	int wart1,wart2;
	std :: string nazwa;
	Tablica* roz=operowana;
	menu("\n\n------Menu tablicy------\n");
	scanf(" %hd",&wyb);
	switch(wyb)
	{
		case 1:
			printf("Podaj nazwe pliku: ");
			scanf("%s",nazwa.c_str());
			if(roz==NULL) roz=new Tablica(); //zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			switch(roz->zaladuj(nazwa))
			{
				case -1:
					printf("\nNie udalo sie otworzyc pliku!\n");
					break;
				case 0:
					printf("\nWartosc wewnatrz pliku nie jest wartoscia dodatnia!\n");
					break;
				case 1:
					printf("\nPrawidlowo wczytano dane z pliku.\n");
					break;
			}
			roz->wyswietl();
			system("Pause");
			break;
		case 2:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			printf("Podaj wartosc do znalezienia w strukturze: ");
			scanf("%d",&wart1);
			if(roz->znajdz(wart1)) printf("\nWartosc znajduje sie w strukturze\n");
			else printf("\nWartosci nie ma w strukturze!\n");
			system("Pause");
			break;
		case 3:
			if(roz==NULL) roz=new Tablica(); //zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			printf("Podaj na ktorej pozycji dodac (od 0): ");
			scanf("%d",&wart2);
			printf("Podaj wartosc na pozycji: ");
			scanf("%d",&wart1);
			roz->dodaj(wart1,wart2);
			roz->wyswietl();
			system("Pause");
			break;
		case 4:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			printf("Podaj ktora pozycje chcesz usunac: ");
			scanf("%d",&wart2);
			roz->usun(wart2);
			roz->wyswietl();
			system("Pause");
			break;
		case 5:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			roz->wyswietl();
			system("Pause");
			break;
		case 6:
			printf("Podaj rozmiar tablicy: ");
			scanf("%d",&wart1);
			if(roz==NULL) roz=new Tablica(); //zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			roz->generuj(wart1);
			roz->wyswietl();
			system("Pause");
			break;
		case 7:
			printf("1.Znajdz\n2.Dodaj\n3.Usun\nDajesz: "); //funkcja pomiarowa
			scanf("%d",&wyb);
			if(roz==NULL) roz=new Tablica();
			switch(wyb)
			{			
				double Czas;
				int i,j,populus;
				case 1:
					{
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								StartCounter();
								roz->znajdz(POSZUKIWANY);
								Czas+=GetCounter();
	        				}
	        				Czas=Czas/100;
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas);
	        				populus=2*populus;
						}
						break;
					}
				case 2:
					{
						populus=2;
						int klucz,wart;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								wart=rand();
								StartCounter();
								roz->dodaj(wart,0);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na poczatku na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								wart=rand();
								roz->generuj(populus);
								StartCounter();
								roz->dodaj(wart,populus-1);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na koncu na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								klucz=rand()%populus;
								wart=rand();
								StartCounter();
								roz->dodaj(wart,klucz);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						break;
					}
				case 3:
					{
						populus=2;
						int klucz;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								StartCounter();
								roz->usun(0);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na poczatku na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								StartCounter();
								roz->usun(populus-1);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na koncu na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								klucz=rand()%populus;
								StartCounter();
								roz->usun(klucz);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						break;
					}
			}
			system("Pause");
			printf("\nOpuszczanie trybu pomiaru czasu...\n\n");
			break;
		case 0:
			if(roz!=NULL) //porzucanie dorychczasowej struktury
			{
				roz->zniszcz();
				delete roz;
			}
			return;
	}
	system("cls");
	menuTab(roz);
}

void menuList(Lista* operowana)
{
	unsigned short int wyb;
	int wart1,wart2;
	std :: string nazwa;
	Lista* roz=operowana;
	menu("\n\n------Menu listy------\n");
	scanf(" %hd",&wyb);
	switch(wyb)
	{
		case 1:
			if(roz==NULL) roz=new Lista();//zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			printf("Podaj nazwe pliku: ");
			scanf("%s",nazwa.c_str());
			switch(roz->zaladuj(nazwa))
			{
				case -1:
					printf("\nNie udalo sie otworzyc pliku!\n");
					break;
				case 0:
					printf("\nWartosc wewnatrz pliku nie jest wartoscia dodatnia!\n");
					break;
				case 1:
					printf("\nPrawidlowo wczytano dane z pliku.\n");
					break;
			}
			roz->wyswietl();
			system("Pause");
			break;
		case 2:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			printf("Podaj wartosc do znalezienia w strukturze: ");
			scanf("%d",&wart1);
			if(roz->znajdz(wart1)) printf("\nWartosc znajduje sie w strukturze\n");
			else printf("\nWartosci nie ma w strukturze!\n");
			system("Pause");
			break;
		case 3:
			if(roz==NULL) roz=new Lista(); //zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			printf("Podaj na ktorej pozycji dodac (od 0): ");
			scanf("%d",&wart2);
			printf("Podaj wartosc na pozycji: ");
			scanf("%d",&wart1);
			roz->dodaj(wart1,wart2);
			roz->wyswietl();
			system("Pause");
			break;
		case 4:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			printf("Podaj ktora wartosc chcesz usunac: ");
			scanf("%d",&wart1);
			roz->usun(wart1);
			roz->wyswietl();
			system("Pause");
			break;
		case 5:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			roz->wyswietl();
			system("Pause");
			break;
		case 6:
			printf("Podaj rozmiar listy: ");
			scanf("%d",&wart1);
			if(roz==NULL) roz=new Lista();
			roz->generuj(wart1);
			roz->wyswietl();
			system("Pause");
			break;
		case 7:
			printf("1.Znajdz\n2.Dodaj\n3.Usun\nDajesz: "); //funkcja pomiarowa
			scanf("%d",&wyb);
			if(roz==NULL) roz=new Lista();			
			switch(wyb)
			{
				double Czas;
				int i,j,populus;
				case 1:
					{
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								StartCounter();
								roz->znajdz(POSZUKIWANY);
								Czas+=GetCounter();
	        				}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
	        				populus=2*populus;
						}
						break;
					}
				case 2:
					{
						populus=2;
						int klucz,wart;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								wart=rand();
								StartCounter();
								roz->dodaj(wart,0);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na poczatku na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								wart=rand();
								roz->generuj(populus);
								StartCounter();
								roz->dodaj(wart,populus-1);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na koncu na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								klucz=rand()%populus;
								wart=rand();
								StartCounter();
								roz->dodaj(wart,klucz);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						break;
					}
				case 3:
					{
						populus=2;
						int wart;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								wart=rand();
								StartCounter();
								roz->usun(wart);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						break;
					}
			}
			printf("\nOpuszczanie trybu pomiaru czasu...\n\n");
			system("Pause");
			break;	
		case 0:
			if(roz!=NULL) //porzucanie struktury
			{
				roz->zniszcz();
				delete roz;
			}
			return;
	}
	system("cls");
	menuList(roz);
}

void menuKop(Kopiec* operowana) //menu operacji obslugiwanych przez kopiec. Nazwy mowia same za siebie
{
	unsigned short int wyb;
	int wart;
	std :: string nazwa;
	Kopiec* roz=operowana;
	menu("\n\n------Menu kopca------\n");
	scanf(" %hd",&wyb);
	switch(wyb)
	{
		case 1:
			printf("Podaj nazwe pliku: ");
			scanf("%s",nazwa.c_str());
			if(roz==NULL) roz=new Kopiec(); //zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			switch(roz->zaladuj(nazwa))
			{
				case -1:
					printf("\nNie udalo sie otworzyc pliku!\n");
					break;
				case 0:
					printf("\nWartosc wewnatrz pliku nie jest wartoscia dodatnia!\n");
					break;
				case 1:
					printf("\nPrawidlowo wczytano dane z pliku.\n");
					break;
			}
			roz->wyswietl();
			system("Pause");
			break;
		case 2:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			printf("Podaj wartosc do znalezienia w strukturze: ");
			scanf("%d",&wart);
			if(roz->znajdz(wart)) printf("\nWartosc znajduje sie w strukturze\n");
			else printf("\nWartosci nie ma w strukturze!\n");
			system("Pause");
			break;
		case 3:
			if(roz==NULL) roz=new Kopiec(); //zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			printf("Podaj wartosc do dodania: ");
			scanf("%d",&wart);
			roz->dodaj(wart);
			roz->wyswietl();
			system("Pause");
			break;
		case 4:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			printf("Podaj ktora wartosc chcesz usunac: ");
			scanf("%d",&wart);
			roz->usun(wart);
			roz->wyswietl();
			system("Pause");
			break;
		case 5:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			roz->wyswietl();
			system("Pause");
			break;
		case 6:
			printf("Podaj rozmiar kopca: ");
			scanf("%d",&wart);
			if(roz==NULL) roz=new Kopiec(); //zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			roz->generuj(wart);
			roz->wyswietl();
			system("Pause");
			break;
		case 7:
			printf("1.Znajdz\n2.Dodaj\n3.Usun\nDajesz: "); //funkcja pomiarowa
			scanf("%d",&wyb);
			if(roz==NULL) roz=new Kopiec();			
			switch(wyb)
			{
				double Czas;
				int i,j,populus;
				case 1:
					{
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;	
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								StartCounter();
								roz->znajdz(POSZUKIWANY);
								Czas+=GetCounter();
	        				}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
	        				populus=2*populus;
						}
						break;
					}
				case 2:
					{
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								wart=rand();
								StartCounter();
								roz->dodaj(wart);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						break;
					}
				case 3:
					{
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								wart=rand();
								StartCounter();
								roz->usun(wart);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						break;
					}
			}
			printf("\nOpuszczanie trybu pomiaru czasu...\n\n");
			system("Pause");
			break;
		case 0:
			if(roz!=NULL) //porzucanie dorychczasowej struktury
			{
				roz->zniszcz();
				delete roz;
			}
			return;
	}
	system("cls");
	menuKop(roz);
}

void menuCz(DrzewoCz* operowana)
{
	unsigned short int wyb;
	int wart;
	std :: string nazwa;
	DrzewoCz* roz=operowana;
	menu("\n\n----Menu drzewa czerwono-czarnego----\n");
	scanf(" %hd",&wyb);
	switch(wyb)
	{
		case 1:
			printf("Podaj nazwe pliku: ");
			scanf("%s",nazwa.c_str());
			if(roz==NULL) roz=new DrzewoCz(); //zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			switch(roz->zaladuj(nazwa))
			{
				case -1:
					printf("\nNie udalo sie otworzyc pliku!\n");
					break;
				case 0:
					printf("\nWartosc wewnatrz pliku nie jest wartoscia dodatnia!\n");
					break;
				case 1:
					printf("\nPrawidlowo wczytano dane z pliku.\n");
					break;
			}
			roz->wyswietl();
			system("Pause");
			break;
		case 2:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			printf("Podaj wartosc do znalezienia w strukturze: ");
			scanf("%d",&wart);
			if(roz->znajdz(wart)) printf("\nWartosc znajduje sie w strukturze\n");
			else printf("\nWartosci nie ma w strukturze!\n");
			system("Pause");
			break;
		case 3:
			if(roz==NULL) roz=new DrzewoCz(); //zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			printf("Podaj wartosc do dodania: ");
			scanf("%d",&wart);
			roz->dodaj(wart);
			roz->wyswietl();
			system("Pause");
			break;
		case 4:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			printf("Podaj ktora wartosc chcesz usunac: ");
			scanf("%d",&wart);
			roz->usun(wart);
			roz->wyswietl();
			system("Pause");
			break;
		case 5:
			if(roz==NULL) 
			{
				Err(BRAK);
				break;
			}
			roz->wyswietl();
			system("Pause");
			break;
		case 6:
			printf("Podaj rozmiar drzewa: ");
			scanf("%d",&wart);
			if(roz==NULL) roz=new DrzewoCz(); //zapobieganie siegania do funkcji nieistniejacej we wskazniku 0
			roz->generuj(wart);
			roz->wyswietl();
			system("Pause");
			break;
		case 7:
			printf("1.Znajdz\n2.Dodaj\n3.Usun\nDajesz: "); //funkcja pomiarowa
			scanf("%d",&wyb);
			if(roz==NULL) roz=new DrzewoCz();			
			switch(wyb)
			{
				double Czas;
				int i,j,populus;
				case 1:
					{
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;	
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								StartCounter();
								roz->znajdz(POSZUKIWANY);
								Czas+=GetCounter();
	        				}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
	        				populus=2*populus;
						}
						break;
					}
				case 2:
					{
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								wart=rand();
								StartCounter();
								roz->dodaj(wart);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						break;
					}
				case 3:
					{
						populus=2;
						for(int i=0;i<15;i++)
						{
							Czas=0;
							for(j=0;j<100;j++)
							{
								roz->generuj(populus);
								wart=rand();
								StartCounter();
								roz->usun(wart);
	        					Czas=GetCounter();
							}
							printf("\nOperacja na %d elementach zajmuje srednio %.6f ms\n",populus, Czas/100);
							populus=populus*2;
						}
						break;
					}
			}
			printf("\nOpuszczanie trybu pomiaru czasu...\n\n");
			system("Pause");
			break;
		case 0:
			if(roz!=NULL) //porzucanie dorychczasowej struktury
			{
				roz->zniszcz();
				delete roz;
			}
			return;
	}
	
	system("cls");
	menuCz(roz);
}

void menuWyboru() //menu pozwalajace na wybor struktury na ktorej chcemy operowac
{
	unsigned short int wyb;
	printf("\n\n---Menu wyboru struktury---\n");
	printf("1.Tablica\n");
	printf("2.Lista dwukierunkowa\n");
	printf("3.Kopiec binarny\n");
	printf("4.Drzewo czerwono-czarne\n");
	printf("0.Zakoncz dzialanie programu\n");
	printf("Podaj numer opcji: ");
	scanf(" %hd",&wyb);
	switch(wyb)
	{
		case 1:
			menuTab((Tablica*)NULL);
			break;
		case 2:
			menuList((Lista*)NULL);
			break;
		case 3:
			menuKop((Kopiec*)NULL);
			break;
		case 4:
			menuCz((DrzewoCz*)NULL);
			break;	
		case 0:
			return;
		default:
			printf("Dana opcja nie jest obslugiwana!\n\n");
	}
	menuWyboru();
}

int main()
{
	menuWyboru();
	printf("\nKonczenie pracy programu...\n");
	system("Pause");
	return 0;
}
