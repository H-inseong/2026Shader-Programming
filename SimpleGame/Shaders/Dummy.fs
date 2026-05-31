#version 330

layout(location=0) out vec4 FragColor;

uniform sampler2D u_MovieTexture;

in float	v_Grey;
in vec2		v_Tex;

const float c_PI = 3.1415926;

mat2 rotate2D(float radian)
{
	return mat2(cos(radian), -sin(radian),
				sin(radian),  cos(radian));
}

float Linepattern()
{
	float Hlinecount = 20;
	float Vlinecount = 5;
	Hlinecount /= 2;
	Vlinecount /= 2;

	float linewidth = 1;
	linewidth = 50 / linewidth;

	float per = -0.5 * c_PI;

	float greyscale = pow(abs(sin(v_Tex.y * c_PI * Vlinecount + per)), linewidth);
	greyscale += pow(abs(sin(v_Tex.x * c_PI * Hlinecount + per)), linewidth);

	return greyscale;
}

void main()
{
	vec2 newTex = rotate2D(0) * v_Tex;

	FragColor = Linepattern() * texture(u_MovieTexture, newTex);
}
