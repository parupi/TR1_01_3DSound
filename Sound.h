#pragma once
#define _USE_MATH_DEFINES

#include <windows.h>
#include <stdio.h>
#include <al.h>
#include <alc.h>
#include <math.h>

#pragma comment(lib, "OpenAL32.lib")
//fopenの警告を無視
#pragma warning(disable:4996)
class Sound
{
private:
	ALuint buffer;
	ALuint source;
public:
	int Initialize();
	void PlayAudio(const ALfloat* ListenerPos, const ALfloat* SourcePos);
	~Sound();
};

