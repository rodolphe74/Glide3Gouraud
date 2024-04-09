/***************************************************************************
 *
 *  File:               startup.cpp
 *  Content:    Glide Startup V1.2
 *  Author:             Boris Donko [boris.donko@uni-mb.si]
 *
 ***************************************************************************/

#define WIN32_LEAN_AND_MEAN


//#include <glide.h>
#include <windows.h>
#include "globals.h"
#include "3d.h"
#include "mathc.h"
#include "mat.h"
#include "obj.h"

#include <string>  
#include <iostream> 
#include <sstream> 
#include <fstream>



//typedef struct {
//	FxFloat x, y;	// (x,y) coordinates
//	FxU32 argb;		// color of each vertex
//} Vertex;

//Vertex t[3], l[2];


Fx::Vertex t[4], l[2];

//std::ofstream fout("Fx.log");

// Global 3D objects
object *o;
light *lg;
object *lo;

// 3D maths objects
vec fromPosition{ 0.0f, 0.0f, 8.0f };
vec toTarget{ 0.0f, 0.0f, 0.0f };
vec up{ 0.0f, 1.0f, 0.0f };
mat view(4, 4);
mat perspective(4, 4);


int Start(HWND hwin)
{
	Obj *oo = new Obj("./cube.obj");
	delete oo;


	/*vec fromPosition{ 0.0f, 0.0f, 5.0f };
	vec toTarget{ 0.0f, 0.0f, 0.0f };
	vec up{ 0.0f, 1.0f, 0.0f };*/
	/*mat view(4, 4);*/
	_lookAt(fromPosition, toTarget, up, view);
	//fout << view << std::endl;

	/*mat perspective(4, 4);*/
	_perspective((float)TO_RADIAN(90.0f), 1.0f, 0.1f, 100.0f, perspective);
	//fout << perspective << std::endl;

	//typedef struct light {
	//	float	pos[VEC4_SIZE];
	//	float	intensity;
	//	color		c;
	//} light;

	color c = { 255, 255, 255 };
	lg = create_light(0.0f, 0.0f, 8.0f, c, 255.0f);
	//light = new Light(0.0f, 0.0f, 8.0f, color(255, 255, 255), 255.0f);

	mat rotationMatX(4, 4);
	//fout << rotationMatX << std::endl;
	//mat4_rotation_y(rotationMatX.getData(), TO_RADIAN(30.0f));
	rotationMatX.rotationZ((float)TO_RADIAN(30.0f));
	//fout << rotationMatX << std::endl;


	mat translation(4, 4);
	vec v({ 1, 2, 3 });
	translation.translation(v);
	//fout << translation << std::endl;


	// Load a 3d object
	o = create_object(0);
	o->color = c;
	create_object_from_obj_file(o, (char *)"./cube.obj");
	//fout << "object size :" << o->length << std::endl;
	/*renderObject(lg, o, view, perspective, fromPosition, 640, 480, false);*/
	//free_object(o);

	//WCHAR    str1[50];
	//WCHAR    str2[50];

	//MultiByteToWideChar(0, 0, "Press the <ESC> key to exit", 50, str1, 50);
	//MultiByteToWideChar(0, 0, "Glide 3 Tutorial - Tutorial demo 4", 50, str2, 50);

	//MessageBox(hwin,
	//	str1,
	//	str2,
	//	0);

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
	//grVertexLayout(GR_PARAM_XY, 0, GR_PARAM_ENABLE);
	//grVertexLayout(GR_PARAM_PARGB, 8, GR_PARAM_ENABLE);

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

	// Set coordinates for line to [(100,450),(50,50)]
	l[0].x = 100; l[0].y = 50;  l[0].argb = 0x000000ff; l[0].z = 20;
	l[1].x = 450; l[1].y = 50;  l[1].argb = 0x00ff0000; l[1].z = 20;

	// Set coordinates for triangle to [(300,300),(400,400),(400,300)]
	t[0].x = t[0].y = 100;		t[0].argb = 0x00ff0000;
	t[1].x = t[1].y = 450;		t[1].argb = 0x0000ff00;
	t[2].x = 450; t[2].y = 100; t[2].argb = 0x000000ff;
	t[3].x = 200; t[3].y = 200; t[2].argb = 0x000000ff;

	return 1;
}

void End()
{
	// Correctly terminates Glide
	grGlideShutdown();
	free_object(o);
}

int Update()
{
	// Clear buffers : color buffer = 0, alpha buffer and depth buffer : not used
	grBufferClear(0, 0, 0xFFFF);

	// Note that there is no need to set the constant color.  It's not used.

	// Draw primitives
	//grDrawLine(&l[0], &l[1]);
	//grDrawTriangle(&t[0], &t[1], &t[2]);

	//grCullMode(GR_CULL_POSITIVE);
	//grDrawVertexArrayContiguous(GR_POLYGON, 5, t, sizeof(Fx::Vertex));

	mat rotationMatY(4, 4);
	mat rotationMatZ(4, 4);
	rotationMatY.rotationY((float)TO_RADIAN(1.0f));
	transform_object(o, rotationMatY);
	rotationMatZ.rotationZ((float)TO_RADIAN(0.8f));
	transform_object(o, rotationMatZ);

	renderObject(lg, o, view, perspective, fromPosition, 640, 480, false);

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
