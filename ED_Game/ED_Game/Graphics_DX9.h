#pragma once
#pragma comment(lib, "winmm.lib")
//DirectX
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class Graphics_DX9
{

	//Application
	HWND				m_hWnd;			// Handle to the window
	bool				m_bVsync;		// Boolean for vertical syncing

	//Direct3D Variables
	IDirect3D9*				m_pD3DObject;	// Direct3D 9 Object
	IDirect3DDevice9*		m_pD3DDevice;	// Direct3D 9 Device
	D3DCAPS9				m_D3DCaps;		// Device Capabilities
	D3DPRESENT_PARAMETERS	D3Dpp;

	//Font
	ID3DXFont*			m_pD3DFont;		// Font Object

	//Sprites
	ID3DXSprite*		m_pD3DSprite;	// Sprite Object
	IDirect3DTexture9*	m_pTexture;		// Texture Object for a sprite
	D3DXIMAGE_INFO		m_imageInfo;	// File details of a texture

	//Input
	IDirectInput8* m_pDIObject;
	IDirectInputDevice8* m_pDIKeyboard;
	IDirectInputDevice8* m_pDIMouse;
	DIMOUSESTATE mouseState;
	unsigned char Buffer[256];
	bool m_BoolBuf[256];
	bool LeftMouseDown;
	bool RightMouseDown;

	//State
	int State;
	void setState(int iState);
	bool windowed;
	void EnableFullscreen(bool);

	//mouse
	long m_Mousex, m_Mousey;

	//Game, TODO put game class here

	//Frame data
	long mFPS;
	long long mMilliSecPerFrame;
	
public:
	//constructor & destructor
	Graphics_DX9(void);
	~Graphics_DX9(void);

	//Init: takes in window paramaters to tie directX to the window display
	void Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed);

	//Update: takes in delta time
	void Update(float dt);

	//Render: thankfully we can use the handles from init so no parameters
	void Render();

	//shutdown: clean up, run at end of program
	void Shutdown();



};