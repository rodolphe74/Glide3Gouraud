#pragma once

#include <glide.h>
#include <string>
#include <fstream>
#include <chrono>
#include "globals.h"
#include "matrix.h"
#include "obj.h"


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define M_PI 3.14159265358979323846
#define TO_RADIAN(x) (x * (M_PI / 180))
#define VEC3_SIZE 3
#define VEC4_SIZE 4

namespace Fx {
	typedef struct {
		FxFloat x, y;	// (x,y) coordinates
		FxFloat z;
		FxU32 argb;		// color of each vertex
	} Vertex;
}

void __lookAt(Matrix &position, Matrix &target, Matrix &up, Matrix &mat);
void __perspective(float fov_y, float aspect, float n, float f, Matrix &mat);
void __rotationX(REAL angle, Matrix &mat);
void __rotationY(REAL angle, Matrix &mat);
void __rotationZ(REAL angle, Matrix &mat);
void __translateObject(Obj &o, Matrix &v);
void __transformObject(Obj &o, Matrix &m);
light *create_light(float x, float y, float z, color c, float i);
void createSphere(Obj &o, int sectors, int stacks, float radius);
void __renderObject(light *l, Obj &o, Matrix &view, Matrix &perspective, Matrix &from, int w, int h, int onlyVertices);
//void render_vertices(vertex **vertices, int length, float *camera, float *projection, int w, int h);

void startLap();
void endLap(std::string desc);

