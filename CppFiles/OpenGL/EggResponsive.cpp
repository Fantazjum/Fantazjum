#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <math.h>

//***************************************
//wczytanie plikow biblioteki OpenGL
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
//***************************************

#define PI 3.14159265359

GLsizei hor, ver;
GLfloat alpha[2] = { 0.0, 0.0 };
double zoom = 1.0;
int x, y, xtmp, ytmp, przedzialy = 1;
int dir[2];
short model = 1;
bool kierunek = false;
bool left = false;
float*** kolorki;
float*** punkty;
float*** ksiezyc;

float*** jajo(int rozmiar)
{
    int i, j;
    double v, u = 0.0, size = 1.0 / rozmiar;
    float*** chmura;
    chmura = (float***)malloc(rozmiar * sizeof(float**));
    for (i = 0; i < rozmiar; i++)
    {
        chmura[i] = (float**)malloc(rozmiar * sizeof(float*));
        v = 0.0;
        for (j = 0; j < rozmiar; j++)
        {
            chmura[i][j] = new float[3];
            chmura[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(PI * v);
            chmura[i][j][1] = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5.0; // bez -5 jajko jest za wysoko
            chmura[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(PI * v);
            v += size;
        }
        u += size;
    }
    return chmura;
}

void scale()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat Ratio = hor / ver;
    if (hor <= ver) glOrtho(-7.5 * zoom, 7.5 * zoom, -7.5 / Ratio * zoom, 7.5 / Ratio * zoom, 20.0, -20.0);
    else glOrtho(-7.5 * Ratio * zoom, 7.5 * Ratio * zoom, -5.5 * zoom, 5.5 * zoom, 20.0, -20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void spin()
{
    if (kierunek)
    {
        alpha[1] += 0.05;
        if (alpha[1] >= 360.0) alpha[1] -= 360.0;
    }
    else
    {

        alpha[0] += 0.05;
        if (alpha[0] >= 360.0) alpha[0] -= 360.0;
    }
    glutPostRedisplay();
}

void moonInit()
{
    glScalef(0.2, 0.2, 0.2);
    glRotatef(5 * alpha[0] + 5 * alpha[1], 0.0, 1.0, 0.0);
    glTranslatef(30.0, 5.0, 0.0);
    glRotatef(alpha[1], 1.0, 0.0, 0.0);
}

void point(int rozmiar, float*** source)
{
    int i, j;
    glColor3f(0.75, 0.5, 0.1);
    glBegin(GL_POINTS);
    for (i = 0; i < rozmiar; i++)
    {
        for (j = 0; j < rozmiar; j++)
        {
            glVertex3fv(source[i][j]);
        }
    }
    glEnd();
}

void line(int rozmiar, float*** source)
{
    int i, j;
    glColor3f(0.1, 0.75, 0.3);
    glBegin(GL_LINES);
    for (i = 0; i < rozmiar; i++)
    {
        for (j = 0; j < rozmiar - 1; j++)
        {
            glVertex3fv(source[i][j]);
            glVertex3fv(source[(i + 1) % rozmiar][j]); //linie pionowe

            glVertex3fv(source[i][j]);
            glVertex3fv(source[i][j + 1]); //linie poziome
        }
        glVertex3fv(source[i][j]);
        glVertex3fv(source[(i + 1) % rozmiar][j]);
        if (i > 0)
        {
            glVertex3fv(source[i][j]);
            glVertex3fv(source[rozmiar - i][0]); //zlapanie punktow po drugiej stronie dziury w jajku zamiast robienia kinder joy
        }
    }
    glEnd();
}

void triangle(int rozmiar, float*** source)
{
    int i, j;
    glBegin(GL_TRIANGLES);
    for (i = 0; i < rozmiar; i++)
    {
        for (j = 0; j < rozmiar - 1; j++)
        {
            glColor3fv(kolorki[i][j]);
            glVertex3fv(source[i][j]);
            glColor3fv(kolorki[(i + 1) % rozmiar][j]);
            glVertex3fv(source[(i + 1) % rozmiar][j]);
            glColor3fv(kolorki[i][j + 1]);
            glVertex3fv(source[i][j + 1]);

            glColor3fv(kolorki[i][j + 1]);
            glVertex3fv(source[i][j + 1]);
            glColor3fv(kolorki[(i + 1) % rozmiar][j]);
            glVertex3fv(source[(i + 1) % rozmiar][j]);
            glColor3fv(kolorki[(i + 1) % rozmiar][j + 1]);
            glVertex3fv(source[(i + 1) % rozmiar][j + 1]);
        }

        if (i > 0)
        {
            glColor3fv(kolorki[i][j]);
            glVertex3fv(source[i][j]);
            glColor3fv(kolorki[rozmiar - i][0]);
            glVertex3fv(source[rozmiar - i][0]);
            glColor3fv(kolorki[(i + 1) % rozmiar][j]);
            glVertex3fv(source[(i + 1) % rozmiar][j]);

            glColor3fv(kolorki[i][0]);
            glVertex3fv(source[i][0]);
            glColor3fv(kolorki[rozmiar - i][j]);
            glVertex3fv(source[rozmiar - i][j]);
            glColor3fv(kolorki[(i + 1) % rozmiar][0]);
            glVertex3fv(source[(i + 1) % rozmiar][0]);
        }
    }
    glEnd();
}

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    scale();
    
    gluLookAt(5.0 * dir[0] / hor, 5.0 * dir[1] / ver, 10.0, 5.0 * dir[0] / hor, 5.0 * dir[1] / ver, 0.0, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotatef(alpha[0], 0.0, 1.0, 0.0);
    glRotatef(alpha[1], 1.0, 0.0, 0.0);
    switch (model)
    {
        case 1:
        {
            point(przedzialy, punkty);
            glPopMatrix();

            glPushMatrix();
            moonInit();
            point(przedzialy / 10, ksiezyc);
            glPopMatrix();
            break;
        }
        case 2:
        {
            line(przedzialy, punkty);
            glPopMatrix();
            
            glPushMatrix();
            moonInit();
            line(przedzialy / 10, ksiezyc);
            glPopMatrix();
            break;
        }
        case 3:
        {
            triangle(przedzialy, punkty);
            glPopMatrix();

            glPushMatrix();
            moonInit();
            triangle(przedzialy / 10, ksiezyc);
            glPopMatrix();
            break;
        }
    }
        
    glFlush();
    glutSwapBuffers();
}

void input(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);
    else if (key == 'r' || key == 'R')
    {
        dir[0] = dir[1] = 0;
    }
    else if (key == 'p' || key == 'P')
    {
        model = 1;
        alpha[0] = 0.0;
        alpha[1] = 0.0;
    }
    else if (key == 'l' || key == 'L')
    {
        model = 2;
        alpha[0] = 0.0;
        alpha[1] = 0.0;
    }
    else if (key == 't' || key == 'T')
    {
        model = 3;
        alpha[0] = 0.0;
        alpha[1] = 0.0;
    }
    else if (key == '+') zoom -= 0.05; //ze wzgledu na brak odpowiedzi scrolla przyblizenie jest realizowane za pomoca klawiszy
    else if (key == '-') zoom += 0.05;
    else kierunek = !kierunek;
}

void mWait(int xM, int yM) //funkcja zwracajaca wspolrzedne myszki zanim kliknie sie przycisk
{
    x = xM;
    y = yM;
}

void kot(int xM, int yM) //funkcja odpowiadajaca za przesuwanie myszki
{
    dir[0] = xtmp + x - xM;
    dir[1] = ytmp + yM - y;
}

void mysz(int button, int state, int x, int y)
{
    if (button == 0)
    {
        if (state == GLUT_DOWN)
        {
            glutMotionFunc(kot);
            xtmp = dir[0];
            ytmp = dir[1];
        }
    }
    /*
    *   Niedzialajaca funkcja reakcji na scroll myszki
    if (button == 3 || button == 4)
    {
        if (state == GLUT_UP) zoom -= 0.05;
        else zoom += 0.05;
    }*/
}

void skalowanie(GLsizei horizontal, GLsizei vertical)
{
    if (vertical == 0) vertical = 1;
    hor = horizontal;
    ver = vertical;
    glViewport(0, 0, horizontal, vertical);
}

void init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //wnetrze okna wybrano jako czarne, by punkty patrzyly z otchlani
}

void instruckje()
{
    printf("Dostepne akcje:\n");
    printf("LPM - zacznij przesuwac obiekt\n");
    printf("R - resetuj przesuniecie obiektu\n");
    printf("P - rysowanie jajka jako chmury punktow (domyslne)\n");
    printf("L - rysowanie jajka jako siatki lin\n");
    printf("T - rysowanie jajka z uzyciem triangulacji\n");
    printf("-/+ - oddalenie/przyblizenie obrazu\n");
    printf("ESC - zakonczenie dzialania programu\n");
    printf("Dowolny inny przycisk zmienia os obrotu");
}

int main()
{
    srand(time(NULL));
    int test = -1;
    while (test)
    {
        printf("Program rysuje jajko z podzielonego kwadratu parametrycznego.\n");
        printf("0 konczy proces programu.\n");
        printf("Powiedz ile przedzialow boku parametrycznego wybierasz: ");
        scanf_s("%d", &przedzialy);
        if (!przedzialy) return 0;
        if (przedzialy < 20)
        {
            printf("Potrzeba wiecej przedzialow! (min. 20)\n");
            system("Pause");
            system("cls");
        }
        else test = 0;
    }
    punkty = jajo(przedzialy);
    ksiezyc = jajo(przedzialy / 10);
    instruckje();
    kolorki = (float***)malloc(przedzialy * sizeof(float**));
    for (int i = 0; i < przedzialy; i++)
    {
        kolorki[i] = (float**)malloc(przedzialy * sizeof(float*));
        for (int j = 0; j < przedzialy; j++)
        {
            kolorki[i][j] = new float[3];
            kolorki[i][j][0] = (float)rand() / RAND_MAX;
            kolorki[i][j][1] = (float)rand() / RAND_MAX;
            kolorki[i][j][2] = (float)rand() / RAND_MAX;
        }
    }
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Jajo");
    glutDisplayFunc(draw);
    glutReshapeFunc(skalowanie);
    glutKeyboardFunc(input);
    glutPassiveMotionFunc(mWait);
    glutMouseFunc(mysz);
    glutIdleFunc(spin);
    init();
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}
