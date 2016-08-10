#define VC_EXTRALEAN
#include <Windows.h>

HWND				g_hWnd;			//Winow Handle
HINSTANCE			g_hInstance;	//Application Handle
bool				g_bWindowed;	// Window or fullscreen
//DirectX plug
#include "Graphics_DX9.h"; 
//NOTE: the game will basically be defined through here

#define SCREEN_WIDTH 1980
#define SCREEN_HEIGHT 1080
#define WINDOW_TITLE L"Game"

Graphics_DX9 DXObj;									

int WINAPI wWinMain(HINSTANCE hInstance,		// Handle to the application
	HINSTANCE hPrevInstance,					// Handle to the previous app
	LPTSTR lpCmdLine,							// Command line string
	int lpCmdShow);								// Show window value

LRESULT CALLBACK WndProc(HWND hWnd,				// Handle to the window
	UINT message,								// Incoming Message
	WPARAM wparam,								// Message Info
	LPARAM lparam);								// Message Info

void InitWindow(void)
{
	WNDCLASSEX wndClass;  
	ZeroMemory(&wndClass, sizeof(wndClass));

	// set up the window
	wndClass.cbSize			= sizeof(WNDCLASSEX);			// size of window structure
	wndClass.lpfnWndProc	= (WNDPROC)WndProc;				// message callback
	wndClass.lpszClassName	= WINDOW_TITLE;					// class name
	wndClass.hInstance		= g_hInstance;					// handle to the application
	wndClass.hCursor		= LoadCursor(NULL, IDC_ARROW);	// default cursor
	wndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOWFRAME);	// background brush

	// register a new type of window
	RegisterClassEx(&wndClass);

	g_hWnd = CreateWindow(
		WINDOW_TITLE, WINDOW_TITLE, 												// window class name and title
		g_bWindowed ? WS_OVERLAPPEDWINDOW | WS_VISIBLE:(WS_POPUP | WS_VISIBLE),		// window style
		CW_USEDEFAULT, CW_USEDEFAULT,												// x and y coordinates
		SCREEN_WIDTH, SCREEN_HEIGHT,												// width and height of window
		NULL, NULL,																	
		g_hInstance,																// handle to application
		NULL);

	// Display the window
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);
}

int WINAPI wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int lpCmdShow){
	g_hInstance = hInstance;	// Store application handle
	g_bWindowed = true;			// Windowed mode or full-screen

	
	InitWindow();	// Init the window

	
	MSG msg; 
	ZeroMemory(&msg, sizeof(msg));	// Use this msg structure to catch window messages

	// Initialize DirectX/Game here																	
	DXObj.Init(g_hWnd,hInstance,g_bWindowed);

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;
	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
	// Main Windows/Game Loop
	while(msg.message != WM_QUIT)
	{


		__int64 currTimeStamp = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
		float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt; 
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//Update and Render game here																
			DXObj.Update(dt);
			DXObj.Render();	
		}
		prevTimeStamp = currTimeStamp; 
	}

	//Put all end of life code here																		
	DXObj.Shutdown();

	// Unregister window
	UnregisterClassW(WINDOW_TITLE, g_hInstance);

	// Return successful
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// attempt to handle your messages
	switch(message)
	{
	case (WM_PAINT):
		{
			InvalidateRect(hWnd,NULL,TRUE);
			break;
		}		
	case(WM_DESTROY):
		{
			PostQuitMessage(0); 
			break;
		}
	case(WM_KEYDOWN):
		{
			switch(wparam)
			{
				// Escape Key will close the application
				// Remove/Modify this call when creating a real game with menus, etc.
			case VK_ESCAPE:
				{
					PostQuitMessage(0);
					break;
				}
			}
		}
	}

	// pass to default handler
	return DefWindowProc(hWnd, message, wparam, lparam);
}




