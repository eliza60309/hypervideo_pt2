#include "Sound.h"
#include "Windows.h"
#include <string>

SoundPlayer::SoundPlayer()
{
}

SoundPlayer::SoundPlayer(char *c)
{
	SoundPath = c;
	Setup();
}

int SoundPlayer::SetPath(char* c)
{
	SoundPath = c;
	return 0;
}

int SoundPlayer::Setup()
{
	std::string open = "open ";
	open += SoundPath;
	mciSendString(open.c_str(), NULL, 0, NULL);
	SoundPlayCmd = std::string("play ") + SoundPath + std::string(" from 0");
	SoundPauseCmd = std::string("pause ") + SoundPath;
	SoundResumeCmd = std::string("resume ") + SoundPath;
	SoundStopCmd = std::string("stop ") + SoundPath;
	return 0;
}

int SoundPlayer::SoundPlay()
{
	mciSendString(SoundPlayCmd.c_str(), NULL, 0, NULL);
	return 0;
}

int SoundPlayer::SoundPause()
{
	mciSendString(SoundPauseCmd.c_str(), NULL, 0, NULL);
	return 0;
}

int SoundPlayer::SoundResume()
{
	mciSendString(SoundResumeCmd.c_str(), NULL, 0, NULL);
	return 0;
}

int SoundPlayer::SoundStop()
{
	mciSendString(SoundStopCmd.c_str(), NULL, 0, NULL);
	return 0;
}