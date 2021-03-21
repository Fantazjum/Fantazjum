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

#define PI 3.14159265359

GLsizei hor, ver;
GLfloat alpha[2] = { 0.0, 0.0 };
GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
double zoom = 1.0;
int x, y, xtmp, ytmp, przedzialy = 1;
int dir[2];
INT16 model = 1;
INT16 col = 0;
bool kierunek = false;
bool left = false;
float*** punkty;
float*** normPoint;
float*** ksiezyc;

float*** jajo(int rozmiar) {
    int i, j;
    double v, u = 0.0, size = 1.0 / rozmiar;
    float*** chmura;
    chmura = new float**[rozmiar];
    for (i = 0; i < rozmiar; i++) {
        chmura[i] = new float*[rozmiar];
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
    if (kierunek) {
        alpha[1] += 0.05;
        if (alpha[1] >= 360.0) alpha[1] -= 360.0;
    } else {
        alpha[0] += 0.05;
        if (alpha[0] >= 360.0) alpha[0] -= 360.0;
    }
    glutPostRedisplay();
}

void moonInit() {
    glScalef(0.2, 0.2, 0.2);
    glRotatef(5 * alpha[0] + 5 * alpha[1], 0.0, 1.0, 0.0);
    glTranslatef(30.0, 5.0, 0.0);
    glRotatef(alpha[1], 1.0, 0.0, 0.0);
}

// parametry wykorzystywanego materiału
void materialism() {
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess = { 20.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
}

// parametry oswietlenia uzywanego w rysowaniu
void lightning() {
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat att_constant = { 1.0 };
    GLfloat att_linear = { 0.05 };
    GLfloat att_quadratic = { 0.001 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 90);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);
}

// oswietlenie sterowane myszka - kolory teczy
void pointLight() {
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    switch (col) {
    case 0:  // fioletowy
        light_ambient[0] = 0.15;
        light_ambient[1] = 0.1;
        light_ambient[2] = 0.2;
        light_diffuse[0] = 0.4;
        light_diffuse[1] = 0.1;
        light_diffuse[2] = 0.6;
        light_specular[0] = 0.4;
        light_specular[1] = 0.1;
        light_specular[2] = 0.6;
        break;
    case 1:  // indygo
        light_ambient[0] = 0.1;
        light_ambient[1] = 0.0;
        light_ambient[2] = 0.25;
        light_diffuse[0] = 0.2;
        light_diffuse[1] = 0.1;
        light_diffuse[2] = 0.9;
        light_specular[0] = 0.2;
        light_specular[1] = 0.1;
        light_specular[2] = 0.9;
        break;
    case 2:  // niebieski
        light_ambient[0] = 0.1;
        light_ambient[1] = 0.1;
        light_ambient[2] = 0.2;
        light_diffuse[0] = 0.2;
        light_diffuse[1] = 0.3;
        light_diffuse[2] = 0.8;
        light_specular[0] = 0.2;
        light_specular[1] = 0.3;
        light_specular[2] = 0.8;
        break;
    case 3:  // zielony
        light_ambient[0] = 0.1;
        light_ambient[1] = 0.2;
        light_ambient[2] = 0.1;
        light_diffuse[0] = 0.3;
        light_diffuse[1] = 0.8;
        light_diffuse[2] = 0.3;
        light_specular[0] = 0.3;
        light_specular[1] = 0.8;
        light_specular[2] = 0.3;
        break;
    case 4:  // zolty
        light_ambient[0] = 0.2;
        light_ambient[1] = 0.2;
        light_ambient[2] = 0.0;
        light_diffuse[0] = 0.7;
        light_diffuse[1] = 0.7;
        light_diffuse[2] = 0.1;
        light_specular[0] = 0.7;
        light_specular[1] = 0.7;
        light_specular[2] = 0.1;
        break;
    case 5:  // pomaranczowy
        light_ambient[0] = 0.2;
        light_ambient[1] = 0.15;
        light_ambient[2] = 0.1;
        light_diffuse[0] = 0.9;
        light_diffuse[1] = 0.6;
        light_diffuse[2] = 0.2;
        light_specular[0] = 0.9;
        light_specular[1] = 0.6;
        light_specular[2] = 0.2;
        break;
    case 6:  // czerwony
        light_ambient[0] = 0.2;
        light_ambient[1] = 0.1;
        light_ambient[2] = 0.1;
        light_diffuse[0] = 0.7;
        light_diffuse[1] = 0.2;
        light_diffuse[2] = 0.2;
        light_specular[0] = 0.7;
        light_specular[1] = 0.2;
        light_specular[2] = 0.2;
        break;
    }
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
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
    glColor3f(0.33, 0.33, 0.33);
    for (i = 0; i < rozmiar; i++) {
        for (j = 0; j < rozmiar - 1; j++) {
            glNormal3fv(norm[i][j]);
            glVertex3fv(source[i][j]);
            glNormal3fv(norm[(i + 1) % rozmiar][j]);
            glVertex3fv(source[(i + 1) % rozmiar][j]);
            glNormal3fv(norm[i][j + 1]);
            glVertex3fv(source[i][j + 1]);

            glNormal3fv(norm[i][j + 1]);
            glVertex3fv(source[i][j + 1]);
            glNormal3fv(norm[(i + 1) % rozmiar][j]);
            glVertex3fv(source[(i + 1) % rozmiar][j]);
            glNormal3fv(norm[(i + 1) % rozmiar][j + 1]);
            glVertex3fv(source[(i + 1) % rozmiar][j + 1]);
        }

        if (i > 0) {
            glNormal3fv(norm[i][j]);
            glVertex3fv(source[i][j]);
            glNormal3fv(norm[rozmiar - i][0]);
            glVertex3fv(source[rozmiar - i][0]);
            glNormal3fv(norm[(i + 1) % rozmiar][j]);
            glVertex3fv(source[(i + 1) % rozmiar][j]);

            glNormal3fv(norm[i][0]);
            glVertex3fv(source[i][0]);
            glNormal3fv(norm[rozmiar - i][j]);
            glVertex3fv(source[rozmiar - i][j]);
            glNormal3fv(norm[(i + 1) % rozmiar][0]);
            glVertex3fv(source[(i + 1) % rozmiar][0]);
        }
    }
    glEnd();
}

void draw() {
    double azym, elew;
    azym = static_cast<double>(x) / ver * 2 * PI;
    elew = static_cast<double>(y) / hor * 2 * PI;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    light_position[0] = 7.0 * cos(azym) * cos(elew);
    light_position[1] = 7.0 * sin(elew);
    light_position[2] = 7.0 * sin(azym) * cos(elew);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glPopMatrix();
    scale();
    gluLookAt(5.0 * dir[0] / hor, 5.0 * dir[1] / ver,
        10.0, 5.0 * dir[0] / hor, 5.0 * dir[1] / ver,
        0.0, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotatef(alpha[0], 0.0, 1.0, 0.0);
    glRotatef(alpha[1], 1.0, 0.0, 0.0);
    switch (model) {
        case 1: {
            glDisable(GL_LIGHTING);
            point(przedzialy, punkty);
            glPopMatrix();
            glPushMatrix();
            moonInit();
            point(przedzialy / 10, ksiezyc);
            glPopMatrix();
            break;
        }
        case 2: {
            glDisable(GL_LIGHTING);
            line(przedzialy, punkty);
            glPopMatrix();
            glPushMatrix();
            moonInit();
            line(przedzialy / 10, ksiezyc);
            glPopMatrix();
            break;
        }
        case 3: {
            glEnable(GL_LIGHTING);
            triangle(przedzialy, punkty, normPoint);
            glPopMatrix();
            glPushMatrix();
            glRotatef(alpha[0]*5 + 5*alpha[1], 0.0, 1.0, 0.0);
            GLfloat light_pos[] = { 1.0, 1.0, 10.0, 0.0 };
            glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
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
    } else if (key == 'c' || key == 'C') {
        if (col == 6)
            col = 0;
        else
            col++;
        pointLight();
    } else if (key == 'l' || key == 'L') {
        model = 2;
        alpha[0] = 0.0;
        alpha[1] = 0.0;
    } else if (key == 't' || key == 'T') {
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
    // wykorzystywane swiatlo
    pointLight();
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
}

void instruckje() {
    printf("Dostepne akcje:\n");
    printf("LPM - zacznij przesuwac obiekt\n");
    printf("C - zmien kolor swiatla z myszki\n");
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
    ksiezyc = jajo(przedzialy / 10);
    normPoint = normalise(przedzialy);
    instruckje();
    materialism();
    lightning();
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
    glutMainLoop();
    return 0;
}
