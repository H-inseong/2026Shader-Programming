#version 330

uniform float u_Time;

in vec3 a_Position;
in float a_Mass;
in vec2 a_Vel;
in float a_RandomValue;
in float a_RandomValue2;

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

float random(float x)
{
    return fract(sin(x * 12.9898) * 43758.5453);
}

void Falling()
{
	float startTime = random(a_RandomValue);
	float scale = 0.5 + random(a_RandomValue * 123.45) * 1.5;
	float newTime = u_Time - startTime;
	
	if ( newTime > 0 ) {
		
	float t = mod(newTime, 1.0);
	float tt = t*t;
	float vx, vy;
	float sx, sy;

	vx = a_Vel.x / 50;
	vy = a_Vel.y / 50;

	sx = a_Position.x * scale + cos(a_RandomValue2 * 2 * c_PI);
	sy = a_Position.y * scale + sin(a_RandomValue2 * 2 * c_PI);

	vec4 newPos;

	newPos.x = sx + vx * t;
	newPos.y = sy + vy * t + 0.5 * c_G * tt;
	newPos.z = 0;
	newPos.w = 1;

	gl_Position = newPos;

	}
	else
	{
			gl_Position = vec4(-100, -100, 0, 1);
	}

}


void main()
{
	Falling();
}
