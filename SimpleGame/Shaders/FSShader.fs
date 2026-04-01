#version 330

layout(location=0) out vec4 FragColor;
uniform float u_Time;
in vec2 v_TPos;

const float c_PI = 3.141592f;

void simple()
{
	FragColor = vec4(v_TPos, sin(u_Time), 1);
}


void Linepattern()
{
	float Hlinecount = 20;
	float Vlinecount = 5;
	Hlinecount /= 2;
	Vlinecount /= 2;

	float linewidth = 1;
	linewidth = 50 / linewidth;

	float per = -0.5 * c_PI * u_Time;

	float greyscale = pow(abs(sin(v_TPos.y * c_PI * Vlinecount + per)), linewidth);
	greyscale += pow(abs(sin(v_TPos.x * c_PI * Hlinecount + per)), linewidth);

	FragColor = vec4(greyscale);
}

void Circlepattern()
{
	
}


void main()
{
	Circlepattern();
}
