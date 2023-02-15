/* INDEXARE
Elemente de noutate:
   - folosirea indexarii varfurilor: elemente asociate (matrice, buffer)
   - desenarea se face folosind functia glDrawElements()
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
#include <SOIL.h>

using namespace std;

//////////////////////////////////////

GLuint
	VaoId,
	VboId,
	EboId,
	ColorBufferId,
	ProgramId,
	myMatrixLocation,
	codColLocation;
GLuint texture;

int codCol;
float width = 80.f, height = 60.f;
glm::mat4 
	myMatrix, resizeMatrix = glm::ortho(-width, width, -height, height), matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 30.0f, 0.0)), matrScale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0, 0.5, 0.0));;

void CreateVBO(void)
{
	// coordonatele varfurilor
	static const GLfloat Vertices[] =
	{
	-5.0f, -5.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	 5.0f,  -5.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	 5.0f,  5.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
	-5.0f,  5.0f, 0.0f, 1.0f,    1.0f, 0.0f, 1.0f,  0.0f, 1.0f,

	};
	// culorile varfurilor
	//static const GLfloat vf_col[] =
	//{
	//1.0f, 0.0f, 0.0f, 1.0f,
	//0.0f, 1.0f, 0.0f, 1.0f,
	//0.0f, 0.0f, 1.0f, 1.0f,
	//1.0f, 0.0f, 1.0f, 1.0f,
	//};
	// indici pentru trasarea unor primitive
	static const GLuint Indices[] =
	{
	0, 1, 2, 3, 0, 2
	};

	// se creeaza un buffer nou pentru varfuri
	glGenBuffers(1, &VboId);
	// buffer pentru indici
	glGenBuffers(1, &EboId);
	// se creeaza / se leaga un VAO (Vertex Array Object)
	glGenVertexArrays(1, &VaoId);

	// legare VAO
	glBindVertexArray(VaoId);

	// buffer-ul este setat ca buffer curent
	glBindBuffer(GL_ARRAY_BUFFER, VboId);

	//// buffer-ul va contine atat coordonatele varfurilor, cat si datele de culoare
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vf_col) + sizeof(vf_pos), NULL, GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vf_pos), vf_pos);
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(vf_pos), sizeof(vf_col), vf_col);

	//// buffer-ul pentru indici
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vf_ind), vf_ind, GL_STATIC_DRAW);

	//// se activeaza lucrul cu atribute; atributul 0 = pozitie, atributul 1 = culoare, acestea sunt indicate corect in VBO
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(vf_pos));
	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	// punctele sunt "copiate" in bufferul curent
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// legarea buffer-ului "Element" (indicii)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	// indicii sunt "copiati" in bufferul curent
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	// se activeaza lucrul cu atribute; atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	// se activeaza lucrul cu atribute; atributul 2 = coordonate de texturare
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

}
void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &EboId);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void LoadTexture(void)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image("text_smiley_face.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CreateShaders(void)
{
	ProgramId = LoadShaders("06_02_init.vert", "06_02_init.frag");
	glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	CreateVBO();
	CreateShaders();
	codColLocation = glGetUniformLocation(ProgramId, "codCuloare");
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
}
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	myMatrix = resizeMatrix;

	// Functii pentru texturi
	LoadTexture();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	codCol = 0;
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));
	//Scalare + Translatie
	myMatrix = resizeMatrix * matrTransl * matrScale;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));
	// Translatie + Scalare
	myMatrix = resizeMatrix * matrScale*matrTransl;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	codCol = 2;
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));
	// De realizat desenul folosind segmente de dreapta
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
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Utilizarea indexarii varfurilor");
	glewInit();
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(RenderFunction);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

