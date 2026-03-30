#version 330

in vec3 a_Position;
in vec2 a_TexCoord;

out vec2 v_TPos;

void main()
{
	v_TPos = a_TexCoord;
	gl_Position = vec4(a_Position, 1.0);

}
