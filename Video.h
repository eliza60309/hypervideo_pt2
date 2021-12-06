#ifndef VIDEO
#define VIDEO

#if _MSC_VER > 1000
#pragma once
#endif

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "Image.h"
#include "Windows.h"


#define FRAMES 9000
#define BUFFER 100
#define FPS 30
#define WIDTH 352
#define HEIGHT 288


class VideoPlayer
{
	std::string VideoPath;
	MyImage DummyFrame;

	public:
	MyImage Frame[FRAMES];
	int FrameTime[FRAMES];
	bool LoadedFrames[FRAMES] = {};
	int FramesCount;
	int NowPlaying;
	long long int StartTime, PauseTime;
	long long int TimeElapsed;
	bool Playing, Paused;
	bool KillThread1, KillThread2;
	HANDLE Thread1, Thread2;
	int ThreadStart1, ThreadStart2;
	bool PreBuffered;

	public:
	VideoPlayer();
	int SetPath(char *);
	int Setup();
	int Buffer(int);
	int UnloadFrame(int);
	int SetStartTime();
	int SetPauseTime();
	int IsPlaying();
	int IsPaused();

	int LoadFrames(int);
	int LoadFramesDoubleThread(int);
	int KillThreads();

	MyImage *GetFrame();
	long long int GetTime();

	int VideoPlay();
	int VideoPause();
	int VideoResume();
	int VideoStop();
};
#endif