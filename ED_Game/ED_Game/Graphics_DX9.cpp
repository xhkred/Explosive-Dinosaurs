#include "Graphics_DX9.h"
#include <math.h>

Graphics_DX9::Graphics_DX9(void)
{
	// Init or NULL objects before use to avoid any undefined behavior
	m_bVsync		= false;
	m_pD3DObject	= 0;
	m_pD3DDevice	= 0;
	setState(0);
	windowed = true;
	m_Mousex = m_Mousey = 0;
	LeftMouseDown = false;
	RightMouseDown = false;

}

Graphics_DX9::~Graphics_DX9(void)
{
	//incase shutdown wasn't called correctly
	Shutdown();
}

void Graphics_DX9::EnableFullscreen(bool aFullscrn)
{
	if(aFullscrn)
	{
		if(!D3Dpp.Windowed)
			return;
		//get the screen Resolution
		int width  = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN); 

		D3Dpp.BackBufferFormat = D3DFMT_X8R8G8B8; //not specifically sure what this is
		D3Dpp.BackBufferWidth  = width;//setting up back buffer size
		D3Dpp.BackBufferHeight = height;
		D3Dpp.Windowed         = false;//I AM NOT WINDOWED

		SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_POPUP); //set window to POPUP which removes the borders
		SetWindowPos(m_hWnd, HWND_TOP, 0, 0,
			width, height, SWP_NOZORDER | SWP_SHOWWINDOW); //Basically, reset the window size and refresh the above option for use, or "use the new settings"

	}
	else//GOING WINDOWED
	{
		// Are we already in windowed mode?
		if( D3Dpp.Windowed )
			return;
		// Default to a client rectangle of 800x600.
		RECT R = {0, 0, 800, 600};
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		D3Dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		D3Dpp.BackBufferWidth  = 800;
		D3Dpp.BackBufferHeight = 600;
		D3Dpp.Windowed         = true;
		SetWindowLongPtr(m_hWnd,GWL_STYLE,WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_hWnd, HWND_TOP, 100, 100,
			R.right, R.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);

	}
	// Reset the device with the changes.
	m_pD3DSprite->OnLostDevice();
	m_pD3DFont->OnLostDevice();
	m_pD3DDevice->Reset(&D3Dpp);
	m_pD3DSprite->OnResetDevice();
	m_pD3DFont->OnResetDevice();
}

void Graphics_DX9::Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed)
{
	LARGE_INTEGER Timer;
	QueryPerformanceCounter(&Timer);
	srand(Timer.QuadPart);
	m_hWnd = hWnd;

	// Create the D3D Object
	m_pD3DObject = Direct3DCreate9(D3D_SDK_VERSION);

	// Find the width and height of window using hWnd and GetWindowRect()
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Set D3D Device presentation parameters before creating the device
	ZeroMemory(&D3Dpp, sizeof(D3Dpp));  // NULL the structure's memory

	D3Dpp.hDeviceWindow					= hWnd;										// Handle to the focus window
	D3Dpp.Windowed						= bWindowed;								// Windowed or Full-screen boolean
	D3Dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;								// Format of depth/stencil buffer, 24 bit depth, 8 bit stencil
	D3Dpp.EnableAutoDepthStencil		= TRUE;										// Enables Z-Buffer (Depth Buffer)
	D3Dpp.BackBufferCount				= 1;										// Change if need of > 1 is required at a later date
	D3Dpp.BackBufferFormat				= D3DFMT_X8R8G8B8;							// Back-buffer format, 8 bits for each pixel
	D3Dpp.BackBufferHeight				= height;									// Make sure resolution is supported, use adapter modes
	D3Dpp.BackBufferWidth				= width;									// (Same as above)
	D3Dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;					// Discard back-buffer, must stay discard to support multi-sample
	D3Dpp.PresentationInterval			= m_bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE; // Present back-buffer immediately, unless V-Sync is on								
	D3Dpp.Flags							= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;		// This flag should improve performance, if not set to NULL.
	D3Dpp.FullScreen_RefreshRateInHz	= bWindowed ? 0 : D3DPRESENT_RATE_DEFAULT;	// Full-screen refresh rate, use adapter modes or default
	D3Dpp.MultiSampleQuality			= 0;										// MSAA currently off, check documentation for support.
	D3Dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;						// MSAA currently off, check documentation for support.

	// Check device capabilities
	DWORD deviceBehaviorFlags = 0;
	m_pD3DObject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_D3DCaps);

	// Determine vertex processing mode
	if(m_D3DCaps.DevCaps & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		// Hardware vertex processing supported? (Video Card)
		deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;	
	}
	else
	{
		// If not, use software (CPU)
		deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING; 
	}

	// If hardware vertex processing is on, check pure device support
	if(m_D3DCaps.DevCaps & D3DDEVCAPS_PUREDEVICE && deviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		deviceBehaviorFlags |= D3DCREATE_PUREDEVICE;	
	}

	// Create the D3D Device with the present parameters and device flags above
	m_pD3DObject->CreateDevice(
		D3DADAPTER_DEFAULT,		// which adapter to use, set to primary
		D3DDEVTYPE_HAL,			// device type to use, set to hardware rasterization
		hWnd,					// handle to the focus window
		deviceBehaviorFlags,	// behavior flags
		&D3Dpp,					// presentation parameters
		&m_pD3DDevice);			// returned device pointer

	// Load a font for private use for this process
	D3DXCreateFont(m_pD3DDevice, 30, 0, FW_BOLD, 0, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"),
		&m_pD3DFont);

	// Create a sprite object, note you will only need one for all 2D sprites

	D3DXCreateSprite(m_pD3DDevice, &m_pD3DSprite);

	//LOAD TEXTURES HERE			*******************************************************************			TODO

	DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDIObject, NULL);

	m_pDIObject->CreateDevice(GUID_SysKeyboard, &m_pDIKeyboard, NULL); 
	m_pDIObject->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);

	m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard); 
	m_pDIMouse->SetDataFormat(&c_dfDIMouse);

	m_pDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
	m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//Run Game.init here
}

void  Graphics_DX9::Update(float dt)
{
	static float numFrames     = 0.0f;
	static float timeElapsed = 0.0f;
	static float Timer = 0.0f;
	static float Counter = 0.0f;
	Counter += 1.0f;
	// Increment the frame count.
	numFrames += 1.0f;
	// Accumulate how much time has passed.
	timeElapsed += dt;
	// Has one second passed?--we compute the frame statistics once
	// per second.   Note that the time between frames can vary, so
	// these stats are averages over a second.
	if( timeElapsed >= 1.0f )
	{
		// Frames Per Second = numFrames / timeElapsed,
		// but timeElapsed approx. equals 1.0, so
		// frames per second = numFrames.
		mFPS = numFrames;
		// Average time, in milliseconds, it took to render a
		// single frame.
		mMilliSecPerFrame = 1000.0f / mFPS;
		// Reset time counter and frame count to prepare
		// for computing the average stats over the next second.
		timeElapsed = 0.0f;
		numFrames     = 0.0f;
	}
}

void Graphics_DX9::Render()
{
	if(!m_pD3DDevice)
		return;
	RECT rect;
	D3DCOLOR UITEXT = D3DCOLOR_ARGB(255,130,130,130);
	rect.top = 0;
	rect.left = 0;
	rect.right = D3Dpp.BackBufferWidth;
	rect.bottom = D3Dpp.BackBufferHeight;
	char cFPS[256];
	_ltoa_s(mFPS,cFPS,10);
	m_pD3DDevice->Clear(0,0,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_ARGB(0,0,0,0), 1,0);
	m_pD3DDevice->BeginScene();

	m_pD3DFont->DrawTextA(0,cFPS, -1, &rect, DT_TOP | DT_LEFT | DT_NOCLIP, UITEXT);

	
	m_pD3DDevice->EndScene();
	m_pD3DDevice->Present(NULL,NULL,NULL,NULL);
}

void Graphics_DX9::setState(int iState){
	State = iState;
}

void Graphics_DX9::Shutdown()
{
	// Release COM objects in the opposite order they were created in
	if(m_pDIKeyboard){
		m_pDIKeyboard->Unacquire();
		m_pDIKeyboard->Release();
		m_pDIKeyboard = 0;}
	if(m_pDIMouse){
		m_pDIMouse->Unacquire();
		m_pDIMouse->Release();
		m_pDIMouse = 0;} 
	if(m_pDIObject){
		m_pDIObject->Release();
		m_pDIObject =0;
	}
	// Sprite
	if(m_pD3DSprite){
		m_pD3DSprite->Release();
		m_pD3DSprite=0;}
	// Font
	if(m_pD3DFont){
		m_pD3DFont->Release();
		m_pD3DFont=0;}
	// 3DDevice	
	if(m_pD3DDevice){
		m_pD3DDevice->Release();
		m_pD3DDevice=0;}
	// 3DObject
	if(m_pD3DObject){
		m_pD3DObject->Release();
		m_pD3DObject=0;}
}

