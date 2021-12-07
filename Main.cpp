//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Creates a white RGB image with a black line and displays it.
// Two images are displayed on the screen.
// Left Pane: Input Image, Right Pane: Modified Image
//
// Author - Parag Havaldar
// Code used by students as a starter code to display and modify images
//
//*****************************************************************************


// Include class files
#include "Image.h"
#include "Sound.h"
#include "Video.h"
#include <iostream>
#include "Windows.h"
#include "MMSystem.h"
#include "shobjidl.h"

#define MAX_LOADSTRING 200
#define FRAMES 9000
#define BUFFER 100
#define FPS 30

#include <string>
#include <sstream>
#include <cstdlib>

// Global Variables:
MyImage			vid1[FRAMES], vid2[FRAMES];
MyImage			*leftImg, *rightImg;
int LeftPlayingFrame = 0;
int RightPlayingFrame = 0;
int frametime[FRAMES];
bool loadedframe1[FRAMES] = {};
bool loadedframe2[FRAMES] = {};

SoundPlayer sp1, sp2;
VideoPlayer vp1, vp2;

char			SoundPath1[_MAX_PATH];			// sound wav file
char			SoundPath2[_MAX_PATH];			// sound wav file
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

using namespace std;
/*
DWORD WINAPI loadframe1(void* data) {
	for (int i = BUFFER; i < FRAMES; i += 2)
	{
		while (LeftPlayingFrame + BUFFER < i)
			Sleep(10);
		if (vid1[i].ReadImage())
			loadedframe1[i] = true;
		else
			AfxMessageBox("Could not read image");
	}
	return 0;
}

DWORD WINAPI loadframe2(void* data) {
	int paused = false;
	for (int i = BUFFER + 1; i < FRAMES; i += 2)
	{
		while (LeftPlayingFrame + BUFFER < i)
			Sleep(10);
		if (vid1[i].ReadImage())
			loadedframe1[i] = true;
		else
			AfxMessageBox("Could not read image");
	}
	return 0;
}

DWORD WINAPI loadframe3(void* data) {
	for (int i = BUFFER; i < FRAMES; i += 2)
	{
		while (LeftPlayingFrame + BUFFER < i)
			Sleep(10);
		if (vid2[i].ReadImage())
			loadedframe2[i] = true;
		else
			AfxMessageBox("Could not read image");
	}
	return 0;
}

DWORD WINAPI loadframe4(void* data) {
	int paused = false;
	for (int i = BUFFER + 1; i < FRAMES; i += 2)
	{
		while (LeftPlayingFrame + BUFFER < i)
			Sleep(10);
		if (vid2[i].ReadImage())
			loadedframe2[i] = true;
		else
			AfxMessageBox("Could not read image");
	}
	return 0;
}
*/

// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	//opens the log console
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// Read in the image and its copy
	// width and height is currently fixed to 352 288
	int w = 352, h = 288;
	char FramePath1[_MAX_PATH] = {};
	char FramePath2[_MAX_PATH] = {};

	sscanf(lpCmdLine, "%s %s %s %s", &FramePath1, &SoundPath1, &FramePath2, &SoundPath2);

	//setting sound path
	sp1.SetPath(SoundPath1);
	sp2.SetPath(SoundPath2);
	sp1.Setup();
	sp2.Setup();
	vp1.SetPath(FramePath1);
	vp2.SetPath(FramePath2);
	vp1.Setup();
	vp2.Setup();
	//vp1.LoadFramesDoubleThread(BUFFER);
	//vp2.LoadFramesDoubleThread(BUFFER);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//file dialog
void openFile(HWND hWnd) {

}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
#define IDT_TIMER_1 1001
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
// TO DO: part useful to render video frames, may place your own code here in this function
// You are free to change the following code in any way in order to display the video
	static bool start = false;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

	switch (message)
	{
		case WM_TIMER:
			switch (wParam)
			{
				case IDT_TIMER_1:
				{

					SetTimer(hWnd, IDT_TIMER_1, 5, (TIMERPROC)NULL);
					RedrawWindow(hWnd, &rt, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				}
				break;
			}
			break;

        case WM_CREATE:
    		CreateWindow(TEXT("button"), TEXT("Play"),
    					 WS_VISIBLE | WS_CHILD,
    					 10, 60, 50, 25,
    					 hWnd, (HMENU)ID_BUTTONLPLAY, NULL, NULL);
    		CreateWindow(TEXT("button"), TEXT("Pause"),
    					 WS_VISIBLE | WS_CHILD,
    					 80, 60, 50, 25,
    					 hWnd, (HMENU)ID_BUTTONLPAUSE, NULL, NULL);
    		CreateWindow(TEXT("button"), TEXT("Stop"),
    					 WS_VISIBLE | WS_CHILD,
    					 150, 60, 50, 25,
    					 hWnd, (HMENU)ID_BUTTONLSTOP, NULL, NULL);

            CreateWindow(TEXT("button"), TEXT("Play"),
     					 WS_VISIBLE | WS_CHILD,
     					 220, 60, 50, 25,
     					 hWnd, (HMENU)ID_BUTTONRPLAY, NULL, NULL);
     		CreateWindow(TEXT("button"), TEXT("Pause"),
     					 WS_VISIBLE | WS_CHILD,
     					 290, 60, 50, 25,
     					 hWnd, (HMENU)ID_BUTTONRPAUSE, NULL, NULL);
     		CreateWindow(TEXT("button"), TEXT("Stop"),
     					 WS_VISIBLE | WS_CHILD,
     					 360, 60, 50, 25,
     					 hWnd, (HMENU)ID_BUTTONRSTOP, NULL, NULL);

    		break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case ID_READ_LEFT_VIDEO:
					//openFile(hWnd);
					if (vp1.IsPlaying())
					{
						vp1.VideoPause();
						sp1.SoundPause();
					}
					else if(vp1.IsPaused())
					{
						vp1.VideoResume();
						sp1.SoundResume();
					}
					if (vp2.IsPlaying())
					{
						vp2.VideoPause();
						sp2.SoundPause();
					}
					else if (vp2.IsPaused())
					{
						vp2.VideoResume();
						sp2.SoundResume();
					}
					break;
				case ID_READ_RIGHT_VIDEO:
					if (vp1.IsPlaying() || vp1.IsPaused())
					{
						vp1.VideoStop();
						sp1.SoundStop();
					}
					else
					{
						vp1.VideoPlay();
						sp1.SoundPlay();
					}
					if (vp2.IsPlaying() || vp2.IsPaused())
					{
						vp2.VideoStop();
						sp2.SoundStop();
					}
					else
					{
						vp2.VideoPlay();
						sp2.SoundPlay();
					}
					break;
				case ID_MODIFY_IMAGE:
				   /*PlaySound(TEXT(SoundPath1), NULL, SND_ASYNC);			// New addition to the code to play a wav file
				   outImage.Modify();
				   InvalidateRect(hWnd, &rt, false); */
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{

				if (!start)
				{

					start = true;
					SetTimer(hWnd, IDT_TIMER_1, 5, (TIMERPROC)NULL);

					vp1.VideoPlay();
					vp2.VideoPlay();
					sp1.SoundPlay();
					sp2.SoundPlay();
					//PlaySound(TEXT(SoundPath), NULL, SND_ASYNC);

				}
				hdc = BeginPaint(hWnd, &ps);
				// TO DO: Add any drawing code here...
				//char text[1000];
				//strcpy(text, inImage[frame].getImagePath());
				//DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

				leftImg = vp1.GetFrame();
				rightImg = vp2.GetFrame();
				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = leftImg->getWidth();
				bmi.bmiHeader.biHeight = -leftImg->getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = leftImg->getWidth()* leftImg->getHeight();

				SetDIBitsToDevice(hdc,
								  0,100, leftImg->getWidth(), leftImg->getHeight(),
								  0,0,0, leftImg->getHeight(),
								  leftImg->getImageData(),&bmi,DIB_RGB_COLORS);
				SetDIBitsToDevice(hdc,
								  rightImg->getWidth()+50,100,rightImg->getWidth(),rightImg->getHeight(),
								  0,0,0,rightImg->getHeight(),
								  rightImg->getImageData(),&bmi,DIB_RGB_COLORS);


				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
