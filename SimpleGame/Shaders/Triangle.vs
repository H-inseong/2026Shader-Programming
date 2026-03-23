#version 330

//before drawcall this uniform variable will be set by the application
uniform float u_Time;

//streaming input
in vec3 a_Position;
in float a_Mass;
in vec2 a_Vel;
in float a_RandomValue;

const float c_G = -9.8f;
const float c_PI = 3.141592f;

void sin()
{
	float t = u_Time;

	//시간 배율
	t *= 0.05;

	vec4 newPos;

	newPos.x = a_Position.x + cos(t);
	newPos.y = a_Position.y + sin(t);
	newPos.z = 0;
	newPos.w = 1;

	gl_Position = newPos;
}

void Falling()
{
	float t = mod(u_Time, 2.0);
	float tt = t*t;
	float vx, vy;
	float sx, sy;

	vx = a_Vel.x;
	vy = a_Vel.y;

	sx = a_Position.x + cos(a_RandomValue * 2 * c_PI);
	sy = a_Position.y + sin(a_RandomValue * 2 * c_PI);

	vec4 newPos;

	newPos.x = sx + vx * t;
	newPos.y = sy + vy * t + 0.5 * c_G * tt;
	newPos.z = 0;
	newPos.w = 1;

	gl_Position = newPos;
}


void main()
{
	Falling();
}
