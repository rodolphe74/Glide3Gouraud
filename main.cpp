#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "globals.h"
#include "obj.h"
#include "3d.h"
#include "obj.h"

#include <string>  
#include <iostream> 
#include <sstream> 
#include <fstream>

#include "matrix.h"

Obj *o;
Light *lg;

Matrix _fromPosition_({ 0.0f, 0.0f, 5.0f }, VEC3);
//Matrix _fromPosition_({ 0.0f, 4.0f, 18.0f }, VEC3);
Matrix _toTarget_({ 0.0f, 0.0f, 0.0f }, VEC3);
Matrix _up_({ 0.0f, 1.0f, 0.0f }, VEC3);
Matrix _view_(MAT4);
Matrix _perspective_(MAT4);
Matrix _rotationY_(MAT4);
Matrix _rotationZ_(MAT4);
Matrix _translationY_(VEC4);


int Start(HWND hwin)
{
	lookAt(_fromPosition_, _toTarget_, _up_, _view_);
	perspective((float)TO_RADIAN(90.0f), 1.0f, 0.1f, 100.0f, _perspective_);

	rotationY((float)TO_RADIAN(1.0f/2), _rotationY_);
	rotationZ((float)TO_RADIAN(0.8f/2), _rotationZ_);

	Color c = { 255, 255, 255 };
	lg = createLight(0.0f, 0.0f, 8.0f, c, 255.0f);

	// o = new Obj("./Donkey.obj");

	o = new Obj("./cube3.obj");

	//o = new Obj("./scenez.obj");
	//o->loadMaterials("scene.mtl");

	// Init Glide
	grGlideInit();
	// Select first subsystem
	grSstSelect(0);

	// Open Window 640x480 at 60Hz
	grSstWinOpen((FxU32)hwin,
		GR_RESOLUTION_640x480,
		GR_REFRESH_60Hz,
		GR_COLORFORMAT_ARGB,
		GR_ORIGIN_LOWER_LEFT,
		2,
		1);

	// Tell glide were to find the coordinates and the colors
	grCoordinateSpace(GR_WINDOW_COORDS);
	grVertexLayout(GR_PARAM_XY, 0, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_Z, 8, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_PARGB, 12, GR_PARAM_ENABLE);


	// Use LOCAL, CONSTANT color
	grColorCombine(GR_COMBINE_FUNCTION_LOCAL,
		GR_COMBINE_FACTOR_NONE,
		GR_COMBINE_LOCAL_ITERATED,
		GR_COMBINE_OTHER_NONE,
		FXFALSE);

	grDepthBufferMode(GR_DEPTHBUFFER_ZBUFFER);
	grDepthBufferFunction(GR_CMP_LESS);
	grDepthMask(FXTRUE);

	initLap();

	return 1;
}

void End()
{
	// Correctly terminates Glide
	grGlideShutdown();
	delete o;
}

int Update()
{
	// Clear buffers : color buffer = 0, alpha buffer and depth buffer : not used
	grBufferClear(0x410994, 0, 0xFFFF);

	startLap();
	transformObject(*o, _rotationY_);
	transformObject(*o, _rotationZ_);
	renderObject(lg, *o, _view_, _perspective_, _fromPosition_, 640, 480, false);
	endLap("Update");

	// Wait for vertical retrace and Swap buffers.
	grBufferSwap(1);

	return 1;
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		if (!Start(hwnd))
			return -1;
		return 0;
	case WM_DESTROY:
		meanLap("Update");
		End();
		PostQuitMessage(0);
		return 0;
	case WM_SETCURSOR:
		SetCursor(FALSE);
		return TRUE;
	case WM_ACTIVATEAPP:
		if (!wParam)
			DestroyWindow(hwnd);
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	WCHAR name[50];
	MultiByteToWideChar(0, 0, "WinGlide", 50, name, 50);
	//static char name[] = "WinGlide";
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = name;
	RegisterClass(&wc);

	HWND hwnd = CreateWindow(
		name,
		name,
		WS_POPUP,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		hInstance,
		NULL);

	if (hwnd == NULL)
		return FALSE;
	ShowWindow(hwnd, nCmdShow);

	MSG msg;
	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
				return msg.wParam;
			DispatchMessage(&msg);
		}
		if (!Update())
			DestroyWindow(hwnd);
	}
	return 0;
}
