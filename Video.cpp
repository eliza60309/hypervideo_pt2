#include "Video.h"
VideoPlayer::VideoPlayer()
{
}

int VideoPlayer::SetPath(char* c)
{
	VideoPath = c;
	return 0;
}

int VideoPlayer::Setup()
{
	KillThread1 = false;
	KillThread2 = false;
	NowPlaying = 0;
	FramesCount = 0;
	Playing = false;
	for (int i = 1; i <= FRAMES; i++)
	{
		std::stringstream ss;
		Frame[i - 1].Delete();
		Frame[i - 1].setWidth(WIDTH);
		Frame[i - 1].setHeight(HEIGHT);
		if (i < 10)
			ss << "000" << i;
		else if (i < 100)
			ss << "00" << i;
		else if (i < 1000)
			ss << "0" << i;
		else
			ss << i;
		std::string imgpath = std::string(VideoPath) + ss.str() + std::string(".rgb");
		Frame[i - 1].setImagePath(imgpath.c_str());
		FrameTime[i - 1] = (1000 * (i - 1)) / 30;
		LoadedFrames[i - 1] = false;
	}
	Buffer(0);
	return 0;
}

int VideoPlayer::Buffer(int start)
{
	for (int i = 0; i < BUFFER; i++)
	{
		if (LoadedFrames[i])
			continue;
		if (Frame[i].ReadImage())
		{
			LoadedFrames[i] = true;
			FramesCount++;
		}
		else
			AfxMessageBox("Could not read image");
	}
	PreBuffered = true;
	std::cout << "Buffered " << BUFFER << " frames" << std::endl;

	return 0;
}

DWORD WINAPI loadframe(void* data) {
	VideoPlayer* vp = (VideoPlayer*)data;
	for (int i = vp->ThreadStart1; i < FRAMES; i++)
	{
		if (vp->KillThread1)
		{
			vp->KillThread1 = false;
			vp->Thread1 = NULL;
			break;
		}
		while (vp->NowPlaying + BUFFER < i)
			Sleep(10);
		if (vp->LoadedFrames[i])
			continue;
		if (vp->Frame[i].ReadImage())
		{
			vp->LoadedFrames[i] = true;
			vp->FramesCount++;
		}
		else
			AfxMessageBox("Could not read image");
	}

	return 0;
}

DWORD WINAPI loadframe1(void* data) {
	VideoPlayer* vp = (VideoPlayer*)data;
	for (int i = vp->ThreadStart1; i < FRAMES; i += 2)
	{
		if (vp->KillThread1)
		{
			vp->KillThread1 = false;
			vp->Thread1 = NULL;
			return 0;
		}
		//std::cout << "T1" << std::endl;
		while (vp->NowPlaying + BUFFER < i)
		{
			if (vp->KillThread1)
			{
				vp->KillThread1 = false;
				vp->Thread1 = NULL;
				return 0;
			}
			Sleep(10);
		}
		if (vp->LoadedFrames[i])
			continue;
		if (vp->Frame[i].ReadImage())
		{
			vp->LoadedFrames[i] = true;
			vp->FramesCount++;
			std::cout << "Loaded " << i << std::endl;
		}
		else
			AfxMessageBox("Could not read image");
	}
	std::cout << "Thread Ended" << std::endl;
	return 0;
}

DWORD WINAPI loadframe2(void* data) {
	VideoPlayer* vp = (VideoPlayer*)data;
	for (int i = vp->ThreadStart2; i < FRAMES; i += 2)
	{
		if (vp->KillThread2)
		{
			vp->KillThread2 = false;
			vp->Thread2 = NULL;
			return 0;
		}
		//std::cout << "T2" << std::endl;
		while (vp->NowPlaying + BUFFER < i)
		{
			if (vp->KillThread2)
			{
				vp->KillThread2 = false;
				vp->Thread2 = NULL;
				return 0;
			}
			Sleep(10);
		}
		if (vp->LoadedFrames[i])
			continue;
		if (vp->Frame[i].ReadImage())
		{
			vp->LoadedFrames[i] = true;
			vp->FramesCount++;
			std::cout << "Loaded " << i << std::endl;
		}
		else
			AfxMessageBox("Could not read image");
	}
	std::cout << "Thread Ended" << std::endl;
	return 0;
}

int VideoPlayer::LoadFrames(int start)
{
	ThreadStart1 = start;
	Thread1 = CreateThread(NULL, 0, loadframe, this, 0, NULL);
	return 0;
}

int VideoPlayer::LoadFramesDoubleThread(int start)
{
	ThreadStart1 = start;
	ThreadStart2 = start + 1;
	Thread1 = CreateThread(NULL, 0, loadframe1, this, 0, NULL);
	Thread2 = CreateThread(NULL, 0, loadframe2, this, 0, NULL);
	return 0;
}

long long int VideoPlayer::GetTime()
{
	SYSTEMTIME st;
	FILETIME ft;
	ULARGE_INTEGER time;
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	memcpy(&time, &ft, sizeof(time));
	return (long long int)time.QuadPart / 10000;
}

int VideoPlayer::SetStartTime()
{
	StartTime = GetTime();
	return 0;
}

int VideoPlayer::SetPauseTime()
{
	PauseTime = GetTime();
	return 0;
}

MyImage* VideoPlayer::GetFrame()
{
	if (Playing)
	{
		long long int TimeElapsed = GetTime() - StartTime;
		while (NowPlaying < FRAMES && TimeElapsed >= FrameTime[NowPlaying + 1])
		{
			UnloadFrame(NowPlaying);
			NowPlaying++;
		}
	}

	return &Frame[NowPlaying];
}


int VideoPlayer::UnloadFrame(int n)
{
	if (LoadedFrames[n])
	{
		Frame[n].Delete();
		FramesCount--;
		LoadedFrames[n] = false;
	}
	return 0;
}

int VideoPlayer::VideoPlay()
{
	if (Playing)
	{
		AfxMessageBox("Already Playing");
		return 0;
	}
	if (Paused)
	{
		AfxMessageBox("Please Use Resume");
		return 0;
	}
	SetStartTime();
	if(PreBuffered && !Thread1)
		LoadFramesDoubleThread(BUFFER);
	else if(!Thread1)
		LoadFramesDoubleThread(0);
	Playing = true;
	return 0;
}

int VideoPlayer::VideoPlayFrom(int ms)
{
	if (Playing)
	{
		AfxMessageBox("Already Playing");
		return 0;
	}
	if (Paused)
	{
		AfxMessageBox("Please Use Resume");
		return 0;
	}
	SetStartTime();
	StartTime -= ms;
	int frame = 0;
	for (int i = 0; i < FRAMES; i++)
	{
		if (GetTime() - StartTime <= FrameTime[i])
		{
			frame = i;
			break;
		}
	}
	KillThreads();
	UnloadAllFrames();
	if (PreBuffered && !Thread1)
		LoadFramesDoubleThread(BUFFER + frame);
	else if (!Thread1)
		LoadFramesDoubleThread(frame);
	Playing = true;
	return 0;
}

int VideoPlayer::VideoPause()
{
	if (Paused)
	{
		AfxMessageBox("Already Paused");
		return 0;
	}
	SetPauseTime();
	Playing = false;
	Paused = true;
	return 0;
}

int VideoPlayer::VideoResume()
{
	if (!Paused)
	{
		AfxMessageBox("Not Paused");
		return 0;
	}
	StartTime = GetTime() - PauseTime + StartTime;
	Playing = true;
	Paused = false;
	return 0;
}

int VideoPlayer::VideoStop()
{
	if (Paused)
		VideoResume();
	KillThreads();
	Playing = false;
	SetStartTime();
	SetPauseTime();
	std::cout << "Framecnt--------" << FramesCount << std::endl;
	UnloadAllFrames();
	std::cout << "Framecnt--------" << FramesCount << std::endl;
	LoadFramesDoubleThread(0);
	while (!FramesCount);
	NowPlaying = 0;
	return 0;
}

int VideoPlayer::KillThreads()
{
	KillThread1 = true;
	KillThread2 = true;
	while (Thread1 || Thread2)
		Sleep(10);
	KillThread1 = false;
	KillThread2 = false;
	return 0;
}

int VideoPlayer::IsPlaying()
{
	return Playing;
}

int VideoPlayer::IsPaused()
{
	return Paused;
}

int VideoPlayer::UnloadAllFrames()
{
	for (int i = 0; i < FRAMES; i++)
		if (LoadedFrames[i])
		{
			UnloadFrame(i);
			LoadedFrames[i] = false;
			FramesCount--;
		}
	return 0;
}
