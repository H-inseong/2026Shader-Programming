#version 330

layout(location=0) out vec4 FragColor;
uniform float u_Time;
uniform vec4 u_DropInfo[1000]; // x, y, startTime, lifeTime

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

// in Fragment Shader. avoid using if statement, it will cause performance issue. branch, hash problem
void Circlepattern()
{
	float Radius = 0.5;
	float linewidth = 0.01;
	vec2 Center = vec2(0.5);
	vec2 CurPos = v_TPos.xy;
	float dist = distance(Center, CurPos);

	if (dist > Radius - linewidth && dist < Radius  )
	{
		FragColor = vec4(1);
	}
	else
	{
		FragColor = vec4(0);
	}
	
}

void CircleSin()
{
	vec2 Center = vec2(0.5);
	vec2 currPos = v_TPos.xy;
	float dist = distance(Center, currPos);
	float Circlecount = 10;

	float dir = -1; // 1 inside, -1 outside
	float per = dir * 0.5 * c_PI * u_Time;

	float value = abs(sin(dist * c_PI * Circlecount + per));

	FragColor = vec4(pow(value, 100));
}

void RainDrop()
{	
	float temp = 0;

	for(int i= 0; i < 1000; ++i)
	{
		float lTime = u_DropInfo[i].w;
		float sTime = u_DropInfo[i].z;
		float newTime = u_Time - sTime;

		if(newTime > 0)
		{
			newTime = fract(newTime/lTime);
			float OneMinusNewTime = 1 - newTime;
			float t = newTime * lTime;

			vec2 Center = u_DropInfo[i].xy;
			vec2 currPos = v_TPos.xy;

			float range = t / 8;
			float dist = distance(Center, currPos);
	
			float Circlecount = 3;
			float dir = -1; // 1 inside, -1 outside
			float per = dir * t * 30;

			float value = pow(abs(sin(dist * c_PI*2 * Circlecount + per)), 100);
			float fade =  30 * clamp(range - dist, 0, 1);

			temp += value * fade * OneMinusNewTime;
		}
		else
		{
		}
	}
	FragColor = vec4(temp);
}


void main()
{
	RainDrop();
}
