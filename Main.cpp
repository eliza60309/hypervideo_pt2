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
#include "Video.h"
#include <iostream>
#include "Windows.h"
#include "MMSystem.h"
#include "shobjidl.h"
#include "sound.h"
#include "fstream"



#define MAX_LOADSTRING 200
#define FRAMES 9000
#define BUFFER 100
#define FPS 30

#include <string>
#include <sstream>
#include <cstdlib>
#include <map>
#include <vector>
#include <deque>
// Global Variables:
MyImage			vid1[FRAMES], vid2[FRAMES];
MyImage* leftImg, * rightImg;
int LeftPlayingFrame = 0;
int RightPlayingFrame = 0;
int frametime[FRAMES];
bool loadedframe1[FRAMES] = {};
bool loadedframe2[FRAMES] = {};

SoundPlayer sp1, sp2;
VideoPlayer vp1, vp2;

char			SoundPath1[_MAX_PATH];			// sound wav file
char			SoundPath2[_MAX_PATH];			// sound wav file
char            HyperPath[_MAX_PATH];
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

enum class LCLICKSTATUS { LDOWN = 1, LUP = 2 };
LCLICKSTATUS lclickStatus = LCLICKSTATUS::LUP;
enum class RCLICKSTATUS { RDOWN = 1, RUP = 2 };
RCLICKSTATUS rclickStatus = RCLICKSTATUS::RUP;

enum class LINKSTATUS { NORMAL = 1, LINKING = 2 };
LINKSTATUS linkStatus = LINKSTATUS::NORMAL;

int boxB = rand() % 205 + 50, boxG = rand() % 205 + 50, boxR = rand() % 205 + 50;
int click_X=0,click_Y=0;

int box_time=45;
using namespace std;
deque<int> current_hyper_frame_deque;
map<int, vector<vector<int>>> drawing_rec_map;
map<int, vector<vector<int>>> hyperlink_map;
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
	sscanf(lpCmdLine, "%s %s %s %s %s", &FramePath1, &SoundPath1, &FramePath2, &SoundPath2, &HyperPath);

	//setting sound path

	sp1.SetPath(SoundPath1);
	sp2.SetPath(SoundPath2);
	sp1.Setup();
	sp2.Setup();
	vp1.SetPath(FramePath1);
	vp2.SetPath(FramePath2);
	vp1.Setup();
	vp2.Setup();
	vp1.LoadFramesDoubleThread(BUFFER);
	vp2.LoadFramesDoubleThread(BUFFER);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	ifstream box_file(HyperPath);
	string line;
	while (getline(box_file, line)) {
		stringstream ss(line);
		string s;


		getline(ss, s , ' ');
		int key=stoi(s);
		getline(ss, s , ' ');
		int map_frame=stoi(s);

		vector<int> val_tmp_vec;
		val_tmp_vec.push_back(map_frame);
		for (int i = 0; i < 4; i++) {

			getline(ss, s , ' ');
			val_tmp_vec.push_back(stoi(s));
			//cout << stoi(s) << endl;
		}
		// cout<<"key:"<<key<<endl;
		for (auto i: val_tmp_vec){
				cout << i << ' ';
		}

		hyperlink_map[key].push_back(val_tmp_vec);
	}

	// Perform application initialization:

	if (!InitInstance(hInstance, nCmdShow))
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

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
bool check_mouse_location(int x,int y,vector<int> rec	){
	cout<<"rec:"<<rec[1]<<" "<<rec[3]<<" "<<rec[2]<<" "<<rec[4]<<endl;
	if(x>=rec[1]&&x<=rec[3]&&y>=rec[2]&&y<=rec[4]){
		return true;
	}
	else{
		return false;
	}


}
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
	int mousePositionX, mousePositionY;

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
			10, 60, 104, 25,
			hWnd, (HMENU)ID_BUTTONLPLAY, NULL, NULL);
		CreateWindow(TEXT("button"), TEXT("Pause"),
			WS_VISIBLE | WS_CHILD,
			124, 60, 104, 25,
			hWnd, (HMENU)ID_BUTTONLPAUSE, NULL, NULL);
		CreateWindow(TEXT("button"), TEXT("Stop"),
			WS_VISIBLE | WS_CHILD,
			238, 60, 104, 25,
			hWnd, (HMENU)ID_BUTTONLSTOP, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("Play"),
			WS_VISIBLE | WS_CHILD,
			412, 60, 104, 25,
			hWnd, (HMENU)ID_BUTTONRPLAY, NULL, NULL);
		CreateWindow(TEXT("button"), TEXT("Pause"),
			WS_VISIBLE | WS_CHILD,
			526, 60, 104, 25,
			hWnd, (HMENU)ID_BUTTONRPAUSE, NULL, NULL);
		CreateWindow(TEXT("button"), TEXT("Stop"),
			WS_VISIBLE | WS_CHILD,
			640, 60, 104, 25,
			hWnd, (HMENU)ID_BUTTONRSTOP, NULL, NULL);
		linkStatus = LINKSTATUS::LINKING;
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;

		case ID_BUTTONLPLAY:
			if (!vp1.IsPlaying() && !vp1.IsPaused()) {
				cout << "Play" << endl;
				vp1.VideoPlay();
				sp1.SoundPlay(0);
			}
			else if (vp1.IsPaused()) {
				cout << "Resume" << endl;
				vp1.VideoResume();
				sp1.SoundResume();
			}
			break;
		case ID_BUTTONLPAUSE:
			vp1.VideoPause();
			sp1.SoundPause();
			InvalidateRect(hWnd, &rt, false);
			break;
		case ID_BUTTONLSTOP:
			vp1.VideoStop();
			sp1.SoundStop();
			break;
		case ID_BUTTONRPLAY:
			if (!vp2.IsPlaying() && !vp2.IsPaused()) {
				cout << "Play" << endl;
				vp2.VideoPlay();
				sp2.SoundPlay(0);
			}
			else if (vp2.IsPaused()) {
				cout << "Resume" << endl;
				vp2.VideoResume();
				sp2.SoundResume();
			}
			break;
		case ID_BUTTONRPAUSE:
			vp2.VideoPause();
			sp2.SoundPause();
			InvalidateRect(hWnd, &rt, false);
			break;
		case ID_BUTTONRSTOP:
			vp2.VideoStop();
			sp2.SoundStop();
			break;


		case ID_MODIFY_IMAGE:
			// PlaySound(TEXT(SoundPath1), NULL, SND_ASYNC);			// New addition to the code to play a wav file
			// outImage.Modify();
			// InvalidateRect(hWnd, &rt, false);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_LBUTTONDOWN:

		mousePositionX = GET_X_LPARAM(lParam);
		mousePositionY = GET_Y_LPARAM(lParam);
		if (linkStatus == LINKSTATUS::LINKING) {

				lclickStatus = LCLICKSTATUS::LDOWN;

		}

		break;
	case WM_LBUTTONUP:
		// get final bounding box coordinates

		mousePositionX = GET_X_LPARAM(lParam);
		mousePositionY = GET_Y_LPARAM(lParam);
		if (lclickStatus == LCLICKSTATUS::LDOWN) {
			if (mousePositionX >= 0 && mousePositionX < 0 + leftImg->getWidth() &&
				mousePositionY >= 100 && mousePositionY < 100 + leftImg->getHeight()) {		// click on left video
				click_X = mousePositionX, click_Y = mousePositionY - 100;
				lclickStatus = LCLICKSTATUS::LDOWN;
				// cout<<"click_X:"<<click_X<<" click_Y:"<<click_Y<<endl;
			}
			if (!current_hyper_frame_deque.empty()){
				// cout<<"size:"<<current_hyper_frame_deque.size()<<endl;
				for (int n=0;n<current_hyper_frame_deque.size();n++){
					for (int i = 0; i < hyperlink_map[current_hyper_frame_deque[n]].size(); i++) {
						bool check=check_mouse_location(click_X,click_Y,hyperlink_map[current_hyper_frame_deque[n]][i]);
						// cout<<"check:"<<check<<endl;
						if (check){
							cout<<"GO Hyper!!!"<<endl;
							if (vp1.IsPlaying()){
								vp1.VideoPause();
								sp1.SoundPause();
							}
							vp2.VideoStop();
							sp2.SoundStop();
							vp2.VideoPlayFrom(hyperlink_map[current_hyper_frame_deque[n]][i][0]);
							sp2.SoundPlay(hyperlink_map[current_hyper_frame_deque[n]][i][0]);
						}

					}
				}

			}
		}




		lclickStatus = LCLICKSTATUS::LUP;
		break;
	case WM_PAINT:
	{

		if (!start)
		{

			start = true;
			SetTimer(hWnd, IDT_TIMER_1, 5, (TIMERPROC)NULL);

			//PlaySound(TEXT(SoundPath), NULL, SND_ASYNC);

		}
		hdc = BeginPaint(hWnd, &ps);
		// TO DO: Add any drawing code here...
		//char text[1000];
		//strcpy(text, inImage[frame].getImagePath());
		//DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

		leftImg = vp1.GetFrame();
		rightImg = vp2.GetFrame();
		// cout<<"current frame:"<<vp1.NowPlaying<<endl;

		//check need to draw rectangle or not
		if (hyperlink_map.count(vp1.NowPlaying)==1 ){
			// cout<<"add frame:"<<vp1.NowPlaying<<endl;
			if ((!current_hyper_frame_deque.empty()&&current_hyper_frame_deque.back()!=vp1.NowPlaying)||current_hyper_frame_deque.empty()){
				current_hyper_frame_deque.push_back(vp1.NowPlaying);
			}

		}

		else{
			if (!current_hyper_frame_deque.empty()){
				if (vp1.NowPlaying-current_hyper_frame_deque[0]>box_time){
					// drawing_rec_map.erase(current_hyper_frame_deque[0]);
					current_hyper_frame_deque.pop_front();

				}
			}


		}
		//draw to-draw rectangle
		if (!current_hyper_frame_deque.empty()){
			for (int n=0;n<current_hyper_frame_deque.size();n++){
				for (int i = 0; i < hyperlink_map[current_hyper_frame_deque[n]].size(); i++) {
					// cout<<"hyper frame:"<<current_hyper_frame_deque[n]<<endl;
					// cout<<"drawing rec size:"<<hyperlink_map[current_hyper_frame_deque[n]].size()<<endl;
					leftImg->TargetArea(hyperlink_map[current_hyper_frame_deque[n]][i][1], hyperlink_map[current_hyper_frame_deque[n]][i][2], hyperlink_map[current_hyper_frame_deque[n]][i][3], hyperlink_map[current_hyper_frame_deque[n]][i][4], boxB, boxG, boxR);

				}
			}

		}



		BITMAPINFO bmi;
		CBitmap bitmap;
		memset(&bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth = leftImg->getWidth();
		bmi.bmiHeader.biHeight = -leftImg->getHeight();  // Use negative height.  DIB is top-down.
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = leftImg->getWidth() * leftImg->getHeight();

		SetDIBitsToDevice(hdc,
			0, 100, leftImg->getWidth(), leftImg->getHeight(),
			0, 0, 0, leftImg->getHeight(),
			leftImg->getImageData(), &bmi, DIB_RGB_COLORS);
		SetDIBitsToDevice(hdc,
			rightImg->getWidth() + 50, 100, rightImg->getWidth(), rightImg->getHeight(),
			0, 0, 0, rightImg->getHeight(),
			rightImg->getImageData(), &bmi, DIB_RGB_COLORS);


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
