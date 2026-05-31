#version 330

uniform float u_Time;

in vec3 a_Pos;	// -0.5 ~ 0.5

const float c_PI = 3.141592f;
out float v_Grey;
out vec2  v_Tex;

void Frag()
{
	float radian = (a_Pos.x + 0.5) * 2 * c_PI;

	
	float tX = a_Pos.x + 0.5;
	float tY = 1.0 - (a_Pos.y + 0.5);

	float value = a_Pos.x + 0.5;

	float newX = a_Pos.x;
	float newY = a_Pos.y * (1.0 - value*0.5) + value * 0.25 * sin(radian - u_Time);

	vec4 final = vec4(newX, newY, 0, 1);
	vec4 newPosition = final;
	gl_Position = newPosition;

	v_Grey = (sin(radian - u_Time) + 1.0) / 2;
	v_Tex = vec2(tX, tY);
}

void main()
{
	Frag();
}
