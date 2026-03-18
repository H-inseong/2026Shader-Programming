#version 330

//before drawcall this uniform variable will be set by the application
uniform float u_Time;

//streaming input
in vec3 a_Position;
in float a_Mass;
in vec2 a_Vel;

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
	float vx, vy;
	vx = a_Vel.x;
	vy = a_Vel.y;

	float t = mod(u_Time, 2.0);
	float tt = t*t;

	vec4 newPos;

	newPos.x = a_Position.x + vx * t;
	newPos.y = a_Position.y + vy * t + 0.5 * c_G * tt;
	newPos.z = 0;
	newPos.w = 1;

	gl_Position = newPos;
}


void main()
{
	Falling();
}
