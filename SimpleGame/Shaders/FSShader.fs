#version 330

layout(location=0) out vec4 FragColor;
uniform float u_Time;
in vec2 v_TPos;

void main()
{
	if ( v_TPos.x + v_TPos.y > 0.5) {
		discard;
	}

	FragColor = vec4(v_TPos, sin(u_Time), 1);
}
