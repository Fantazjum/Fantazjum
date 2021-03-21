#include <cstdlib>
#include <cstdio>
#include <ctime>

//***************************************
//wczytanie plikow biblioteki OpenGL
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
//***************************************

//zmienne globalne mowiace programowi co rysowac
int levels, seed;
float scaling = 0.5;

//klasa reprezentujaca rysowane kwadraty
class Psychadelic
{
    float xb, xe, yb, ye;
    int lv;
public:
    Psychadelic(float a, float b, float c, float d, int poziom)
    {
        xb = a;
        xe = b;
        yb = c;
        ye = d;
        lv = poziom;
    }

    void draw()
    {
        if (lv == 0)
        {
            float a, b, c, d, scale;
            if (scaling)
            {
                scale = scaling * (xe - xb);
                a = scale * rand() / RAND_MAX - scale / 2;
                b = scale * rand() / RAND_MAX - scale / 2;
                c = scale * rand() / RAND_MAX - scale / 2;
                d = scale * rand() / RAND_MAX - scale / 2;
            }
            else a = b = c = d = 0;
            glBegin(GL_POLYGON);

            glColor3f(1.0f, 0.0f, 0.0f); // wierzchołek czerwony
            glVertex2f(xb+a, yb+b);
            glColor3f(0.0f, 1.0f, 0.0f); // wierzchołek zielony
            glVertex2f(xe+c, yb+b/2);
            glColor3f(0.0f, 0.0f, 1.0f); // wierzcholek niebieski
            glVertex2f(xe+c/2, ye+d/2);
            glColor3f(0.0f, 0.0f, 0.0f); // wierzchołek czarny
            glVertex2f(xb+a/2, ye+d);

            glEnd();

            glFlush();
        }
        else
        {
            int i;
            Psychadelic* recursive;
            int xs = 0, ys = 0; //skalowanie dla rekursywnego rysowania
            float x = xe - xb;
            x = x / 3.0;
            if (x < 0) x = -x;
            float y = ye - yb;
            y = y / 3.0;
            if (y < 0) y = -y;
            for (i = 0; i < 9; i++)
            {
                if (i == 4) //srodka nie rysujemy
                {
                    xs++;
                    continue;
                }
                recursive = new Psychadelic(xb + xs * x, xb + (xs + 1) * x, yb + (ys)*y, yb + (ys + 1) * y, lv - 1);
                recursive->draw();
                delete recursive;
                xs++; //przejscie do kolejnej kolumny
                if (xs == 3)
                {
                    xs = 0;
                    ys++;
                } //przejscie do kolejnego wiersza
            }
        }
    }
};

//informacja co program rysuje
void RenderScene(void)
{
    srand(seed);    //inicjalizujemy tutaj, aby dywan nie falowal
    glClear(GL_COLOR_BUFFER_BIT);
    
    Psychadelic* dywan = new Psychadelic(-0.9f,0.9f,-0.9f,0.9f,levels);
    dywan->draw();
}

void MyInit(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //wnetrze okna wybrano jako biale, aby sprawiac sobie cierpienie w nocy
}

//Menu tekstowe aplikacji konsolowej
void menu()
{
    printf("Rysowanie dywanikow sierpinskiego:\n");
    printf("-----------MENU GLOWNE-----------\n");
    printf("1.Dywan z 3 poziomami\n");
    printf("2.Dywan poturbowany z 4 poziomami\n");
    printf("3.Dywan poturbowany o dowolnej ilosci poziomow\n");
    printf("0.Zakoncz program\n");
}



int main()
{
    seed = time(NULL);
    short scan = -1; //zmienna reprezentujaca opcje wybrana przez uzytkownika
    short draw = 0;
    while (scan) //petla wywolujaca program glowny
    {
        menu();
        scanf_s("%hd", &scan);
        switch (scan)
        {
        case 1:
            scan = 0;
            draw = 1;
            levels = 3;
            scaling = 0;
            break;
        case 2:
            scan = 0;
            draw = 1;
            levels = 4;
            printf("Podaj stopien deforamcji dywanu: ");
            scanf_s("%f", &scaling);
            break;
        case 3:
            scan = 0;
            draw = 1;
            printf("Podaj ilosc poziomow: ");
            scanf_s("%d",&levels);
            printf("Podaj stopien deforamcji dywanu: ");
            scanf_s("%f", &scaling);
            break;
        case 0:
            break;
        default:
            printf("Nieobslugiwana opcja!\n");
            system("Pause");
            system("cls");
        }
    }
    if (draw)
    {
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
        glutCreateWindow("Dywany Sierpinskiego");
        MyInit();
        glutDisplayFunc(RenderScene);
        glutMainLoop();
    }
    system("Pause");
    return 0;
}
