#version 330

uniform vec4 u_Trans;
in vec3 a_Pos;

out vec2  v_Tex;

const float c_PI = 3.141592f;

void main()
{
	vec4 newPostion;
	newPostion.xy = (a_Pos.xy * u_Trans.w) + u_Trans.xy;
	newPostion.z = a_Pos.z;
	newPostion.w = 1.0;

	gl_Position = newPostion;


	//vertex shader의 범위를 fragment shader의 텍스쳐 좌표로 변환
	v_Tex.x = (a_Pos.x + 1.0) / 2.0;
	v_Tex.y = 1.0 - (a_Pos.y + 1.0) / 2.0;
}
