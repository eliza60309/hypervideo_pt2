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
	std::string SoundOpenCmd;
	std::string SoundCloseCmd;

	public:
	SoundPlayer();
	int SetPath(char *);
	int Setup();
	int SoundPlay(int);
	int SoundPause();
	int SoundResume();
	int SoundStop();
	int SoundClose();
	int SoundOpen();

};
#endif