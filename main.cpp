#include <GL/glut.h>
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#define SENS_ROT 5.0
#define SENS_OBS 10.0
#define SENS_TRANSL 10.0

GLfloat distance = 50;
GLfloat azimuth = 0;
GLfloat incidence = 0;
GLfloat twist = 0;
GLfloat angle, fAspect;

GLfloat rotacaoX, rotacaoY, rotacaoX_ini, rotacaoY_ini;

GLfloat observacaoX, observacaoY, obsZ, observacaoX_ini, observacaoY_ini, obsZ_ini;

GLfloat movimentoCarro = 20.f;
GLfloat pXMove = 10.f;
GLfloat pZMove = 0.f;
GLfloat pYMove = 2.f;
GLfloat pXRotate = 0.f;
GLfloat pYRotate = 0.f;
GLfloat alturaPulo = 0.0f, velocidadePulo = 0.0f, cYPos = 0.0f;

bool isJumping = false;
int x_ini, y_ini, mouse_button;

GLuint id, idSol;

void animacaoPulo(int value) {
	if (alturaPulo > 0.0) {
		pYMove = cYPos + alturaPulo;
		velocidadePulo -= 0.005; 
		alturaPulo += velocidadePulo;

		glutTimerFunc(10, animacaoPulo, 0);
	} else {
		pYMove = cYPos;
}

glutPostRedisplay();
}

void polarView() {
	glTranslatef(0.0, 0.0, -distance);
	glRotatef(-twist, 0.0, 0.0, 1.0);
	glRotatef(-incidence, 1.0, 0.0, 0.0);
	glRotatef(-azimuth, 0.0, 0.0, 1.0);
}

void iluminacao() {
	GLfloat luzAmbiente[4] = {0.33, 0.25, 0.15, 1.0};
	GLfloat luzDifusa[4] = {0.5, 0.5, 0.5, 1.0};
	GLfloat luzEspecular[4] = {1.0, 0.1, 0.1, 1.0};
	GLfloat posicaoLuz[4] = {10, 2, 0, 0};
	GLint   spot_luz1         = 30;

	GLfloat especularidade[4] = {1.0, 1.0, 1.0, 1.0};
	GLint especMaterial = 60;

	glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);
	glMateriali(GL_FRONT, GL_SHININESS, especMaterial);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
	glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, spot_luz1);
}

void observation() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	iluminacao();
	glTranslatef(-observacaoX, -observacaoY, -obsZ);
	glRotatef(rotacaoX, 1, 0, 0);
	glRotatef(rotacaoY, 0, 1, 0);
}

void visualization() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angle, fAspect, 1, 700);
	observation();
}

void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		x_ini = x;
		y_ini = y;
		observacaoX_ini = observacaoX;
		observacaoY_ini = observacaoY;
		obsZ_ini = obsZ;
		rotacaoX_ini = rotacaoX;
		rotacaoY_ini = rotacaoY;
		mouse_button = button;
	}

	else mouse_button = -1;
}

void carroMov(int x, int y) {
	if (mouse_button == GLUT_LEFT_BUTTON) {
		int deltax = x_ini - x;
		int deltay = y_ini - y;
		rotacaoY = rotacaoY_ini - deltax / SENS_ROT;
		rotacaoX = rotacaoX_ini - deltay / SENS_ROT;
	} else if (mouse_button == GLUT_RIGHT_BUTTON) {
		int deltaz = y_ini - y;
		obsZ = obsZ_ini + deltaz / SENS_OBS;
	} else if (mouse_button == GLUT_MIDDLE_BUTTON) {
		int deltax = x_ini - x;
		int deltay = y_ini - y;
		observacaoX = observacaoX_ini + deltax / SENS_TRANSL;
		observacaoY = observacaoY_ini - deltay / SENS_TRANSL;
	}

	observation();
	glutPostRedisplay();
}

void Reshape(GLsizei w, GLsizei h) {
	if (h == 0)
		h = 1;

	glViewport(0, 0, w, h);
	fAspect = (GLfloat)w / (GLfloat)h;
	visualization();
}

void display() {
	/* Limpa todos os pixels da tela */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	iluminacao();

	glPushMatrix(); // chao textura
	glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0, -10, 0);
	glEnable(GL_TEXTURE_2D);
    //text chao
    glBindTexture(GL_TEXTURE_2D, id);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-200.0f, 0.0f, -200.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(200.0f, 0.0f, -200.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(200.0f, 0.0f, 200.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-200.0f, 0.0f, 200.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix(); //sol
	glColor3d(1, 1, 0.2);
	glTranslatef(150, 100, -100);
	glutSolidSphere(10, 100, 100);
	glPopMatrix();

	//carro
	glPushMatrix();
    polarView();

	/// Corpo do carro
    glColor3d(0.0, 0.0, 1.0); 
    glTranslatef(movimentoCarro, 0, 0);

    // Caroceria
    glPushMatrix();
    glScalef(4.0, 0.6, 1.5);
    glutSolidCube(15);
    glPopMatrix();

    // Teto
    glPushMatrix();
    glColor3d(0.7, 0.7, 1.0); 
    glTranslatef(-5, 12, 0);
    glScalef(6.666, 0.2, 3); 
    glutSolidCube(7.5);
    glPopMatrix();

    // Janelas
    glColor3d(1.0, 0.7, 1.0);

    //frente
    glPushMatrix();
    glTranslatef(14, 7, 0);
    glRotatef(90, 0, 45, 0);
    glScalef(1.2,0.7, 0.02);
    glutSolidCube(15);
    glPopMatrix();

    //direita
    glPushMatrix();
    glTranslatef(-8, 7.5, 10);
    glRotatef(0, 0, 1, 0);
    glScalef(3, 0.5, 0.02);
    glutSolidCube(15);
    glPopMatrix();

	//esquerda
    glPushMatrix();
    glTranslatef(-8, 7.5, -10);
    glRotatef(0, 0, 1, 0);
    glScalef(3, 0.5, 0.02);
    glutSolidCube(15);
    glPopMatrix();

    // Rodas
    glColor3d(0, 0, 0);

    //traseira direita
    glPushMatrix();
    glTranslatef(-17, -6, 7.5);
    glutSolidTorus(2, 4, 100, 100);
    glPopMatrix();

    //traseira esqeurda
    glPushMatrix();
    glTranslatef(-17, -6, -7.5);
    glutSolidTorus(2, 4, 100, 100);
    glPopMatrix();

    //dianteira direita
    glPushMatrix();
    glTranslatef(17, -6, 7.5);
    glutSolidTorus(2, 4, 100, 100);
    glPopMatrix();

    //dianteira esquerda
    glPushMatrix();
    glTranslatef(17, -6, -7.5);
    glutSolidTorus(2, 4, 100, 100);
    glPopMatrix();

    glPopMatrix(); 

	glPushMatrix(); // lixeira
		glTranslatef(40, -10, 20);
		glRotatef(90, -1, 0, 0);
		glColor3d(0.5, 0.5, 0.5);
		glutSolidCylinder(5, 10, 100, 100);
		glutSolidCylinder(1, 10.5, 100, 100);

		glPushMatrix();
			glTranslatef(0, -6, 10.2);
			glRotatef(90, -1, 0, 0);
			glColor3d(0.5, 0.5, 0.5);
			glutSolidCylinder(6, 12, 2, 2);
		glPopMatrix();
	glPopMatrix();

	glPushMatrix(); // boneco

		// cabeca
		glTranslatef(pXMove, pYMove, pZMove);
		glRotatef(pXRotate, 1.0f, 0.0f, 0.0f); // rotaçao eixo x
		glRotatef(pYRotate, 0.0f, 1.0f, 0.0f); // rotaçao eixo y
		glColor3d(0, 0, 1);
		glutSolidSphere(2, 100, 100);

		// corpo
		glPushMatrix();
			glTranslatef(0, -2, 0);
			glRotatef(90, 50, 0, 0);
			glColor3d(0, 0, 1);
			glutSolidCylinder(2, 5, 40, 40);
		glPopMatrix();

		// parte inferior
		glPushMatrix();
			glTranslatef(-1, -7, 0);
			glRotatef(90, 50, 0, 0);
			glColor3d(0, 0, 1);
			glutSolidCylinder(0.8, 5, 40, 40);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(1, -7, 0);
			glRotatef(90, 50, 0, 0);
			glColor3d(0, 0, 1);
			glutSolidCylinder(0.8, 5, 40, 40);
		glPopMatrix();

		// superior
		glPushMatrix();
			glTranslatef(-1, -2, 0);
			glRotatef(45, 0, 0, 0);
			glColor3d(0, 0, 1);
			glutSolidCylinder(0.5, 5, 40, 40);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(1, -2, 0);
			glRotatef(45, 0, 0, 0);
			glColor3d(0, 0, 1);
			glutSolidCylinder(0.5, 5, 40, 40);
		glPopMatrix();

	glPopMatrix();
	glutSwapBuffers();
}

void myReshape(int winWidth, int winHeight) {
	int tam = winWidth < winHeight ? winWidth : winHeight;
	glViewport(0, 0, tam, tam); // reset the viewport
}

/*comandos especiais do teclado*/
void keySpecial(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		movimentoCarro -= 2.f;
		break;
	case GLUT_KEY_RIGHT:
		movimentoCarro += 2.f;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void myKeyboard(unsigned char c, int x, int y) {
	switch (c) {
	case 27: // esc em ascii
		exit(0);
		break;
	case 'q':
		pXRotate += 5.f;
		break;
	case 'e':
		pXRotate -=5.f;
		break;
	case 'z':
		pYRotate += 5.f;
		break;
	case 'c':
		pYRotate -= 5.f;
		break;
	case 'd':
		pXMove += 5.f;
		break;
	case 'a':
		pXMove -= 5.f;
		break;
	case 'w':
		pZMove += 5.f;
		break;
	case 's':
		pZMove -= 5.f;
		break;
	case 32: //espaco em ascii
		if (alturaPulo == 0.0) {
			cYPos = pYMove;
			alturaPulo = 1.0;
			velocidadePulo = 0.1;
			animacaoPulo(0);
		}
		else{
			alturaPulo = 0.0;
			animacaoPulo(0);
		}
			break;
	default:
		break;
	}

	glutPostRedisplay();
}

// imprimir as coordenadas com o clique do mouse
void myMouse(int b, int s, int x, int y) {
	switch (b) {
	case GLUT_LEFT_BUTTON:
		printf("(%d, %d)\n", x, y);
	}
}

// função detecta mouse e salva coordenadas
void myMotion(int x, int y) {
	printf("Motion: (%d, %d)\n", x, y);
}

void init() {
	// cor fundo
	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

    SDL_Surface* image = IMG_Load("piso.jpeg");

	// ativa textura
    glEnable(GL_TEXTURE_2D);

    // gerando id textura
    glGenTextures(1, &id);

    // carrega e aplica a textura das pedrinha
    glBindTexture(GL_TEXTURE_2D, id);

    glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);

	// filtro
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// descobre formato da imagem
	GLint format = image->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA;

	// carrega imagem disco
	glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);

	SDL_FreeSurface(image);

	angle = 90;
	rotacaoX = 0;
	rotacaoY = 10;
	observacaoX = 20;
	observacaoY = 0;
	obsZ = 100;
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(5, 5);
	glutInitWindowSize(450, 450);
	glutCreateWindow("Trabalho carro uhul");
	glutDisplayFunc(display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(keySpecial);
	glutMouseFunc(mouse);
	glutMotionFunc(carroMov);
	init();
	glutMainLoop();

	return 0;
}