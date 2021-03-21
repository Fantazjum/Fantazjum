  // Copyright 2020 Aleksy Slizewski
// wczytanie plikow biblioteki OpenGL
// **********************************
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
// **********************************

#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>

// wczytanie biblioteki do wczytywania obrazow jako tekstury
// *********************************************************
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// *********************************************************

#define PI 3.14159265359

GLsizei hor, ver;
GLfloat alpha[2] = { 0.0, 0.0 };
double zoom = 1.0;
int x, y, xtmp, ytmp, przedzialy = 1;
int dir[2];
INT16 model = 1;
bool kierunek = false;
bool left = false;
float*** pos;
float*** punkty;
float*** ksiezyc;
float*** normPoint;
float*** normMoon;

INT16 tekstura = 0;
GLuint* tekstury;
int width, height, nrChannels;
unsigned char* data;
int timer;

float*** jajo(int rozmiar) {
    int i, j;
    double v, u = 0.0, size = 1.0 / rozmiar;
    float*** chmura;
    chmura = new float** [rozmiar];
    for (i = 0; i < rozmiar; i++) {
        chmura[i] = new float* [rozmiar];
        v = 0.0;
        for (j = 0; j < rozmiar; j++) {
            chmura[i][j] = new float[3];
            chmura[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) -
                270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(PI * v);
            chmura[i][j][1] = 160 * pow(u, 4) - 320 * pow(u, 3) +
                160 * pow(u, 2) - 5.0;  // bez -5 jajko jest za wysoko
            chmura[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) -
                270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(PI * v);
            v += size;
        }
        u += size;
    }
    return chmura;
}

void textureInit() {
    tekstury = new GLuint[3];
    glGenTextures(3, tekstury);

    data = stbi_load("OldWin.png", &width, &height, &nrChannels, 0);
    glBindTexture(GL_TEXTURE_2D, tekstury[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    data = stbi_load("LightColor.png", &width, &height, &nrChannels, 0);
    glBindTexture(GL_TEXTURE_2D, tekstury[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    data = stbi_load("PrintColor.png", &width, &height, &nrChannels, 0);
    glBindTexture(GL_TEXTURE_2D, tekstury[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
}

float*** normalise(int rozmiar) {
    int i, j;
    double v, u = 0.0, size = 1.0 / rozmiar;
    float norm;
    float*** normal;
    normal = new float** [rozmiar];
    for (i = 0; i < rozmiar; i++) {
        normal[i] = new float* [rozmiar];
        v = 0.0;
        for (j = 0; j < rozmiar; j++) {
            normal[i][j] = new float[3];
            normal[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) -
                270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(PI * v) *
                PI * (640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);
            norm = normal[i][j][0] * normal[i][j][0];
            normal[i][j][1] = (90 * pow(u, 5) - 225 * pow(u, 4) +
                270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) *
                (450 * pow(u, 4) - 900 * pow(u, 3) + 810 * pow(u, 2) -
                    360 * u - 45) * (-PI) * (cos(PI * v) * cos(PI * v) +
                        sin(PI * v) * sin(PI * v));
            norm += normal[i][j][1] * normal[i][j][1];
            normal[i][j][2] = (90 * pow(u, 5) - 225 * pow(u, 4) +
                270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(PI * v) *
                PI * (640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);
            norm += normal[i][j][2] * normal[i][j][2];
            norm = sqrt(norm);
            normal[i][j][0] /= norm;
            normal[i][j][1] /= norm;
            normal[i][j][2] /= norm;
            if (i >= przedzialy / 2) {
                normal[i][j][0] *= -1;
                normal[i][j][1] *= -1;
                normal[i][j][2] *= -1;
            }
            v += size;
        }
        u += size;
    }
    return normal;
}

void scale() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat Ratio = hor / ver;
    if (hor <= ver) glOrtho(-7.5 * zoom, 7.5 * zoom,
        -7.5 / Ratio * zoom, 7.5 / Ratio * zoom, 20.0, -20.0);
    else
        glOrtho(-7.5 * Ratio * zoom, 7.5 * Ratio * zoom,
            -5.5 * zoom, 5.5 * zoom, 20.0, -20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void spin() {
    if (model == 3) timer++;
    if (timer >= 7200) {
        timer = 0;
        tekstura = (tekstura + 1) % 3;
        glBindTexture(GL_TEXTURE_2D, tekstury[tekstura]);
    }
    if (kierunek) {
        alpha[1] += 0.05;
        // rowniez jako licznik liczacy do 7200 cykli
        if (alpha[1] >= 360.0) alpha[1] -= 360.0;
    } else { alpha[0] += 0.05; }
    glutPostRedisplay();
}

void moonInit() {
    glScalef(0.2, 0.2, 0.2);
    glRotatef(5 * alpha[0] + 5 * alpha[1], 0.0, 1.0, 0.0);
    glTranslatef(30.0, 5.0, 0.0);
    glRotatef(alpha[1], 1.0, 0.0, 0.0);
}

void point(int rozmiar, float*** source) {
    int i, j;
    glColor3f(0.75, 0.5, 0.1);
    glBegin(GL_POINTS);
    for (i = 0; i < rozmiar; i++) {
        for (j = 0; j < rozmiar; j++) {
            glVertex3fv(source[i][j]);
        }
    }
    glEnd();
}

void line(int rozmiar, float*** source) {
    int i, j;
    glColor3f(0.1, 0.75, 0.3);
    glBegin(GL_LINES);
    for (i = 0; i < rozmiar; i++) {
        for (j = 0; j < rozmiar - 1; j++) {
            glVertex3fv(source[i][j]);
            glVertex3fv(source[(i + 1) % rozmiar][j]);  // linie pionowe

            glVertex3fv(source[i][j]);
            glVertex3fv(source[i][j + 1]);  // linie poziome
        }
        glVertex3fv(source[i][j]);
        glVertex3fv(source[(i + 1) % rozmiar][j]);
        if (i > 0) {
            glVertex3fv(source[i][j]);
            glVertex3fv(source[rozmiar - i][0]);
            // zlapanie punktow po drugiej stronie
            // dziury w jajku zamiast robienia kinder joy
        }
    }
    glEnd();
}

void triangle(int rozmiar, float*** source, float*** norm) {
    int i, j;
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 1.0);
    for (i = 0; i < rozmiar; i++) {
        for (j = 0; j < rozmiar - 1; j++) {
            glNormal3fv(norm[i][j]);
            glTexCoord2f(0.20, 0.15);
            glVertex3fv(source[i][j]);
            glNormal3fv(norm[(i + 1) % rozmiar][j]);
            glTexCoord2f(0.30, 0.1);
            glVertex3fv(source[(i + 1) % rozmiar][j]);
            glNormal3fv(norm[i][j + 1]);
            glTexCoord2f(0.20, 0.0);
            glVertex3fv(source[i][j + 1]);

            glNormal3fv(norm[i][j + 1]);
            glTexCoord2f(0.8, 0.0);
            glVertex3fv(source[i][j + 1]);
            glNormal3fv(norm[(i + 1) % rozmiar][j]);
            glTexCoord2f(0.6, 0.1);
            glVertex3fv(source[(i + 1) % rozmiar][j]);
            glNormal3fv(norm[(i + 1) % rozmiar][j + 1]);
            glTexCoord2f(0.8, 0.15);
            glVertex3fv(source[(i + 1) % rozmiar][j + 1]);
        }

        if (i > 0) {
            glNormal3fv(norm[i][j]);
            glTexCoord2f(0.20, 0.70);
            glVertex3fv(source[i][j]);
            glNormal3fv(norm[rozmiar - i][0]);
            glTexCoord2f(0.25, 0.75);
            glVertex3fv(source[rozmiar - i][0]);
            glNormal3fv(norm[(i + 1) % rozmiar][j]);
            glTexCoord2f(0.20, 0.80);
            glVertex3fv(source[(i + 1) % rozmiar][j]);

            glNormal3fv(norm[i][0]);
            glTexCoord2f(0.70, 0.70);
            glVertex3fv(source[i][0]);
            glNormal3fv(norm[rozmiar - i][j]);
            glTexCoord2f(0.75, 0.75);
            glVertex3fv(source[rozmiar - i][j]);
            glNormal3fv(norm[(i + 1) % rozmiar][0]);
            glTexCoord2f(0.70, 0.80);
            glVertex3fv(source[(i + 1) % rozmiar][0]);
        }
    }
    glEnd();
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scale();
    gluLookAt(5.0 * dir[0] / hor, 5.0 * dir[1] / ver,
        10.0, 5.0 * dir[0] / hor, 5.0 * dir[1] / ver,
        0.0, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotatef(alpha[0], 0.0, 1.0, 0.0);
    glRotatef(alpha[1], 1.0, 0.0, 0.0);
    switch (model) {
    case 1: {
        glDisable(GL_TEXTURE_2D);
        point(przedzialy, punkty);
        glPopMatrix();
        glPushMatrix();
        moonInit();
        point(przedzialy / 10, ksiezyc);
        glPopMatrix();
        break;
    }
    case 2: {
        glDisable(GL_TEXTURE_2D);
        line(przedzialy, punkty);
        glPopMatrix();
        glPushMatrix();
        moonInit();
        line(przedzialy / 10, ksiezyc);
        glPopMatrix();
        break;
    }
    case 3: {
        glEnable(GL_TEXTURE_2D);
        triangle(przedzialy, punkty, normPoint);
        glPopMatrix();
        glPushMatrix();
        moonInit();
        triangle(przedzialy / 10, ksiezyc, normMoon);
        glPopMatrix();
        break;
    }
    }
    glFlush();
    glutSwapBuffers();
}

void input(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    } else if (key == 'r' || key == 'R') {
        dir[0] = dir[1] = 0;
    } else if (key == 'p' || key == 'P') {
        model = 1;
        alpha[0] = 0.0;
        alpha[1] = 0.0;
    } else if (key == 'l' || key == 'L') {
        model = 2;
        alpha[0] = 0.0;
        alpha[1] = 0.0;
    } else if (key == 't' || key == 'T') {
        timer = 0;
        model = 3;
        alpha[0] = 0.0;
        alpha[1] = 0.0;
    } else if (key == '+') {
        if (zoom > 0.05) zoom -= 0.05;
        // ze wzgledu na brak odpowiedzi scrolla
        // przyblizenie jest realizowane za pomoca klawiszy
    } else if (key == '-') {
        zoom += 0.05;
    } else { kierunek = !kierunek; }
}

// funkcja zwracajaca wspolrzedne myszki zanim kliknie sie przycisk
void mWait(int xM, int yM) {
    x = xM;
    y = yM;
}

// funkcja odpowiadajaca za przesuwanie myszki
void kot(int xM, int yM) {
    dir[0] = xtmp + x - xM;
    dir[1] = ytmp + yM - y;
}

void mysz(int button, int state, int x, int y) {
    if (button == 0) {
        if (state == GLUT_DOWN) {
            glutMotionFunc(kot);
            xtmp = dir[0];
            ytmp = dir[1];
        }
    }
    /*   Niedzialajaca funkcja reakcji na scroll myszki
    if (button == 3 || button == 4)
    {
        if (state == GLUT_UP) zoom -= 0.05;
        else zoom += 0.05;
    } */
}

void skalowanie(GLsizei horizontal, GLsizei vertical) {
    if (vertical == 0) vertical = 1;
    hor = horizontal;
    ver = vertical;
    glViewport(0, 0, horizontal, vertical);
}

void init(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // wnetrze okna wybrano jako czarne, by punkty patrzyly z otchlani
}

void instruckje() {
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

int main() {
    int test = -1;
    while (test) {
        printf("Program rysuje jajko z ");
        printf("podzielonego kwadratu parametrycznego.\n");
        printf("0 konczy proces programu.\n");
        printf("Powiedz ile przedzialow boku parametrycznego wybierasz: ");
        scanf_s("%d", &przedzialy);
        if (!przedzialy) return 0;
        if (przedzialy < 20) {
            printf("Potrzeba wiecej przedzialow! (min. 20)\n");
            system("Pause");
            system("cls");
        } else {
            test = 0;
        }
    }
    punkty = jajo(przedzialy);
    normPoint = normalise(przedzialy);
    ksiezyc = jajo(przedzialy / 10);
    normMoon = normalise(przedzialy / 10);
    instruckje();

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Jajo");
    glutDisplayFunc(draw);
    glutReshapeFunc(skalowanie);
    glutKeyboardFunc(input);
    glutPassiveMotionFunc(mWait);
    glutMouseFunc(mysz);
    glutIdleFunc(spin);
    textureInit();
    glBindTexture(GL_TEXTURE_2D, tekstury[0]);
    init();
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}
