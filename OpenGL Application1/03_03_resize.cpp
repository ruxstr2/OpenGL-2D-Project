/* TRANSFORMARI.
   - pentru a stabili o fereastra de "decupare" intr-o scena 2D putem folosi 
   atat functia glm::ortho, cat si indicarea explicita a transformarilor
   - in exemplu este decupat dreptunghiul delimitat de xmin, xmax, ymin, ymax
*/

#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h

#include "loadShaders.h"

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

//////////////////////////////////////

GLfloat
	WIN_WIDTH = 900, WIN_HEIGHT = 600;

float
	xmin = 0, xmax = 1005, ymin = 0, ymax = 1200; //a = 1005, b = 1200
float
	deltax = xmax - xmin, deltay = ymax - ymin,
	xcenter = (xmin + xmax) * 0.5, ycenter = (ymin + ymax) * 0.5;

GLuint
	VaoId,
	VboId,
	ColorBufferId,
	ProgramId,
	myMatrixLocation,
	matrScaleLocation,
	matrTranslLocation,
	matrRotlLocation,
	codColLocation;
int codCol;

glm::mat4
	myMatrix, resizeMatrix, matrTransl, mScale, mTrans, matrRot, matrTransl1, matrTransl2;

float PI = 3.141592;

void CreateVBO(void)
{
	// varfurile 
	GLfloat Vertices[] = {
		//Background
		1005.0f, 1200.0f, 0.0f, 1.0f,
		-100.0f, 1200.0f, 0.0f, 1.0f,
		-100.0f, -100.0f, 0.0f, 1.0f,
		1005.0f, -100.0f, 0.0f, 1.0f,
		// poligonul convex 
		375.0f, 600.0f, 0.0f, 1.0f,
		300.0f, 600.0f, 0.0f, 1.0f,
		230.0f, 525.0f, 0.0f, 1.0f,
		300.0f, 450.0f, 0.0f, 1.0f,
		// varfuri pentru axe
		//-400.0f, 0.0f, 0.0f, 1.0f,
		//500.0f,  0.0f, 0.0f, 1.0f,
		//0.0f, -200.0f, 0.0f, 1.0f,
		//0.0f, 400.0f, 0.0f, 1.0f,
		// poligonul concav
		800.0f, 500.0f, 0.0f, 1.0f,
		680.0f, 700.0f, 0.0f, 1.0f,
		600.0f, 400.0f, 0.0f, 1.0f,
		680.0f, 500.0f, 0.0f, 1.0f,

		//poligon convez back
		300.0f, 450.0f, 0.0f, 1.0f,
		230.0f, 525.0f, 0.0f, 1.0f,
		300.0f, 600.0f, 0.0f, 1.0f,
		375.0f, 600.0f, 0.0f, 1.0f,

		//poligon concav back
		680.0f, 500.0f, 0.0f, 1.0f,
		600.0f, 400.0f, 0.0f, 1.0f,
		680.0f, 700.0f, 0.0f, 1.0f,
		800.0f, 500.0f, 0.0f, 1.0f,



	};

	// culorile varfurilor din colturi
	GLfloat Colors[] = {
	  1.0f, 0.0f, 0.0f, 1.0f,
	  0.0f, 1.0f, 0.0f, 1.0f,
	  0.0f, 0.0f, 1.0f, 1.0f,
	  0.8f, 0.4f, 0.0f, 1.0f,
	};

	// se creeaza un buffer nou
	glGenBuffers(1, &VboId);
	// este setat ca buffer curent
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	// punctele sunt "copiate" in bufferul curent
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	// 
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// un nou buffer, pentru culoare
	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	// atributul 1 =  culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
}
void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void CreateShaders(void)
{
	ProgramId = LoadShaders("03_03_Shader.vert", "03_03_Shader.frag");
	glUseProgram(ProgramId);
}


void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // culoarea de fond a ecranului
	CreateVBO();
	CreateShaders();
	codColLocation = glGetUniformLocation(ProgramId, "codCuloare");
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

	// Pentru decupare avem de translatat si de scalat
	// Varianta 1 (recomandat). Se aplica functia glm::ortho
	resizeMatrix = glm::ortho(xmin, xmax, ymin, ymax);

	// Varianta 2. Efectuam explicit compunerea transformarilor
	//mScale = glm::scale(glm::mat4(1.0f), glm::vec3(2.f / deltax, 2.f / deltay, 1.0));
	//mTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-xcenter, -ycenter, 0.0));
	// resizeMatrix = mScale * mTrans;
	GLfloat angle = PI / 8;
	matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));
	// Matricea de translatie pentru dreptunghi
	matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(100.f, 100.f, 0.0));
	matrTransl1 = glm::translate(glm::mat4(1.0f), glm::vec3(-515.f, -550.f, 0.0));
	matrTransl2 = glm::translate(glm::mat4(1.0f), glm::vec3(515.f, 550.f, 0.0));
}
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// Background
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	myMatrix = resizeMatrix; // ATENTIE LA ORDINE  * matrTransl
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawArrays(GL_POLYGON, 0, 4);

	//Poligoanele initiale
	codCol = 2;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 4, 4);
	glDrawArrays(GL_QUADS, 8, 4);

	//Poligonul convex transformat
	myMatrix = resizeMatrix * matrTransl2 * matrRot * matrTransl1;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	//
	glLineWidth(4.0);
	glPolygonMode(GL_BACK, GL_LINE);
	glDrawArrays(GL_POLYGON, 12, 4);
	glDrawArrays(GL_POLYGON, 16, 4);


	glutPostRedisplay();
	glFlush();
}
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutCreateWindow("Utilizarea glm::ortho");
	glewInit();
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

