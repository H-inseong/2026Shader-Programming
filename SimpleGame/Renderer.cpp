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

	//Load textures
	m_RgbTexture = CreatePngTexture("./Textures/rgb.png", GL_NEAREST); // 0
	m_NumsTexture = CreatePngTexture("./Textures/Numbers.png", GL_NEAREST); // 1

	for (int i = 0; i < 10; ++i) // 2~11
	{
		char filePath[256];
		sprintf_s(filePath, "./Textures/%d.png", i);
		m_NumTexture[i] = CreatePngTexture(filePath, GL_NEAREST);
	}

	//Create VBOs
	CreateVertexBufferObjects();

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

	m_ParticleCount = 1000;

	std::vector<float> pD;
	pD.reserve(m_ParticleCount * 48);

	for (size_t i = 0; i < m_ParticleCount; ++i)
	{
		float centerX = 0.0f;
		float centerY = 0.0f;
		float size = 0.05f;
		float mass = 1.0f;
		float rv1 = ((rand() % 100) / 100.0f);        // 0.0 ~ 1.0
		float rv2 = ((rand() % 100) / 100.0f);        // 0.0 ~ 1.0
		float rv3 = ((rand() % 100) / 100.0f);        // 0.0 ~ 1.0

		float vx = ((rand() % 100) / 2.0f) - 1.0f;	// -1.0 ~ 1.0
		float vy = ((rand() % 100) / 2.0f) - 1.0f;	// -1.0 ~ 1.0

		float LT = 10.0f;

		pD.push_back(centerX - size / 2); pD.push_back(centerY - size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(LT);
		pD.push_back(centerX + size / 2); pD.push_back(centerY - size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(LT);
		pD.push_back(centerX + size / 2); pD.push_back(centerY + size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(LT);

		pD.push_back(centerX - size / 2); pD.push_back(centerY - size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(LT);
		pD.push_back(centerX + size / 2); pD.push_back(centerY + size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(LT);
		pD.push_back(centerX - size / 2); pD.push_back(centerY + size / 2); pD.push_back(0.0f);
		pD.push_back(mass); pD.push_back(vx); pD.push_back(vy); pD.push_back(rv1); pD.push_back(rv2);
		pD.push_back(rv3); pD.push_back(LT);
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

}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
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
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

float g_Time = 0;
void Renderer::DrawTriangle()
{
	g_Time += 0.001f;

	glUseProgram(m_TriangleShader);

	//uniform 다렉의 constant buffer과 비슷한 개념
	//location(ID 또는 레지스터 넘버같은 개념) 값을 가져옴
	int uTime = glGetUniformLocation(m_TriangleShader, "u_Time");

	//드로우 콜 전에 uniform 변수에 값을 할당
	glUniform1f(uTime, g_Time);

	int attribPosition =	glGetAttribLocation(m_TriangleShader, "a_Position");
	int		attribMass =	glGetAttribLocation(m_TriangleShader, "a_Mass");
	int attribVelocity =	glGetAttribLocation(m_TriangleShader, "a_Vel");
	
	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribMass);
	glEnableVertexAttribArray(attribVelocity);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTriangle);

	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE,	sizeof(float) * 6, 0); 
	//	attribPosition 의 경우, stride는 6*float, offset은 0									
	glVertexAttribPointer(attribMass, 1, GL_FLOAT, GL_FALSE,		sizeof(float) * 6, (GLvoid*)(sizeof(float)*3));
	// 	attribMass 의 경우, 마지막 1개가 mass이므로, stride는 6*float, offset은 3*float
	glVertexAttribPointer(attribVelocity, 2, GL_FLOAT, GL_FALSE,	sizeof(float) * 6, (GLvoid*)(sizeof(float) * 4));
	// 	arrtibVelocity 의 경우, 마지막 2개가 Vel이므로, stride는 6*float, offset은 4*float
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawParticle()
{
	g_Time += 0.0005f;

	glUseProgram(m_TriangleShader);

	//uniform 다렉의 constant buffer과 비슷한 개념
	//location(ID 또는 레지스터 넘버같은 개념) 값을 가져옴
	int uTime = glGetUniformLocation(m_TriangleShader, "u_Time");

	//드로우 콜 전에 uniform 변수에 값을 할당
	glUniform1f(uTime, g_Time);

	int attribPosition = glGetAttribLocation(m_TriangleShader, "a_Position");
	int		attribMass = glGetAttribLocation(m_TriangleShader, "a_Mass");
	int arrtibVelocity = glGetAttribLocation(m_TriangleShader, "a_Vel");
	int attribRandomValue = glGetAttribLocation(m_TriangleShader, "a_RandomValue");
	int attribRandomValue2 = glGetAttribLocation(m_TriangleShader, "a_RandomValue2");
	int attribRandomValue3 = glGetAttribLocation(m_TriangleShader, "a_RandomValue3");
	int attribLifeTime = glGetAttribLocation(m_TriangleShader, "a_LifeTime");

	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribMass);
	glEnableVertexAttribArray(arrtibVelocity);
	glEnableVertexAttribArray(attribRandomValue);
	glEnableVertexAttribArray(attribRandomValue2);
	glEnableVertexAttribArray(attribRandomValue3);
	glEnableVertexAttribArray(attribLifeTime);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticle);

	auto stride = sizeof(float) * 10;

	// Position, Mass, Velocity, RandomValue1, RandomValue2, RandomValue3, LifeTime
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(attribMass, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(arrtibVelocity, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 4));
	glVertexAttribPointer(attribRandomValue, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(attribRandomValue2, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 7));
	glVertexAttribPointer(attribRandomValue3, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 8));
	glVertexAttribPointer(attribLifeTime, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(float) * 9));


	glDrawArrays(GL_TRIANGLES, 0, m_ParticleCount * 6);
}

int g_CurrNum = 0;
void Renderer::DrawFSShader()
{
	g_Time += 0.0005f;

	auto shader = m_FSShader;
	glUseProgram(m_FSShader);

	int uTime = glGetUniformLocation(m_FSShader, "u_Time");
	glUniform1f(uTime, g_Time);
	int uDropPoints = glGetUniformLocation(m_FSShader, "u_DropInfo");
	glUniform4fv(uDropPoints, 1000, m_DropPoints);
	
	int uRgbTexture = glGetUniformLocation(m_FSShader, "u_RgbTexture");
	glUniform1i(uRgbTexture, 0);

	int uCurrbTexture = glGetUniformLocation(m_FSShader, "u_CurrNumTexture");
	glUniform1i(uCurrbTexture, g_CurrNum + 2);
	g_CurrNum = rand() % 2000;
	Sleep(300);

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