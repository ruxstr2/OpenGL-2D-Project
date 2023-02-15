/*
- Survolarea unui obiect folosind coordonate sferice.
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
using namespace std;

#define INSTANCE_COUNT 40
float const PI = 3.141592;

// identificatori 
GLuint
	//VaoId,
	VboId,
	//EboId,
	//ProgramId,
	//viewLocation,
	//projLocation,
	//codColLocation;
	VaoId,
	//VBPos,
	VBCol,
	VBModelMat,
	EboId,
	ColorBufferId,
	ProgramId,
	viewLocation,
	projLocation,
	codColLocation,
	codCol;

// variabile
//int codCol;

// variabile pentru matricea de vizualizare
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = 0.0f, beta = 0.0f, dist = 200.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0f, Vy = 0.0f, Vz = 1.0f;
float incr_alpha1 = 0.01, incr_alpha2 = 0.01;

// variabile pentru matricea de proiectie
float 
	width = 800, height = 600, 
	znear = 1.f, fov = 90.f*PI/180;

// vectori
glm::vec3 Obs, PctRef, Vert;

// matrice utilizate
glm::mat4 view, projection;

void processNormalKeys(unsigned char key, int x, int y)
{
	switch (key) {
	case '-':
		dist -= 5.0;
		break;
	case '+':
		dist += 5.0;
		break;
	}
	if (key == 27)
		exit(0);
}
void processSpecialKeys(int key, int xx, int yy)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		beta -= 0.01;
		break;
	case GLUT_KEY_RIGHT:
		beta += 0.01;
		break;
	case GLUT_KEY_UP:
		alpha += incr_alpha1;
		if (abs(alpha - PI / 2) < 0.05)
		{
			incr_alpha1 = 0.f;
		}
		else
		{
			incr_alpha1 = 0.01f;
		}
		break;
	case GLUT_KEY_DOWN:
		alpha -= incr_alpha2;
		if (abs(alpha + PI / 2) < 0.05)
		{
			incr_alpha2 = 0.f;
		}
		else
		{
			incr_alpha2 = 0.01f;
		}
		break;
	}
}
void CreateVBO(void)
{
	// varfurile 
	GLfloat Vertices[] =
	{
		// varfurile (~verzi) din planul z=-50  
		// coordonate                   // culori			
		-50.0f,  -50.0f, -50.0f, 1.0f,
		50.0f,  -50.0f,  -50.0f, 1.0f,
		50.0f,  50.0f,  -50.0f, 1.0f,
		-50.0f,  50.0f, -50.0f, 1.0f,
		// varfurile (~rosii) din planul z=+50  
		// coordonate                   // culori
		15.0f,  10.0f, 50.0f, 1.0f, //vf piramida
	};

	// indicii pentru varfuri
	GLubyte Indices[] =
	{
		1, 0, 2,   2, 0, 3,  //  Fata "de jos"
		4, 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, //Laterale
		0, 1, 2, 3,  // Contur fata de jos
		4, 0, 4, 1, 4, 2, 4, 3, //Muchii laterale
	};

	// Culorile instantelor
	glm::vec4 Colors[INSTANCE_COUNT];
	for (int n = 0; n < INSTANCE_COUNT; n++)
	{
		float a = float(n) / 4.0f;
		float b = float(n) / 5.0f;
		float c = float(n) / 6.0f;
		Colors[n][0] = 0.35f + 0.30f * (sinf(a + 2.0f) + 1.0f);
		Colors[n][1] = 0.25f + 0.25f * (sinf(b + 3.0f) + 1.0f);
		Colors[n][2] = 0.25f + 0.35f * (sinf(c + 4.0f) + 1.0f);
		Colors[n][3] = 1.0f;
	}

	// Matricele instantelor
	glm::mat4 MatModel[INSTANCE_COUNT];
	for (int n = 0; n < INSTANCE_COUNT; n++)
	{
		MatModel[n] =
			glm::translate(glm::mat4(1.0f),
				glm::vec3(
					80 * n * cos(10.f * n * 180 / PI),
					40 * n * tan(10.f * n * 180 / PI),
					2 * n))
			* glm::rotate(glm::mat4(1.0f), n * PI / 8, glm::vec3(n, 2 * n * n, n / 3));
	}

	// generare VAO/buffere
	glGenBuffers(1, &VboId); // atribute
	glGenBuffers(1, &EboId); // indici
	///
	glGenVertexArrays(1, &VaoId);
	glGenBuffers(1, &VBCol);
	glGenBuffers(1, &VBModelMat);

	// legarea VAO 
	glBindVertexArray(VaoId);

	//// legare+"incarcare" buffer
	//glBindBuffer(GL_ARRAY_BUFFER, VboId);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW); // "copiere" in bufferul curent
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW); // "copiere" indici in bufferul curent

	//// se activeaza lucrul cu atribute; 
	//glEnableVertexAttribArray(0); // atributul 0 = pozitie
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(1); // atributul 1 = culoare
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
		// 0: Pozitie
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	// 1: Culoare
	glBindBuffer(GL_ARRAY_BUFFER, VBCol); // legare buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);  // rata cu care are loc distribuirea culorilor per instanta

	// 2..5 (2+i): Matrice de pozitie
	glBindBuffer(GL_ARRAY_BUFFER, VBModelMat);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MatModel), MatModel, GL_STATIC_DRAW);
	for (int i = 0; i < 4; i++) // Pentru fiecare coloana
	{
		glEnableVertexAttribArray(2 + i);
		glVertexAttribPointer(2 + i,              // Location
			4, GL_FLOAT, GL_FALSE,                // vec4
			sizeof(glm::mat4),                    // Stride
			(void*)(sizeof(glm::vec4) * i));      // Start offset
		glVertexAttribDivisor(2 + i, 1);
	}

	// Indicii 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}
void DestroyVBO(void)
{
	//glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDeleteBuffers(1, &VboId);
	//glDeleteBuffers(1, &EboId);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &VBCol);
	glDeleteBuffers(1, &VBModelMat);
	glDeleteBuffers(1, &EboId);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}
void CreateShaders(void)
{
	ProgramId = LoadShaders("07_02_Shader.vert", "07_02_Shader.frag");
	glUseProgram(ProgramId);
}
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}
void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // culoarea de fond a ecranului

	// Creare VBO+shader
	CreateVBO();
	CreateShaders();

	// Locatii ptr shader
	viewLocation = glGetUniformLocation(ProgramId, "viewShader");
	projLocation = glGetUniformLocation(ProgramId, "projectionShader");
	codColLocation = glGetUniformLocation(ProgramId, "codColShader");
}
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// CreateVBO(); // decomentati acest rand daca este cazul 
	glBindVertexArray(VaoId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBindBuffer(GL_ARRAY_BUFFER, VBModelMat);

	//pozitia observatorului - se deplaseaza pe sfera
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	// reperul de vizualizare
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);   // se schimba pozitia observatorului	
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz); // pozitia punctului de referinta
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz); // verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// matricea de proiectie, pot fi testate si alte variante
	projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	// Fetele
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	glDrawElementsInstanced(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, 0, INSTANCE_COUNT);

	// Muchiile
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glLineWidth(3.5);
	glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(18), INSTANCE_COUNT);
	//glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(40), INSTANCE_COUNT);
	glDrawElementsInstanced(GL_LINES, 8, GL_UNSIGNED_BYTE, (void*)(22), INSTANCE_COUNT);

	glutSwapBuffers();
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
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1200, 900);
	glutCreateWindow("Survolarea unui cub");
	glewInit();
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(RenderFunction);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

