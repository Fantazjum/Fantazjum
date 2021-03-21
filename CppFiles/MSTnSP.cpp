
#define REP1 50
#define REP2 100
#define REP3 200
#define REP4 300
#define REP5 400
#define REP6 500
#define REP7 600

#define GEST1 20
#define GEST2 60
#define GEST3 99

#include <cstdio>
#include <string>
#include <cstdlib>
#include <fstream>
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


struct Krawedz
{
	int wP; //wierzcholek poczatkowy
	int wK; //wierzcholek koncowy
	int wartosc; //waga albo przepustowosc
};

struct Wierz
{
	int sasiedzi;
	int numer;
	int wartosc; 
	Wierz* trasa;
	Krawedz** sasiad;
};

class GrafMST //reprezentacja macierzowa drzewa rozpinajacego 
{
	int wierzcholki, krawedzie, ciezar;
	int** macierz;
	
	void kopcowanie(Krawedz** k,int i, int lim)
	{
		
		if(2*i>lim) return;
		int min;
		if(k[i-1]->wartosc>k[2*i-1]->wartosc) min=2*i;
		else min=i;
		if(2*i+1<=lim) 
		{
			if(k[min-1]->wartosc>k[2*i]->wartosc) min=2*i+1;
		}
		if(min!=i)
		{
			Krawedz* x=k[i-1];
			k[i-1]=k[min-1];
			k[min-1]=x;
			kopcowanie(k,min,lim);
		}
	}
	
	int find(int* tab, int idx)
	{
		if(tab[idx]==idx) return idx;
		tab[idx]=find(tab,tab[idx]);
		return tab[idx];
	}
	
public:
	void Prim()
	{
		ciezar=0;
		int rozmiar,i,j,check;
		rozmiar=0;
		check=1; //na samym poczatku rozpatrujemy jeden wierzcholek
		bool* zbadane=(bool*)malloc(wierzcholki*sizeof(bool)); //lista wierzcholkow rozpatrzonych
		Krawedz** zbior=(Krawedz**)malloc(2*krawedzie*sizeof(Krawedz*));
		Krawedz** zaakceptowane=(Krawedz**)malloc((wierzcholki-1)*sizeof(Krawedz*));
		for(i=0;i<wierzcholki;i++) zbadane[i]=false;
		zbadane[0]=true; //to gdzie zaczynamy nie ma znaczenia, a ten wierzcholek musi istniec
		for(i=0;i<wierzcholki;i++) 
		{
			if(macierz[0][i])
			{
				zbior[rozmiar]=new Krawedz(); //zbieramy krawedzie do rozpatrywania
				zbior[rozmiar]->wartosc=macierz[0][i];
				zbior[rozmiar]->wK=i;
				zbior[rozmiar]->wP=0;
				rozmiar++;
			}
		}
		for(i=rozmiar/2;i>0;i--) kopcowanie(zbior,i,rozmiar); //w tym momencie zbior jest kopcem
		while(check<wierzcholki)
		{
			for(i=0;i<rozmiar;i++)
			{
				rozmiar--;
				j=zbior[0]->wK;
				if(zbadane[j]) 
				{
					zbior[0]=zbior[rozmiar];
					kopcowanie(zbior,1,rozmiar);
					continue;
				}
				zbadane[j]=true;
				zaakceptowane[check-1]=zbior[0];
				ciezar+=zbior[0]->wartosc;
				zbior[0]=zbior[rozmiar]; 
				kopcowanie(zbior,1,rozmiar);
				check++;
				if(check==wierzcholki) break; //nie ma sensu dalej wciagac krawedzie, mamy juz drzewo spinajace
				for(i=0;i<wierzcholki;i++) 
				{
					if(macierz[j][i]>0)
					{
						zbior[rozmiar]=new Krawedz();
						zbior[rozmiar]->wartosc=macierz[j][i];
						zbior[rozmiar]->wK=i;
						zbior[rozmiar]->wP=j;
						rozmiar++;
					}
				}
				for(i=rozmiar/2;i>0;i--) kopcowanie(zbior,i,rozmiar); //sprawiamy, ze zbior jest kopcem dla kolejnych przeszukiwan
				break;
			}
		}
		for(i=0;i<wierzcholki;i++)
		{
			free(macierz[i]);
			macierz[i]=(int*)calloc(wierzcholki,sizeof(int));
		}
		free(zbadane);
		for(i=0;i<wierzcholki-1;i++) 
		{
			macierz[zaakceptowane[i]->wP][zaakceptowane[i]->wK]=zaakceptowane[i]->wartosc;
			macierz[zaakceptowane[i]->wK][zaakceptowane[i]->wP]=zaakceptowane[i]->wartosc;
		}
		for(i=0;i<rozmiar;i++) delete zbior[i];
		free(zbior);
		free(zaakceptowane);
		krawedzie=wierzcholki-1;
	}
	
	void Kruskal()
	{
		ciezar=0;
		int i,j,k,l,size=0, rozmiar=0;
		int* reprezentant=(int*)malloc(wierzcholki*sizeof(int));
		for(i=0;i<wierzcholki;i++) reprezentant[i]=i;
		int* ranga=(int*)calloc(wierzcholki,sizeof(int));
		Krawedz** kopiec=(Krawedz**)malloc(krawedzie*sizeof(Krawedz*));
		Krawedz** przyjete=(Krawedz**)malloc((wierzcholki-1)*sizeof(Krawedz*));
		for(i=0;i<wierzcholki;i++)
		{
			for(j=i;j<wierzcholki;j++) //ze wzgledu na nieskierowanie grafu wystarczy nam polowa informacji
			{
				if(macierz[i][j])
				{
					kopiec[size]=new Krawedz();
					kopiec[size]->wartosc=macierz[i][j];
					kopiec[size]->wP=i;
					kopiec[size]->wK=j;
					size++;
				}
			}
		}
		for(i=0;i<wierzcholki;i++) //wiecej nie potrzebujemy, zerujemy macierz
		{
			free(macierz[i]);
			macierz[i]=(int*)calloc(wierzcholki,sizeof(int));
		}
		for(i=size/2;i>0;i--) kopcowanie(kopiec,i,size); //kopiec z krawedzi
		l=0;
		for(i=0;i<size;i++)
		{
			l++;
			j=find(reprezentant,kopiec[0]->wP);
			k=find(reprezentant,kopiec[0]->wK);
			if(j==k) 
			{
				delete kopiec[0];
				kopiec[0]=kopiec[size-l];
				kopcowanie(kopiec,1,size-l);
				continue;
			}
			przyjete[rozmiar]=kopiec[0];
			rozmiar++;
			kopiec[0]=kopiec[size-l];
			kopcowanie(kopiec,1,size-l);
			if(ranga[j]<ranga[k])  reprezentant[j]=k;
			else reprezentant[k]=j;
			if(ranga[j]==ranga[k]) ranga[j]++;
		}
		free(ranga);
		free(reprezentant);
		free(kopiec);
		for(i=0;i<rozmiar;i++)
		{
			j=przyjete[i]->wP;
			k=przyjete[i]->wK;
			macierz[j][k]=przyjete[i]->wartosc;
			macierz[k][j]=przyjete[i]->wartosc;
			ciezar+=przyjete[i]->wartosc;
		}
		krawedzie=wierzcholki-1;
	}
	
	int zaladuj(std::string nazwaPliku)
	{
		int dana,data,waga,i;
		FILE* plik;
		plik=fopen(nazwaPliku.c_str(),"rt");
		if(plik==NULL)	return(-1); //brak uchwytu
		fscanf(plik,"%d",&dana);
		fscanf(plik,"%d",&data);
		if(data<0 || dana<(data/2))  //nie mozna miec ujemnej liczby wierzchodlkow ani nie mozemy miec grafu niespojnego
		{
			fclose(plik);
			return 0;
		}
		zniszcz();
		wierzcholki=data;
		krawedzie=dana;
		macierz=(int**)malloc(wierzcholki*sizeof(int*));
		for(i=0;i<wierzcholki;i++) macierz[i]=(int*)calloc(wierzcholki,sizeof(int)); //domyslnie zera, zapelniamy
		for(i=0;i<krawedzie;i++)
		{
			fscanf(plik,"%d %d %d",&dana, &data, &waga);
			macierz[dana][data]=waga;
			macierz[data][dana]=waga;
		}
		fclose(plik);
		return 1;
	}
	
	void generuj(int w, int k)
	{
		int i,idx,inx,waga;
		bool last=false;
		wierzcholki=w;
		krawedzie=k;
		macierz=(int**)malloc(wierzcholki*sizeof(int*));
		for(i=0;i<w;i++) macierz[i]=(int*)calloc(w,sizeof(int));
		for(i=0;i<w-1;i++)
		{
			if(!last)
			{
				idx=rand()%(w-1-i); //wypelniamy macierz trojkatna, aby drzewo na pewno bylo rozpinajace
				idx+=i+1;			//wada: ostatnie wartosci maja najwieksza szanse na wylosowanie do trafienia w ostatnia
				if(idx==w-1) last=true;
			}
			else	//jesli polaczylismy ostatnia pozycje, to pozostale beda na pewno polaczone
			{
					idx=rand()%w; //losujemy bez ograniczen
					while(idx==i || macierz[idx][i]!=0) idx=rand()%w; //upewniamy sie, ze nie trafilismy w istniejaca wartosc lub petle
			}
			waga=rand();
			macierz[i][idx]=waga;
			macierz[idx][i]=waga;
			k--;
		}
		while(k>0)
		{
			inx=rand()%w;
			idx=rand()%w;
			while(macierz[inx][idx]!=0 || inx==idx) //upewniamy sie, ze taka krawedz jeszcze nie istnieje i nie jest petla
			{
				inx=rand()%w;
				idx=rand()%w;
			}
			waga=rand();
			macierz[inx][idx]=waga;
			macierz[idx][inx]=waga;
			k--;
		}
	}
	
	void wyswietl()
	{
		int i,j;
		printf("Reprezentacja macierzowa, wagi krawedzi\n");
		for(i=0;i<=wierzcholki;i++)
		{
			if(i==0) printf("\t");
			else printf("%7d ",i-1);
			for(j=0;j<wierzcholki;j++)
			{
				if(i==0) printf("%7d ",j);
				else
				{
					if(i==j+1) printf("    0   ");
					else if(macierz[i-1][j]!=0) printf("%7d ",macierz[i-1][j]);
					else printf("   Inf  ");
				}
			}
			printf("\n");
		}
		
			if(ciezar) printf("Waga drzewa rozpinajacego: %d\n",ciezar);
	}
	
	void zniszcz()
	{
		ciezar=0;
		if(!macierz) return;
		for(int i=0;i<wierzcholki;i++)
		{
			free(macierz[i]);
		}
		free(macierz);
	}
	
	friend class GrafLST;
};

class GrafMP //reprezentacja macierzowa grafu z najkrotsza sciezka
{
	int wierzcholki, krawedzie, start;
	bool alg;
	int** macierz;
	
	int span(int s, int k)
	{
		if(s==k-1) return s;
		int idx;
		idx=(rand()%(k-s-1))+s+1;
		while(macierz[s][idx]) idx=(rand()%(k-s-1))+s+1;
		macierz[s][idx]=rand();
		span(s,idx);
		span(idx,k);
		return idx;
	}
	
	void kopcowanie(Wierz** w,int i,int lim)
	{
		if(2*i>lim) return;
		int min;
		if(w[i-1]->wartosc>w[2*i-1]->wartosc) min=2*i;
		else min=i;
		if(2*i+1<=lim) 
		{
			if(w[min-1]->wartosc>w[2*i]->wartosc) min=2*i+1;
		}
		if(min!=i)
		{
			Wierz* x=w[i-1];
			w[i-1]=w[min-1];
			w[min-1]=x;
			kopcowanie(w,min,lim);
		}
	}
	
public:
	void Dijkstra()
	{
		alg=true;
		int i,j,k,l,size=wierzcholki;
		Krawedz* kra;
		Wierz** list=(Wierz**)malloc(wierzcholki*sizeof(Wierz*));
		Wierz** kolejka=(Wierz**)malloc(wierzcholki*sizeof(Wierz*));
		Wierz* temp;
		for(i=0;i<wierzcholki;i++)
		{
			list[i]=new Wierz(); //tworzymy liste wierzcholkow
			list[i]->numer=i;
			list[i]->sasiedzi=0;
			list[i]->wartosc=INT_MAX;
			list[i]->sasiad=(Krawedz**)NULL;
			for(j=0;j<wierzcholki;j++) 
			{
				if(macierz[i][j] && j!=start) //upewniamy sie, ze nie bedziemy mieli cyklu
				{
					k=list[i]->sasiedzi;
					list[i]->sasiad=(Krawedz**)realloc(list[i]->sasiad,(k+1)*sizeof(Krawedz*));
					list[i]->sasiad[k]=new Krawedz();
					list[i]->sasiad[k]->wartosc=macierz[i][j];
					list[i]->sasiad[k]->wP=i;
					list[i]->sasiad[k]->wK=j;
					list[i]->sasiedzi++;
				}
			}
			free(macierz[i]);
			macierz[i]=(int*)calloc(wierzcholki,sizeof(int)); //od razu czyscimy, niepotrzebne nam wiecej
		}
		list[start]->wartosc=0;
		for(i=0;i<wierzcholki;i++) kolejka[i]=list[i];
		kolejka[0]=kolejka[start]; //przywracanie wartosci kopca nie ma sensu, wszystkie maja dystans 0
		kolejka[start]=list[0];
		while(size)
		{
			size--;
			temp=kolejka[0];
			l=temp->sasiedzi;
			for(i=0;i<l;i++)
			{
				kra=temp->sasiad[i];
				k=list[kra->wK]->wartosc;
				if(kra->wartosc+temp->wartosc<k && temp->wartosc!=INT_MAX)
				{
					list[kra->wK]->wartosc=kra->wartosc+temp->wartosc;
					list[kra->wK]->trasa=temp;
				}
			}
			if(size==0) break;
			kolejka[0]=kolejka[size];
			for(i=size/2;i>0;i--) kopcowanie(kolejka,i,size); //tyle wywolan bo zmieniamy wiele wartosci
		}
		free(kolejka);
		for(i=0;i<wierzcholki;i++)
		{
			for(j=0;j<list[i]->sasiedzi;j++) delete list[i]->sasiad[j];
			if(list[i]->trasa)macierz[list[i]->trasa->numer][list[i]->numer]=list[i]->wartosc;
		}
		for(i=0;i<wierzcholki;i++) delete list[i];
		free(list);
	}
	
	void FordBell()
	{
		alg=true;
		int i,j,k,rozmiar=0;
		Krawedz** zbior=(Krawedz**)malloc(krawedzie*sizeof(Krawedz*));
		Wierz** list=(Wierz**)malloc(wierzcholki*sizeof(Wierz*));
		Wierz* temp;
		for(i=0;i<wierzcholki;i++)
		{
			list[i]=new Wierz(); //tworzymy liste wierzcholkow
			list[i]->numer=i;
			list[i]->wartosc=INT_MAX;
			list[i]->trasa=(Wierz*)NULL;
			for(j=0;j<wierzcholki;j++)
			{
				if(macierz[i][j])
				{
					zbior[rozmiar]=new Krawedz();
					zbior[rozmiar]->wartosc=macierz[i][j];
					zbior[rozmiar]->wP=i;
					zbior[rozmiar]->wK=j;
					rozmiar++;
				}
			}
			free(macierz[i]);
			macierz[i]=(int*)calloc(wierzcholki,sizeof(int)); //od razu czyscimy, niepotrzebne nam wiecej
		}
		zbior=(Krawedz**)realloc(zbior,rozmiar*sizeof(Krawedz*));
		list[start]->wartosc=0;
		for(i=0;i<wierzcholki-1;i++)
		{
			for(j=0;j<rozmiar;j++)
			{
				if(list[zbior[j]->wK]->wartosc>list[zbior[j]->wP]->wartosc+zbior[j]->wartosc &&list[zbior[j]->wP]->wartosc!=INT_MAX)
				{
					list[zbior[j]->wK]->trasa=list[zbior[j]->wP];
					list[zbior[j]->wK]->wartosc=list[zbior[j]->wP]->wartosc+zbior[j]->wartosc;
				}
			}
		}
		for(j=0;j<rozmiar;j++)
			{
				if(list[zbior[j]->wK]->wartosc>list[zbior[j]->wP]->wartosc+zbior[j]->wartosc &&list[zbior[j]->wP]->wartosc!=INT_MAX)
				{
					printf("Cykl ujemny!\n");
					printf("Stracono dane...\n");
					for(i=0;i<wierzcholki;i++) delete list[i];
					free(list);
					return;
				}
			}
		for(i=0;i<wierzcholki;i++)
		{
			temp=list[i];
			if(temp->trasa)
			{
				macierz[temp->trasa->numer][temp->numer]=temp->wartosc;
			}
		}
		for(i=0;i<wierzcholki;i++) delete list[i];
		free(list);
	}
	
	int zaladuj(std::string nazwaPliku)
	{
		int dana,data,waga,i;
		std::ifstream plik;
		plik.open(nazwaPliku.c_str());
		if(plik==NULL)	return(-1); //brak uchwytu
		plik >> dana;
		plik >> data;
		plik >> waga;
		if(data<0 || dana<(data-1) || waga>data ||waga<0) //te same warunki, przy czym wierzcholek startowy musi istniec w grafie
		{
			plik.close();
			return 0;
		}
		alg=false;
		zniszcz();
		wierzcholki=data;
		krawedzie=dana;
		start=waga;
		macierz=(int**)malloc(wierzcholki*sizeof(int*));
		for(i=0;i<wierzcholki;i++) macierz[i]=(int*)calloc(wierzcholki,sizeof(int));
		for(i=0;i<krawedzie;i++)
		{
			plik >> dana;
			plik >> data;
			plik >> waga;
			macierz[dana][data]=waga;
		}
		plik.close();
		return 1;
	}
	
	void generuj(int w, int k)
	{
		alg=false;
		int i,idx,inx,waga;
		wierzcholki=w;
		krawedzie=k;
		macierz=(int**)malloc(wierzcholki*sizeof(int*));
		for(i=0;i<w;i++) macierz[i]=(int*)calloc(w,sizeof(int));
		idx=rand()%(w-1);
		if(idx==0)macierz[0][span(0,w)]=rand();
		else
		{
			span(0,idx);
			span(idx+1,w);
			macierz[idx][0]=rand();
			macierz[idx][idx+1]=rand();
		}
		start=idx; //ma sens przeprowadzac algorytm tylko dla punktu, ktory moze dojsc do kazdego
		k=k-w+1;
		while(k>0)
		{
			inx=rand()%w;
			idx=rand()%w;
			while(macierz[inx][idx]!=0 || inx==idx) //upewniamy sie, ze taka krawedz jeszcze nie istnieje i nie jest petla
			{
				inx=rand()%w;
				idx=rand()%w;
			}
			waga=rand();
			macierz[inx][idx]=waga;
			k--;
		}
	}
	
	void wyswietl()
	{
		int i,j;
		printf("Reprezentacja macierzowa:\n");
		for(i=0;i<=wierzcholki;i++)
		{
			if(i==0) printf("\t");
			else printf("%7d ",i-1);
			for(j=0;j<wierzcholki;j++)
			{
				if(i==0) printf("%7d ",j);
				else
				{
					if(i==j+1)
					{
				 		if(!alg)printf("    0   ");
				 		else if(j==start) printf("    0   ");
				  		else printf("   Inf  ");
				  		continue;
					}
					if(macierz[i-1][j]!=0) printf("%7d ",macierz[i-1][j]);
					else printf("   Inf  ");
				}
			}
			printf("\n");
		}
	}
	
	void zniszcz()
	{
		if(!macierz) return;
		for(int i=0;i<wierzcholki;i++)
		{
			if(macierz[i])free(macierz[i]);
		}
		free(macierz);
	}
	
	void question()
	{
		printf("Z ktorego wierzcholka chcesz zaczynac?\n");
		printf("Sugerowany wierzcholek nr %d\n",start);
		printf("Podaj numer wierzcholka: ");
		scanf("%d",&start);
		while(start<0 || start>=wierzcholki)
		{
			printf("BLAD! Wybierz wierzcholek wg wskazanej numeracji: ");
			scanf("%d",&start);
		}
	}
	
	friend class GrafLP;
};


class GrafLST //reprezentacja listowa drzewa rozpinajacego
{
	int wierzcholki, krawedzie, ciezar;
	Krawedz*** lista;
	
	void kopcowanie(Krawedz** k, int i, int lim)
	{
		if(2*i>lim) return;
		int min=i;
		if(k[i-1]->wartosc>k[2*i-1]->wartosc) min=2*i;
		if(2*i+1<=lim) if(k[min-1]->wartosc>k[2*i]->wartosc) min=2*i+1;
		if(min!=i)
		{
			Krawedz* x=k[min-1];
			k[min-1]=k[i-1];
			k[i-1]=x;
			kopcowanie(k,min,lim);
		}
	}
	
	int find(int* tab, int idx)
	{
		if(tab[idx]==idx) return idx;
		tab[idx]=find(tab,tab[idx]);
		return tab[idx];
	}

public:
	void Prim()
	{
		ciezar=0;
		int rozmiar,i,j,k,check;
		Krawedz** kopiec=(Krawedz**)malloc(2*krawedzie*sizeof(Krawedz*));
		Krawedz** przyjete=(Krawedz**)malloc((wierzcholki-1)*sizeof(Krawedz*));
		bool* rozpatrzone=(bool*)malloc(wierzcholki*sizeof(bool));
		for(i=0;i<wierzcholki;i++) rozpatrzone[i]=false;
		rozmiar=0;
		check=1;
		rozpatrzone[0]=true;
		j=lista[0][0]->wartosc;
		for(i=1;i<=j;i++)
		{
			kopiec[rozmiar]=lista[0][i];
			rozmiar++;
		}
		for(i=rozmiar/2;i>0;i--) kopcowanie(kopiec,i,rozmiar);
		while(check<wierzcholki)
		{
			for(i=0;i<krawedzie;i++)
			{
				rozmiar--;
				j=kopiec[0]->wK;
				if(rozpatrzone[j]) 
				{
					kopiec[0]=kopiec[rozmiar];
					kopcowanie(kopiec,1,rozmiar);
					continue;
				}
				rozpatrzone[j]=true;
				ciezar+=kopiec[0]->wartosc;
				przyjete[check-1]=kopiec[0];
				kopiec[0]=kopiec[rozmiar];
				kopcowanie(kopiec,1,rozmiar);
				check++;
				if(check==wierzcholki) break;
				k=lista[j][0]->wartosc;
				for(i=1;i<=k;i++)
				{
					kopiec[rozmiar]=lista[j][i];
					rozmiar++;
				}
				for(i=rozmiar/2;i>0;i--) kopcowanie(kopiec, i, rozmiar);
				break;
			}
		}
		free(rozpatrzone);
		free(kopiec);
		Krawedz*** feniks=(Krawedz***)malloc(wierzcholki*sizeof(Krawedz**));
		for(i=0;i<wierzcholki;i++) feniks[i]=(Krawedz**)malloc(wierzcholki*sizeof(Krawedz*));
		for(i=0;i<wierzcholki;i++)
		{
			feniks[i][0]=new Krawedz();
			feniks[i][0]->wartosc=0;
		}
		int r,s;
		for(i=0;i<wierzcholki-1;i++)
		{
			j=przyjete[i]->wP;
			r=feniks[j][0]->wartosc;
			k=przyjete[i]->wK;
			s=feniks[k][0]->wartosc;
			feniks[j][r+1]=new Krawedz();
			feniks[j][r+1]->wartosc=przyjete[i]->wartosc;
			feniks[j][r+1]->wK=k;
			feniks[j][r+1]->wP=j;
			feniks[k][s+1]=new Krawedz();
			feniks[k][s+1]->wartosc=przyjete[i]->wartosc;
			feniks[k][s+1]->wK=j;
			feniks[k][s+1]->wP=k;
			feniks[j][0]->wartosc++;
			feniks[k][0]->wartosc++;
		}
		free(przyjete);
		for(i=0;i<wierzcholki;i++)
		{
			k=feniks[i][0]->wartosc;
			feniks[i]=(Krawedz**)realloc(feniks[i],(k+1)*sizeof(Krawedz*));
		}
		zniszcz();
		lista=feniks;
		krawedzie=wierzcholki-1;
	}
	
	void Kruskal()
	{
		ciezar=0;
		int i,j,k,l,size=0, rozmiar=0;
		int* reprezentant=(int*)malloc(wierzcholki*sizeof(int));
		for(i=0;i<wierzcholki;i++) reprezentant[i]=i;
		int* ranga=(int*)calloc(wierzcholki,sizeof(int));
		Krawedz** kopiec=(Krawedz**)malloc(2*krawedzie*sizeof(Krawedz*));
		Krawedz** przyjete=(Krawedz**)malloc((wierzcholki-1)*sizeof(Krawedz*));
		for(i=0;i<wierzcholki;i++)
		{
			k=lista[i][0]->wartosc;
			for(j=1;j<=k;j++)
			{
				kopiec[size]=lista[i][j];
				size++;
			}
		}
		for(i=size/2;i>0;i--) kopcowanie(kopiec,i,size); //kopiec z krawedzi
		l=0;
		for(i=0;i<size;i++)
		{
			l++;
			j=find(reprezentant,kopiec[0]->wP);
			k=find(reprezentant,kopiec[0]->wK);
			if(j==k) 
			{
				kopiec[0]=kopiec[size-l];
				kopcowanie(kopiec,1,size-1);
				continue;
			}
			przyjete[rozmiar]=kopiec[0];
			rozmiar++;
			ciezar+=kopiec[0]->wartosc;
			kopiec[0]=kopiec[size-l];
			kopcowanie(kopiec,1,size-l);
			if(ranga[j]<ranga[k])  reprezentant[j]=k;
			else reprezentant[k]=j;
			if(ranga[j]==ranga[k]) ranga[j]++;
		}
		free(reprezentant);
		free(ranga);
		free(kopiec);
		Krawedz*** feniks=(Krawedz***)malloc(wierzcholki*sizeof(Krawedz**));
		for(i=0;i<wierzcholki;i++) feniks[i]=(Krawedz**)malloc(wierzcholki*sizeof(Krawedz*));
		for(i=0;i<wierzcholki;i++)
		{
			feniks[i][0]=new Krawedz();
			feniks[i][0]->wartosc=0;
		}
		int r,s;
		for(i=0;i<wierzcholki-1;i++)
		{
			j=przyjete[i]->wP;
			r=feniks[j][0]->wartosc;
			k=przyjete[i]->wK;
			s=feniks[k][0]->wartosc;
			feniks[j][r+1]=new Krawedz();
			feniks[j][r+1]->wartosc=przyjete[i]->wartosc;
			feniks[j][r+1]->wK=k;
			feniks[j][r+1]->wP=j;
			feniks[k][s+1]=new Krawedz();
			feniks[k][s+1]->wartosc=przyjete[i]->wartosc;
			feniks[k][s+1]->wK=j;
			feniks[k][s+1]->wP=k;
			feniks[j][0]->wartosc++;
			feniks[k][0]->wartosc++;
		}
		free(przyjete);
		for(i=0;i<wierzcholki;i++)
		{
			k=feniks[i][0]->wartosc;
			feniks[i]=(Krawedz**)realloc(feniks[i],(k+1)*sizeof(Krawedz*));
		}
		zniszcz();
		lista=feniks;
		krawedzie=wierzcholki-1;
	}
	
	void konwertuj(GrafMST* wzor) //aby zapewnic, ze operujemy na tych samych zbiorach przy generowaniu
	{								//odczyt z pliku po raz drugi jest zbedny
		zniszcz();
		ciezar=0;
		int licznosc,podstawa,i,j;
		krawedzie=wzor->krawedzie;
		wierzcholki=wzor->wierzcholki;
		lista=(Krawedz***)malloc(wierzcholki*sizeof(Krawedz**));
		for(i=0;i<wierzcholki;i++)
		{
			podstawa=20; //ilosc znakow na ekranie
			lista[i]=(Krawedz**)malloc(podstawa*sizeof(Krawedz*));
			licznosc=0;
			for(j=0;j<wierzcholki;j++)
			{
				lista[i][0]=new Krawedz();
				if(wzor->macierz[i][j])
				{
					licznosc++;
					if(licznosc>=podstawa)
					{
						podstawa=2*podstawa;
						lista[i]=(Krawedz**)realloc(lista[i],podstawa*sizeof(Krawedz*));
					}
					lista[i][licznosc]=new Krawedz();
					lista[i][licznosc]->wartosc=wzor->macierz[i][j];
					lista[i][licznosc]->wK=j;
					lista[i][licznosc]->wP=i;
				}
			}
			lista[i]=(Krawedz**)realloc(lista[i],(1+licznosc)*sizeof(Krawedz*));
			lista[i][0]->wartosc=licznosc; //sztuczna krawedz dla wyznaczenia rozmiaru przy odwolywaniu sie
		}
	}
	
	void wyswietl()
	{
		int help,i,j;
		printf("Reprezentacja listowa ~ [wierzcholek, waga]");
		for(i=0;i<wierzcholki;i++)
		{
			printf("\n%7d ",i);
			help=lista[i][0]->wartosc;
			for(j=0;j<help;j++)
			{
				printf("[%d, %d] ",lista[i][j+1]->wK,lista[i][j+1]->wartosc);
			}
		}
		printf("\n");
		if(ciezar) printf("Waga drzewa rozpinajacego wynosi: %d\n", ciezar);
	}
	
	void zniszcz()
	{
		if(lista==NULL) return;
		int help,i,j;
		for(i=0;i<wierzcholki;i++)
		{
			help=lista[i][0]->wartosc;
			for(j=0;j<=help;j++)
			{
				delete lista[i][j];
			}
			free(lista[i]);
		}
		free(lista);
	}
};

class GrafLP //reprezentacja listowa grafu z najkrotsza sciezka
{
	int wierzcholki, krawedzie, start;
	Krawedz*** lista;
	
	void kopcowanie(Wierz** w,int i,int lim)
	{
		if(2*i>lim) return;
		int min;
		if(w[i-1]->wartosc>w[2*i-1]->wartosc) min=2*i;
		else min=i;
		if(2*i+1<=lim) 
		{
			if(w[min-1]->wartosc>w[2*i]->wartosc) min=2*i+1;
		}
		if(min!=i)
		{
			Wierz* x=w[i-1];
			w[i-1]=w[min-1];
			w[min-1]=x;
			kopcowanie(w,min,lim);
		}
	}
	
public:
	void Dijkstra()
	{
		int i,j,k,l,size=wierzcholki;
		Krawedz* kra;
		Krawedz** ark;
		Wierz** list=(Wierz**)malloc(wierzcholki*sizeof(Wierz*));
		Wierz** kolejka=(Wierz**)malloc(wierzcholki*sizeof(Wierz*));
		Wierz* temp;
		for(i=0;i<wierzcholki;i++)
		{
			list[i]=new Wierz(); //tworzymy tablice wierzcholkow
			list[i]->numer=i;
			list[i]->sasiedzi=lista[i][0]->wartosc;
			list[i]->wartosc=INT_MAX;
			list[i]->sasiad=(Krawedz**)malloc(list[i]->sasiedzi*sizeof(Krawedz*));
			list[i]->trasa=(Wierz*)NULL;
			k=list[i]->sasiedzi;
			for(j=0;j<k;j++) 
			{
				list[i]->sasiad[j]=new Krawedz();
				list[i]->sasiad[j]->wP=i;
				list[i]->sasiad[j]->wK=lista[i][j+1]->wK;
				list[i]->sasiad[j]->wartosc=lista[i][j+1]->wartosc;
			}
		}
		zniszcz();
		for(i=0;i<wierzcholki;i++) kolejka[i]=list[i];
		kolejka[0]=kolejka[start]; //przywracanie wartosci kopca nie ma sensu, wszystkie maja ten sam dystans
		kolejka[start]=list[0];
		kolejka[0]->wartosc=0;
		while(size)
		{
			size--;
			temp=kolejka[0];
			for(i=0;i<temp->sasiedzi;i++)
			{
				kra=temp->sasiad[i];
				k=list[kra->wK]->wartosc;
				if(temp->wartosc!=INT_MAX && kra->wartosc+temp->wartosc<k )
				{
					list[kra->wK]->wartosc=kra->wartosc+temp->wartosc;
					list[kra->wK]->trasa=temp;
				}
			}
			if(size==0) break;
			kolejka[0]=kolejka[size];
			for(i=size/2;i>0;i--) kopcowanie(kolejka,i,size); //tyle wywolan bo zmieniamy wiele wartosci
		}
		free(kolejka);
		lista=(Krawedz***)malloc(wierzcholki*sizeof(Krawedz**));
		for(i=0;i<wierzcholki;i++) lista[i]=(Krawedz**)malloc(wierzcholki*sizeof(Krawedz*));
		for(i=0;i<wierzcholki;i++)
		{
			for(j=0;j<list[i]->sasiedzi;j++) delete list[i]->sasiad[j];
			k=1;
			temp=list[i];
			lista[i][0]=new Krawedz();
			lista[i][0]->wP=i;
			lista[i][0]->wK=i;
			lista[i][0]->wartosc=list[i]->wartosc;
			while(temp->trasa)
			{
				temp=temp->trasa;
				lista[i][k]=new Krawedz();
				lista[i][k]->wP=i;
				lista[i][k]->wartosc=temp->wartosc;
				lista[i][k]->wK=temp->numer;
				k++;
			}
			ark=(Krawedz**)malloc((k+1)*sizeof(Krawedz*));
			for(j=0;j<k;j++) //odwracamy kolejnosc, aby uzyskac droge
			{
				ark[k-j]=lista[i][j];
			}
			ark[0]=new Krawedz();
			ark[0]->wartosc=k;
			free(lista[i]);
			lista[i]=ark;
		}
		for(i=0;i<wierzcholki;i++) delete list[i];
		free(list);
	}
	
	void FordBell()
	{
		int i,j,k,l,rozmiar=0;
		Krawedz*** feniks;
		Krawedz** ark;
		Krawedz** zbior=(Krawedz**)malloc(krawedzie*sizeof(Krawedz*));
		Wierz** list=(Wierz**)malloc(wierzcholki*sizeof(Wierz*));
		Wierz* temp;
		for(i=0;i<wierzcholki;i++)
		{
			list[i]=new Wierz(); //tworzymy liste wierzcholkow
			list[i]->numer=i;
			list[i]->wartosc=INT_MAX;
			list[i]->trasa=(Wierz*)NULL;
			k=lista[i][0]->wartosc;
			for(j=0;j<k;j++) //rownolegle tworzymy liste krawedzi
			{
				zbior[rozmiar]=new Krawedz();
				zbior[rozmiar]->wartosc=lista[i][j+1]->wartosc;
				zbior[rozmiar]->wP=i;
				zbior[rozmiar]->wK=lista[i][j+1]->wK;
				rozmiar++;
			}
		}
		zniszcz();
		zbior=(Krawedz**)realloc(zbior,rozmiar*sizeof(Krawedz*));
		list[start]->wartosc=0;
		for(i=0;i<wierzcholki-1;i++)
		{
			for(j=0;j<rozmiar;j++)
			{
				if(list[zbior[j]->wK]->wartosc>list[zbior[j]->wP]->wartosc+zbior[j]->wartosc && list[zbior[j]->wP]->wartosc!=INT_MAX)
				{
					list[zbior[j]->wK]->trasa=list[zbior[j]->wP];
					list[zbior[j]->wK]->wartosc=list[zbior[j]->wP]->wartosc+zbior[j]->wartosc;
				}
			}
		}
		for(j=0;j<rozmiar;j++)
			{
				if(list[zbior[j]->wK]->wartosc>list[zbior[j]->wP]->wartosc+zbior[j]->wartosc && list[zbior[j]->wP]->wartosc!=INT_MAX)
				{
					printf("Cykl ujemny!\n");
					printf("Stracono dane...\n");
					for(i=0;i<rozmiar;i++) delete zbior[i];
					free(zbior);
					for(i=0;i<wierzcholki;i++)
					{
						for(j=0;j<list[i]->sasiedzi;j++) delete list[i]->sasiad[j];
						delete list[i];
					}
					free(list);
					return;
				}
			}
		for(i=0;i<rozmiar;i++) delete zbior[i];
		free(zbior);
		lista=(Krawedz***)malloc(wierzcholki*sizeof(Krawedz**));
		for(i=0;i<wierzcholki;i++) lista[i]=(Krawedz**)malloc(wierzcholki*sizeof(Krawedz*));
		for(i=0;i<wierzcholki;i++)
		{
			for(j=0;j<list[i]->sasiedzi;j++) delete list[i]->sasiad[j];
			k=1;
			temp=list[i];
			lista[i][0]=new Krawedz();
			lista[i][0]->wP=i;
			lista[i][0]->wK=i;
			lista[i][0]->wartosc=list[i]->wartosc;
			while(temp->trasa)
			{
				temp=temp->trasa;
				lista[i][k]=new Krawedz();
				lista[i][k]->wP=i;
				lista[i][k]->wartosc=temp->wartosc;
				lista[i][k]->wK=temp->numer;
				k++;
			}
			ark=(Krawedz**)malloc((k+1)*sizeof(Krawedz*));
			for(j=0;j<k;j++) //odwracamy kolejnosc, aby uzyskac droge
			{
				ark[k-j]=lista[i][j];
			}
			ark[0]=new Krawedz();
			ark[0]->wartosc=k;
			free(lista[i]);
			lista[i]=ark;
		}
		for(i=0;i<wierzcholki;i++) delete list[i];
		free(list);
	}
	
	void konwertuj(GrafMP* wzor)
	{
		zniszcz();
		int licznosc,podstawa,i,j;
		krawedzie=wzor->krawedzie;
		wierzcholki=wzor->wierzcholki;
		lista=(Krawedz***)malloc(wierzcholki*sizeof(Krawedz**));
		for(i=0;i<wierzcholki;i++)
		{
			podstawa=20; //ilosc znakow na ekranie
			lista[i]=(Krawedz**)malloc(podstawa*sizeof(Krawedz*));
			licznosc=0;
			for(j=0;j<wierzcholki;j++)
			{
				lista[i][0]=new Krawedz();
				if(wzor->macierz[i][j])
				{
					licznosc++;
					if(licznosc>=podstawa)
					{
						podstawa=2*podstawa;
						lista[i]=(Krawedz**)realloc(lista[i],podstawa*sizeof(Krawedz*));
					}
					lista[i][licznosc]=new Krawedz();
					lista[i][licznosc]->wartosc=wzor->macierz[i][j];
					lista[i][licznosc]->wK=j;
					lista[i][licznosc]->wP=i;
				}
			}
			lista[i]=(Krawedz**)realloc(lista[i],(1+licznosc)*sizeof(Krawedz*));
			lista[i][0]->wartosc=licznosc; //sztuczna krawedz dla wyznaczenia rozmiaru przy odwolywaniu sie
		}
	}
	
	void copy(GrafMP* cat)
	{
		start=cat->start;
	}
	
	void wyswietl()
	{
		int help,i,j;
		printf("Reprezentacja listowa ~ [wierzcholek, waga]");
		for(i=0;i<wierzcholki;i++)
		{
			printf("\n%7d ",i);
			help=lista[i][0]->wartosc;
			for(j=0;j<help;j++)
			{
				printf("[%d, %d] ",lista[i][j+1]->wK,lista[i][j+1]->wartosc);
			}
		}
		printf("\n");
	}
	
	void zniszcz()
	{
		if(lista==NULL) return;
		int help,i,j;
		for(i=0;i<wierzcholki;i++)
		{
			help=lista[i][0]->wartosc;
			for(j=0;j<=help;j++)
			{
				delete lista[i][j];
			}
			free(lista[i]);
		}
		free(lista);
	}
};



void menu(std::string nazwa) //oszczedzanie sobie drukowania
{
	printf("%s\n",nazwa.c_str());
	printf("1. Wczytaj dane z pliku\n");
	printf("2. Generuj losowy graf\n");
	printf("3. Wyswietl graf\n");
}

void menuMST(GrafLST* dany, GrafMST* dana) //minimum spanning tree
{
	short wyb;
	GrafLST* lista=dany;
	GrafMST* macierz=dana;
	menu("---WYZNACZANIE MINIMALNEGO DRZEWA ROZPINAJACEGO---");
	printf("4. Algorytm Prima\n");
	printf("5. Algorytm Kruskala\n");
	printf("6. Tryb pomiarowy\n");
	printf("0. Powrot do menu wyboru problemu\n");
	printf("Podaj numer opcji: ");
	scanf("%hd", &wyb);
	switch(wyb)
	{
		case 1:
			{
				std::string nazwa;
				printf("Podaj nazwe pliku: ");
				scanf("%s",nazwa.c_str());
				if(macierz==NULL) 
				{
					macierz=new GrafMST();
					lista=new GrafLST();
				}
				switch(macierz->zaladuj(nazwa))
				{
					case (-1):
						printf("BLAD! Nie udalo sie otworzyc pliku!\n");
						break;
					case 0:
						printf("BLAD! W pliku znajduja sie nieprawidlowe dane!\n");
						break;
					case 1:
						macierz->wyswietl();
						lista->konwertuj(macierz);
						lista->wyswietl();
						break;
				}
				system("Pause");
				break;
			}
		case 2:
			{
				int rozmiar,krawedzie;
				float gestosc;
				printf("Polecana ilosc wierzcholkow: 20\n"); //tyle sie miesci na pelnym ekranie (przynajmniej u mnie)
				printf("Podaj ilosc wierzcholkow: ");
				scanf("%d", &rozmiar);
				printf("Podaj gestosc w procentach: ");
				scanf("%f",&gestosc);
				if(gestosc<0 || gestosc>100)
				{
					printf("Nie mozna wygenerowac takiego grafu prostego!\n");
				}
				krawedzie=(int)(rozmiar*(rozmiar-1)*gestosc/200); //nie mozemy miec niecalkowitej ilosci krawedzi
				if(macierz==NULL) 
				{
					macierz=new GrafMST();
					lista=new GrafLST();
				}
				if(rozmiar-1>krawedzie) printf("Nie mozna znalezc drzewa spinajacego dla grafu niespojnego!\n"); //nie moze byc spojny nie majac wystarczajacej ilosci krawedzi
				else 
				{
					macierz->zniszcz();
					macierz->generuj(rozmiar,krawedzie);
					macierz->wyswietl();
					lista->konwertuj(macierz);
					lista->wyswietl();
				}
				system("Pause");
				break;
			}
		case 3:
			if(macierz==NULL)
			{
				printf("Potrzeba grafu do wyswietlenia!\n");
				system("Pause");
				break;
			}
			macierz->wyswietl();
			lista->wyswietl();
			system("Pause");
			break;
		case 4:
			if(macierz==NULL)
			{
				printf("Potrzeba grafu do algorytmu!\n");
				system("Pause");
				break;
			}
			printf("Przed dzialaniem algorytmu:\n");
			macierz->wyswietl();
			lista->wyswietl();
			printf("Po dzialaniu algorytmu:\n");
			macierz->Prim();
			macierz->wyswietl();
			lista->Prim();
			lista->wyswietl();
			system("Pause");
			break;
		case 5:
			if(macierz==NULL)
			{
				printf("Potrzeba grafu do algorytmu!\n");
				system("Pause");
				break;
			}
			printf("Przed dzialaniem algorytmu:\n");
			macierz->wyswietl();
			lista->wyswietl();
			printf("Po dzialaniu algorytmu:\n");
			macierz->Kruskal();
			macierz->wyswietl();
			lista->Kruskal();
			lista->wyswietl();
			system("Pause");
			break;	
		case 6:
			printf("1.Prim\n2.Kruskal\nDajesz: "); //funkcja pomiarowa
			scanf("%d",&wyb);
			if(macierz==NULL) 
			{
				macierz=new GrafMST();
				lista=new GrafLST();
			}
			switch(wyb)
			{
				case 1:
					{
						double Czas;
						int i,j,k,kraw;
						int populus[7]={REP1,REP2,REP3,REP4,REP5,REP6,REP7};
						int procent[3]={GEST1, GEST2, GEST3};
						for(i=0;i<7;i++)
						{
							for(k=0;k<3;k++)
							{
								Czas=0;	
								for(j=0;j<100;j++)
								{
									kraw=populus[i]*(populus[i]-1)*procent[k]/200;
									macierz->generuj(populus[i],kraw);
									StartCounter();
									macierz->Prim();
									Czas+=GetCounter();
									macierz->zniszcz();
	        					}
								printf("\nOperacja na %d elementach w %d %% macierzowo zajmuje srednio %2.6f ms\n",populus[i],procent[k], Czas/100);
								
							}
						}
						for(i=0;i<7;i++)
						{
							for(k=0;k<3;k++)
							{
								Czas=0;	
								for(j=0;j<100;j++)
								{
									kraw=(int)populus[i]*(populus[i]-1)*procent[k]/200;
									macierz->generuj(populus[i],kraw);
									lista->konwertuj(macierz);
									StartCounter();
									lista->Prim();
									Czas+=GetCounter();
									macierz->zniszcz();
	        					}
								printf("\nOperacja na %d elementach w %d %% listowo zajmuje srednio %2.6f ms\n",populus[i],procent[k], Czas/100);
							}
						}
						lista->zniszcz();
						delete macierz;
						delete lista;
						macierz=(GrafMST*)NULL;
						lista=(GrafLST*)NULL;
						system("Pause");
						break;
					}
				case 2:
					{
						double Czas;
						int i,j,k,kraw;
						int populus[7]={REP1,REP2,REP3,REP4,REP5,REP6,REP7};
						int procent[3]={GEST1, GEST2, GEST3};
						for(i=0;i<7;i++)
						{
							for(k=0;k<3;k++)
							{
								Czas=0;	
								for(j=0;j<100;j++)
								{
									kraw=populus[i]*(populus[i]-1)*procent[k]/200;
									macierz->generuj(populus[i],kraw);
									StartCounter();
									macierz->Kruskal();
									Czas+=GetCounter();
									macierz->zniszcz();
	        					}
								printf("\nOperacja na %d elementach w %d %% macierzowo zajmuje srednio %.6f ms\n",populus[i],procent[k], Czas/100);
							}
						}
						for(i=0;i<7;i++)
						{
							for(k=0;k<3;k++)
							{
								Czas=0;	
								kraw=populus[i]*(populus[i]-1)*procent[k]/200;
								for(j=0;j<100;j++)
								{
									macierz->generuj(populus[i],kraw);
									lista->konwertuj(macierz);
									StartCounter();
									lista->Kruskal();
									Czas+=GetCounter();
									macierz->zniszcz();
	        					}
								printf("\nOperacja na %d elementach w %d %% listowo zajmuje srednio %.6f ms\n",populus[i],procent[k], Czas/100);
							}
						}
						lista->zniszcz();
						delete macierz;
						delete lista;
						macierz=(GrafMST*)NULL;
						lista=(GrafLST*)NULL;
						system("Pause");
						break;
					}
				}
				break;
		case 0:
			if(!macierz) //tworzymy macierz i liste razem, wiec wystarczy jedno sprawdzic
			{
				system("cls");
				return;
			}
			macierz->zniszcz();
			lista->zniszcz();
			delete macierz;
			delete lista;
			system("cls");
			return;
	}
	system("cls");
	menuMST(lista, macierz);
}

void menuSP(GrafLP* dany, GrafMP* dana) //shortest path
{
	short wyb;
	GrafLP* lista=dany;
	GrafMP* macierz=dana;
	menu("---WYZNACZANIE NAJKROTSZEJ SCIEZKI W GRAFIE---");
	printf("4. Algorytm Dijkstry\n");
	printf("5. Algorytm Forda-Bellmana\n");
	printf("6. Pomiary\n");
	printf("0. Powrot do menu wyboru problemu\n");
	printf("Podaj numer opcji: ");
	scanf("%hd", &wyb);
	switch(wyb)
	{
		case 1:
			{
				std::string nazwa;
				printf("Podaj nazwe pliku: ");
				scanf("%s",nazwa.c_str());
				if(macierz==NULL) 
				{
					macierz=new GrafMP();
					lista=new GrafLP();
				}
				wyb=macierz->zaladuj(nazwa);
				switch(wyb)
				{
					case (-1):
						printf("BLAD! Nie udalo sie otworzyc pliku!\n");
						break;
					case 0:
						printf("BLAD! W pliku znajduja sie nieprawidlowe dane!\n");
						break;
					case 1:
						macierz->wyswietl();
						lista->konwertuj(macierz);
						lista->copy(macierz);
						lista->wyswietl();
						break;
				}
				system("Pause");
				break;
			}
		case 2:
			{
				int rozmiar,krawedzie;
				float gestosc;
				printf("Polecana ilosc wierzcholkow: 20\n"); //tyle sie miesci na pelnym ekranie (przynajmniej u mnie)
				printf("Podaj ilosc wierzcholkow: ");
				scanf("%d", &rozmiar);
				printf("Podaj gestosc w procentach: ");
				scanf("%f",&gestosc);
				if(gestosc<0 || gestosc>100)
				{
					printf("Nie mozna wygenerowac takiego grafu prostego!\n");
				}
				krawedzie=(int)(rozmiar*(rozmiar-1)*gestosc/200); //nie mozemy miec niecalkowitej ilosci krawedzi
				if(macierz==NULL) 
				{
					macierz=new GrafMP();
					lista=new GrafLP();
				}
				if(rozmiar-1>krawedzie) printf("Nie mozna znalezc drzewa spinajacego dla grafu niespojnego!\n"); //nie moze byc spojny nie majac wystarczajacej ilosci krawedzi
				else 
				{
					macierz->zniszcz();
					macierz->generuj(rozmiar,krawedzie);
					macierz->wyswietl();
					lista->konwertuj(macierz);
					lista->wyswietl();
					macierz->question();
					lista->copy(macierz);
					break;
				}
				system("Pause");
				break;
			}
		case 3:
			if(macierz==NULL)
			{
				printf("Potrzeba grafu do wyswietlenia!\n");
				system("Pause");
				break;
			}
			macierz->wyswietl();
			lista->wyswietl();
			system("Pause");
			break;
		case 4:
			if(macierz==NULL)
			{
				printf("Potrzeba grafu do algorytmu!\n");
				system("Pause");
				break;
			}
			printf("Dane na ktorych dzialamy:\n");
			macierz->wyswietl();
			lista->wyswietl();
			printf("Wyniki:\n");
			macierz->Dijkstra();
			macierz->wyswietl();
			lista->Dijkstra();
			lista->wyswietl();
			system("Pause");
			break;
		case 5:
			if(macierz==NULL)
			{
				printf("Potrzeba grafu do algorytmu!\n");
				system("Pause");
				break;
			}
			printf("Dane na ktorych dzialamy:\n");
			macierz->wyswietl();
			lista->wyswietl();
			printf("Wyniki:\n");
			macierz->FordBell();
			macierz->wyswietl();
			lista->FordBell();
			lista->wyswietl();
			system("Pause");
			break;	
		case 6:
			printf("1.Dijkstra\n2.Ford-Bell\nDajesz: "); //funkcja pomiarowa
			scanf("%d",&wyb);
			if(macierz==NULL) 
			{
				macierz=new GrafMP();
				lista=new GrafLP();
			}		
			switch(wyb)
			{;
				case 1:
					{
						double Czas;
						int i,j,k,kraw;
						int populus[]={REP1,REP2,REP3,REP4,REP5,REP6,REP7};
						int procent[]={GEST1, GEST2, GEST3};
						for(i=0;i<7;i++)
						{
							for(k=0;k<3;k++)
							{
								Czas=0;	
								for(j=0;j<100;j++)
								{
									kraw=(int)populus[i]*(populus[i]-1)*procent[k]/200;
									macierz->generuj(populus[i],kraw);
									StartCounter();
									macierz->Dijkstra();
									Czas+=GetCounter();
									macierz->zniszcz();
	        					}
								printf("\nOperacja na %d elementach w %d %% macierzowo zajmuje srednio %.6f ms\n",populus[i],procent[k], Czas/100);
								
							}
						}for(i=0;i<7;i++)
						{
							for(k=0;k<3;k++)
							{
								Czas=0;	
								for(j=0;j<100;j++)
								{
									kraw=(int)populus[i]*(populus[i]-1)*procent[k]/200;
									macierz->generuj(populus[i],kraw);
									lista->konwertuj(macierz);
									StartCounter();
									lista->Dijkstra();
									Czas+=GetCounter();
									macierz->zniszcz();
	        					}
								printf("\nOperacja na %d elementach w %d %% listowo zajmuje srednio %.6f ms\n",populus[i],procent[k], Czas/100);
								
							}
						}
						lista->zniszcz();
						delete macierz;
						delete lista;
						macierz=(GrafMP*)NULL;
						lista=(GrafLP*)NULL;
						system("Pause");
						break;
					}
				case 2:
					{
						double Czas;
						int i,j,k,kraw;
						int populus[]={REP1,REP2,REP3,REP4,REP5,REP6,REP7};
						int procent[]={GEST1, GEST2, GEST3};
						for(i=0;i<7;i++)
						{
							for(k=0;k<3;k++)
							{
								Czas=0;	
								for(j=0;j<100;j++)
								{
									kraw=(int)populus[i]*(populus[i]-1)*procent[k]/200;
									macierz->generuj(populus[i],kraw);
									StartCounter();
									macierz->FordBell();
									Czas+=GetCounter();
									macierz->zniszcz();
	        					}
								printf("\nOperacja na %d elementach w %d %% macierzowo zajmuje srednio %.6f ms\n",populus[i],procent[k], Czas/100);
								
							}
						}for(i=0;i<7;i++)
						{
							for(k=0;k<3;k++)
							{
								Czas=0;	
								for(j=0;j<100;j++)
								{
									kraw=(int)populus[i]*(populus[i]-1)*procent[k]/200;
									macierz->generuj(populus[i],kraw);
									lista->konwertuj(macierz);
									StartCounter();
									lista->FordBell();
									Czas+=GetCounter();
									macierz->zniszcz();
	        					}
								printf("\nOperacja na %d elementach w %d %% listowo zajmuje srednio %.6f ms\n",populus[i],procent[k], Czas/100);
								
							}
						}
						lista->zniszcz();
						delete macierz;
						delete lista;
						macierz=(GrafMP*)NULL;
						lista=(GrafLP*)NULL;
						system("Pause");
						break;
					}
			}
			break;
		case 0:
			if(!lista)
			{
				system("cls");
				return;
			}
			macierz->zniszcz();
			lista->zniszcz();
			delete macierz;
			delete lista;
			system("cls");
			return;
	}
	system("cls");
	menuSP(lista, macierz);
}


void menuWyboru()
{
	short num;
	printf("------MENU WYBORU PROBLEMU------\n");
	printf("1. Minimalne drzewo rozpinajace\n");
	printf("2. Najkrotsza sciezka w grafie\n");
	printf("0. Zakoncz dzialanie programu\n");
	printf("Podaj numer opcji: ");
	scanf("%hd",&num);
	switch(num)
	{
		case 1:
			system("cls");
			menuMST((GrafLST*)NULL, (GrafMST*)NULL);
			break;
		case 2:
			system("cls");
			menuSP((GrafLP*)NULL,(GrafMP*)NULL);
			break;
		case 0:
			printf("Konczenie pracy programu...\n");
			return;
		default:
			printf("Opcja nieobslugiwana!\n");
			system("Pause");
			system("cls");
			
	}
	menuWyboru();
}

int main()
{
	srand(time(NULL));
	menuWyboru();
	system("Pause");
	return 0;
}
