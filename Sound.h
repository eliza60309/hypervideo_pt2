#ifndef SOUND
#define SOUND

#if _MSC_VER > 1000
#pragma once
#endif

#include <iostream>
#include <string>

class SoundPlayer
{
	std::string SoundPath;
	std::string SoundPlayCmd;
	std::string SoundPauseCmd;
	std::string SoundResumeCmd;
	std::string SoundStopCmd;

	public:
	SoundPlayer();
	int SetPath(char *);
	int Setup();
	int SoundPlay();
	int SoundPause();
	int SoundResume();
	int SoundStop();

};
#endif