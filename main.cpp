#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
//do wczytania pliku
#include "imageloader.h"
//do poruszanie
#define FLY		1

///Wszystkie zmienne odnosza sie do poruszania kamera
int navigationMode = FLY;
float angle=0.0,deltaAngle = 0.0,ratio;
float angle2,angle2Y,angleY;
static int deltaX=-1000,deltaY;
float x=0.0f,y=1.75f,z=5.0f;
float lx=0.0f,ly=0.0f,lz=-1.0f,deltaMove=0.0;
int w1=800,h1=600;
int w=800,h=600;
///

using namespace std;

//klasa terenu, pomoze w uzyskaniu takich danych jak wysokosc, szerokosc, zwraca wysokosc
class Teren {
	private:
		int w; //szerokosc
		int l; //dlugosc
		float** hs; //wysokosc

	public:
		Teren(int w2, int l2)
		{
			w = w2;
			l = l2;

			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}
		}
		//pomocnicze zwrocenie
		int width() {return w;}
		int length() {return l;}

		//ustawienie wysokosc jako (x, z) do y
		void setHeight(int x, int z, float y)
		{
			hs[z][x] = y;
		}

		//pobranie wysokosci  (x, z) i zwrocenie
		float getHeight(int x, int z)
		{
			return hs[z][x];
		}
};

//glowne wczytanie
Teren* Laduj(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Teren* ter = new Teren(image->width, image->height);
	cout << "Rozmiar y: "<<  image->height << endl;
	cout << "Rozmiar x: "<< image->width << endl;
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color = (unsigned char)image->pixels[3 * (y * image->width + x)]; //dowiadujemy sie jaki kolor maja nasze piksele
			float z = height * ((color / 255.0f) - 0.5f); //odcien szarosci
			//cout << "y: "<< y << "x: " << x << "z: " << z << endl;
			ter->setHeight(x, y, z);
		}
	}
	return ter;
}

Teren* _teren;

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			delete _teren;
			exit(0);
        case 13: //enter
            break;
	}
}

void pressKey(int key, int x, int y) {

	switch (key) {
	    //druciak
	    case GLUT_KEY_F1 :
            glPolygonMode(GL_FRONT, GL_LINE);
            glPolygonMode(GL_BACK, GL_LINE);
	        break;
	        //nie druciak XD
	    case GLUT_KEY_F2 :
            glPolygonMode(GL_FRONT, GL_FILL);
            glPolygonMode(GL_BACK, GL_FILL);
	        break;
		case GLUT_KEY_LEFT : deltaAngle = -0.005f;break;
		case GLUT_KEY_RIGHT : deltaAngle = 0.005f;break;
		case GLUT_KEY_UP :
			if (navigationMode == FLY)
				deltaMove = 1;
			else
				deltaMove = 0.1;
			break;
		case GLUT_KEY_DOWN :
			if (navigationMode == FLY)
				deltaMove = -1;
			else
				deltaMove = -0.1;
			break;
	}
}

void releaseKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT : if (deltaAngle < 0.0f)
								 deltaAngle = 0.0f;
							 break;
		case GLUT_KEY_RIGHT : if (deltaAngle > 0.0f)
								 deltaAngle = 0.0f;
							 break;
		case GLUT_KEY_UP :	 if (deltaMove > 0)
								 deltaMove = 0;
							 break;
		case GLUT_KEY_DOWN : if (deltaMove < 0)
								 deltaMove = 0;
							 break;
	}
}

void activeMouseMotion(int x, int y)
{
		angle2 = angle + (x-deltaX)*0.001;
		angle2Y = angleY + (y-deltaY) * 0.001;
		if (angle2Y > 1.57)
			angle2Y = 1.57;
		else if (angle2Y < -1.57)
			angle2Y = -1.57;
		lx = cos(angle2Y)*sin(angle2);
		lz = -cos(angle2Y)*cos(angle2);
		ly = -sin(angle2Y);
}

void mousePress(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) {
//		angle2 = 0;
		deltaX = x;
		deltaY = y;
//		angle2Y = 0;
		navigationMode = FLY;
	}
	else if (state == GLUT_UP) {
		angleY = angle2Y;
		angle = angle2;
		//navigationMode = FLY;
	}
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//domyslna normalizacja
	glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);
	glShadeModel(GL_SMOOTH);
}

void changeSize(int w1, int h1)
	{
	if(h1 == 0)
		h1 = 1;

	w = w1;
	h = h1;
	ratio = 1.0f * w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0, 0, w, h);
	gluPerspective(60,ratio,0.1,10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, x + lx,y + ly,z + lz, 0.0f,1.0f,0.0f);
}

void init() {
	glutIgnoreKeyRepeat(1);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	glutMotionFunc(activeMouseMotion);
	glutMouseFunc(mousePress);
	glutReshapeFunc(changeSize);
}

void orientMe(float ang) // lewo, prawo
{
	lx = sin(ang);
	lz = -cos(ang);
}

void moveMeFlat(float i)
{
	x = x + i*lx*5;
	z = z + i*lz*5;
	y = y + i*ly*5;
}

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	if (deltaMove)
		moveMeFlat(deltaMove);
	if (deltaAngle) {
		angle += deltaAngle;
		orientMe(angle);
	}
	glLoadIdentity();
    //do latania kamery
	gluLookAt(x, y, z,x + 10*lx,y + 10*ly,z + 10*lz,0.0f,1.0f,0.0f);

	glScalef(1, 1, 1);
	glTranslatef(-(float)(_teren->width() - 1) / 2,-15.0f,-(float)(_teren->length() - 1) / 2); //zeby bylo widac na poczatku

	for(int z = 0; z < _teren->length() - 1; z++) {
		//rysowanie
		glBegin(GL_TRIANGLE_STRIP); //jak trainagle tylko ze laczony jest caly czas //Line stripem ladna siatka
		for(int x = 0; x < _teren->width(); x++) {
//warunki kolorow
            if(_teren->getHeight(x,z+1)>3 && _teren->getHeight(x,z+1)<8)glColor3f(0,125,0); //sred
            else if(_teren->getHeight(x,z)<3) glColor3f(0,0,125);  //nis
            else if(_teren->getHeight(x,z+1)>8) glColor3f(125,0,0); //naj

            //wierzcholek 1 dla triangle stripa
			glVertex3f(x, _teren->getHeight(x, z), z);

			 if(_teren->getHeight(x,z+1)>3 && _teren->getHeight(x,z+1)<8)glColor3f(0,125,0); //sred
			 else if(_teren->getHeight(x,z+1)<3) glColor3f(0,0,125); //nis
			 else if(_teren->getHeight(x,z+1)>8) glColor3f(125,0,0); //naj

			  //wierzcholek 2 dla triangle stripa +1
			glVertex3f(x, _teren->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}
	glutSwapBuffers();
}

void update(int value) {
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Teren");
	cout << "Sterowanie: " << endl;
    cout << "Mysz + strzalki - Kamera" << endl;
    cout << "F1 - Druciak" << endl;
    cout << "F2 - Wypelnienie" << endl;
    cout << "ESC - Wyjscie" << endl << endl;
	initRendering();
	_teren = Laduj("1.bmp", 40); //2 moneta
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutTimerFunc(25, update, 0);
    init();
	glutMainLoop();
	return 0;
}
