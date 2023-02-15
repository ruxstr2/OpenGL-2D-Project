// Shader-ul de varfuri  
#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec3 in_Color;
layout (location = 2) in mat4 modelMatrix; // matricea de transformare este atribut instantiat
out vec4 gl_Position; // comentati daca este cazul!
out vec4 ex_Color;
uniform mat4 viewShader;
uniform mat4 projectionShader;

void main(void)
{
    gl_Position = projectionShader*viewShader*modelMatrix*in_Position;
    ex_Color=in_Color;
}
 