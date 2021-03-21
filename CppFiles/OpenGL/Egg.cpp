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

GLfloat alpha[2] = { 0.0, 0.0};
int przedzialy = 1;
short model = 1;
bool kierunek = false;
float*** kolorki;
float*** punkty;
float*** ksiezyc;

float*** jajo()
{
    int i, j;
    double v, u = 0.0, size = 1.0 / przedzialy; 
    float*** chmura;
    chmura = (float***)malloc(przedzialy * sizeof(float**));
    for (i = 0; i < przedzialy; i++)
    {
        chmura[i] = (float**)malloc(przedzialy * sizeof(float*));
        v = 0.0;
        for (j = 0; j < przedzialy; j++)
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

void spin()
{
    if (kierunek)
    {
        alpha[1] += 0,05;
        if (alpha[1] >= 360.0) alpha[1] -= 360.0;
    }
    else
    {

        alpha[0] += 0.05;
        if (alpha[0] >= 360.0) alpha[0] -= 360.0;
    }
    glutPostRedisplay();
}

void draw()
{
    int i, j, mod = przedzialy % 10;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
    glRotatef(alpha[0], 0.0, 1.0, 0.0);
    glRotatef(alpha[1], 1.0, 0.0, 0.0);
    switch (model)
    {
        case 1:
        {
            glColor3f(0.75, 0.5, 0.1);
            glBegin(GL_POINTS);
            for (i = 0; i < przedzialy; i++)
            {
                for (j = 0; j < przedzialy; j++)
                {
                    glVertex3fv(punkty[i][j]);
                }
            }
            glEnd();
            glPopMatrix();

            przedzialy /= 10;
            glPushMatrix();
            glScalef(0.2, 0.2, 0.2);
            glRotatef(5 * alpha[0] + 5 * alpha[1], 0.0, 1.0, 0.0);
            glRotatef(alpha[1], 1.0, 0.0, 0.0);
            glTranslatef(30.0, 5.0, 0.0);
            glBegin(GL_POINTS);
            for (i = 0; i < przedzialy; i++)
            {
                for (j = 0; j < przedzialy; j++)
                {
                    glVertex3fv(ksiezyc[i][j]);
                }
            }
            glEnd();
            glPopMatrix();
            przedzialy *= 10;
            przedzialy += mod;
            break;
        }
        case 2:
        {
            glColor3f(0.1, 0.75, 0.3);
            glBegin(GL_LINES);
            for (i = 0; i < przedzialy; i++)
            {
                for (j = 0; j < przedzialy - 1; j++)
                {
                    glVertex3fv(punkty[i][j]);
                    glVertex3fv(punkty[(i + 1) % przedzialy][j]); //linie pionowe

                    glVertex3fv(punkty[i][j]);
                    glVertex3fv(punkty[i][j + 1]); //linie poziome
                }
                glVertex3fv(punkty[i][j]);
                glVertex3fv(punkty[(i + 1) % przedzialy][j]);
                if (i > 0)
                {
                    glVertex3fv(punkty[i][j]);
                    glVertex3fv(punkty[przedzialy - i][0]); //zlapanie punktow po drugiej stronie dziury w jajku zamiast robienia kinder joy
                }
            }
            glEnd();
            glPopMatrix();

            przedzialy /= 10;
            glPushMatrix();
            glScalef(0.2, 0.2, 0.2);
            glRotatef(5 * alpha[0] + 5 * alpha[1], 0.0, 1.0, 0.0);
            glRotatef(alpha[1], 1.0, 0.0, 0.0);
            glTranslatef(30.0, 5.0, 0.0);
            glBegin(GL_LINES);
            for (i = 0; i < przedzialy; i++)
            {
                for (j = 0; j < przedzialy - 1; j++)
                {
                    glVertex3fv(ksiezyc[i][j]);
                    glVertex3fv(ksiezyc[(i + 1) % przedzialy][j]);

                    glVertex3fv(ksiezyc[i][j]);
                    glVertex3fv(ksiezyc[i][j + 1]);
                }
                glVertex3fv(ksiezyc[i][j]);
                glVertex3fv(ksiezyc[(i + 1) % przedzialy][j]);
                if (i > 0)
                {
                    glVertex3fv(ksiezyc[i][j]);
                    glVertex3fv(ksiezyc[przedzialy - i][0]);
                }
            }
            glEnd();
            glPopMatrix();
            przedzialy *= 10;
            przedzialy += mod;
            break;
        }
        case 3:
        {
            glBegin(GL_TRIANGLES);
            for (i = 0; i < przedzialy; i++)
            {
                for (j = 0; j < przedzialy-1; j++)
                {
                    glColor3fv(kolorki[i][j]);
                    glVertex3fv(punkty[i][j]);
                    glColor3fv(kolorki[(i + 1) % przedzialy][j]);
                    glVertex3fv(punkty[(i + 1) % przedzialy][j]);
                    glColor3fv(kolorki[i][j + 1]);
                    glVertex3fv(punkty[i][j + 1]);
                
                    glColor3fv(kolorki[i][j + 1]);
                    glVertex3fv(punkty[i][j + 1]);
                    glColor3fv(kolorki[(i + 1) % przedzialy][j]);
                    glVertex3fv(punkty[(i + 1) % przedzialy][j]);
                    glColor3fv(kolorki[(i + 1) % przedzialy][j + 1]);
                    glVertex3fv(punkty[(i + 1) % przedzialy][j + 1]);
                }

                if (i > 0)
                {
                    glColor3fv(kolorki[i][j]);
                    glVertex3fv(punkty[i][j]);
                    glColor3fv(kolorki[przedzialy - i][0]);
                    glVertex3fv(punkty[przedzialy - i][0]);
                    glColor3fv(kolorki[(i + 1) % przedzialy][j]);
                    glVertex3fv(punkty[(i + 1) % przedzialy][j]);
                
                    glColor3fv(kolorki[i][0]);
                    glVertex3fv(punkty[i][0]);
                    glColor3fv(kolorki[przedzialy - i][j]);
                    glVertex3fv(punkty[przedzialy - i][j]);
                    glColor3fv(kolorki[(i + 1) % przedzialy][0]);
                    glVertex3fv(punkty[(i + 1) % przedzialy][0]);
                }
            }
            glEnd();
            glPopMatrix();

            przedzialy /= 10;
            glPushMatrix();
            glScalef(0.2, 0.2, 0.2);
            glRotatef(5 * alpha[0] + 5 * alpha[1], 0.0, 1.0, 0.0);
            glRotatef(alpha[1], 1.0, 0.0, 0.0);
            glTranslatef(30.0, 5.0, 0.0);
            glColor3f(0.75, 0.5, 0.1);
            glBegin(GL_TRIANGLES);
            for (i = 0; i < przedzialy; i++)
            {
                for (j = 0; j < przedzialy - 1; j++)
                {
                    glColor3fv(kolorki[i][j]);
                    glVertex3fv(ksiezyc[i][j]);
                    glColor3fv(kolorki[(i + 1) % przedzialy][j]);
                    glVertex3fv(ksiezyc[(i + 1) % przedzialy][j]);
                    glColor3fv(kolorki[i][j + 1]);
                    glVertex3fv(ksiezyc[i][j + 1]);

                    glColor3fv(kolorki[i][j + 1]);
                    glVertex3fv(ksiezyc[i][j + 1]);
                    glColor3fv(kolorki[(i + 1) % przedzialy][j]);
                    glVertex3fv(ksiezyc[(i + 1) % przedzialy][j]);
                    glColor3fv(kolorki[(i + 1) % przedzialy][j + 1]);
                    glVertex3fv(ksiezyc[(i + 1) % przedzialy][j + 1]);
                }

                if (i > 0)
                {
                    glColor3fv(kolorki[i][j]);
                    glVertex3fv(ksiezyc[i][j]);
                    glColor3fv(kolorki[przedzialy - i][0]);
                    glVertex3fv(ksiezyc[przedzialy - i][0]);
                    glColor3fv(kolorki[(i + 1) % przedzialy][j]);
                    glVertex3fv(ksiezyc[(i + 1) % przedzialy][j]);

                    glColor3fv(kolorki[i][0]);
                    glVertex3fv(ksiezyc[i][0]);
                    glColor3fv(kolorki[przedzialy - i][j]);
                    glVertex3fv(ksiezyc[przedzialy - i][j]);
                    glColor3fv(kolorki[(i + 1) % przedzialy][0]);
                    glVertex3fv(ksiezyc[(i + 1) % przedzialy][0]);
                }
            }
            glEnd();
            glPopMatrix();
            przedzialy *= 10;
            przedzialy += mod;
            break;
        }
    }
    
    glFlush();
    glutSwapBuffers();
}

void input(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);
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
    else kierunek = !kierunek;
    draw();
}

void skalowanie(GLsizei horizontal, GLsizei vertical)
{
    GLfloat AspectRatio;
    if (vertical == 0) vertical = 1;
    glViewport(0, 0, horizontal, vertical);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    if (horizontal <= vertical) glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
    else glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);
    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();
}

void init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //wnetrze okna wybrano jako czarne, by punkty patrzyly z otchlani
}

int main()
{
    srand(time(NULL));
    int mod, test = -1;
    while (test)
    {
        printf("Program rysuje jajko z podzielonego kwadratu parametrycznego.\n");
        printf("0 konczy proces programu.\n");
        printf("Powiedz ile przedzialow boku parametrycznego wybierasz: ");
        scanf_s("%d",&przedzialy);
        if (!przedzialy) return 0;
        if (przedzialy < 20)
        {
            printf("Potrzeba wiecej przedzialow! (min. 20)\n");
            system("Pause");
            system("cls");
        }
        else test = 0;
    }
    punkty = jajo();
    mod = przedzialy % 10;
    przedzialy /= 10;
    ksiezyc = jajo();
    przedzialy *= 10;
    przedzialy += mod;
    printf("Aby zakonczyc nacisnij ESC!");
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
    glutIdleFunc(spin);
    init();
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}
