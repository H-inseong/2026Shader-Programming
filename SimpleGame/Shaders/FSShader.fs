#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_TPos;

void main()
{
	FragColor = vec4(v_TPos, 0, 1);
}
