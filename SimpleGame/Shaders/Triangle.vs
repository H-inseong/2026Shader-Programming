#version 330

//before drawcall this uniform variable will be set by the application
uniform float u_Time;

//streaming input
in vec3 a_Position;

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


void main()
{
	sin();
}
