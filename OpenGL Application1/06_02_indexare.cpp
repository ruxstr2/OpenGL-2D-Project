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

using namespace std;

//////////////////////////////////////

GLuint
	VaoId,
	VboId,
	EboId,
	ColorBufferId,
	ProgramId,
	myMatrixLocation;

float width = 80.f, height = 60.f;
glm::mat4 
	myMatrix, resizeMatrix = glm::ortho(-width, width, -height, height);

void CreateVBO(void)
{
	// coordonatele varfurilor
	static const GLfloat vf_pos[] =
	{
	
	15.0f, 0.0f, 0.0f, 1.0f, //0
		30.0f, 0.0f, 0.0f, 1.0f, //1
	12.0f,  8.82f, 0.0f, 1.0f, //2
		24.0f, 17.64f, 0.0f, 1.0f, //3
	4.5f,  14.265f, 0.0f, 1.0f, //4
		9.0f, 28.53f, 0.0f, 1.0f, //5
	-4.5f,  14.265f, 0.0f, 1.0f, //6
		-9.0f, 28.53f, 0.0f, 1.0f, //7
	-12.0f, 8.82f, 0.0f, 1.0f, //8
		-24.0f, 17.64f, 0.0f, 1.0f, //9
	-15.0f, -0.0f, 0.0f, 1.0f, //10
		-30.0f, 0.0f, 0.0f, 1.0f, //11
	-12.0f, -8.82f, 0.0f, 1.0f, //12
		-24.0f, -17.64f, 0.0f, 1.0f, //13
	-4.5f, -14.265f, 0.0f, 1.0f, //14
		-9.0f, -28.53f, 0.0f, 1.0f, //15
	4.5f, -14.265f, 0.0f, 1.0f, //16
		9.0f, -28.53f, 0.0f, 1.0f, //17
	12.0f, -8.82f, 0.0f, 1.0f, //18
		24.0f, -17.64f, 0.0f, 1.0f, //19

	};
	// culorile varfurilor
	static const GLfloat vf_col[] =
	{
	1.0f, 0.0f, 0.0f, 1.0f, //0
	0.0f, 1.0f, 0.0f, 1.0f, //1
	0.0f, 0.0f, 1.0f, 1.0f, //2
	1.0f, 0.0f, 1.0f, 1.0f, //3
	1.0f, 0.0f, 1.0f, 1.0f, //4
	0.0f, 0.0f, 0.0f, 1.0f, //5
	0.0f, 1.0f, 0.0f, 1.0f, //6
	1.0f, 0.0f, 0.0f, 1.0f, //7
	0.0f, 0.0f, 0.0f, 1.0f, //8
	0.0f, 0.0f, 1.0f, 1.0f, //9
	0.0f, 1.0f, 0.0f, 1.0f, //10
	0.0f, 0.0f, 0.0f, 1.0f, //11
	0.0f, 0.0f, 1.0f, 1.0f, //12
	1.0f, 0.0f, 0.0f, 1.0f, //13
	0.0f, 1.0f, 1.0f, 1.0f, //14
	1.0f, 0.0f, 1.0f, 1.0f, //15
	1.0f, 0.0f, 0.0f, 1.0f, //16
	0.0f, 1.0f, 1.0f, 1.0f, //17
	0.0f, 1.0f, 1.0f, 1.0f, //18
	1.0f, 0.0f, 1.0f, 1.0f, //19
	};
	// indici pentru trasarea unor primitive
	static const GLuint vf_ind[] =
	{
	1, 0, 2, 1, 3,
	3, 2, 4, 3, 5,
	5, 4, 6, 5, 7,
	7, 6, 8, 7, 9,
	9, 8, 10, 9, 11,
	11, 10, 12, 11, 13,  
	13, 12, 14, 13, 15,
	15, 14, 16, 15, 17,
	17, 16, 18, 17, 19,
	19, 18, 0, 19, 1,
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

	// buffer-ul va contine atat coordonatele varfurilor, cat si datele de culoare
	glBufferData(GL_ARRAY_BUFFER, sizeof(vf_col) + sizeof(vf_pos), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vf_pos), vf_pos);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vf_pos), sizeof(vf_col), vf_col);

	// buffer-ul pentru indici
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vf_ind), vf_ind, GL_STATIC_DRAW);

	// se activeaza lucrul cu atribute; atributul 0 = pozitie, atributul 1 = culoare, acestea sunt indicate corect in VBO
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(vf_pos));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

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

void CreateShaders(void)
{
	ProgramId = LoadShaders("06_02_Shader.vert", "06_02_Shader.frag");
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
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix"); 
}
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_LINE_STRIP, 50, GL_UNSIGNED_INT, (void*)(0));
	//glEnable(GL_POINT_SMOOTH);
	//glPointSize(20.0);
	//glDrawArrays(GL_POINTS, 0, 20);
	//glDisable(GL_POINT_SMOOTH);
	// De realizat desenul folosind segmente de dreapta
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
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

