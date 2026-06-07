#include "stdafx.h"
#include "Renderer.h"
#include "LoadPng.h"

#include <vector>
#include <assert.h>
#include <windows.h>
Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_TriangleShader = CompileShaders("./Shaders/Triangle.vs", "./Shaders/Triangle.fs");
	m_FSShader = CompileShaders("./Shaders/FSShader.vs", "./Shaders/FSShader.fs");
	m_DummyShader = CompileShaders("./Shaders/Dummy.vs", "./Shaders/Dummy.fs");
	m_TextureShader = CompileShaders("./Shaders/Texture.vs", "./Shaders/Texture.fs");


	//Load textures
	m_RgbTexture = CreatePngTexture("./Textures/rgb.png", GL_NEAREST); // 0
	m_NumsTexture = CreatePngTexture("./Textures/Numbers.png", GL_NEAREST); // 1

	for (int i = 0; i < 10; ++i) // 2~11
	{
		char filePath[256];
		sprintf_s(filePath, "./Textures/%d.png", i);
		m_NumTexture[i] = CreatePngTexture(filePath, GL_NEAREST);
	}
	m_ParticleTexture = CreatePngTexture("./Textures/particle.png", GL_NEAREST); // 12
	m_ParticleSpriteTexture = CreatePngTexture("./Textures/particleSprite.png", GL_NEAREST); // 13
	m_MovieTexture = CreatePngTexture("./Textures/Movie.png", GL_NEAREST); // 14

	//Create VBOs
	CreateVertexBufferObjects();
	GenDummyMesh(200, 200);


	int index = 0;
	for (int i = 0; i < 1000; ++i)
	{
		m_DropPoints[index++] = 1 * ((float)rand() / (float)RAND_MAX); //x 0~1
		m_DropPoints[index++] = 1 * ((float)rand() / (float)RAND_MAX); //y 0~1
		m_DropPoints[index++] = 5 * ((float)rand() / (float)RAND_MAX); //startTime 0~3
		m_DropPoints[index++] = 1 * ((float)rand() / (float)RAND_MAX); //lifeTime 0~1
	}


	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}

	GenFBOs();
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

GLuint Renderer::CreatePngTexture(char* filePath, GLuint samplingMethod)
{
	//Load Png
	std::vector<unsigned char> image;

	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);

	if (error != 0)
	{
		std::cout << "PNG image loading failed:" << filePath << std::endl;
		assert(0);
	}

	GLuint temp;
	glGenTextures(1, &temp);
	glBindTexture(GL_TEXTURE_2D, temp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, &image[0]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingMethod);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingMethod);

	return temp;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	float centerX = 0;
	float centerY = 0;
	float size = 0.1f;
	float mass = 1.f;
	float vx = 1;
	float vy = 3;



	float triangle[]
		=
	{
		centerX - size / 2, centerY - size / 2,	0, mass, vx, vy,
		centerX + size / 2, centerY - size / 2,	0, mass, vx, vy,
		centerX + size / 2, centerY + size / 2,	0, mass, vx, vy,	//tri1

		centerX - size / 2, centerY - size / 2,	0, mass, vx, vy,
		centerX + size / 2, centerY + size / 2,	0, mass, vx, vy,
		centerX - size / 2, centerY + size / 2,	0, mass, vx, vy,	//tri2

	};

	glGenBuffers(1, &m_VBOTriangle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTriangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	m_ParticleCount = 2000;

	//x,y,z , Mass, vx, vy, RandomValue1, RandomValue2, RandomValue3, tx, ty, r,g,b -> �� 14���� float
	const int floatsPerVertex = 14;
	const int verticesPerParticle = 6;
	const int floatsPerParticle = floatsPerVertex * verticesPerParticle;

	std::vector<float> pD;
	pD.reserve(m_ParticleCount * floatsPerParticle);

	for (size_t i = 0; i < m_ParticleCount; ++i)
	{
		float centerX = 0.0f;
		float centerY = 0.0f;
		float size = 0.1f;
		float mass = 1.0f;
		float vx = ((rand() % 100) / 25.0f) - 2.0f;	// -2.0 ~ 2.0
		float vy = ((rand() % 100) / 25.0f) - 2.0f;	// -2.0 ~ 2.0
		
		float rv1 = ((rand() % 100) / 100.0f);        // 0.0 ~ 1.0
		float rv2 = ((rand() % 100) / 100.0f);        // 0.0 ~ 1.0
		float rv3 = ((rand() % 100) / 100.0f);        // 0.0 ~ 1.0

		float R = ((rand() % 100) / 100.0f);
		float G = ((rand() % 100) / 100.0f);
		float B = ((rand() % 100) / 100.0f);

		pD.push_back(centerX - size / 2); pD.push_back(centerY - size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(0.f); pD.push_back(1.f); pD.push_back(R); pD.push_back(G); pD.push_back(B);
		pD.push_back(centerX + size / 2); pD.push_back(centerY - size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(1.f); pD.push_back(1.f); pD.push_back(R); pD.push_back(G); pD.push_back(B);
		pD.push_back(centerX + size / 2); pD.push_back(centerY + size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(1.f); pD.push_back(0.f); pD.push_back(R); pD.push_back(G); pD.push_back(B);

		pD.push_back(centerX - size / 2); pD.push_back(centerY - size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(0.f); pD.push_back(1.f); pD.push_back(R); pD.push_back(G); pD.push_back(B);
		pD.push_back(centerX + size / 2); pD.push_back(centerY + size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(1.f); pD.push_back(0.f); pD.push_back(R); pD.push_back(G); pD.push_back(B);
		pD.push_back(centerX - size / 2); pD.push_back(centerY + size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(0.f); pD.push_back(0.f); pD.push_back(R); pD.push_back(G); pD.push_back(B);
	}

	glGenBuffers(1, &m_VBOParticle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);
	glBufferData(GL_ARRAY_BUFFER, pD.size() * sizeof(float), pD.data(), GL_STATIC_DRAW);


	float FSRect[]
		=
	{
		-1.f, -1.f, 0.f, 0, 1,
		 1.f,  1.f, 0.f, 1, 0,
		-1.f,  1.f, 0.f, 0, 0,
		
		-1.f, -1.f, 0.f, 0, 1,
		 1.f, -1.f, 0.f, 1, 1,
		 1.f,  1.f, 0.f, 1, 0
	};
	glGenBuffers(1, &m_VBOFS);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FSRect), FSRect, GL_STATIC_DRAW);

	float texRect[]
		=
	{
		-1.f, -1.f, 0.f,
		 1.f,  1.f, 0.f,
		-1.f,  1.f, 0.f,

		-1.f, -1.f, 0.f,
		 1.f,  1.f, 0.f,
		 1.f, -1.f, 0.f,
	};

	glGenBuffers(1, &m_VBOTexture);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTexture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texRect), texRect, GL_STATIC_DRAW);

}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//���̴� �ڵ带 ���̴� ������Ʈ�� �Ҵ�
	glShaderSource(ShaderObj, 1, p, Lengths);

	//�Ҵ�� ���̴� �ڵ带 ������
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj �� ���������� ������ �Ǿ����� Ȯ��
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL �� shader log �����͸� ������
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram �� attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //�� ���̴� ���α׷� ����

	if (ShaderProgram == 0) { //���̴� ���α׷��� ����������� Ȯ��
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs �� vs ������ �ε���
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs �� fs ������ �ε���
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram �� vs.c_str() ���ؽ� ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram �� fs.c_str() �����׸�Ʈ ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach �Ϸ�� shaderProgram �� ��ŷ��
	glLinkProgram(ShaderProgram);

	//��ũ�� �����ߴ��� Ȯ��
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program �α׸� �޾ƿ�
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done." << std::endl;

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

}



void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::GenDummyMesh(int resolX, int resolY)
{
	float basePosX = -.5f;
	float basePosY = -.5f;
	float targetPosX = 0.5f;
	float targetPosY = 0.5f;
	int pointCountX = resolX;
	int pointCountY = resolY;

	float width = targetPosX - basePosX;
	float height = targetPosY - basePosY;

	float* point = new float[pointCountX * pointCountY * 2];
	float* vertices = new float[(pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3];

	m_DummyVertexCount = (pointCountX - 1) * (pointCountY - 1) * 2 * 3;
	//Prepare points
	for (int x = 0; x < pointCountX; x++)
	{
		for (int y = 0; y < pointCountY; y++)
		{
			point[(y * pointCountX + x) * 2 + 0] = basePosX + width * (x / (float)(pointCountX - 1));
			point[(y * pointCountX + x) * 2 + 1] = basePosY + height * (y / (float)(pointCountY - 1));
		}
	}
	//Make triangles
	int vertIndex = 0;
	for (int x = 0; x < pointCountX - 1; x++)
	{
		for (int y = 0; y < pointCountY - 1; y++)
		{
			//Triangle part 1
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			//Triangle part 2
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
		}
	}
	glGenBuffers(1, &m_VBODummy);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBODummy);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3, vertices, GL_STATIC_DRAW);
}

void Renderer::GenFBOs()
{
	//Gen Texture
	glGenTextures(1, &m_FBO_Texture);
	glBindTexture(GL_TEXTURE_2D, m_FBO_Texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Gen Render Buffer
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//Gen FBO
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBO_Texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FBO creation failed. Status: " << status << std::endl;
		assert(0);
	}
	
	//Gen Texture
	glGenTextures(1, &m_FBO1_Texture);
	glBindTexture(GL_TEXTURE_2D, m_FBO1_Texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Gen Render Buffer
	GLuint depthBuffer1;
	glGenRenderbuffers(1, &depthBuffer1);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer1);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//Gen FBO
	glGenFramebuffers(1, &m_FBO1);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBO1_Texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer1);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FBO creation failed. Status: " << status << std::endl;
		assert(0);
	}

	//Gen Texture
	GLuint textureId;
	glGenTextures(1, &m_FBO2_Texture);
	glBindTexture(GL_TEXTURE_2D, m_FBO2_Texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Gen Render Buffer
	GLuint depthBuffer2;
	glGenRenderbuffers(1, &depthBuffer2);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//Gen FBO
	glGenFramebuffers(1, &m_FBO2);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBO2_Texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer2);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FBO creation failed. Status: " << status << std::endl;
		assert(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // 원상복구
}


void Renderer::Update(float fTimeElapsed)
{
	m_Time += fTimeElapsed;
}
	
void Renderer::DrawTriangle()
{
	glUseProgram(m_TriangleShader);

	//uniform �ٷ��� constant buffer�� ����� ����
	//location(ID �Ǵ� �������� �ѹ����� ����) ���� ������
	int uTime = glGetUniformLocation(m_TriangleShader, "u_Time");

	//��ο� �� ���� uniform ������ ���� �Ҵ�
	glUniform1f(uTime, m_Time);

	int attribPosition =	glGetAttribLocation(m_TriangleShader, "a_Position");
	int		attribMass =	glGetAttribLocation(m_TriangleShader, "a_Mass");
	int attribVelocity =	glGetAttribLocation(m_TriangleShader, "a_Vel");
	
	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribMass);
	glEnableVertexAttribArray(attribVelocity);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTriangle);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE,	sizeof(float) * 6, 0); 
	//	attribPosition �� ���, stride�� 6*float, offset�� 0									
	glVertexAttribPointer(attribMass, 1, GL_FLOAT, GL_FALSE,		sizeof(float) * 6, (GLvoid*)(sizeof(float)*3));
	// 	attribMass �� ���, ������ 1���� mass�̹Ƿ�, stride�� 6*float, offset�� 3*float
	glVertexAttribPointer(attribVelocity, 2, GL_FLOAT, GL_FALSE,	sizeof(float) * 6, (GLvoid*)(sizeof(float) * 4));
	// 	arrtibVelocity �� ���, ������ 2���� Vel�̹Ƿ�, stride�� 6*float, offset�� 4*float
	glDrawArrays(GL_TRIANGLES, 0, 6);
}



void Renderer::DrawParticle()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m_TriangleShader);

	//uniform �ٷ��� constant buffer�� ����� ����
	//location(ID �Ǵ� �������� �ѹ����� ����) ���� ������
	int uTime = glGetUniformLocation(m_TriangleShader, "u_Time");
	int uParticleTexture = glGetUniformLocation(m_TriangleShader, "u_ParticleTexture");
	int uParticleSpriteTexture = glGetUniformLocation(m_TriangleShader, "u_ParticleSpriteTexture");

	//��ο� �� ���� uniform ������ ���� �Ҵ�
	glUniform1f(uTime, m_Time);
	glUniform1i(uParticleTexture, 0);
	glUniform1i(uParticleSpriteTexture, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ParticleTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_ParticleSpriteTexture);

	int attribPosition		= glGetAttribLocation(m_TriangleShader, "a_Position");
	int	attribMass			= glGetAttribLocation(m_TriangleShader, "a_Mass");
	int arrtibVelocity		= glGetAttribLocation(m_TriangleShader, "a_Vel");
	int attribRandomValue	= glGetAttribLocation(m_TriangleShader, "a_RandomValue");
	int attribRandomValue2	= glGetAttribLocation(m_TriangleShader, "a_RandomValue2");
	int attribRandomValue3	= glGetAttribLocation(m_TriangleShader, "a_RandomValue3");
	int attribTex			= glGetAttribLocation(m_TriangleShader, "a_Tex");
	int attribRGB			= glGetAttribLocation(m_TriangleShader, "a_RGB");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribMass);
	glEnableVertexAttribArray(arrtibVelocity);
	glEnableVertexAttribArray(attribRandomValue);
	glEnableVertexAttribArray(attribRandomValue2);
	glEnableVertexAttribArray(attribRandomValue3);
	glEnableVertexAttribArray(attribTex);
	glEnableVertexAttribArray(attribRGB);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);

	auto stride = sizeof(float) * 14;

	// Position, Mass, Velocity, RandomValue1, RandomValue2, RandomValue3, Tex, RGB
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(attribMass, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(arrtibVelocity, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 4));
	glVertexAttribPointer(attribRandomValue, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(attribRandomValue2, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 7));
	glVertexAttribPointer(attribRandomValue3, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 8));
	glVertexAttribPointer(attribTex, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 9));
	glVertexAttribPointer(attribRGB, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 11));

	glDrawArrays(GL_TRIANGLES, 0, m_ParticleCount * 6);
	glDisable(GL_BLEND);
}

int g_CurrNum = 0;
void Renderer::DrawFSShader()
{
	auto shader = m_FSShader;
	glUseProgram(m_FSShader);

	int uTime = glGetUniformLocation(m_FSShader, "u_Time");
	glUniform1f(uTime, m_Time);
	int uDropPoints = glGetUniformLocation(m_FSShader, "u_DropInfo");
	glUniform4fv(uDropPoints, 1000, m_DropPoints);
	
	int uRgbTexture = glGetUniformLocation(m_FSShader, "u_RgbTexture");
	glUniform1i(uRgbTexture, 0);

	int uCurrbTexture = glGetUniformLocation(m_FSShader, "u_CurrNumTexture");
	glUniform1i(uCurrbTexture, g_CurrNum + 2);
	g_CurrNum = rand() % 20000;

	int uNumsTexture = glGetUniformLocation(m_FSShader, "u_NumsTexture");
	glUniform1i(uNumsTexture, 1);

	int uInputNum = glGetUniformLocation(m_FSShader, "u_InputNum");
	glUniform1i(uInputNum, g_CurrNum);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_RgbTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_NumsTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[0]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[1]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[2]);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[3]);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[4]);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[5]);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[6]);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[7]);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[8]);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, m_NumTexture[9]);



	int attribPosition = glGetAttribLocation(m_FSShader, "a_Position");
	int attribTexCoord = glGetAttribLocation(m_FSShader, "a_TexCoord");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribTexCoord);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFS);

	auto stride = sizeof(float) * 5;

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(attribTexCoord, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 3));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawDummy()
{
	int shader = m_DummyShader;
	glUseProgram(m_DummyShader);

	int uTime = glGetUniformLocation(m_DummyShader, "u_Time");
	glUniform1f(uTime, m_Time);
	int uDropPoints = glGetUniformLocation(m_DummyShader, "u_DropInfo");
	glUniform4fv(uDropPoints, 1000, m_DropPoints);

	int uMovieTex = glGetUniformLocation(m_DummyShader, "u_MovieTexture");
	glUniform1i(uMovieTex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_MovieTexture);

	int attribPosition = glGetAttribLocation(m_DummyShader, "a_Pos");
	glEnableVertexAttribArray(attribPosition);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBODummy);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, m_DummyVertexCount);

	DrawTexture(m_MovieTexture, 0, 0, .2f);
}

void Renderer::DrawDummy_FBO()
{
	//Bind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glViewport(0, 0, 512, 512);
	DrawDummy();

	//Unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_WindowSizeX, m_WindowSizeY);

	DrawTexture(m_FBO_Texture, 0, 0, .5f, false);

}

void Renderer::DrawTotal()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glViewport(0, 0, 512, 512);
	DrawDummy();

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO1);

	DrawFSShader();

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO2);

	DrawParticle();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_WindowSizeX, m_WindowSizeY);

	DrawTexture(m_FBO_Texture, -0.5, 0.5, .3f, false);
	DrawTexture(m_FBO1_Texture, 0, 0.5, .3f, false);
	DrawTexture(m_FBO2_Texture, -0.3, -0.5, .3f, false);
}


void Renderer::DrawTexture(GLuint texID, float x, float y, float scale, bool flip)
{

	int shader = m_TextureShader;
	glUseProgram(shader);

	int uTex = glGetUniformLocation(shader, "u_Texture");
	glUniform1i(uTex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	int uTrans = glGetUniformLocation(shader, "u_Trans");
	glUniform4f(uTrans, x, y, 1, scale);
	
	int uFlip = glGetUniformLocation(shader, "u_Flip");
	glUniform1i(uFlip, flip ? 1 : 0);

	int attribPosition = glGetAttribLocation(shader, "a_Pos");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTexture);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

}