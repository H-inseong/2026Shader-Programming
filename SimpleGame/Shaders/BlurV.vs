#version 330

in vec3 a_Pos;
in vec2 a_Tex;

uniform vec4 u_Trans;
out vec2 v_Tex;

void main()
{
	vec4 newPosition = vec4(a_Pos, 1.0);
	gl_Position = newPosition;
	v_Tex = a_Tex;
}
