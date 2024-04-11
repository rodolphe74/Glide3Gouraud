/***************************************************************************
 *
 *  File:               startup.cpp
 *  Content:    Glide Startup V1.2
 *  Author:             Boris Donko [boris.donko@uni-mb.si]
 *
 ***************************************************************************/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "globals.h"
#include "obj.h"
#include "3d.h"
#include "mat.h"
#include "obj.h"

#include <string>  
#include <iostream> 
#include <sstream> 
#include <fstream>

#include "matrix.h"

Obj *o;
light *lg;

// 3D maths objects
vec fromPosition{ 0.0f, 0.0f, 20.0f };
vec toTarget{ 0.0f, 0.0f, 0.0f };
vec up{ 0.0f, 1.0f, 0.0f };
mat view(4, 4);
mat perspective(4, 4);


int Start(HWND hwin)
{
	// DEBUG
	//Matrix m(VTYPE::VEC3);
	//m.vecSetAt(0, 1);
	//m.vecSetAt(1, 2);
	//m.vecSetAt(2, 3);
	//std::ofstream out("matrix.log", std::ios_base::app);
	//out << m << std::endl;
	//m.setType(VTYPE::MAT4);
	//m.matSetAt(0, 0, 1);
	//m.matSetAt(1, 0, 2);
	//m.matSetAt(2, 0, 3);
	//m.matSetAt(3, 0, 4);
	//m.matSetAt(0, 1, 5);
	//m.matSetAt(1, 1, 6);
	//m.matSetAt(2, 1, 7);
	//m.matSetAt(3, 1, 8);
	//m.matSetAt(0, 2, 9);
	//m.matSetAt(1, 2, 10);
	//m.matSetAt(2, 2, 11);
	//m.matSetAt(3, 2, 12);
	//m.matSetAt(0, 3, 13);
	//m.matSetAt(1, 3, 14);
	//m.matSetAt(2, 3, 15);
	//m.matSetAt(3, 3, 16);
	//out << m << std::endl;


	//Matrix n(VTYPE::VEC4);
	//n.vecSetAt(0, 1);
	//n.vecSetAt(1, 2);
	//n.vecSetAt(2, 3);
	//n.vecSetAt(3, 4);
	//// n.matMul(m);
	//out << m << std::endl;
	//out << n << std::endl;
	//n.matMul(m);
	//out << n << std::endl;


	//Matrix p({ 1, 2, 3, 4, 5, 6, 7, 8, 9 }, VTYPE::MAT3);
	//Matrix q({ 10, 11, 12, 13, 14, 15, 16, 17, 18 }, VTYPE::MAT3);
	//out << p << std::endl;
	//out << q << std::endl;

	//Matrix r(VTYPE::MAT3);
	//startLap();
	//for (int i = 0; i < 1000000; i++) {
	//	r.copy(p);
	//	r.matMul(q);
	//}
	//endLap("mul");
	//out << r << std::endl;

	//startLap();
	//q.storeTransposed();	// allow faster column slices retrieval
	//for (int i = 0; i < 1000000; i++) {
	//	r.copy(p);
	//	r.matMulMmx(q);
	//}
	//endLap("mulmmx");
	//out << r << std::endl;

	//Matrix s({ 1, 2, 3, 4, 5, 6, 7, 8, 9 }, MAT3);
	//Matrix t({ 10, 11, 12, 13, 14, 15, 16, 17, 18 }, MAT3);
	//s.matMul(t);
	//out << s << std::endl;
	//Matrix u({ 1, 2, 3, 4, 5, 6, 7, 8, 9 }, MAT3);
	//t.storeTransposed();
	//u.matMulMmx(t);
	//out << u << std::endl;

	//exit(1);
	////////


	_lookAt(fromPosition, toTarget, up, view);
	_perspective((float)TO_RADIAN(90.0f), 1.0f, 0.1f, 100.0f, perspective);

	color c = { 255, 255, 255 };
	lg = create_light(0.0f, 0.0f, 8.0f, c, 255.0f);

	o = new Obj("./Donkey.obj");

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
	grBufferClear(0, 0, 0xFFFF);

	mat rotationMatY(4, 4);
	rotationMatY.rotationY((float)TO_RADIAN(1.0f));
	transformObject(*o, rotationMatY);

	mat rotationMatZ(4, 4);
	rotationMatZ.rotationZ((float)TO_RADIAN(0.8f));
	transformObject(*o, rotationMatZ);

	renderObject(lg, *o, view, perspective, fromPosition, 640, 480, false);

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
