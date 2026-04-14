#version 330

in vec3 a_Position;
in vec2 a_TexCoord;

out vec2 v_Tex;

void main()
{
	v_Tex = a_TexCoord;
	gl_Position = vec4(a_Position, 1.0);

}
