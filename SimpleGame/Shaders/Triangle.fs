#version 330
layout(location=0) out vec4 FragColor;

in float	v_Grey;
in vec3		v_Color;
in vec2		v_Tex;

uniform sampler2D u_ParticleTexture;
uniform sampler2D u_ParticleSpriteTexture;

void CircleParticle()
{
	float d = distance(vec2(0.5, 0.5), v_Tex);
	if(d<0.5)
	{
		FragColor = vec4(v_Color, clamp(0.5 - d, 0, 0.5) * 2);
	}
	else
	{
		FragColor = vec4(0);
	}
}

void SingleTexture()
{
	FragColor = texture(u_ParticleTexture, v_Tex) * v_Grey;
}

void AnimTexture()
{
	float resolX = 9.0;
	float resolY = 9.0;

	float index = floor((1 - v_Grey) * (resolX * resolY - 1));

	float tx = v_Tex.x / resolX;
	float ty = v_Tex.y / resolY;
	float offsetX = fract(index / resolX);
	float offsetY = floor(index / resolX) / resolY;

	float d = distance(vec2(0.5, 0.5), v_Tex);
	float value = clamp(0.5 - d, 0, 0.5) * 2;

	vec2 tex = vec2(tx + offsetX, ty + offsetY);
	FragColor = texture(u_ParticleSpriteTexture, tex) * v_Grey;
	FragColor.a *= value;
}

void main()
{
	AnimTexture();
}
