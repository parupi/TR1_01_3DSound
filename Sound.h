#pragma once
#define _USE_MATH_DEFINES

#include <windows.h>
#include <stdio.h>
#include <al.h>
#include <alc.h>
#include <math.h>
#include <function.h>

enum SoundType {
	CUT,
	FIRE,
	WATER,
};

#pragma comment(lib, "OpenAL32.lib")
//fopenの警告を無視
#pragma warning(disable:4996)
class Sound
{
private:
	ALuint buffer;
	ALuint source;
public:
	int Initialize(int SoundType);
	void PlayAudio(const Vector3 ListenerPos, const Vector3 SourcePos);
	~Sound();
};

