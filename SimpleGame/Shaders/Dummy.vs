#version 330

in vec3 a_Pos;	// -0.5 ~ 0.5

void main()
{
	float newX = ;
	float newY = ;


	vec4 final = vec4(newX, newY, 0, 1);
	vec4 newPosition = vec4(final, 1);
	gl_Position = newPosition;
}
