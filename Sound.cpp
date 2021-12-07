#include "Sound.h"
#include "Windows.h"
#include <string>

SoundPlayer::SoundPlayer()
{
}

int SoundPlayer::SetPath(char* c)
{
	SoundPath = c;
	return 0;
}

int SoundPlayer::Setup()
{
	SoundOpenCmd = std::string("open ") + SoundPath;
	SoundPlayCmd = std::string("play ") + SoundPath + std::string(" from ");
	SoundPauseCmd = std::string("pause ") + SoundPath;
	SoundResumeCmd = std::string("resume ") + SoundPath;
	SoundStopCmd = std::string("stop ") + SoundPath;
	SoundCloseCmd = std::string("close ") + SoundPath;
	return 0;
}

int SoundPlayer::SoundPlay(int ms)
{
	SoundOpen();
	char c[10] = {};
	itoa(ms, c, 10);
	std::string s(SoundPlayCmd + c);
	mciSendString(s.c_str(), NULL, 0, NULL);
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
	SoundClose();
	return 0;
}

int SoundPlayer::SoundOpen()
{
	mciSendString(SoundOpenCmd.c_str(), NULL, 0, NULL);
	return 0;
}

int SoundPlayer::SoundClose()
{
	mciSendString(SoundCloseCmd.c_str(), NULL, 0, NULL);
	return 0;
}