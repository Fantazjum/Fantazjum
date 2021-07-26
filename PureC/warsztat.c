#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <errno.h>
#include <time.h>

#define TOOLNUM 20
#define ESCAPE 27
#define WORKSPACE 1
#define MECHANIC 2
#define ASSISTANT 3
#define WALL 4
#define CAR 5
#define DOWN 20

typedef struct{
	int id;
	int table[2];
	int* coords;
	bool* endThread;
	unsigned* luck;
}person;

typedef struct{
	int* coords;
	int station;
	bool toRepair;
}car;

//lista do zachowania kolejności rysowania
typedef struct element{
	struct element* next;
	int id;
}el;

typedef struct{
	el* head;
	el* tail;
}elList;

elList lista, naprawa;
char symbols[4] = {' ','W','M','A'};
int **workspacePlacement, wallPlacement[4][2]; //tylko wierzcholki
int row, col, offset; //wymiary ekranu i wydzielenie przestrzeni na paski postępu
int mechThreads = 17, assistThreads = 11;
int doormin, doormax, lpark, rpark; //limity dla poruszania się asystentów
int toolTotal = TOOLNUM, cars = 50, workspaces = 32;
pthread_mutex_t *tools, *work, *carLock, *moved, *FIFO, *enlist;
pthread_cond_t *drawn;
bool *returning, *occupied;
car *carPlacement;
bool end = false, change = false;
bool **changeTable;
int **changeRecord;

void *drawing(void* pointer)
{
	int i, j;
	pthread_mutex_lock(moved);
	while(!end)
	{
		while(!end && !change)
		{
			pthread_cond_wait(drawn, moved);
		}
		if(!end) //rysowanie zmian, kolorowanie według własnych ustaleń
		{
			for(i = 0; i < row; i++)
			{
				for(j = 0; j < col; j++)
				{
					if(changeTable[i][j])
					{
						changeTable[i][j] = false;
						switch(changeRecord[i][j])
						{
							case 0:
								attron(COLOR_PAIR(0));
								mvaddch(i, j, ' ');
								attroff(COLOR_PAIR(0));
								break;
							case WORKSPACE:
								attron(COLOR_PAIR(WORKSPACE));
								mvaddch(i, j, ' ');
								attroff(COLOR_PAIR(WORKSPACE));
								break;
							case MECHANIC:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, symbols[MECHANIC]);
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case ASSISTANT:
								attron(COLOR_PAIR(ASSISTANT));
								mvaddch(i, j, symbols[ASSISTANT]);
								attroff(COLOR_PAIR(ASSISTANT));
								break;
							case 4:
								attron(COLOR_PAIR(WORKSPACE));
								mvaddch(i, j, ACS_DIAMOND);
								attroff(COLOR_PAIR(WORKSPACE));
								break;
							case CAR:
								attron(COLOR_PAIR(CAR));
								mvaddch(i, j, ' ');
								attroff(COLOR_PAIR(CAR));
								break;
							case 6:
								attron(COLOR_PAIR(CAR));
								mvaddch(i, j, symbols[ASSISTANT]);
								attroff(COLOR_PAIR(CAR));
								break;
							case 7:
								attron(COLOR_PAIR(WORKSPACE));
								mvaddch(i, j, ACS_LANTERN);
								attroff(COLOR_PAIR(WORKSPACE));
								break;
							case 8:
								attron(COLOR_PAIR(WORKSPACE));
								mvaddch(i, j, ' ');
								attroff(COLOR_PAIR(WORKSPACE));
								break;
							case 10:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, '0');
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case 11:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, '1');
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case 12:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, '2');
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case 13:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, '3');
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case 14:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, '4');
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case 15:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, '5');
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case 16:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, '6');
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case 17:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, '7');
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case 18:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, '8');
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case 19:
								attron(COLOR_PAIR(MECHANIC));
								mvaddch(i, j, '9');
								attroff(COLOR_PAIR(MECHANIC));
								break;
							case 20:
								attron(COLOR_PAIR(WALL));
								mvaddch(i, j, ACS_HLINE);
								attroff(COLOR_PAIR(WALL));
							default:
								if(changeRecord[i][j] > 'A'-1 && (changeRecord[i][j] < 'Z'+1 || changeRecord[i][j] > 'a'-1) && changeRecord[i][j] < 'z'+1)
								{
									if(changeRecord[i][j] < 'a')
									{
										attron(COLOR_PAIR(MECHANIC));
										mvaddch(i, j, (char)changeRecord[i][j]);
										attroff(COLOR_PAIR(MECHANIC));
									}
									else
									{
										attron(COLOR_PAIR(ASSISTANT));
										mvaddch(i, j, (char)changeRecord[i][j]-32); //wielkie litery
										attroff(COLOR_PAIR(ASSISTANT));
									}
								}
								break;
						}
						usleep(100);
					}
					if(end) break;
				}
				if(end) break;
			}
			if(end) break;
			refresh();
		}
		change = false;
	}
	pthread_mutex_unlock(moved);
}

void *assist(void* pointer)
{
	person* assistant = (person*)pointer;
	el* new;
	int* coords = assistant->coords;
	int currCar, i, ret, space, lastCar = rand_r(assistant->luck)%cars;
	int goal[2];
	int tablet[2];
	tablet[0] = assistant->table[0];
	tablet[1] = assistant->table[1];
	bool driving = false;
	bool leading = false;
	bool held = false;
	bool flag;
	car* used;
	while(!*(assistant->endThread))
	{	
		if(driving)
		{
			flag = true;
			if(leading)
			{
				if(used->coords[1] < wallPlacement[0][1]) //lewa część mapy
				{
					if(coords[1] < lpark) //już po dobrej stronie
					{
						if(coords[0] == used->coords[0]) //jeśli stoi na wysokości auta, musimy przesunąć
						{
							//ze względu na sięganie po mutexy nie wydawało mi się bezpieczne tworzenie zewnętrznej funkcji do blokowania
							//wszystkie wywołania kodu o tych funkcjach są w klamrach zaczętych odpowiednim komentarzem
							{ //zajęcie rysowania
								ret = 1;
								while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); //akcje w tym bloku są krótkie
								if(*(assistant->endThread)) break;
								new = (el*)malloc(sizeof(el));
								new->id = assistant->id;
								new->next = (el*)NULL;
								if(lista.head == NULL)	lista.head = new;
								else 	lista.tail->next = new;
								lista.tail = new;
								pthread_mutex_unlock(enlist); //dodanie do listy następuje przed odczytem
								while(lista.head != new) usleep(10000); //dodawanie jest tylko na koniec listy, a usuwanie z początku
								ret = pthread_mutex_trylock(moved); //usuwanie zachodzi na koniec działania, odczyt nie powinien mieć problemów
								while (!*(assistant->endThread) && ret != 0) 
								{
									usleep(1000);
									ret = pthread_mutex_trylock(moved);
								}
								if(*(assistant->endThread))
								{ 
									free(new);
									if(!ret) pthread_mutex_unlock(moved);
									break;
								}
							}
							//zejście z drogi auta
							changeRecord[coords[0]][coords[1]] = 0;
							changeTable[coords[0]][coords[1]] = true;
							if(used->coords[0] < doormax) coords[0]--;
							else coords[0]++;
							changeRecord[coords[0]][coords[1]] = ASSISTANT;
							changeTable[coords[0]][coords[1]] = true;
							change = true;
							{ //zwolnienie rysowania
								pthread_mutex_unlock(moved);
								pthread_cond_signal(drawn);
								ret = pthread_mutex_trylock(enlist);
								while(ret != 0) ret = pthread_mutex_trylock(enlist);
								lista.head = new->next;
								if(new == lista.tail) lista.tail = (el*)NULL;
								free(new);
								pthread_mutex_unlock(enlist);
							}
							usleep(300000);
						}
						goal[0] = used->coords[0];
						goal[1] = used->coords[1] - 1;
						flag = false;
					}
					else if(coords[1] < wallPlacement[1][1] && coords[0] > wallPlacement[0][0]) //wewnątrz
					{ 
						if(coords[0] < doormin) goal[0] = doormin;
						else goal[0] = doormax;
						goal[1] = wallPlacement[0][1] + 1;
					}
					else //przypadek występujący na starcie i gdy auto jest po przeciwnej stronie
					{
						while(coords[0] > wallPlacement[0][0] - 3)
						{
							{ //zajęcie rysowania
								ret = 1;
								while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); //akcje w tym bloku są krótkie
								if(*(assistant->endThread)) break;
								new = (el*)malloc(sizeof(el));
								new->id = assistant->id;
								new->next = (el*)NULL;
								if(lista.head == NULL)	lista.head = new;
								else 	lista.tail->next = new;
								lista.tail = new;
								pthread_mutex_unlock(enlist);
								while(lista.head != new) usleep(10000);
								ret = pthread_mutex_trylock(moved);
								while (!*(assistant->endThread) && ret != 0) 
								{
									usleep(1000);
									ret = pthread_mutex_trylock(moved);
								}
								if(*(assistant->endThread))
								{ 
									free(new);
									if(!ret) pthread_mutex_unlock(moved);
									break;
								}
							}
							//wyjście ponad garaż
							changeRecord[coords[0]][coords[1]] = 0;
							changeTable[coords[0]--][coords[1]] = true;
							changeRecord[coords[0]][coords[1]] = ASSISTANT;
							changeTable[coords[0]][coords[1]] = true;
							change = true;
							{ //zwolnienie rysowania
								pthread_mutex_unlock(moved);
								pthread_cond_signal(drawn);
								ret = pthread_mutex_trylock(enlist);
								while(ret != 0) ret = pthread_mutex_trylock(enlist);
								lista.head = new->next;
								if(new == lista.tail) lista.tail = (el*)NULL;
								free(new);
								pthread_mutex_unlock(enlist);
							}
							usleep(300000);
						}
						goal[0] = used->coords[0] - 1;
						goal[1] = lpark;
					}
				}
				else
				{
					if(coords[1] > rpark)
					{
						if(coords[0] == used->coords[0])
						{
							{ //zajęcie rysowania
								ret = 1;
								while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); //akcje w tym bloku są krótkie
								if(*(assistant->endThread)) break;
								new = (el*)malloc(sizeof(el));
								new->id = assistant->id;
								new->next = (el*)NULL;
								if(lista.head == NULL)	lista.head = new;
								else 	lista.tail->next = new;
								lista.tail = new;
								pthread_mutex_unlock(enlist);
								while(lista.head != new) usleep(10000);
								ret = pthread_mutex_trylock(moved);
								while (!*(assistant->endThread) && ret != 0) 
								{
									usleep(1000);
									ret = pthread_mutex_trylock(moved);
								}
								if(*(assistant->endThread))
								{ 
									free(new);
									if(!ret) pthread_mutex_unlock(moved);
									break;
								}
							}
							//zejście z drogi auta
							changeRecord[coords[0]][coords[1]] = 0;
							changeTable[coords[0]--][coords[1]] = true;
							changeRecord[coords[0]][coords[1]] = ASSISTANT;
							changeTable[coords[0]][coords[1]] = true;
							change = true;
							{ //zwolnienie rysowania
								pthread_mutex_unlock(moved);
								pthread_cond_signal(drawn);
								ret = pthread_mutex_trylock(enlist);
								while(ret != 0) ret = pthread_mutex_trylock(enlist);
								lista.head = new->next;
								if(new == lista.tail) lista.tail = (el*)NULL;
								free(new);
								pthread_mutex_unlock(enlist);
							}
							usleep(300000);
						}
						goal[0] = used->coords[0];
						goal[1] = used->coords[1] + 1;
					}
					else if(coords[1] > wallPlacement[0][1] && coords[0] > wallPlacement[0][0])
					{
						if(coords[0] < doormin) goal[0] = doormin;
						else goal[0] = doormax;
						goal[1] = wallPlacement[1][1] - 1;
					}
					else
					{
						while(coords[0] > wallPlacement[0][0] - 3)
						{
							{ //zajęcie rysowania
								ret = 1;
								while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
								if(*(assistant->endThread)) break;
								new = (el*)malloc(sizeof(el));
								new->id = assistant->id;
								new->next = (el*)NULL;
								if(lista.head == NULL)	lista.head = new;
								else 	lista.tail->next = new;
								lista.tail = new;
								pthread_mutex_unlock(enlist);
								while(lista.head != new) usleep(10000);
								ret = pthread_mutex_trylock(moved); 
								while (!*(assistant->endThread) && ret != 0) 
								{
									usleep(1000);
									ret = pthread_mutex_trylock(moved);
								}
								if(*(assistant->endThread))
								{ 
									free(new);
									if(!ret) pthread_mutex_unlock(moved);
									break;
								}
							}
							//wyjście ponad garaż
							changeRecord[coords[0]][coords[1]] = 0;
							changeTable[coords[0]--][coords[1]] = true;
							changeRecord[coords[0]][coords[1]] = ASSISTANT;
							changeTable[coords[0]][coords[1]] = true;
							change = true;
							{ //zwolnienie rysowania
								pthread_mutex_unlock(moved);
								pthread_cond_signal(drawn);
								ret = pthread_mutex_trylock(enlist);
								while(ret != 0) ret = pthread_mutex_trylock(enlist);
								lista.head = new->next;
								if(new == lista.tail) lista.tail = (el*)NULL;
								free(new);
								pthread_mutex_unlock(enlist);
							}
							usleep(300000);
						}
						goal[0] = used->coords[0] - 1;
						goal[1] = rpark;
					}
				}
				
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				{//czyszczenie paska postępu
					changeRecord[tablet[0]][tablet[1]] = 0;
					changeTable[tablet[0]][tablet[1]] = true;
					changeRecord[tablet[0]][tablet[1]+1] = 0;
					changeTable[tablet[0]][tablet[1]+1] = true;
					changeRecord[tablet[0]][tablet[1]+2] = 0;
					changeTable[tablet[0]][tablet[1]+2] = true;
					changeRecord[tablet[0]][tablet[1]+3] = 0;
					changeTable[tablet[0]][tablet[1]+3] = true;
					changeRecord[tablet[0]][tablet[1]+4] = 0;
					changeTable[tablet[0]][tablet[1]+4] = true;
					changeRecord[tablet[0]][tablet[1]+5] = 0;
					changeTable[tablet[0]][tablet[1]+5] = true;
					changeRecord[tablet[0]][tablet[1]+6] = 0;
					changeTable[tablet[0]][tablet[1]+6] = true;
					changeRecord[tablet[0]][tablet[1]+7] = 0;
					changeTable[tablet[0]][tablet[1]+7] = true;
					changeRecord[tablet[0]][tablet[1]+8] = 0;
					changeTable[tablet[0]][tablet[1]+8] = true;
					changeRecord[tablet[0]][tablet[1]+9] = 0;
					changeTable[tablet[0]][tablet[1]+9] = true;
				}
				{//aktualizacja tablicy o informację co się dzieje
					changeRecord[tablet[0]+1][tablet[1]] = 'z';
					changeTable[tablet[0]+1][tablet[1]] = true;
					changeRecord[tablet[0]+1][tablet[1]+1] = 'a';
					changeTable[tablet[0]+1][tablet[1]+1] = true;
					changeRecord[tablet[0]+1][tablet[1]+2] = 'b';
					changeTable[tablet[0]+1][tablet[1]+2] = true;
					changeRecord[tablet[0]+1][tablet[1]+3] = 'i';
					changeTable[tablet[0]+1][tablet[1]+3] = true;
					changeRecord[tablet[0]+1][tablet[1]+4] = 'e';
					changeTable[tablet[0]+1][tablet[1]+4] = true;
					changeRecord[tablet[0]+1][tablet[1]+5] = 'r';
					changeTable[tablet[0]+1][tablet[1]+5] = true;
					changeRecord[tablet[0]+1][tablet[1]+6] = 'a';
					changeTable[tablet[0]+1][tablet[1]+6] = true;
					changeRecord[tablet[0]+1][tablet[1]+7] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+7] = true;
					changeRecord[tablet[0]+1][tablet[1]+8] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+8] = true;
					changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+9] = true;
				}
				change = true;
				{ //zwalnianie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
			}
			else 
			{
				if(coords[1] < wallPlacement[0][1] || coords[1] > wallPlacement[1][1]) //na zewnątrz
				{
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//zejście asystenta z linii auta
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]++][coords[1]] = true;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);	 
					if(coords[0] < doormax) goal[0] = doormin;
					else goal[0] = doormax;
					if(coords[1] > wallPlacement[1][1]) goal[1] = wallPlacement[1][1] + 1;
					else goal[1] = wallPlacement[0][1] - 1;
				}
				else
				{
					if(coords[0] < workspacePlacement[space][0]) goal[0] = workspacePlacement[space][0] - 1;
					else goal[0] = workspacePlacement[space][0] + 1;
					goal[1] = workspacePlacement[space][1];
					flag = false;
				}
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				{//czyszczenie paska postępu
					changeRecord[tablet[0]][tablet[1]] = 0;
					changeTable[tablet[0]][tablet[1]] = true;
					changeRecord[tablet[0]][tablet[1]+1] = 0;
					changeTable[tablet[0]][tablet[1]+1] = true;
					changeRecord[tablet[0]][tablet[1]+2] = 0;
					changeTable[tablet[0]][tablet[1]+2] = true;
					changeRecord[tablet[0]][tablet[1]+3] = 0;
					changeTable[tablet[0]][tablet[1]+3] = true;
					changeRecord[tablet[0]][tablet[1]+4] = 0;
					changeTable[tablet[0]][tablet[1]+4] = true;
					changeRecord[tablet[0]][tablet[1]+5] = 0;
					changeTable[tablet[0]][tablet[1]+5] = true;
					changeRecord[tablet[0]][tablet[1]+6] = 0;
					changeTable[tablet[0]][tablet[1]+6] = true;
					changeRecord[tablet[0]][tablet[1]+7] = 0;
					changeTable[tablet[0]][tablet[1]+7] = true;
					changeRecord[tablet[0]][tablet[1]+8] = 0;
					changeTable[tablet[0]][tablet[1]+8] = true;
					changeRecord[tablet[0]][tablet[1]+9] = 0;
					changeTable[tablet[0]][tablet[1]+9] = true;
				}
				{//aktualizacja tablicy o informację co się dzieje
					changeRecord[tablet[0]+1][tablet[1]] = 'o';
					changeTable[tablet[0]+1][tablet[1]] = true;
					changeRecord[tablet[0]+1][tablet[1]+1] = 'd';
					changeTable[tablet[0]+1][tablet[1]+1] = true;
					changeRecord[tablet[0]+1][tablet[1]+2] = 'b';
					changeTable[tablet[0]+1][tablet[1]+2] = true;
					changeRecord[tablet[0]+1][tablet[1]+3] = 'i';
					changeTable[tablet[0]+1][tablet[1]+3] = true;
					changeRecord[tablet[0]+1][tablet[1]+4] = 'e';
					changeTable[tablet[0]+1][tablet[1]+4] = true;
					changeRecord[tablet[0]+1][tablet[1]+5] = 'r';
					changeTable[tablet[0]+1][tablet[1]+5] = true;
					changeRecord[tablet[0]+1][tablet[1]+6] = 'a';
					changeTable[tablet[0]+1][tablet[1]+6] = true;
					changeRecord[tablet[0]+1][tablet[1]+7] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+7] = true;
					changeRecord[tablet[0]+1][tablet[1]+8] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+8] = true;
					changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+9] = true;
				}
				change = true;
				{ //zwalnianie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
			}
			while((coords[0] != goal[0] || coords[1] != goal[1]) && !*(assistant->endThread))
			{
				while(coords[1] != goal[1] && !*(assistant->endThread))
				{
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved);
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//zbliżanie się do celu w poziomie
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]] = true;
					if(coords[1] > goal[1]) coords[1]--;
					else coords[1]++;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);
				}
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//zbliżanie się do celu w pionie
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				if(coords[0] > goal[0]) coords[0]--;
				else coords[0]++;
				changeRecord[coords[0]][coords[1]] = ASSISTANT;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(300000);
			}
			if(flag && leading) //wskazane przez flagę muszą dogonić kolejny krok
			{
				if(coords[1] < wallPlacement[0][1] + 3) //przejście z lewej strony
				{
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//przejście asystenta o jeden krok wgłąb
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]--] = true;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);	 
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//przejście asystenta na lewą stronę
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]--] = true;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					goal[0] = used->coords[0] - 1;
					goal[1] = used->coords[1] - 1;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);	
				}
				else //przejście z prawej strony
				{
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//przejście asystenta o jeden krok wgłąb
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]++] = true;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);	 
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//przejście asystenta na prawą stronę
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]++] = true;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					goal[0] = used->coords[0] - 1;
					goal[1] = used->coords[1] + 1;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);	
				}
			}
			else if(flag)
			{
				if(coords[1] < wallPlacement[0][1]) //wejście z lewej strony
				{
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//wejście asystenta w próg
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]++] = true;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);	 
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//przejście asystenta przez drzwi
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]++] = true;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);
					goal[1] = workspacePlacement[space][1];
					if(workspacePlacement[space][0] > coords[0]) goal[0] = workspacePlacement[space][0] - 1;
					else goal[0] = workspacePlacement[space][0] + 1;
				}
				else //wejście z prawej strony
				{
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//wejście asystenta w próg
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]--] = true;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);	 
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//przejście asystenta przez drzwi
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]--] = true;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);
					goal[1] = workspacePlacement[space][1];
					if(workspacePlacement[space][0] > coords[0]) goal[0] = workspacePlacement[space][0] - 1;
					else goal[0] = workspacePlacement[space][0] + 1;
				}
			}
			if(flag) //część wspólna w gonieniu
			{	
				while((coords[0] != goal[0] || coords[1] != goal[1]) && !*(assistant->endThread))
				{
					while(coords[0] != goal[0] && !*(assistant->endThread))
					{
						{ //zajęcie rysowania
							ret = 1;
							while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
							if(*(assistant->endThread)) break;
							new = (el*)malloc(sizeof(el));
							new->id = assistant->id;
							new->next = (el*)NULL;
							if(lista.head == NULL)	lista.head = new;
							else 	lista.tail->next = new;
							lista.tail = new;
							pthread_mutex_unlock(enlist);
							while(lista.head != new) usleep(10000);
							ret = pthread_mutex_trylock(moved); 
							while (!*(assistant->endThread) && ret != 0) 
							{
								usleep(1000);
								ret = pthread_mutex_trylock(moved);
							}
							if(*(assistant->endThread))
							{ 
								free(new);
								if(!ret) pthread_mutex_unlock(moved);
								break;
							}
						}
						//zbliżenie się do auta w pionie
						changeRecord[coords[0]][coords[1]] = 0;
						changeTable[coords[0]][coords[1]] = true;
						if(coords[0]>goal[0]) coords[0]--;
						else coords[0]++;
						changeRecord[coords[0]][coords[1]] = ASSISTANT;
						changeTable[coords[0]][coords[1]] = true;
						change = true;
						{ //zwolnienie rysowania
							pthread_mutex_unlock(moved);
							pthread_cond_signal(drawn);
							ret = pthread_mutex_trylock(enlist);
							while(ret != 0) ret = pthread_mutex_trylock(enlist);
							lista.head = new->next;
							if(new == lista.tail) lista.tail = (el*)NULL;
							free(new);
							pthread_mutex_unlock(enlist);
						}
						usleep(300000);
					}
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//zbliżenie się do auta w poziomie
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]] = true;
					if(coords[1]>goal[1]) coords[1]--;
					else coords[1]++;
					changeRecord[coords[0]][coords[1]] = ASSISTANT;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);
				}
			}
			if(leading)
			{
				if(coords[0] > used->coords[0]) flag = false; //od tego momentu pomocnicza flaga do orientacji
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//zejście asystenta na poziom auta
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				if(flag) coords[0] ++;
				else coords[0]--;
				changeRecord[coords[0]][coords[1]] = ASSISTANT;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(300000);
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//'wejście' asystenta do auta
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				if(used->coords[1] < lpark) coords[1]++;
				else coords[1]--;
				changeRecord[coords[0]][coords[1]] = CAR+1;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);	 
				if(doormin > coords[0]) goal[0] = doormin + 1; //ustalenie kolejnego celu
				else goal[0] = doormax - 1;
				if(coords[1] < lpark) goal[1] = wallPlacement[0][1] - 2;
				else goal[1] = wallPlacement[1][1] + 2;
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//wyjechanie z miejsca parkingowego
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				if(flag) coords[0]++;
				else coords[0]--;
				changeRecord[coords[0]][coords[1]] = CAR+1;
				changeTable[coords[0]][coords[1]] = true;
				{//czyszczenie paska postępu
					changeRecord[tablet[0]][tablet[1]] = 0;
					changeTable[tablet[0]][tablet[1]] = true;
					changeRecord[tablet[0]][tablet[1]+1] = 0;
					changeTable[tablet[0]][tablet[1]+1] = true;
					changeRecord[tablet[0]][tablet[1]+2] = 0;
					changeTable[tablet[0]][tablet[1]+2] = true;
					changeRecord[tablet[0]][tablet[1]+3] = 0;
					changeTable[tablet[0]][tablet[1]+3] = true;
					changeRecord[tablet[0]][tablet[1]+4] = 0;
					changeTable[tablet[0]][tablet[1]+4] = true;
					changeRecord[tablet[0]][tablet[1]+5] = 0;
					changeTable[tablet[0]][tablet[1]+5] = true;
					changeRecord[tablet[0]][tablet[1]+6] = 0;
					changeTable[tablet[0]][tablet[1]+6] = true;
					changeRecord[tablet[0]][tablet[1]+7] = 0;
					changeTable[tablet[0]][tablet[1]+7] = true;
					changeRecord[tablet[0]][tablet[1]+8] = 0;
					changeTable[tablet[0]][tablet[1]+8] = true;
					changeRecord[tablet[0]][tablet[1]+9] = 0;
					changeTable[tablet[0]][tablet[1]+9] = true;
				}
				{//aktualizacja tablicy o informację co się dzieje
					changeRecord[tablet[0]+1][tablet[1]] = 'z';
					changeTable[tablet[0]+1][tablet[1]] = true;
					changeRecord[tablet[0]+1][tablet[1]+1] = 'a';
					changeTable[tablet[0]+1][tablet[1]+1] = true;
					changeRecord[tablet[0]+1][tablet[1]+2] = 'w';
					changeTable[tablet[0]+1][tablet[1]+2] = true;
					changeRecord[tablet[0]+1][tablet[1]+3] = 'o';
					changeTable[tablet[0]+1][tablet[1]+3] = true;
					changeRecord[tablet[0]+1][tablet[1]+4] = 'z';
					changeTable[tablet[0]+1][tablet[1]+4] = true;
					changeRecord[tablet[0]+1][tablet[1]+5] = 'i';
					changeTable[tablet[0]+1][tablet[1]+5] = true;
					changeRecord[tablet[0]+1][tablet[1]+6] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+6] = true;
					changeRecord[tablet[0]+1][tablet[1]+7] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+7] = true;
					changeRecord[tablet[0]+1][tablet[1]+8] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+8] = true;
					changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+9] = true;
				}
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
			}
			else
			{
				if(coords[0] > workspacePlacement[space][0]) flag = false;
				else flag = true;
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//'wejście' asystenta do auta
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				if(flag) coords[0]++;
				else coords[0]--;
				changeRecord[coords[0]][coords[1]] = CAR+1;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);	 
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//opuszczenie stołu warsztatowego
				changeRecord[coords[0]][coords[1]] = WORKSPACE;
				changeTable[coords[0]][coords[1]] = true;
				if(flag) coords[0]--;
				else coords[0]++;
				changeRecord[coords[0]][coords[1]] = CAR+1;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
				if(flag) goal[0] = doormax - 2;
				else goal[0] = doormin + 2;
				if(used->coords[1] < lpark) goal[1] = wallPlacement[0][1] + 2;
				else goal[1] = wallPlacement[1][1] - 2;
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//przejście dodatkowe pole bliżej środka, aby minimalizować kolizje
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				if(flag) coords[0]--;
				else coords[0]++;
				changeRecord[coords[0]][coords[1]] = CAR+1;
				changeTable[coords[0]][coords[1]] = true;
				{//czyszczenie paska postępu
					changeRecord[tablet[0]][tablet[1]] = 0;
					changeTable[tablet[0]][tablet[1]] = true;
					changeRecord[tablet[0]][tablet[1]+1] = 0;
					changeTable[tablet[0]][tablet[1]+1] = true;
					changeRecord[tablet[0]][tablet[1]+2] = 0;
					changeTable[tablet[0]][tablet[1]+2] = true;
					changeRecord[tablet[0]][tablet[1]+3] = 0;
					changeTable[tablet[0]][tablet[1]+3] = true;
					changeRecord[tablet[0]][tablet[1]+4] = 0;
					changeTable[tablet[0]][tablet[1]+4] = true;
					changeRecord[tablet[0]][tablet[1]+5] = 0;
					changeTable[tablet[0]][tablet[1]+5] = true;
					changeRecord[tablet[0]][tablet[1]+6] = 0;
					changeTable[tablet[0]][tablet[1]+6] = true;
					changeRecord[tablet[0]][tablet[1]+7] = 0;
					changeTable[tablet[0]][tablet[1]+7] = true;
					changeRecord[tablet[0]][tablet[1]+8] = 0;
					changeTable[tablet[0]][tablet[1]+8] = true;
					changeRecord[tablet[0]][tablet[1]+9] = 0;
					changeTable[tablet[0]][tablet[1]+9] = true;
				}
				{//aktualizacja tablicy o informację co się dzieje
					changeRecord[tablet[0]+1][tablet[1]] = 'o';
					changeTable[tablet[0]+1][tablet[1]] = true;
					changeRecord[tablet[0]+1][tablet[1]+1] = 'd';
					changeTable[tablet[0]+1][tablet[1]+1] = true;
					changeRecord[tablet[0]+1][tablet[1]+2] = 'w';
					changeTable[tablet[0]+1][tablet[1]+2] = true;
					changeRecord[tablet[0]+1][tablet[1]+3] = 'o';
					changeTable[tablet[0]+1][tablet[1]+3] = true;
					changeRecord[tablet[0]+1][tablet[1]+4] = 'z';
					changeTable[tablet[0]+1][tablet[1]+4] = true;
					changeRecord[tablet[0]+1][tablet[1]+5] = 'i';
					changeTable[tablet[0]+1][tablet[1]+5] = true;
					changeRecord[tablet[0]+1][tablet[1]+6] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+6] = true;
					changeRecord[tablet[0]+1][tablet[1]+7] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+7] = true;
					changeRecord[tablet[0]+1][tablet[1]+8] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+8] = true;
					changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
					changeTable[tablet[0]+1][tablet[1]+9] = true;
				}
				change = true;
				{ //zwalnianie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
			}
			while((coords[0] != goal[0] || coords[1] != goal[1]) && !*(assistant->endThread))
			{
				while(coords[1] != goal[1] && !*(assistant->endThread))
				{
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//poruszanie się autem w poziomie
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]] = true;
					if(coords[1] > goal[1]) coords[1]--;
					else coords[1]++;
					changeRecord[coords[0]][coords[1]] = CAR+1;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(100000);
				}
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//poruszanie się autem w pionie
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				if(coords[0] > goal[0]) coords[0]--;
				else coords[0]++;
				changeRecord[coords[0]][coords[1]] = CAR+1;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
			}
			if(leading)
			{
				if(coords[1] < wallPlacement[0][1]) flag = true;
				else flag = false;
			}
			else
			{
				if(used->coords[1] < wallPlacement[1][1]) flag = false;
				else flag = true;
			}
			//przechodzenie przez drzwi
			{ //zajęcie rysowania
				ret = 1;
				while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
				if(*(assistant->endThread)) break;
				new = (el*)malloc(sizeof(el));
				new->id = assistant->id;
				new->next = (el*)NULL;
				if(lista.head == NULL)	lista.head = new;
				else 	lista.tail->next = new;
				lista.tail = new;
				pthread_mutex_unlock(enlist);
				while(lista.head != new) usleep(10000);
				ret = pthread_mutex_trylock(moved); 
				while (!*(assistant->endThread) && ret != 0) 
				{
					usleep(1000);
					ret = pthread_mutex_trylock(moved);
				}
				if(*(assistant->endThread))
				{ 
					free(new);
					if(!ret) pthread_mutex_unlock(moved);
					break;
				}
			}
			changeRecord[coords[0]][coords[1]] = 0;
			changeTable[coords[0]][coords[1]] = true;
			if(flag) coords[1]++;
			else coords[1]--;
			changeRecord[coords[0]][coords[1]] = CAR + 1;
			changeTable[coords[0]][coords[1]] = true;
			{ //zwolnienie rysowania
				pthread_mutex_unlock(moved);
				pthread_cond_signal(drawn);
				ret = pthread_mutex_trylock(enlist);
				while(ret != 0) ret = pthread_mutex_trylock(enlist);
				lista.head = new->next;
				if(new == lista.tail) lista.tail = (el*)NULL;
				free(new);
				pthread_mutex_unlock(enlist);
			}
			usleep(100000);
			{ //zajęcie rysowania
				ret = 1;
				while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
				if(*(assistant->endThread)) break;
				new = (el*)malloc(sizeof(el));
				new->id = assistant->id;
				new->next = (el*)NULL;
				if(lista.head == NULL)	lista.head = new;
				else 	lista.tail->next = new;
				lista.tail = new;
				pthread_mutex_unlock(enlist);
				while(lista.head != new) usleep(10000);
				ret = pthread_mutex_trylock(moved); 
				while (!*(assistant->endThread) && ret != 0) 
				{
					usleep(1000);
					ret = pthread_mutex_trylock(moved);
				}
				if(*(assistant->endThread))
				{ 
					free(new);
					if(!ret) pthread_mutex_unlock(moved);
					break;
				}
			}
			changeRecord[coords[0]][coords[1]] = 0;
			changeTable[coords[0]][coords[1]] = true;
			if(flag) coords[1]++;
			else coords[1]--;
			changeRecord[coords[0]][coords[1]] = CAR + 1;
			changeTable[coords[0]][coords[1]] = true;
			{ //zwolnienie rysowania
				pthread_mutex_unlock(moved);
				pthread_cond_signal(drawn);
				ret = pthread_mutex_trylock(enlist);
				while(ret != 0) ret = pthread_mutex_trylock(enlist);
				lista.head = new->next;
				if(new == lista.tail) lista.tail = (el*)NULL;
				free(new);
				pthread_mutex_unlock(enlist);
			}
			usleep(100000);
			{ //zajęcie rysowania
				ret = 1;
				while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
				if(*(assistant->endThread)) break;
				new = (el*)malloc(sizeof(el));
				new->id = assistant->id;
				new->next = (el*)NULL;
				if(lista.head == NULL)	lista.head = new;
				else 	lista.tail->next = new;
				lista.tail = new;
				pthread_mutex_unlock(enlist);
				while(lista.head != new) usleep(10000);
				ret = pthread_mutex_trylock(moved); 
				while (!*(assistant->endThread) && ret != 0) 
				{
					usleep(1000);
					ret = pthread_mutex_trylock(moved);
				}
				if(*(assistant->endThread))
				{ 
					free(new);
					if(!ret) pthread_mutex_unlock(moved);
					break;
				}
			}
			changeRecord[coords[0]][coords[1]] = 0;
			changeTable[coords[0]][coords[1]] = true;
			if(flag) coords[1]++;
			else coords[1]--;
			changeRecord[coords[0]][coords[1]] = CAR + 1;
			changeTable[coords[0]][coords[1]] = true;
			{ //zwolnienie rysowania
				pthread_mutex_unlock(moved);
				pthread_cond_signal(drawn);
				ret = pthread_mutex_trylock(enlist);
				while(ret != 0) ret = pthread_mutex_trylock(enlist);
				lista.head = new->next;
				if(new == lista.tail) lista.tail = (el*)NULL;
				free(new);
				pthread_mutex_unlock(enlist);
			}
			usleep(100000);
			if(leading)
			{
				if(coords[0] < workspacePlacement[space][0]) goal[0] = workspacePlacement[space][0] - 1;
				else goal[0] = workspacePlacement[space][0] + 1;
				goal[1] = workspacePlacement[space][1];
			} 
			else
			{
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				changeRecord[coords[0]][coords[1]] = 0; //minimalizacja kolizji
				changeTable[coords[0]][coords[1]] = true;
				if(flag) coords[1]++;
				else coords[1]--;
				changeRecord[coords[0]][coords[1]] = CAR + 1;
				changeTable[coords[0]][coords[1]] = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				if(flag) coords[1]++;
				else coords[1]--;
				changeRecord[coords[0]][coords[1]] = CAR + 1;
				changeTable[coords[0]][coords[1]] = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
				goal[1] = used->coords[1];
				if(used->coords[0] < coords[0]) goal[0] = used->coords[0] + 1;
				else goal[0] = used->coords[0] - 1;
			}
			while((coords[0] != goal[0] || coords[1] != goal[1]) && !*(assistant->endThread))
			{
				while(coords[0] != goal[0] && !*(assistant->endThread))
				{
					{ //zajęcie rysowania
						ret = 1;
						while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
						if(*(assistant->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = assistant->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000);
						ret = pthread_mutex_trylock(moved); 
						while (!*(assistant->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(assistant->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//poruszanie się autem w pionie
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]] = true;
					if(coords[0] > goal[0]) coords[0]--;
					else coords[0]++;
					changeRecord[coords[0]][coords[1]] = CAR+1;
					changeTable[coords[0]][coords[1]] = true;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(100000);
				}
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//poruszanie się autem w poziomie
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				if(coords[1] > goal[1]) coords[1]--;
				else coords[1]++;
				changeRecord[coords[0]][coords[1]] = CAR+1;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
			}
			
			if(leading)
			{
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//zostawianie auta do naprawy
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				changeRecord[workspacePlacement[space][0]][coords[1]] = WORKSPACE + 3;
				changeTable[workspacePlacement[space][0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//'wysiadanie' asystenta
				changeRecord[coords[0]][coords[1]] = ASSISTANT;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				used->toRepair = true;
				used->station = space;
				leading = false;
			}
			else
			{
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//odstawienie auta na parking
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				coords[0] = used->coords[0];
				changeRecord[coords[0]][coords[1]] = CAR + 1;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
				{ //zajęcie rysowania
					ret = 1;
					while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist);
					if(*(assistant->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = assistant->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000);
					ret = pthread_mutex_trylock(moved); 
					while (!*(assistant->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(assistant->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//'wysiadanie' asystenta
				changeRecord[coords[0]][used->coords[1]] = CAR;
				changeTable[coords[0]][used->coords[1]] = true;
				if(used->coords[1] < lpark) coords[1]--;
				else coords[1]++;
				changeRecord[coords[0]][coords[1]] = ASSISTANT;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				returning[currCar] = false;
				do{ ret = pthread_mutex_trylock(&(work[space])); }while(ret && !*(assistant->endThread));
				if(!ret) 
				{
					occupied[space] = false;
					pthread_mutex_unlock(&(work[space]));
				}
			}
			pthread_mutex_unlock(&(carLock[currCar]));
			usleep(100000);
			driving = false;
			held = false;
		}
		else
		{
			{ //zajęcie rysowania
				ret = 1;
				while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
				if(*(assistant->endThread)) break;
				new = (el*)malloc(sizeof(el));
				new->id = assistant->id;
				new->next = (el*)NULL;
				if(lista.head == NULL)	lista.head = new;
				else 	lista.tail->next = new;
				lista.tail = new;
				pthread_mutex_unlock(enlist);
				while(lista.head != new) usleep(10000); 
				ret = pthread_mutex_trylock(moved); 
				while (!*(assistant->endThread) && ret != 0) 
				{
					usleep(1000);
					ret = pthread_mutex_trylock(moved);
				}
				if(*(assistant->endThread))
				{ 
					free(new);
					if(!ret) pthread_mutex_unlock(moved);
					break;
				}
			}
			{//czyszczenie paska postępu
				changeRecord[tablet[0]][tablet[1]] = 0;
				changeTable[tablet[0]][tablet[1]] = true;
				changeRecord[tablet[0]][tablet[1]+1] = 0;
				changeTable[tablet[0]][tablet[1]+1] = true;
				changeRecord[tablet[0]][tablet[1]+2] = 0;
				changeTable[tablet[0]][tablet[1]+2] = true;
				changeRecord[tablet[0]][tablet[1]+3] = 0;
				changeTable[tablet[0]][tablet[1]+3] = true;
				changeRecord[tablet[0]][tablet[1]+4] = 0;
				changeTable[tablet[0]][tablet[1]+4] = true;
				changeRecord[tablet[0]][tablet[1]+5] = 0;
				changeTable[tablet[0]][tablet[1]+5] = true;
				changeRecord[tablet[0]][tablet[1]+6] = 0;
				changeTable[tablet[0]][tablet[1]+6] = true;
				changeRecord[tablet[0]][tablet[1]+7] = 0;
				changeTable[tablet[0]][tablet[1]+7] = true;
				changeRecord[tablet[0]][tablet[1]+8] = 0;
				changeTable[tablet[0]][tablet[1]+8] = true;
				changeRecord[tablet[0]][tablet[1]+9] = 0;
				changeTable[tablet[0]][tablet[1]+9] = true;
			}
			{//aktualizacja tablicy o informację co się dzieje
				changeRecord[tablet[0]+1][tablet[1]] = 's';
				changeTable[tablet[0]+1][tablet[1]] = true;
				changeRecord[tablet[0]+1][tablet[1]+1] = 'z';
				changeTable[tablet[0]+1][tablet[1]+1] = true;
				changeRecord[tablet[0]+1][tablet[1]+2] = 'u';
				changeTable[tablet[0]+1][tablet[1]+2] = true;
				changeRecord[tablet[0]+1][tablet[1]+3] = 'k';
				changeTable[tablet[0]+1][tablet[1]+3] = true;
				changeRecord[tablet[0]+1][tablet[1]+4] = 'a';
				changeTable[tablet[0]+1][tablet[1]+4] = true;
				changeRecord[tablet[0]+1][tablet[1]+5] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+5] = true;
				changeRecord[tablet[0]+1][tablet[1]+6] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+6] = true;
				changeRecord[tablet[0]+1][tablet[1]+7] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+7] = true;
				changeRecord[tablet[0]+1][tablet[1]+8] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+8] = true;
				changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+9] = true;
			}
			change = true;
			{ //zwalnianie rysowania
				pthread_mutex_unlock(moved);
				pthread_cond_signal(drawn);
				ret = pthread_mutex_trylock(enlist);
				while(ret != 0) ret = pthread_mutex_trylock(enlist);
				lista.head = new->next;
				if(new == lista.tail) lista.tail = (el*)NULL;
				free(new);
				pthread_mutex_unlock(enlist);
			}
			usleep(100000);
			for(i = 0; i < cars; i++)
			{ //priorytet mają naprawione auta
				ret = pthread_mutex_trylock(&(carLock[i]));
				if(!ret)
				{
					if(returning[i])
					{
						currCar = i;
						used = &(carPlacement[i]);
						space = used->station;
						driving = true;
						held = true; //dla przerwania tej części (break tylko na jednym poziomie działa)
						{ //zajęcie rysowania
							ret = 1;
							while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
							if(*(assistant->endThread)) break;
							new = (el*)malloc(sizeof(el));
							new->id = assistant->id;
							new->next = (el*)NULL;
							if(lista.head == NULL)	lista.head = new;
							else 	lista.tail->next = new;
							lista.tail = new;
							pthread_mutex_unlock(enlist);
							while(lista.head != new) usleep(10000); 
							ret = pthread_mutex_trylock(moved); 
							while (!*(assistant->endThread) && ret != 0) 
							{
								usleep(1000);
								ret = pthread_mutex_trylock(moved);
							}
							if(*(assistant->endThread))
							{ 
								free(new);
								if(!ret) pthread_mutex_unlock(moved);
								break;
							}
						}
						{//ustawianie paska postępu
							changeRecord[tablet[0]][tablet[1]] = 8;
							changeTable[tablet[0]][tablet[1]] = true;
							changeRecord[tablet[0]][tablet[1]+1] = 8;
							changeTable[tablet[0]][tablet[1]+1] = true;
							changeRecord[tablet[0]][tablet[1]+2] = 8;
							changeTable[tablet[0]][tablet[1]+2] = true;
							changeRecord[tablet[0]][tablet[1]+3] = 8;
							changeTable[tablet[0]][tablet[1]+3] = true;
							changeRecord[tablet[0]][tablet[1]+4] = 8;
							changeTable[tablet[0]][tablet[1]+4] = true;
							changeRecord[tablet[0]][tablet[1]+5] = 8;
							changeTable[tablet[0]][tablet[1]+5] = true;
							changeRecord[tablet[0]][tablet[1]+6] = 8;
							changeTable[tablet[0]][tablet[1]+6] = true;
							changeRecord[tablet[0]][tablet[1]+7] = 8;
							changeTable[tablet[0]][tablet[1]+7] = true;
							changeRecord[tablet[0]][tablet[1]+8] = 8;
							changeTable[tablet[0]][tablet[1]+8] = true;
							changeRecord[tablet[0]][tablet[1]+9] = 8;
							changeTable[tablet[0]][tablet[1]+9] = true;
						}
						change = true;
						{ //zwalnianie rysowania
							pthread_mutex_unlock(moved);
							pthread_cond_signal(drawn);
							ret = pthread_mutex_trylock(enlist);
							while(ret != 0) ret = pthread_mutex_trylock(enlist);
							lista.head = new->next;
							if(new == lista.tail) lista.tail = (el*)NULL;
							free(new);
							pthread_mutex_unlock(enlist);
						}
						usleep(100000);
						break;
					}
					else pthread_mutex_unlock(&(carLock[i]));
				}
			}
			if(!held)
			{
				i = lastCar; //aby każde auto miało szansę być naprawione
				do{
					i = (i + 1) % cars;
					ret = pthread_mutex_trylock(&(carLock[i]));
					if(!ret)
					{
						if(returning[i]) continue;
						currCar = i;
						for(i=0;i<workspaces;i++)
						{
							ret = pthread_mutex_trylock(&(work[i]));
							if(!ret) continue;
							if(occupied[i])
							{
								pthread_mutex_unlock(&(work[i]));
								continue;
							}
							occupied[i] = true;
							pthread_mutex_unlock(&(work[i]));
							driving = true;
							used = &(carPlacement[currCar]);
							space = i;
							lastCar = currCar;
							leading = true;
							held = true;
							{ //zajęcie rysowania
								ret = 1;
								while(!*(assistant->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
								if(*(assistant->endThread)) break;
								new = (el*)malloc(sizeof(el));
								new->id = assistant->id;
								new->next = (el*)NULL;
								if(lista.head == NULL)	lista.head = new;
								else 	lista.tail->next = new;
								lista.tail = new;
								pthread_mutex_unlock(enlist);
								while(lista.head != new) usleep(10000); 
								ret = pthread_mutex_trylock(moved); 
								while (!*(assistant->endThread) && ret != 0) 
								{
									usleep(1000);
									ret = pthread_mutex_trylock(moved);
								}
								if(*(assistant->endThread))
								{ 
									free(new);
									if(!ret) pthread_mutex_unlock(moved);
									break;
								}
							}
							{//ustawianie paska postępu
								changeRecord[tablet[0]][tablet[1]+5] = 8;
								changeTable[tablet[0]][tablet[1]+5] = true;
								changeRecord[tablet[0]][tablet[1]+6] = 8;
								changeTable[tablet[0]][tablet[1]+6] = true;
								changeRecord[tablet[0]][tablet[1]+7] = 8;
								changeTable[tablet[0]][tablet[1]+7] = true;
								changeRecord[tablet[0]][tablet[1]+8] = 8;
								changeTable[tablet[0]][tablet[1]+8] = true;
								changeRecord[tablet[0]][tablet[1]+9] = 8;
								changeTable[tablet[0]][tablet[1]+9] = true;
							}
							change = true;
							{ //zwalnianie rysowania
								pthread_mutex_unlock(moved);
								pthread_cond_signal(drawn);
								ret = pthread_mutex_trylock(enlist);
								while(ret != 0) ret = pthread_mutex_trylock(enlist);
								lista.head = new->next;
								if(new == lista.tail) lista.tail = (el*)NULL;
								free(new);
								pthread_mutex_unlock(enlist);
							}
							usleep(100000);
							break;
						}
						if(!ret) pthread_mutex_unlock(&(carLock[currCar]));
						break; //jesli zaden stol warstatowy nie jest dostepny, to kolejne auta nie pomoga
					}
				}while(!*(assistant->endThread) && i!=lastCar);
			}	
		}
	}
	if(held) pthread_mutex_unlock(&(carLock[currCar]));
	free(coords);
	free(assistant->luck);
	free(pointer);
}

void *mech(void* pointer)
{
	person* mechanic = (person*)pointer;
	el* new, *order;
	int coords[2]; //obecna pozycja, w kontraście ze startową
	int tablet[2];
	int steps, control, grade, step; //do kontroli wypełniania paska
    coords[0] = mechanic->coords[0];
	coords[1] = mechanic->coords[1];
	tablet[0] = mechanic->table[0];
	tablet[1] = mechanic->table[1];
	int loops, toolNum, i, j, k, ret, temp, lastCar = 0;
	int toolUsed[TOOLNUM];
	bool repairing = false;
	bool *toolReserved = (bool*)calloc(TOOLNUM, sizeof(bool));
	rand_r(mechanic->luck);
	while(!*(mechanic->endThread))
	{
		if(repairing)
		{
			{ //zajęcie rysowania
				ret = 1; //kod do rysowania jest taki sam jak dla obu wątków, uwagi dla assist odnoszą się też tu
				while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
				if(*(mechanic->endThread)) break;
				new = (el*)malloc(sizeof(el));
				new->id = mechanic->id;
				new->next = (el*)NULL;
				if(lista.head == NULL)	lista.head = new;
				else 	lista.tail->next = new;
				lista.tail = new;
				pthread_mutex_unlock(enlist);
				while(lista.head != new) usleep(10000); 
				ret = pthread_mutex_trylock(moved); 
				while (!*(mechanic->endThread) && ret != 0) 
				{
					usleep(1000);
					ret = pthread_mutex_trylock(moved);
				}
				if(*(mechanic->endThread))
				{ 
					free(new);
					if(!ret) pthread_mutex_unlock(moved);
					break;
				}
			}
			{//czyszczenie paska postępu
				changeRecord[tablet[0]][tablet[1]] = 0;
				changeTable[tablet[0]][tablet[1]] = true;
				changeRecord[tablet[0]][tablet[1]+1] = 0;
				changeTable[tablet[0]][tablet[1]+1] = true;
				changeRecord[tablet[0]][tablet[1]+2] = 0;
				changeTable[tablet[0]][tablet[1]+2] = true;
				changeRecord[tablet[0]][tablet[1]+3] = 0;
				changeTable[tablet[0]][tablet[1]+3] = true;
				changeRecord[tablet[0]][tablet[1]+4] = 0;
				changeTable[tablet[0]][tablet[1]+4] = true;
				changeRecord[tablet[0]][tablet[1]+5] = 0;
				changeTable[tablet[0]][tablet[1]+5] = true;
				changeRecord[tablet[0]][tablet[1]+6] = 0;
				changeTable[tablet[0]][tablet[1]+6] = true;
				changeRecord[tablet[0]][tablet[1]+7] = 0;
				changeTable[tablet[0]][tablet[1]+7] = true;
				changeRecord[tablet[0]][tablet[1]+8] = 0;
				changeTable[tablet[0]][tablet[1]+8] = true;
				changeRecord[tablet[0]][tablet[1]+9] = 0;
				changeTable[tablet[0]][tablet[1]+9] = true;
			}
			{//aktualizacja tablicy o informację co się dzieje
				changeRecord[tablet[0]+1][tablet[1]] = 'N';
				changeTable[tablet[0]+1][tablet[1]] = true;
				changeRecord[tablet[0]+1][tablet[1]+1] = 'A';
				changeTable[tablet[0]+1][tablet[1]+1] = true;
				changeRecord[tablet[0]+1][tablet[1]+2] = 'R';
				changeTable[tablet[0]+1][tablet[1]+2] = true;
				changeRecord[tablet[0]+1][tablet[1]+3] = 'Z';
				changeTable[tablet[0]+1][tablet[1]+3] = true;
				changeRecord[tablet[0]+1][tablet[1]+4] = 'E';
				changeTable[tablet[0]+1][tablet[1]+4] = true;
				changeRecord[tablet[0]+1][tablet[1]+5] = 'D';
				changeTable[tablet[0]+1][tablet[1]+5] = true;
				changeRecord[tablet[0]+1][tablet[1]+6] = 'I';
				changeTable[tablet[0]+1][tablet[1]+6] = true;
				changeRecord[tablet[0]+1][tablet[1]+7] = 'A';
				changeTable[tablet[0]+1][tablet[1]+7] = true;
				changeRecord[tablet[0]+1][tablet[1]+8] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+8] = true;
				changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+9] = true;
			}
			change = true;
			{ //zwalnianie rysowania
				pthread_mutex_unlock(moved);
				pthread_cond_signal(drawn);
				ret = pthread_mutex_trylock(enlist);
				while(ret != 0) ret = pthread_mutex_trylock(enlist);
				lista.head = new->next;
				if(new == lista.tail) lista.tail = (el*)NULL;
				free(new);
				pthread_mutex_unlock(enlist);
			}
			usleep(100000);
			steps = toolNum/10;
			grade = 1;
			if(steps == 0) 
			{
				steps = toolNum;
				grade = 10/steps; 
			}
			steps--;
			control = 0;
			step = 0;
			for(i=0; i < toolNum; i++)
			{
				ret = 1;
				j = 0; //próba zajęcia narzędzi przez określoną liczbę czasu
				while(ret != 0 && j++ < loops && !*(mechanic->endThread))
				{
					ret = pthread_mutex_trylock(&(tools[toolUsed[i]]));
					usleep(50000);
				}
				if(ret != 0 || *(mechanic->endThread)) break;
				{ //zajęcie rysowania
					ret = 1;
					while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(mechanic->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = mechanic->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(mechanic->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(mechanic->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//zajęcie narzędzi
				temp = (mechanic->id - assistThreads)/10;
				if(temp) //jeśli id mechanika jest dwucyfrowe
				{
					changeRecord[toolUsed[i]][col-4] = temp + 10;
					changeTable[toolUsed[i]][col-4] = true;
				}
				changeRecord[toolUsed[i]][col-3] = (mechanic->id - assistThreads)%10 + 10;
				changeTable[toolUsed[i]][col-3] = true;
				for(k = 0; k<grade; k++)
				{
					if(step+k>9) break;
					changeRecord[tablet[0]][tablet[1]+step+k] = 8;
					changeTable[tablet[0]][tablet[1]+step+k] = true;
				}
				if((control++)%steps == 0) step++;
				change = true;
				{ //zwalnianie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
			}
			if(i!=toolNum || *(mechanic->endThread)) //problem z zarezerwowaniem narzędzi
			{
				for(i; i > -1; i--)
				{	
					{ //zajęcie rysowania
						ret = 1;
						while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
						if(*(mechanic->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = mechanic->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000); 
						ret = pthread_mutex_trylock(moved); 
						while (!*(mechanic->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(mechanic->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//zwolnienie narzędzi
					pthread_mutex_unlock(&(tools[toolUsed[i]]));
					changeRecord[toolUsed[i]][col-4] = 0;
					changeTable[toolUsed[i]][col-4] = true;
					changeRecord[toolUsed[i]][col-3] = 0;
					changeTable[toolUsed[i]][col-3] = true;
					for(k = 0; k<grade; k++)
					{
						if(step*grade+k < 0 || step*grade+k > 0) break;
						changeRecord[tablet[0]][tablet[1]+step*grade+k] = 0;
						changeTable[tablet[0]][tablet[1]+step*grade+k] = true;
					}
					if((control--)%steps == 0) step--;
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(100000);
				}
				if(*(mechanic->endThread)) continue;
				//dodanie do kolejki (jeśli jest w stanie bez, to pozwalam, inaczej procesy nie byłyby współbieżne)
				order = (el*)malloc(sizeof(el));
				order->id = mechanic->id;
				order->next = (el*)NULL;
				do{	ret = pthread_mutex_trylock(FIFO); }while(ret && !*(mechanic->endThread));
				if(*(mechanic->endThread))
				{
					free(order);
					if(!ret) pthread_mutex_unlock(FIFO);
					continue;
				}
				if(naprawa.head == NULL) naprawa.head = order;
				else naprawa.tail->next = order;
				naprawa.tail = order;
				pthread_mutex_unlock(FIFO);
				while(naprawa.head != order && !*(mechanic->endThread)) usleep(400000);
				if(*(mechanic->endThread))
				{
					free(order);
					continue;
				}
				for(i=0; i < toolNum; i++)
				{
					ret = 1;
					while(ret && !*(mechanic->endThread))
					{
						ret = pthread_mutex_trylock(&(tools[toolUsed[i]]));
						usleep(50000);
					}
					if(*(mechanic->endThread)) break;
					{ //zajęcie rysowania
						ret = 1;
						while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
						if(*(mechanic->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = mechanic->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000); 
						ret = pthread_mutex_trylock(moved); 
						while (!*(mechanic->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(mechanic->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//zajęcie narzędzi
					temp = (mechanic->id - assistThreads)/10;
					if(temp)
					{
						changeRecord[toolUsed[i]][col-4] = temp + 10;
						changeTable[toolUsed[i]][col-4] = true;
					}
					changeRecord[toolUsed[i]][col-3] = (mechanic->id - assistThreads)%10 + 10;
					changeTable[toolUsed[i]][col-3] = true;
					for(k = 0; k<grade; k++)
					{
						if(step*grade+k>9 || step*grade+k<0) break;
						changeRecord[tablet[0]][tablet[1]+step*grade+k] = 8;
						changeTable[tablet[0]][tablet[1]+step*grade+k] = true;
					}
					if((control++)%steps == 0) step++;
					change = true;
					{ //zwalnianie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(100000);
				}
				do{	ret = pthread_mutex_trylock(FIFO); }while(ret && !*(mechanic->endThread));
				if(*(mechanic->endThread))
				{
					free(order);
					if(!ret) pthread_mutex_unlock(FIFO);
					for(i; i > -1; i--)
					{	
						{ //zajęcie rysowania
							ret = 1;
							while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
							if(*(mechanic->endThread)) break;
							new = (el*)malloc(sizeof(el));
							new->id = mechanic->id;
							new->next = (el*)NULL;
							if(lista.head == NULL)	lista.head = new;
							else 	lista.tail->next = new;
							lista.tail = new;
							pthread_mutex_unlock(enlist);
							while(lista.head != new) usleep(10000); 
							ret = pthread_mutex_trylock(moved); 
							while (!*(mechanic->endThread) && ret != 0) 
							{
								usleep(1000);
								ret = pthread_mutex_trylock(moved);
							}
							if(*(mechanic->endThread))
							{ 
								free(new);
								if(!ret) pthread_mutex_unlock(moved);
								break;
							}
						}
						//zwalnianie narzędzi
						pthread_mutex_unlock(&(tools[toolUsed[i]]));
						changeRecord[toolUsed[i]][col-4] = 0;
						changeTable[toolUsed[i]][col-4] = true;
						changeRecord[toolUsed[i]][col-3] = 0;
						changeTable[toolUsed[i]][col-3] = true;
						change = true;
						{ //zwalnianie rysowania
							pthread_mutex_unlock(moved);
							pthread_cond_signal(drawn);
							ret = pthread_mutex_trylock(enlist);
							while(ret != 0) ret = pthread_mutex_trylock(enlist);
							lista.head = new->next;
							if(new == lista.tail) lista.tail = (el*)NULL;
							free(new);
							pthread_mutex_unlock(enlist);
						}
						usleep(100000);
					}
					continue;
				}
				if(naprawa.tail == order) naprawa.tail = (el*)NULL;
				naprawa.head = order->next;
				pthread_mutex_unlock(FIFO);
				free(order);
			}
			temp = carPlacement[lastCar].station;
			{ //zajęcie rysowania
				ret = 1;
				while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
				if(*(mechanic->endThread)) break;
				new = (el*)malloc(sizeof(el));
				new->id = mechanic->id;
				new->next = (el*)NULL;
				if(lista.head == NULL)	lista.head = new;
				else 	lista.tail->next = new;
				lista.tail = new;
				pthread_mutex_unlock(enlist);
				while(lista.head != new) usleep(10000); 
				ret = pthread_mutex_trylock(moved); 
				while (!*(mechanic->endThread) && ret != 0) 
				{
					usleep(1000);
					ret = pthread_mutex_trylock(moved);
				}
				if(*(mechanic->endThread))
				{ 
					free(new);
					if(!ret) pthread_mutex_unlock(moved);
					break;
				}
			}
			{//czyszczenie paska postępu
				changeRecord[tablet[0]][tablet[1]] = 0;
				changeTable[tablet[0]][tablet[1]] = true;
				changeRecord[tablet[0]][tablet[1]+1] = 0;
				changeTable[tablet[0]][tablet[1]+1] = true;
				changeRecord[tablet[0]][tablet[1]+2] = 0;
				changeTable[tablet[0]][tablet[1]+2] = true;
				changeRecord[tablet[0]][tablet[1]+3] = 0;
				changeTable[tablet[0]][tablet[1]+3] = true;
				changeRecord[tablet[0]][tablet[1]+4] = 0;
				changeTable[tablet[0]][tablet[1]+4] = true;
				changeRecord[tablet[0]][tablet[1]+5] = 0;
				changeTable[tablet[0]][tablet[1]+5] = true;
				changeRecord[tablet[0]][tablet[1]+6] = 0;
				changeTable[tablet[0]][tablet[1]+6] = true;
				changeRecord[tablet[0]][tablet[1]+7] = 0;
				changeTable[tablet[0]][tablet[1]+7] = true;
				changeRecord[tablet[0]][tablet[1]+8] = 0;
				changeTable[tablet[0]][tablet[1]+8] = true;
				changeRecord[tablet[0]][tablet[1]+9] = 0;
				changeTable[tablet[0]][tablet[1]+9] = true;
			}
			{//aktualizacja tablicy o informację co się dzieje
				changeRecord[tablet[0]+1][tablet[1]] = 'N';
				changeTable[tablet[0]+1][tablet[1]] = true;
				changeRecord[tablet[0]+1][tablet[1]+1] = 'A';
				changeTable[tablet[0]+1][tablet[1]+1] = true;
				changeRecord[tablet[0]+1][tablet[1]+2] = 'P';
				changeTable[tablet[0]+1][tablet[1]+2] = true;
				changeRecord[tablet[0]+1][tablet[1]+3] = 'R';
				changeTable[tablet[0]+1][tablet[1]+3] = true;
				changeRecord[tablet[0]+1][tablet[1]+4] = 'A';
				changeTable[tablet[0]+1][tablet[1]+4] = true;
				changeRecord[tablet[0]+1][tablet[1]+5] = 'W';
				changeTable[tablet[0]+1][tablet[1]+5] = true;
				changeRecord[tablet[0]+1][tablet[1]+6] = 'I';
				changeTable[tablet[0]+1][tablet[1]+6] = true;
				changeRecord[tablet[0]+1][tablet[1]+7] = 'A';
				changeTable[tablet[0]+1][tablet[1]+7] = true;
				changeRecord[tablet[0]+1][tablet[1]+8] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+8] = true;
				changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+9] = true;
			}
			change = true;
			{ //zwalnianie rysowania
				pthread_mutex_unlock(moved);
				pthread_cond_signal(drawn);
				ret = pthread_mutex_trylock(enlist);
				while(ret != 0) ret = pthread_mutex_trylock(enlist);
				lista.head = new->next;
				if(new == lista.tail) lista.tail = (el*)NULL;
				free(new);
				pthread_mutex_unlock(enlist);
			}
			usleep(100000);
			steps = loops/10;
			grade = 1;
			if(steps == 0) 
			{
				steps = loops;
				grade = 10/steps; 
			}
			if(steps*grade>loops) steps++;
			control = 0;
			step = 0;
			j = 0;
			while(j++ < loops) //naprawa trwająca określoną ilość pętli
			{
				{ //zajęcie rysowania
					ret = 1;
					while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(mechanic->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = mechanic->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(mechanic->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(mechanic->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//naprawa w toku
				changeRecord[workspacePlacement[temp][0]][workspacePlacement[temp][1]] = WORKSPACE + 6;
				changeTable[workspacePlacement[temp][0]][workspacePlacement[temp][1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(150000);
					
				{ //zajęcie rysowania
					ret = 1;
					while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(mechanic->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = mechanic->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(mechanic->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(mechanic->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//auto w stacji naprawczej (również do iluzji pracowania nad autem)
				changeRecord[workspacePlacement[temp][0]][workspacePlacement[temp][1]] = WORKSPACE + 3;
				changeTable[workspacePlacement[temp][0]][workspacePlacement[temp][1]] = true;
				for(k = 0; k<grade; k++)
				{
					if(step*grade+k>9 || step*grade+k<0) break;
					changeRecord[tablet[0]][tablet[1]+step*grade+k] = 8;
					changeTable[tablet[0]][tablet[1]+step*grade+k] = true;
				}
				if((control++)%steps == 0) step++;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(150000);
			}
			for(i = toolNum - 1; i > -1; i--)
			{	
				{ //zajęcie rysowania
					ret = 1;
					while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(mechanic->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = mechanic->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(mechanic->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(mechanic->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//zwalnianie narzędzi
				pthread_mutex_unlock(&(tools[toolUsed[i]]));
				changeRecord[toolUsed[i]][col-4] = 0;
				changeTable[toolUsed[i]][col-4] = true;
				changeRecord[toolUsed[i]][col-3] = 0;
				changeTable[toolUsed[i]][col-3] = true;
				change = true;
				{ //zwalnianie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(100000);
			}
			(carPlacement[lastCar]).toRepair = false;
			returning[lastCar] = true;
			repairing = false;
			pthread_mutex_unlock(&(carLock[lastCar]));
			{ //zajęcie rysowania
				ret = 1; //kod do rysowania jest taki sam jak dla obu wątków, uwagi dla assist odnoszą się też tu
				while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
				if(*(mechanic->endThread)) break;
				new = (el*)malloc(sizeof(el));
				new->id = mechanic->id;
				new->next = (el*)NULL;
				if(lista.head == NULL)	lista.head = new;
				else 	lista.tail->next = new;
				lista.tail = new;
				pthread_mutex_unlock(enlist);
				while(lista.head != new) usleep(10000); 
				ret = pthread_mutex_trylock(moved); 
				while (!*(mechanic->endThread) && ret != 0) 
				{
					usleep(1000);
					ret = pthread_mutex_trylock(moved);
				}
				if(*(mechanic->endThread))
				{ 
					free(new);
					if(!ret) pthread_mutex_unlock(moved);
					break;
				}
			}
			{//czyszczenie paska postępu
				changeRecord[tablet[0]][tablet[1]] = 0;
				changeTable[tablet[0]][tablet[1]] = true;
				changeRecord[tablet[0]][tablet[1]+1] = 0;
				changeTable[tablet[0]][tablet[1]+1] = true;
				changeRecord[tablet[0]][tablet[1]+2] = 0;
				changeTable[tablet[0]][tablet[1]+2] = true;
				changeRecord[tablet[0]][tablet[1]+3] = 0;
				changeTable[tablet[0]][tablet[1]+3] = true;
				changeRecord[tablet[0]][tablet[1]+4] = 0;
				changeTable[tablet[0]][tablet[1]+4] = true;
				changeRecord[tablet[0]][tablet[1]+5] = 0;
				changeTable[tablet[0]][tablet[1]+5] = true;
				changeRecord[tablet[0]][tablet[1]+6] = 0;
				changeTable[tablet[0]][tablet[1]+6] = true;
				changeRecord[tablet[0]][tablet[1]+7] = 0;
				changeTable[tablet[0]][tablet[1]+7] = true;
				changeRecord[tablet[0]][tablet[1]+8] = 0;
				changeTable[tablet[0]][tablet[1]+8] = true;
				changeRecord[tablet[0]][tablet[1]+9] = 0;
				changeTable[tablet[0]][tablet[1]+9] = true;
			}
			{//aktualizacja tablicy o informację co się dzieje
				changeRecord[tablet[0]+1][tablet[1]] = 'W';
				changeTable[tablet[0]+1][tablet[1]] = true;
				changeRecord[tablet[0]+1][tablet[1]+1] = 'R';
				changeTable[tablet[0]+1][tablet[1]+1] = true;
				changeRecord[tablet[0]+1][tablet[1]+2] = 'A';
				changeTable[tablet[0]+1][tablet[1]+2] = true;
				changeRecord[tablet[0]+1][tablet[1]+3] = 'C';
				changeTable[tablet[0]+1][tablet[1]+3] = true;
				changeRecord[tablet[0]+1][tablet[1]+4] = 'A';
				changeTable[tablet[0]+1][tablet[1]+4] = true;
				changeRecord[tablet[0]+1][tablet[1]+5] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+5] = true;
				changeRecord[tablet[0]+1][tablet[1]+6] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+6] = true;
				changeRecord[tablet[0]+1][tablet[1]+7] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+7] = true;
				changeRecord[tablet[0]+1][tablet[1]+8] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+8] = true;
				changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+9] = true;
			}
			change = true;
			{ //zwalnianie rysowania
				pthread_mutex_unlock(moved);
				pthread_cond_signal(drawn);
				ret = pthread_mutex_trylock(enlist);
				while(ret != 0) ret = pthread_mutex_trylock(enlist);
				lista.head = new->next;
				if(new == lista.tail) lista.tail = (el*)NULL;
				free(new);
				pthread_mutex_unlock(enlist);
			}
			usleep(100000);
			while((coords[1] != mechanic->coords[1] || coords[0] != mechanic->coords[0]) && !*(mechanic->endThread))
			{ //powrót na miejsce startowe
				while(coords[0] != mechanic->coords[0] && !*(mechanic->endThread)) 
				{
					{ //zajęcie rysowania
						ret = 1;
						while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
						if(*(mechanic->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = mechanic->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000); 
						ret = pthread_mutex_trylock(moved); 
						while (!*(mechanic->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(mechanic->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					//wracanie do pozycji startowej w pionie
					changeRecord[coords[0]][coords[1]] = 0;
					changeTable[coords[0]][coords[1]] = true;
					if(coords[0]>mechanic->coords[0]) coords[0]--;
					else coords[0]++;
					changeRecord[coords[0]][coords[1]] = MECHANIC;
					changeTable[coords[0]][coords[1]] = true;				
					change = true;
					{ //zwolnienie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(300000);
				}
				{ //zajęcie rysowania
					ret = 1;
					while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(mechanic->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = mechanic->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(mechanic->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(mechanic->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//wracanie do pozycji startowej w poziomie
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]] = true;
				if(coords[1]>mechanic->coords[1]) coords[1]--;
				else coords[1]++;
				changeRecord[coords[0]][coords[1]] = MECHANIC;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(300000);
			}
		}
		else //gdy nie naprawia
		{
			{ //zajęcie rysowania
				ret = 1; //kod do rysowania jest taki sam jak dla obu wątków, uwagi dla assist odnoszą się też tu
				while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
				if(*(mechanic->endThread)) break;
				new = (el*)malloc(sizeof(el));
				new->id = mechanic->id;
				new->next = (el*)NULL;
				if(lista.head == NULL)	lista.head = new;
				else 	lista.tail->next = new;
				lista.tail = new;
				pthread_mutex_unlock(enlist);
				while(lista.head != new) usleep(10000); 
				ret = pthread_mutex_trylock(moved); 
				while (!*(mechanic->endThread) && ret != 0) 
				{
					usleep(1000);
					ret = pthread_mutex_trylock(moved);
				}
				if(*(mechanic->endThread))
				{ 
					free(new);
					if(!ret) pthread_mutex_unlock(moved);
					break;
				}
			}
			{//czyszczenie paska postępu
				changeRecord[tablet[0]][tablet[1]] = 0;
				changeTable[tablet[0]][tablet[1]] = true;
				changeRecord[tablet[0]][tablet[1]+1] = 0;
				changeTable[tablet[0]][tablet[1]+1] = true;
				changeRecord[tablet[0]][tablet[1]+2] = 0;
				changeTable[tablet[0]][tablet[1]+2] = true;
				changeRecord[tablet[0]][tablet[1]+3] = 0;
				changeTable[tablet[0]][tablet[1]+3] = true;
				changeRecord[tablet[0]][tablet[1]+4] = 0;
				changeTable[tablet[0]][tablet[1]+4] = true;
				changeRecord[tablet[0]][tablet[1]+5] = 0;
				changeTable[tablet[0]][tablet[1]+5] = true;
				changeRecord[tablet[0]][tablet[1]+6] = 0;
				changeTable[tablet[0]][tablet[1]+6] = true;
				changeRecord[tablet[0]][tablet[1]+7] = 0;
				changeTable[tablet[0]][tablet[1]+7] = true;
				changeRecord[tablet[0]][tablet[1]+8] = 0;
				changeTable[tablet[0]][tablet[1]+8] = true;
				changeRecord[tablet[0]][tablet[1]+9] = 0;
				changeTable[tablet[0]][tablet[1]+9] = true;
			}
			{//aktualizacja tablicy o informację co się dzieje
				changeRecord[tablet[0]+1][tablet[1]] = 'K';
				changeTable[tablet[0]+1][tablet[1]] = true;
				changeRecord[tablet[0]+1][tablet[1]+1] = 'R';
				changeTable[tablet[0]+1][tablet[1]+1] = true;
				changeRecord[tablet[0]+1][tablet[1]+2] = 'E';
				changeTable[tablet[0]+1][tablet[1]+2] = true;
				changeRecord[tablet[0]+1][tablet[1]+3] = 'C';
				changeTable[tablet[0]+1][tablet[1]+3] = true;
				changeRecord[tablet[0]+1][tablet[1]+4] = 'I';
				changeTable[tablet[0]+1][tablet[1]+4] = true;
				changeRecord[tablet[0]+1][tablet[1]+5] = 0;
				changeTable[tablet[0]+1][tablet[1]+5] = true;
				changeRecord[tablet[0]+1][tablet[1]+6] = 'S';
				changeTable[tablet[0]+1][tablet[1]+6] = true;
				changeRecord[tablet[0]+1][tablet[1]+7] = 'I';
				changeTable[tablet[0]+1][tablet[1]+7] = true;
				changeRecord[tablet[0]+1][tablet[1]+8] = 'E';
				changeTable[tablet[0]+1][tablet[1]+8] = true;
				changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
				changeTable[tablet[0]+1][tablet[1]+9] = true;
			}
			change = true;
			{ //zwalnianie rysowania
				pthread_mutex_unlock(moved);
				pthread_cond_signal(drawn);
				ret = pthread_mutex_trylock(enlist);
				while(ret != 0) ret = pthread_mutex_trylock(enlist);
				lista.head = new->next;
				if(new == lista.tail) lista.tail = (el*)NULL;
				free(new);
				pthread_mutex_unlock(enlist);
			}
			usleep(100000);
			loops = rand_r(mechanic->luck)%5+4;	
			steps = loops/10;
			grade = 1;
			if(steps == 0) 
			{
				steps = loops;
				grade = 10/steps; 
			}
			steps--;
			control = 0;
			step = 0;		
			i = 0;
			while(i++<loops && !*(mechanic->endThread)) //chodzenie w koło, odpoczynek
			{
				{ //zajęcie rysowania
					ret = 1;
					while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(mechanic->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = mechanic->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(mechanic->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(mechanic->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//pierwszy krok chodzenia w koło
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]++] = true;
				changeRecord[coords[0]][coords[1]] = MECHANIC;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(300000);
				
				{ //zajęcie rysowania
					ret = 1;
					while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(mechanic->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = mechanic->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(mechanic->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(mechanic->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//drugi krok chodzenia w koło
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]++][coords[1]] = true;
				changeRecord[coords[0]][coords[1]] = MECHANIC;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(300000);
				
				{ //zajęcie rysowania
					ret = 1;
					while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(mechanic->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = mechanic->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(mechanic->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(mechanic->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//trzeci krok chodzenia w koło
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]][coords[1]--] = true;
				changeRecord[coords[0]][coords[1]] = MECHANIC;
				changeTable[coords[0]][coords[1]] = true;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(300000);
				
				{ //zajęcie rysowania
					ret = 1;
					while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
					if(*(mechanic->endThread)) break;
					new = (el*)malloc(sizeof(el));
					new->id = mechanic->id;
					new->next = (el*)NULL;
					if(lista.head == NULL)	lista.head = new;
					else 	lista.tail->next = new;
					lista.tail = new;
					pthread_mutex_unlock(enlist);
					while(lista.head != new) usleep(10000); 
					ret = pthread_mutex_trylock(moved); 
					while (!*(mechanic->endThread) && ret != 0) 
					{
						usleep(1000);
						ret = pthread_mutex_trylock(moved);
					}
					if(*(mechanic->endThread))
					{ 
						free(new);
						if(!ret) pthread_mutex_unlock(moved);
						break;
					}
				}
				//czwarty krok chodzenia w koło
				changeRecord[coords[0]][coords[1]] = 0;
				changeTable[coords[0]--][coords[1]] = true;
				changeRecord[coords[0]][coords[1]] = MECHANIC;
				changeTable[coords[0]][coords[1]] = true;
				for(k = 0; k<grade; k++)
				{
					if(step+k>9) break;
					changeRecord[tablet[0]][tablet[1]+step+k] = 8;
					changeTable[tablet[0]][tablet[1]+step+k] = true;
				}
				if((control++)%steps == 0) step++;
				change = true;
				{ //zwolnienie rysowania
					pthread_mutex_unlock(moved);
					pthread_cond_signal(drawn);
					ret = pthread_mutex_trylock(enlist);
					while(ret != 0) ret = pthread_mutex_trylock(enlist);
					lista.head = new->next;
					if(new == lista.tail) lista.tail = (el*)NULL;
					free(new);
					pthread_mutex_unlock(enlist);
				}
				usleep(300000);
			}
			if(!*(mechanic->endThread))
			{
				i = lastCar;
				do{
					i = (i + 1) % cars;
					ret = pthread_mutex_trylock(&(carLock[i]));
					if(ret != 0) continue;
					if(!carPlacement[i].toRepair) 
					{
						pthread_mutex_unlock(&(carLock[i]));
						continue;
					}
					lastCar = i;
					repairing = true;
					{ //przesunięcie się do stacji naprawczej
						{ //zajęcie rysowania
							ret = 1; 
							while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
							if(*(mechanic->endThread)) break;
							new = (el*)malloc(sizeof(el));
							new->id = mechanic->id;
							new->next = (el*)NULL;
							if(lista.head == NULL)	lista.head = new;
							else 	lista.tail->next = new;
							lista.tail = new;
							pthread_mutex_unlock(enlist);
							while(lista.head != new) usleep(10000); 
							ret = pthread_mutex_trylock(moved); 
							while (!*(mechanic->endThread) && ret != 0) 
							{
								usleep(1000);
								ret = pthread_mutex_trylock(moved);
							}
							if(*(mechanic->endThread))
							{ 
								free(new);
								if(!ret) pthread_mutex_unlock(moved);
								break;
							}
						}
						//przybliżenie się do stacji, aby nie mieszać się z krążącymi
						changeRecord[coords[0]][coords[1]] = 0;
						changeTable[coords[0]][coords[1]] = true;
						if(coords[0]>workspacePlacement[temp][0]) coords[0]--;
						else coords[0]++;
						changeRecord[coords[0]][coords[1]] = MECHANIC;
						changeTable[coords[0]][coords[1]] = true;
						{//czyszczenie paska postępu
							changeRecord[tablet[0]][tablet[1]] = 0;
							changeTable[tablet[0]][tablet[1]] = true;
							changeRecord[tablet[0]][tablet[1]+1] = 0;
							changeTable[tablet[0]][tablet[1]+1] = true;
							changeRecord[tablet[0]][tablet[1]+2] = 0;
							changeTable[tablet[0]][tablet[1]+2] = true;
							changeRecord[tablet[0]][tablet[1]+3] = 0;
							changeTable[tablet[0]][tablet[1]+3] = true;
							changeRecord[tablet[0]][tablet[1]+4] = 0;
							changeTable[tablet[0]][tablet[1]+4] = true;
							changeRecord[tablet[0]][tablet[1]+5] = 0;
							changeTable[tablet[0]][tablet[1]+5] = true;
							changeRecord[tablet[0]][tablet[1]+6] = 0;
							changeTable[tablet[0]][tablet[1]+6] = true;
							changeRecord[tablet[0]][tablet[1]+7] = 0;
							changeTable[tablet[0]][tablet[1]+7] = true;
							changeRecord[tablet[0]][tablet[1]+8] = 0;
							changeTable[tablet[0]][tablet[1]+8] = true;
							changeRecord[tablet[0]][tablet[1]+9] = 0;
							changeTable[tablet[0]][tablet[1]+9] = true;
						}
						{//aktualizacja tablicy o informację co się dzieje
							changeRecord[tablet[0]+1][tablet[1]] = 'I';
							changeTable[tablet[0]+1][tablet[1]] = true;
							changeRecord[tablet[0]+1][tablet[1]+1] = 'D';
							changeTable[tablet[0]+1][tablet[1]+1] = true;
							changeRecord[tablet[0]+1][tablet[1]+2] = 'Z';
							changeTable[tablet[0]+1][tablet[1]+2] = true;
							changeRecord[tablet[0]+1][tablet[1]+3] = 'I';
							changeTable[tablet[0]+1][tablet[1]+3] = true;
							changeRecord[tablet[0]+1][tablet[1]+4] = 'E';
							changeTable[tablet[0]+1][tablet[1]+4] = true;
							changeRecord[tablet[0]+1][tablet[1]+5] = DOWN;
							changeTable[tablet[0]+1][tablet[1]+5] = true;
							changeRecord[tablet[0]+1][tablet[1]+6] = DOWN;
							changeTable[tablet[0]+1][tablet[1]+6] = true;
							changeRecord[tablet[0]+1][tablet[1]+7] = DOWN;
							changeTable[tablet[0]+1][tablet[1]+7] = true;
							changeRecord[tablet[0]+1][tablet[1]+8] = DOWN;
							changeTable[tablet[0]+1][tablet[1]+8] = true;
							changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
							changeTable[tablet[0]+1][tablet[1]+9] = true;
						}
						change = true;
						{ //zwolnienie rysowania
								pthread_mutex_unlock(moved);
								pthread_cond_signal(drawn);
								ret = pthread_mutex_trylock(enlist);
								while(ret != 0) ret = pthread_mutex_trylock(enlist);
								lista.head = new->next;
								if(new == lista.tail) lista.tail = (el*)NULL;
								free(new);
								pthread_mutex_unlock(enlist);
							}
						usleep(300000);
						{ //zajęcie rysowania
								ret = 1; 
								while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
								if(*(mechanic->endThread)) break;
								new = (el*)malloc(sizeof(el));
								new->id = mechanic->id;
								new->next = (el*)NULL;
								if(lista.head == NULL)	lista.head = new;
								else 	lista.tail->next = new;
								lista.tail = new;
								pthread_mutex_unlock(enlist);
								while(lista.head != new) usleep(10000); 
								ret = pthread_mutex_trylock(moved); 
								while (!*(mechanic->endThread) && ret != 0) 
								{
									usleep(1000);
									ret = pthread_mutex_trylock(moved);
								}
								if(*(mechanic->endThread))
								{ 
									free(new);
									if(!ret) pthread_mutex_unlock(moved);
									break;
								}
							}
						//zbliżanie się do stacji naprawczej w pionie dla niemieszalności
						changeRecord[coords[0]][coords[1]] = 0;
						changeTable[coords[0]][coords[1]] = true;
						if(coords[0]>workspacePlacement[temp][0]) coords[0]--;
						else coords[0]++;
						changeRecord[coords[0]][coords[1]] = MECHANIC;
						changeTable[coords[0]][coords[1]] = true;
						change = true;
						{ //zwolnienie rysowania
							pthread_mutex_unlock(moved);
							pthread_cond_signal(drawn);
							ret = pthread_mutex_trylock(enlist);
							while(ret != 0) ret = pthread_mutex_trylock(enlist);
							lista.head = new->next;
							if(new == lista.tail) lista.tail = (el*)NULL;
							free(new);
							pthread_mutex_unlock(enlist);
						}
						usleep(300000);
						//dojście do auta na stacji naprawczej
						temp = carPlacement[lastCar].station;
						while((coords[0] != workspacePlacement[temp][0] || coords[1] != workspacePlacement[temp][1] - 1) && !*(mechanic->endThread) )
						{
							while(coords[1] != workspacePlacement[temp][1] - 1 && !*(mechanic->endThread))
							{
								{ //zajęcie rysowania
									ret = 1; 
									while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
									if(*(mechanic->endThread)) break;
									new = (el*)malloc(sizeof(el));
									new->id = mechanic->id;
									new->next = (el*)NULL;
									if(lista.head == NULL)	lista.head = new;
									else 	lista.tail->next = new;
									lista.tail = new;
									pthread_mutex_unlock(enlist);
									while(lista.head != new) usleep(10000); 
									ret = pthread_mutex_trylock(moved); 
									while (!*(mechanic->endThread) && ret != 0) 
									{
										usleep(1000);
										ret = pthread_mutex_trylock(moved);
									}
									if(*(mechanic->endThread))
									{ 
										free(new);
										if(!ret) pthread_mutex_unlock(moved);
										break;
									}
								}
								//zbliżanie się do stacji naprawczej w poziomie
								changeRecord[coords[0]][coords[1]] = 0;
								changeTable[coords[0]][coords[1]] = true;
								if(coords[1]>workspacePlacement[temp][1] - 1) coords[1]--;
								else coords[1]++;
								changeRecord[coords[0]][coords[1]] = MECHANIC;
								changeTable[coords[0]][coords[1]] = true;
								change = true;
								{ //zwolnienie rysowania
									pthread_mutex_unlock(moved);
									pthread_cond_signal(drawn);
									ret = pthread_mutex_trylock(enlist);
									while(ret != 0) ret = pthread_mutex_trylock(enlist);
									lista.head = new->next;
									if(new == lista.tail) lista.tail = (el*)NULL;
									free(new);
									pthread_mutex_unlock(enlist);
								}
								usleep(300000);
							}
							{ //zajęcie rysowania
								ret = 1; 
								while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
								if(*(mechanic->endThread)) break;
								new = (el*)malloc(sizeof(el));
								new->id = mechanic->id;
								new->next = (el*)NULL;
								if(lista.head == NULL)	lista.head = new;
								else 	lista.tail->next = new;
								lista.tail = new;
								pthread_mutex_unlock(enlist);
								while(lista.head != new) usleep(10000); 
								ret = pthread_mutex_trylock(moved); 
								while (!*(mechanic->endThread) && ret != 0) 
								{
									usleep(1000);
									ret = pthread_mutex_trylock(moved);
								}
								if(*(mechanic->endThread))
								{ 
									free(new);
									if(!ret) pthread_mutex_unlock(moved);
									break;
								}
							}
							//zbliżanie się do stacji naprawczej w pionie
							changeRecord[coords[0]][coords[1]] = 0;
							changeTable[coords[0]][coords[1]] = true;
							if(coords[0]>workspacePlacement[temp][0]) coords[0]--;
							else coords[0]++;
							changeRecord[coords[0]][coords[1]] = MECHANIC;
							changeTable[coords[0]][coords[1]] = true;
							change = true;
							{ //zwolnienie rysowania
								pthread_mutex_unlock(moved);
								pthread_cond_signal(drawn);
								ret = pthread_mutex_trylock(enlist);
								while(ret != 0) ret = pthread_mutex_trylock(enlist);
								lista.head = new->next;
								if(new == lista.tail) lista.tail = (el*)NULL;
								free(new);
								pthread_mutex_unlock(enlist);
							}
							usleep(300000);
						}
					}
					{ //zajęcie rysowania
						ret = 1;
						while(!*(mechanic->endThread) && ret != 0)	ret = pthread_mutex_trylock(enlist); 
						if(*(mechanic->endThread)) break;
						new = (el*)malloc(sizeof(el));
						new->id = mechanic->id;
						new->next = (el*)NULL;
						if(lista.head == NULL)	lista.head = new;
						else 	lista.tail->next = new;
						lista.tail = new;
						pthread_mutex_unlock(enlist);
						while(lista.head != new) usleep(10000); 
						ret = pthread_mutex_trylock(moved); 
						while (!*(mechanic->endThread) && ret != 0) 
						{
							usleep(1000);
							ret = pthread_mutex_trylock(moved);
						}
						if(*(mechanic->endThread))
						{ 
							free(new);
							if(!ret) pthread_mutex_unlock(moved);
							break;
						}
					}
					{//czyszczenie paska postępu
						changeRecord[tablet[0]][tablet[1]] = 0;
						changeTable[tablet[0]][tablet[1]] = true;
						changeRecord[tablet[0]][tablet[1]+1] = 0;
						changeTable[tablet[0]][tablet[1]+1] = true;
						changeRecord[tablet[0]][tablet[1]+2] = 0;
						changeTable[tablet[0]][tablet[1]+2] = true;
						changeRecord[tablet[0]][tablet[1]+3] = 0;
						changeTable[tablet[0]][tablet[1]+3] = true;
						changeRecord[tablet[0]][tablet[1]+4] = 0;
						changeTable[tablet[0]][tablet[1]+4] = true;
						changeRecord[tablet[0]][tablet[1]+5] = 0;
						changeTable[tablet[0]][tablet[1]+5] = true;
						changeRecord[tablet[0]][tablet[1]+6] = 0;
						changeTable[tablet[0]][tablet[1]+6] = true;
						changeRecord[tablet[0]][tablet[1]+7] = 0;
						changeTable[tablet[0]][tablet[1]+7] = true;
						changeRecord[tablet[0]][tablet[1]+8] = 0;
						changeTable[tablet[0]][tablet[1]+8] = true;
						changeRecord[tablet[0]][tablet[1]+9] = 0;
						changeTable[tablet[0]][tablet[1]+9] = true;
					}
					{//aktualizacja tablicy o informację co się dzieje
						changeRecord[tablet[0]+1][tablet[1]] = 'P';
						changeTable[tablet[0]+1][tablet[1]] = true;
						changeRecord[tablet[0]+1][tablet[1]+1] = 'O';
						changeTable[tablet[0]+1][tablet[1]+1] = true;
						changeRecord[tablet[0]+1][tablet[1]+2] = 'D';
						changeTable[tablet[0]+1][tablet[1]+2] = true;
						changeRecord[tablet[0]+1][tablet[1]+3] = 'C';
						changeTable[tablet[0]+1][tablet[1]+3] = true;
						changeRecord[tablet[0]+1][tablet[1]+4] = 'H';
						changeTable[tablet[0]+1][tablet[1]+4] = true;
						changeRecord[tablet[0]+1][tablet[1]+5] = 'O';
						changeTable[tablet[0]+1][tablet[1]+5] = true;
						changeRecord[tablet[0]+1][tablet[1]+6] = 'D';
						changeTable[tablet[0]+1][tablet[1]+6] = true;
						changeRecord[tablet[0]+1][tablet[1]+7] = 'Z';
						changeTable[tablet[0]+1][tablet[1]+7] = true;
						changeRecord[tablet[0]+1][tablet[1]+8] = 'I';
						changeTable[tablet[0]+1][tablet[1]+8] = true;
						changeRecord[tablet[0]+1][tablet[1]+9] = DOWN;
						changeTable[tablet[0]+1][tablet[1]+9] = true;
					}
					change = true;
					{ //zwalnianie rysowania
						pthread_mutex_unlock(moved);
						pthread_cond_signal(drawn);
						ret = pthread_mutex_trylock(enlist);
						while(ret != 0) ret = pthread_mutex_trylock(enlist);
						lista.head = new->next;
						if(new == lista.tail) lista.tail = (el*)NULL;
						free(new);
						pthread_mutex_unlock(enlist);
					}
					usleep(100000);
					loops = rand_r(mechanic->luck)%6+10;
					toolNum = rand_r(mechanic->luck)%(TOOLNUM/3-2)+2; //od dwoch do trzeciej części calkowitej ilosci narzedzi
					for(i=0; i<toolNum; i++)
					{
						temp = rand_r(mechanic->luck)%TOOLNUM;
						while(toolReserved[temp]) temp = rand_r(mechanic->luck)%TOOLNUM;
						toolReserved[temp]=true;
						toolUsed[i]=temp;
					}
					free(toolReserved);
					toolReserved = (bool*)calloc(TOOLNUM, sizeof(bool));
					break;
				}while(i != lastCar && !*(mechanic->endThread));
			}
		}
	}
	if(repairing) pthread_mutex_unlock(&(carLock[lastCar]));
	free(toolReserved);
	free(mechanic->luck);
	free(mechanic->coords);
	free(pointer);
}

int main() 
{
	char ch;
	int *position, i, j, k, temp, tablesPerRow;
	bool *control, *retEnd;
	pthread_t *mech_id, *assist_id, draw;
	
	{ //inicjalizacja znaczników niepotrzebujących wymiarów okna
		srand(time(NULL));
		mech_id = (pthread_t*)malloc(mechThreads*sizeof(pthread_t));
		assist_id = (pthread_t*)malloc(assistThreads*sizeof(pthread_t));
		control = (bool*)calloc(mechThreads+assistThreads, sizeof(bool));
		retEnd = (bool*)calloc(mechThreads+assistThreads, sizeof(bool));
		tools = (pthread_mutex_t*)malloc(toolTotal*sizeof(pthread_mutex_t));
		work = (pthread_mutex_t*)malloc(workspaces*sizeof(pthread_mutex_t));
		carLock = (pthread_mutex_t*)malloc(cars*sizeof(pthread_mutex_t));
		workspacePlacement = (int**)malloc(workspaces*sizeof(int*));
		returning = (bool*)calloc(cars, sizeof(bool));
		occupied = (bool*)calloc(workspaces, sizeof(bool));
		carPlacement = (car*)malloc(cars*sizeof(car));
		drawn = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
		moved = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		enlist = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		FIFO = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		lista.head = (el*)NULL;
		lista.tail = (el*)NULL;
	}
	
	{ //inicjalizacja programu i ważniejszych zmiennych
		initscr();
		noecho();
		curs_set(0);
		start_color();
		init_pair(WORKSPACE, COLOR_RED, COLOR_YELLOW);
		init_pair(MECHANIC, COLOR_GREEN, COLOR_BLACK);
		init_pair(ASSISTANT, COLOR_CYAN, COLOR_BLACK);
		init_pair(WALL, COLOR_BLACK, COLOR_WHITE);
		init_pair(CAR, COLOR_CYAN, COLOR_RED);
		getmaxyx(stdscr, row, col);
		attron(COLOR_PAIR(0));
		mvprintw(row-1, 0, "Nacisnij ESC aby zakonczyc!");
		tablesPerRow = (col - 4) / 13;
		offset = 3*((mechThreads+assistThreads)/tablesPerRow);
	}
	
	attron(COLOR_PAIR(WALL));
	{ //rysowanie ścian warsztatu
		temp = (row - offset) / 5;
		wallPlacement[0][0] = temp + offset + 2;
		wallPlacement[1][0] = temp + offset + 2;
		wallPlacement[2][0] = row - temp + 2;
		wallPlacement[3][0] = row - temp + 2;
		temp = (col - 4) / 5;
		wallPlacement[0][1] = temp;
		wallPlacement[1][1] = col - temp - 4;
		wallPlacement[2][1] = col - temp - 4;
		wallPlacement[3][1] = temp;
		temp = (wallPlacement[3][0]-wallPlacement[0][0])/3;
		doormin = wallPlacement[0][0] + temp + 1;
		doormax = wallPlacement[3][0] - temp;
		move(wallPlacement[0][0], wallPlacement[0][1]);
		hline(ACS_HLINE, wallPlacement[1][1]-wallPlacement[0][1]);
		vline(ACS_VLINE, temp);
		move(wallPlacement[1][0], wallPlacement[1][1]);
		vline(ACS_VLINE, temp);
		move(doormax, wallPlacement[2][1]);
		vline(ACS_VLINE, temp);
		move(doormax, wallPlacement[3][1]);
		vline(ACS_VLINE, temp);
		move(wallPlacement[3][0], wallPlacement[3][1]);
		hline(ACS_HLINE, wallPlacement[2][1]-wallPlacement[3][1]);
		mvaddch(wallPlacement[0][0], wallPlacement[0][1], ACS_ULCORNER);
		mvaddch(wallPlacement[3][0], wallPlacement[3][1], ACS_LLCORNER);
		mvaddch(wallPlacement[2][0], wallPlacement[2][1], ACS_LRCORNER);
		mvaddch(wallPlacement[1][0], wallPlacement[1][1], ACS_URCORNER);
	}
	j=0; //tworzenie paneli kontrolnych
	for(i=0; i<assistThreads+mechThreads; i++)
	{
		move(j, (i%tablesPerRow)*13);
		hline(ACS_HLINE, 11);
		vline(ACS_VLINE, 2);
		addch(ACS_ULCORNER);
		move(j, (i%tablesPerRow + 1)*13 - 2);
		vline(ACS_VLINE, 2);
		addch(ACS_URCORNER);
		move(j+2, (i%tablesPerRow)*13);
		hline(ACS_HLINE, 11);
		addch(ACS_LLCORNER);
		mvaddch(j+2, (i%tablesPerRow + 1)*13 - 2, ACS_LRCORNER);
		if((i+1)%tablesPerRow == 0) j += 3;
	}
	attroff(COLOR_PAIR(WALL));
	doormax--;
	
	attron(COLOR_PAIR(ASSISTANT));
	for(i=0;i<toolTotal;i++) //tworzenie reprezentacji narzędzi
	{
		pthread_mutex_init(&(tools[i]), NULL);
		mvaddch(i, col-1, ACS_BLOCK);
	}
	attroff(COLOR_PAIR(ASSISTANT));
	
	j = 0;
	k = 0;
	temp = (col - 4) / 10;
	lpark = wallPlacement[0][1] - temp + 3;
	rpark = wallPlacement[1][1] + temp - 3;
	attron(COLOR_PAIR(CAR));
	for(i=0;i<cars;i++) //rysowanie aut
	{
		pthread_mutex_init(&(carLock[i]), NULL);
		position = (int*)malloc(2*sizeof(int));
		position[0] = wallPlacement[0][0] + j - 2;
		if(j%4==0)
		{
			position[1] = lpark - k;
		}
		else
		{
			position[0] -= 1;
			position[1] = rpark + k;
		}
		carPlacement[i].coords = position;
		carPlacement[i].toRepair = false;
		mvaddch(position[0], position[1], ' ');
		j = (j+2)%20;
		if(j==0) k += 3;
	}
	attroff(COLOR_PAIR(CAR));
	lpark++;
	rpark--;
	
	temp *= 6;
	j = 0;
	k = 0;
	attron(COLOR_PAIR(WORKSPACE));
	for(i=0;i<workspaces;i++) //rysowanie stanowisk naprawczych
	{
		workspacePlacement[i] = (int*)malloc(2*sizeof(int));
		pthread_mutex_init(&(work[i]), NULL);
		workspacePlacement[i][0] = wallPlacement[0][0] + k + 2;
		workspacePlacement[i][1] = wallPlacement[0][1] + j + 4;
		j += 5;
		if(j>temp)
		{
			j = 0;
			k = wallPlacement[2][0] - wallPlacement[0][0] - 4;
		}
		mvaddch(workspacePlacement[i][0], workspacePlacement[i][1], ' ');
	}	
	attroff(COLOR_PAIR(WORKSPACE));

	{ //inicjalizacja wątku rysującego
		changeTable = (bool**)malloc(row*sizeof(bool*));
		changeRecord = (int**)malloc(row*sizeof(int*));
		for(i=0;i<row;i++)
		{
			changeTable[i]=(bool*)calloc(col,sizeof(bool));
			changeRecord[i]=(int*)calloc(col,sizeof(int));
		}
		pthread_cond_init(drawn, NULL);
		pthread_mutex_init(moved, NULL);
		pthread_mutex_init(enlist, NULL);
		pthread_mutex_init(FIFO, NULL);
		pthread_create(&draw, NULL,
					drawing, (void*)NULL);
	}

	j = 0;
	attron(COLOR_PAIR(ASSISTANT));
	for(i=0;i<assistThreads; i++) //rysowanie i inicjalizacja wątków asystentów
	{
		person* assistant = (person*)malloc(sizeof(person));
		unsigned* fate = (unsigned*)malloc(sizeof(unsigned));
		*fate = (unsigned) rand() / 2;
		position = (int*)malloc(2*sizeof(int));
		position[0] = wallPlacement[0][0] - 2;
		position[1] = wallPlacement[0][1] + 2 + j;
		j += 8;
		mvaddch(position[0], position[1], symbols[ASSISTANT]);
		k = (i%tablesPerRow)*13;
		mvprintw((i/tablesPerRow)*3, k+1, "%d", i);
		k++;

		assistant->coords = position;
		assistant->id = i + 1;
		assistant->endThread = &(control[i]);
		assistant->luck = fate;
		assistant->table[0] = (i/tablesPerRow)*3 + 1;
		assistant->table[1] = k;
		pthread_create(&(assist_id[i]), NULL,
				assist, (void*)assistant);
	}
	temp = wallPlacement[3][0] - wallPlacement[0][0];
	temp /= 2;
	j = 0;
	attron(COLOR_PAIR(MECHANIC));
	for(i=0;i<mechThreads; i++) //rysowanie i inicjalizacja wątków mechaników
	{
		person* mechanic = (person*)malloc(sizeof(person));
		unsigned* fate = (unsigned*)malloc(sizeof(unsigned));
		*fate = (unsigned) rand() / 2;
		position = (int*)malloc(2*sizeof(int));
		position[0] = wallPlacement[0][0] + temp - 1;
		position[1] = wallPlacement[0][1] + j + 9;
		j += 4;
		mvaddch(position[0], position[1], symbols[MECHANIC]);
		k = ((i+assistThreads)%tablesPerRow)*13;
		mvprintw(((i+assistThreads)/tablesPerRow)*3, k+1, "%d", i);
		k++;

		mechanic->coords = position;
		mechanic->id = i + assistThreads + 1;
		mechanic->endThread = &(control[i+assistThreads]);
		mechanic->luck = fate;
		mechanic->table[0] = 3*((i+assistThreads)/tablesPerRow) + 1;
		mechanic->table[1] = k;
		pthread_create(&(mech_id[i]), NULL,
				mech, (void*)mechanic);
	}

	while((ch = getch()) != ESCAPE) //pętla programu
	{
	}

	//kończenie programu
	for(i = 0; i < assistThreads; i++)
	{
		control[i] = true;
		pthread_join(assist_id[i], NULL);
	}
	for(i = 0; i < mechThreads; i++)
	{
		control[assistThreads+i] = true;
		pthread_join(mech_id[i], NULL);
	}
	for(i = 0; i < toolTotal; i++) pthread_mutex_destroy(&(tools[i]));
	for(i = 0; i < workspaces; i++) 
	{
		pthread_mutex_destroy(&(work[i]));
		free(workspacePlacement[i]);
	}
	for(i = 0; i < cars; i++)
	{
		pthread_mutex_destroy(&(carLock[i]));
		free(carPlacement[i].coords);
	}
	for(i = 0; i < row; i++)
	{
		free(changeTable[i]);
		free(changeRecord[i]);
	}
	end = true;
	pthread_cond_signal(drawn);
	pthread_join(draw, NULL);
	pthread_cond_destroy(drawn);
	pthread_mutex_destroy(moved);
	pthread_mutex_destroy(enlist);
	pthread_mutex_destroy(FIFO);
	endwin();
	free(drawn);
	free(enlist);
	free(moved);
	free(FIFO);
	free(changeTable[i]);
	free(changeRecord[i]);
	free(control);
	free(tools);
	free(work);
	free(mech_id);
	free(assist_id); 
	free(returning);
	free(carPlacement);
	free(carLock);
	return 0;
}

