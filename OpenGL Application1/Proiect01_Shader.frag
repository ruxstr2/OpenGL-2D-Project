#version 330

in vec4 ex_Color;
uniform int codCuloare;
 
out vec4 out_Color;

void main(void)
{
  switch (codCuloare)
  {
	case 0: 
	  out_Color = ex_Color;
	  break;
	case 1: 
		out_Color=vec4 (1.0, 0.0, 0.0, 0.0);
		break;
	case 2:
		out_Color=vec4 (0.0, 0.5, 0.0, 0.0);
		break;
	case 3:
		out_Color=vec4 (0.0, 0.0, 0.8, 0.0);
		break;
	case 4:
		out_Color=vec4 (0.7, 0.4, 0.1, 0.0);
		break;
	case 5:
		out_Color=vec4 (0.4, 0.3, 0.1, 0.0);
		break;
	case 6:
		out_Color=vec4 (0.0, 0.2, 0.1, 0.0);
		break;
	case 7:
		out_Color=vec4 (0.6, 0.9, 0.6, 0.0);
		break;
	default:
		break;
  };
}
 