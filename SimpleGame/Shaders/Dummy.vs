#version 330

uniform float u_Time;
uniform vec4 u_DropInfo[1000]; // x, y, startTime, lifeTime

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

void Circle()
{
	float accum = 0;
	for(int i = 0; i < 1000; i++)
	{
		vec2 center = u_DropInfo[i].xy - vec2(0.5, 0.5);
		vec2 pos = a_Pos.xy;
		float lTime = u_DropInfo[i].z;
		float sTime = u_DropInfo[i].w;
		float nTime = u_Time - sTime;

		if(nTime > 0)
		{
			float lVal = fract(nTime / lTime);
			float oneMinus = 1.0 - lVal;
			float t = lVal * lTime;
			float d = distance(pos, center);
			float range = t/30.0;
			float fade = 10 * clamp(range - d, 0.0, 1.0);
			float sinValue = pow(abs(sin(d * 4 * c_PI * 8 - t * 3)), 3.0);

			accum += sinValue * fade * oneMinus;
		}
	}

	v_Grey = accum;
	gl_Position = vec4(a_Pos.x, a_Pos.y + 0.05*accum, a_Pos.z, 1.0);
}


void main()
{
	Circle();
}
