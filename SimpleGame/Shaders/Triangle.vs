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
	float scale = random(a_RandomValue * 123.45);
	float newTime = u_Time - startTime;
	
	if ( newTime > 0 ) {
		
		float t = mod(newTime, 2.0);
		float tt = t*t;
		float vx, vy;
		float sx, sy;

		vx = a_Vel.x / 500;
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

void ImpactCircle()
{
    float startTime = random(a_RandomValue);
    // 스케일이 0이 되는 것을 방지하기 위해 최소값을 더함
    float scale = random(a_RandomValue * 123.45) * 0.5 + 0.5; 
    float newTime = u_Time - startTime;
    
    if (newTime > 0.0) {
        
        // 애니메이션 주기 설정 (예: 3초마다 새로운 궤적으로 반복)
        float cycleTime = 3.0;
        float t = mod(newTime, cycleTime);
        
        // 1. 시작 위치 (화면 전체에 무작위로 퍼진 상태)
        // random 결과(0.0 ~ 1.0)를 -1.0 ~ 1.0 범위로 확장
        vec2 startPos;
        startPos.x = a_Position.x * scale + (random(a_RandomValue * 11.1) * 2.0 - 1.0);
        startPos.y = a_Position.y * scale + (random(a_RandomValue * 22.2) * 2.0 - 1.0);
        
        // 2. 목표 위치 (반지름 0.5인 원형 배열)
        vec2 targetPos;
        float radius = 0.5;
        targetPos.x = a_Position.x * scale + cos(a_RandomValue2 * 2.0 * c_PI) * radius;
        targetPos.y = a_Position.y * scale + sin(a_RandomValue2 * 2.0 * c_PI) * radius;
        
        // 3. 감쇠 진동 수식 (Damped Spring) 계산
        // k: 마찰력/감쇠 계수 (값이 클수록 빨리 멈춤)
        // omega: 진동수 (값이 클수록 더 많이 튕김)
        float k = 3.0;
        float omega = 15.0;
        
        // t_spring은 t=0일 때 0.0, 시간이 지날수록 진동하며 1.0에 수렴함
        float t_spring = 1.0 - exp(-k * t) * cos(omega * t);
        
        // 4. mix 함수를 이용한 두 지점 간의 보간
        vec2 currentPos = mix(startPos, targetPos, t_spring);
        
        vec4 newPos;
        newPos.x = currentPos.x;
        newPos.y = currentPos.y;
        newPos.z = 0.0;
        newPos.w = 1.0;
        
        gl_Position = newPos;

    }
    else
    {
        gl_Position = vec4(-100.0, -100.0, 0.0, 1.0);
    }
}

void main()
{
	ImpactCircle();
}
