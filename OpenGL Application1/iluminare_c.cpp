/*
Desenarea a doua obiecte folosind matrice/buffere diferite pentru varfuri/culori/indici.
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

// identificatori 
GLuint
VaoId1, VaoId2,
VboId1, VboId2,
EboId1, EboId2,
ProgramId,
viewLocation,
projLocation,
codColLocation,
ColorBufferId,
myMatrixLocation,
matrUmbraLocation,
matrRotlLocation,
lightColorLocation,
lightPosLocation,
viewPosLocation;

float const PI = 3.141592f;
float const U_MIN = -PI / 2, U_MAX = PI / 2, V_MIN = 0, V_MAX = 2 * PI;
int const NR_PARR = 10, NR_MERID = 20;
float step_u = (U_MAX - U_MIN) / NR_PARR, step_v = (V_MAX - V_MIN) / NR_MERID;

// alte variabile
int codCol;
float radius = 50;
int index, index_aux;
//Cilindru
float radius2 = 30;

float const U_MIN2 = 0, U_MAX2 = radius2, V_MIN2 = -PI, V_MAX2 = PI;
// (2) numarul de paralele/meridiane, de fapt numarul de valori ptr parametri
int const NR_PARR2 = 20, NR_MERID2 = 20;
// (3) pasul cu care vom incrementa u, respectiv v
float step_u2 = (U_MAX2 - U_MIN2) / NR_PARR2, step_v2 = (V_MAX2 - V_MIN2) / NR_MERID2;

// variabile pentru matricea de vizualizare
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = 0.0f, beta = 0.0f, dist = 300.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0f, Vy = 0.0f, Vz = -1.0f;
float incr_alpha1 = 0.01f, incr_alpha2 = 0.01f;

// variabile pentru matricea de proiectie
float width = 800, height = 600, znear = 1, fov = 30;

// pentru fereastra de vizualizare 
GLint winWidth = 1000, winHeight = 600;

// sursa de lumina
float xL = 500.f, yL = 100.f, zL = 400.f;

// matricea umbrei
float matrUmbra[4][4];

// vectori
glm::vec3 Obs, PctRef, Vert;

// matrice utilizate
glm::mat4 view, projection;
// matrice utilizate
glm::mat4 myMatrix, matrRot;

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
		beta -= 0.01f;
		break;
	case GLUT_KEY_RIGHT:
		beta += 0.01f;
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
void CreateVAO1(void)
{
	// SFERA
	// Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices1[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors1[(NR_PARR + 1) * NR_MERID];
	GLushort Indices1[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u; // valori pentru u si v
			float v = V_MIN + merid * step_v;
			float x_vf = radius * cosf(u) * cosf(v); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius * cosf(u) * sinf(v);
			float z_vf =40.0f +2*radius + radius * sinf(u);

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices1[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			/*Colors1[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));*/
			Colors1[index] = glm::vec3(0.1f, 0.4f, 0.1f);
			Indices1[index] = index;

			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices1[(NR_PARR + 1) * NR_MERID + index_aux] = index;

			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices1[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices1[AUX + 4 * index + 1] = index2;
				Indices1[AUX + 4 * index + 2] = index3;
				Indices1[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId1);
	glBindVertexArray(VaoId1);
	glGenBuffers(1, &VboId1); // atribute
	glGenBuffers(1, &EboId1); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices1) + sizeof(Colors1), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices1), Vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices1), sizeof(Colors1), Colors1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices1), Indices1, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices1)));
}

void CreateVAO2(void)
{
	//// CUBUL 
	//// 
	//GLfloat Vertices2[] =
	//{
	//	-20.0f, -20.0f, 50.0f, 1.0f,
	//	 20.0f, -20.0f, 50.0f, 1.0f,
	//	 20.0f,  20.0f, 50.0f, 1.0f,
	//	-20.0f,  20.0f, 50.0f, 1.0f,
	//	-20.0f, -20.0f, 90.0f, 1.0f,
	//	 20.0f, -20.0f, 90.0f, 1.0f,
	//	 20.0f,  20.0f, 90.0f, 1.0f,
	//	-20.0f,  20.0f, 90.0f, 1.0f
	//};

	//GLfloat Colors2[] =
	//{
	//	0.8f, 0.8f, 0.8f,
	//	0.7f, 0.7f, 0.7f,
	//	0.6f, 0.6f, 0.6f,
	//	0.5f, 0.5f, 0.5f,
	//	0.4f, 0.4f, 0.4f,
	//	0.3f, 0.3f, 0.3f,
	//	0.2f, 0.2f, 0.2f,
	//	0.1f, 0.1f, 0.1f
	//};

	//GLushort Indices2[] =
	//{
	//  1, 2, 0,   2, 0, 3,
	//  2, 3, 6,   6, 3, 7,
	//  7, 3, 4,   4, 3, 0,
	//  4, 0, 5,   5, 0, 1,
	//  1, 2, 5,   5, 2, 6,
	//  5, 6, 4,   4, 6, 7,
	//};
	// CILINDRU 
	// (4) Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices2[(NR_PARR2 + 1) * NR_MERID2];
	glm::vec3 Colors2[(NR_PARR2 + 1) * NR_MERID2];
	GLushort Indices2[2 * (NR_PARR2 + 1) * NR_MERID2 + 4 * (NR_PARR2 + 1) * NR_MERID2];
	for (int merid = 0; merid < NR_MERID2; merid++)
	{
		for (int parr = 0; parr < NR_PARR2 + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN2 + parr * step_u2; // valori pentru u si v
			float v = V_MIN2 + merid * step_v2;
			float x_vf = 0.3 * radius2 * cosf(v); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = 0.3 * radius2 * sinf(v);
			float z_vf = 3*(radius2 - u);
			//float z_vf = u;

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR2 + 1) + parr;
			Vertices2[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			//Colors[index] = glm::vec3(0.1f + u/radius, 0.1f + cosf(v), 0.1f + -1.5 * u/radius);
			Colors2[index] = glm::vec3(0.58f, 0.29f , 0.01f);
			Indices2[index] = index;

			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID2)+merid;
			Indices2[(NR_PARR2 + 1) * NR_MERID2 + index_aux] = index;

			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR2 + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR2 + 1) * NR_MERID2;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR2 + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID2 - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR2 + 1);
					index3 = index3 % (NR_PARR2 + 1);
				}
				Indices2[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices2[AUX + 4 * index + 1] = index2;
				Indices2[AUX + 4 * index + 2] = index3;
				Indices2[AUX + 4 * index + 3] = index4;
			}
		}
	};
	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId2);
	glBindVertexArray(VaoId2);
	glGenBuffers(1, &VboId2); // atribute
	glGenBuffers(1, &EboId2); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2) + sizeof(Colors2), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices2), sizeof(Colors2), Colors2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices2), Indices2, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices2)));
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId1);
	glDeleteBuffers(1, &EboId1);
	glDeleteBuffers(1, &VboId2);
	glDeleteBuffers(1, &EboId2);
}
void CreateShaders(void)
{
	ProgramId = LoadShaders("iluminare_c.vert", "09_03_Shader.frag");
	glUseProgram(ProgramId);
}
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}
void Initialize(void)
{
	myMatrix = glm::mat4(1.0f);
	matrRot = glm::rotate(glm::mat4(1.0f), PI / 8, glm::vec3(0.0, 0.0, 1.0));
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	CreateVAO1();
	CreateVAO2();
	CreateShaders();
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	matrUmbraLocation = glGetUniformLocation(ProgramId, "matrUmbra");
	viewLocation = glGetUniformLocation(ProgramId, "viewShader");
	projLocation = glGetUniformLocation(ProgramId, "projectionShader");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	lightColorLocation = glGetUniformLocation(ProgramId, "lightColor");
	lightPosLocation = glGetUniformLocation(ProgramId, "lightPos");
	viewPosLocation = glGetUniformLocation(ProgramId, "viewPos");
}
void reshapeFcn(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	winWidth = newWidth;
	winHeight = newHeight;
	width = winWidth / 10, height = winHeight / 10;
}

void setMVP(void)
{
	//pozitia observatorului
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	// reperul de vizualizare
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);   // se schimba pozitia observatorului	
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz); // pozitia punctului de referinta
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz); // verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// matricea de proiectie 
	projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	setMVP();

	// SFERA
	glBindVertexArray(VaoId1);
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}
	
	// CILINDRUL
	glBindVertexArray(VaoId2);
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	for (int patr = 0; patr < (NR_PARR2 + 1) * NR_MERID2; patr++)
	{
		if ((patr + 1) % (NR_PARR2 + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR2 + 1) * (NR_MERID2)+4 * patr) * sizeof(GLushort)));
	}
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (GLvoid*)(0));

	float D = -0.5f;
	matrUmbra[0][0] = zL + D; matrUmbra[0][1] = 0; matrUmbra[0][2] = 0; matrUmbra[0][3] = 0;
	matrUmbra[1][0] = 0; matrUmbra[1][1] = zL + D; matrUmbra[1][2] = 0; matrUmbra[1][3] = 0;
	matrUmbra[2][0] = -xL; matrUmbra[2][1] = -yL; matrUmbra[2][2] = D; matrUmbra[2][3] = -1;
	matrUmbra[3][0] = -D * xL; matrUmbra[3][1] = -D * yL; matrUmbra[3][2] = -D * zL; matrUmbra[3][3] = zL;
	glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);
	//umbra cilindru
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	for (int patr = 0; patr < (NR_PARR2 + 1) * NR_MERID2; patr++)
	{
		if ((patr + 1) % (NR_PARR2 + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR2 + 1) * (NR_MERID2)+4 * patr) * sizeof(GLushort)));
	}

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
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Doua obiecte");
	glewInit();
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(RenderFunction);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutReshapeFunc(reshapeFcn);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}