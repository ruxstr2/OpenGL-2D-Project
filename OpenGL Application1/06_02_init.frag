
// Shader-ul de fragment / Fragment shader  
#version 330

in vec4 ex_Color;
in vec2 tex_Coord;
out vec4 out_Color;

vec4 red = vec4(1.0,0.0,0.0,1.0);
vec4 green= vec4(0.0,1.0,0.0,1.0);

uniform sampler2D myTexture;
uniform int codCuloare;

void main(void)
  {

		switch (codCuloare)
	{
		case 0: 
		out_Color = ex_Color;
		break;
		case 1: 
			out_Color=mix(red,green,0.4);
			break;
		case 2:
			//out_Color = mix(texture(myTexture, tex_Coord), ex_Color, 0.2);
			out_Color = texture(myTexture, tex_Coord);
			break;
		default:
			break;
  };
  //out_Color=ex_Color;
  //out_Color=mix(red,green,0.9);
  // out_Color = mix(texture(myTexture, tex_Coord), ex_Color, 0.2);
  }
 
 