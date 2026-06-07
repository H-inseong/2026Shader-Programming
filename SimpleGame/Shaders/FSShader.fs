#version 330

layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 FragColor1;
layout(location=2) out vec4 FragColor2;

uniform float u_Time;
uniform sampler2D u_RgbTexture;
uniform sampler2D u_CurrNumTexture;
uniform sampler2D u_NumsTexture;
uniform int u_InputNum;

uniform vec4 u_DropInfo[1000]; // x,y: center, z: start time, w: life time

in vec2 v_Tex;

const float c_PI = 3.141592f;

void simple()
{
	FragColor = vec4(v_Tex, sin(u_Time), 1);
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

	float greyscale = pow(abs(sin(v_Tex.y * c_PI * Vlinecount + per)), linewidth);
	greyscale += pow(abs(sin(v_Tex.x * c_PI * Hlinecount + per)), linewidth);

	FragColor = vec4(greyscale);
}

// in Fragment Shader. avoid using if statement, it will cause performance issue. branch, hash problem
void Circlepattern()
{
	float Radius = 0.5;
	float linewidth = 0.01;
	vec2 Center = vec2(0.5);
	vec2 CurPos = v_Tex.xy;
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
	vec2 currPos = v_Tex.xy;
	float dist = distance(Center, currPos);
	float Circlecount = 10;

	float dir = -1; // 1 inside, -1 outside
	float per = dir * 0.5 * c_PI * u_Time;

	float value = abs(sin(dist * c_PI * Circlecount + per));

	FragColor = vec4(pow(value, 100));
}

vec4 RainDrop()
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
			vec2 currPos = v_Tex.xy;

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
	return vec4(temp);
}

vec4 flag()
{
	float speed = 10;
	float amp = 0.1;
	float sin_input = v_Tex.x * c_PI * 2 - u_Time * speed;
	float sin_value = v_Tex.x * amp * (((sin(sin_input) + 1) / 2) - 0.5 )  + 0.5;
	float fWidth = 0.0;
	float width = 0.5 * mix(1, fWidth, v_Tex.x);
	float grey = 0;

	if (v_Tex.y < sin_value + width / 2  && v_Tex.y > sin_value - width / 2)
	{
		grey = 1;
	}	
	else
	{
		grey = 0;
	}
	return vec4(grey);
}

void TextureSampling()
{
	vec4 c0;
	vec4 c1;
	vec4 c2;
	vec4 c3;
	vec4 c4;

	float offsetX = 0.01;

	c0 = texture(u_RgbTexture, vec2(v_Tex.x - offsetX * 2 , v_Tex.y));
	c1 = texture(u_RgbTexture, vec2(v_Tex.x - offsetX * 1 , v_Tex.y));
	c2 = texture(u_RgbTexture, vec2(v_Tex.x - offsetX * 0 , v_Tex.y));
	c3 = texture(u_RgbTexture, vec2(v_Tex.x + offsetX * 1 , v_Tex.y));
	c4 = texture(u_RgbTexture, vec2(v_Tex.x + offsetX * 2 , v_Tex.y));

	vec4 sum = c0 + c1 + c2 + c3 + c4;
	sum = sum / 5;

	FragColor =  sum;
}

void  MiddleLine_mirrorTexture()
{
	float tx = v_Tex.x;
	float ty = 1 - abs(v_Tex.y - 0.5) * 2;

	FragColor = texture(u_RgbTexture, vec2(tx, ty));
}

void  Right_mirrorTexture()
{
	float tx = v_Tex.x;
	float ty = v_Tex.y;

	tx = fract(v_Tex.x * 3);
	ty /= 3;
	
	float offsetX = 0;
	float offsetY = (2 - floor(v_Tex.x * 3)) / 3;

	tx += offsetX;
	ty += offsetY;

	FragColor = texture(u_RgbTexture, vec2(tx, ty));
}

void  Right_NonemirrorTexture()
{
	float tx = v_Tex.x;
	float ty = v_Tex.y;

	tx = fract(v_Tex.x * 3);
	ty /= 3;
	
	float offsetX = 0;
	float offsetY = floor(v_Tex.x * 3) / 3;

	tx += offsetX;
	ty += offsetY;

	FragColor = texture(u_RgbTexture, vec2(tx, ty));
}

vec4 Brick()
{
	float resolX = 2;
	float resolY = 2;
	float shear = 0.5;

	float offsetX = fract(ceil(v_Tex.y * resolX) * shear);
	float offsetY = 0;

	float tx = fract(v_Tex.x * resolX + offsetX);
	float ty = fract(v_Tex.y * resolY + offsetY);

	return vec4(texture(u_RgbTexture, vec2(tx, ty)));
}

vec4 Brick_90rotate()
{
	float resolX = 2;
	float resolY = 2;
	float shear = 0.5;

	float offsetX = 0;
	float offsetY = abs(0.5 - fract(ceil(v_Tex.x * resolY) * shear));

	float tx = fract(v_Tex.x * resolX + offsetX);
	float ty = fract(v_Tex.y * resolY + offsetY);

	return vec4(texture(u_RgbTexture, vec2(tx, ty)));
}

void Num()
{
	float index = float(u_InputNum);

	float tx = v_Tex.x / 5;
	float ty = v_Tex.y / 2;
	
	float offsetX = fract(index / 5);
	float offsetY = floor(index / 5) / 2;

	vec2 newTex = vec2(tx + offsetX, ty + offsetY);
	FragColor = texture(u_NumsTexture, newTex);

}
void Num_AI()
{
	float num = max(float(u_InputNum), 1.0); 

	float digitCount = floor(log(num) / log(10.0)) + 1.0; 

	float col = floor(v_Tex.x * digitCount);
	float tiledX = fract(v_Tex.x * digitCount);

	float tx = tiledX / 5.0;
	float ty = v_Tex.y / 2.0;

	float power = pow(10.0, (digitCount - 1.0) - col); 
	float index = floor(mod(float(u_InputNum) / power, 10.0));

	float offsetX = fract(index / 5.0);
	float offsetY = floor(index / 5.0) / 2.0;
	
	vec2 newTex = vec2(tx + offsetX, ty + offsetY);
	FragColor = texture(u_NumsTexture, newTex);
}

void Q()
{
	float tx = 0.0;
	float ty = 0.0;

	float offsetX = 0.0;
	float offsetY = 0.0;

	FragColor = texture(u_RgbTexture, vec2(tx + offsetX, ty + offsetY));
}

// 가로 R B G 
void Q6()
{
	float tx = fract(v_Tex.x * 3);
	float ty = v_Tex.y/3;

	float offsetX = 0;
	float offsetY = abs(floor(v_Tex.x * 3) / 3 - 2); // 답은 맞는데 좋진 않음

	FragColor = texture(u_RgbTexture, vec2(tx + offsetX, ty + offsetY));
}
//step도 약간 반칙임
void Q62()
{
	float tx = fract(v_Tex.x * 3);
	float ty = v_Tex.y/3 ;

	float offsetX = 0;
	float offsetY = step(1.0/3.0, v_Tex.x) * 2.0/3.0 + step(2.0/3.0, v_Tex.x) * -1.0/3.0;

	FragColor = texture(u_RgbTexture, vec2(tx + offsetX, ty + offsetY));
}

//세로 G B R
void Q7()
{
	float tx = v_Tex.x;
	float ty = fract(v_Tex.y * 3) / 3;

	float offsetX = 0;
	float offsetY = fract((floor(v_Tex.y*3) + 1) / 3);

	FragColor = texture(u_RgbTexture, vec2(tx + offsetX, ty + offsetY));
}

// 숫자 7 출력
void Q8()
{
	float tx = v_Tex.x / 5;
	float ty = v_Tex.y / 2;
	//.0이나 .f 붙여서 실수로 만들어 줘야함 
	float offsetX = fract(7.0 / 5);
	float offsetY = floor(7.0 / 5) / 2;

	FragColor = texture(u_NumsTexture, vec2(tx + offsetX, ty + offsetY));
}

// 2 3 동시 출력
void Q9()
{
	float tx = v_Tex.x / 5 * 2;
	float ty = v_Tex.y / 2;
	
	float offsetX = fract(2.0 / 5);
	float offsetY = 0;

	FragColor = texture(u_NumsTexture, vec2(tx + offsetX, ty + offsetY));
}

//index 사용해서 숫자 스프라이팅
vec4 Q10()
{
	float index = 8;

	float tx = v_Tex.x / 5;
	float ty = v_Tex.y / 2;
	
	float offsetX = fract(index / 5);
	float offsetY = floor(index / 5) / 2;

	return vec4(texture(u_NumsTexture, vec2(tx + offsetX, ty + offsetY)));
}

void main()
{
	FragColor = RainDrop();
	FragColor1 = flag();
	FragColor2 = Brick();
}
