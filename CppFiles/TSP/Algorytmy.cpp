#include <cstdio>
#include <string>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <Windows.h>
#include "Commons.h"
#include "Zachlanne.h"
#include "Dokladne.h"
#include "Lokalne.h"
#include "Genetyczne.h"

void menu()
{
	printf("-------MENU ALGORYTMOW PROBLEMU KOMIWOJAZERA-------\n");
	printf("1.Algorytmy zachlanne\n");
	printf("2.Algorytmy dokladne\n");
	printf("3.Algorytmy metaheurystyczne poszukiwania lokalnego\n");
	printf("4.Algorytmy metaheurestyczne ewolucyjne\n");
	printf("0.Zakonczenie dzialania programu\n");
	printf("Podaj numer opcji: ");
}

int main()
{
	short decyzja=-1;
	while (decyzja)
	{
		menu();
		scanf_s("%hd", &decyzja);
		switch (decyzja)
		{
		case 1:
			system("cls");
			zach();
			break;
		case 2:
			system("cls");
			dok();
			break;
		case 3:
			system("cls");
			lok();
			break;
		case 4:
			system("cls");
			ewol();
			break;
		case 0:
			printf("Konczenie pracy programu!\n");
			system("Pause");
			break;
		default:
			printf("Nieobslugiwana opcja!\n");
			printf("Sprobuj ponownie!\n");
			system("Pause");
			break;
		}
		system("cls");
	}
	return 0;
}