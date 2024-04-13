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

void lookAt(Matrix &position, Matrix &target, Matrix &up, Matrix &mat);
void perspective(float fov_y, float aspect, float n, float f, Matrix &mat);
void rotationX(REAL angle, Matrix &mat);
void rotationY(REAL angle, Matrix &mat);
void rotationZ(REAL angle, Matrix &mat);
void translateObject(Obj &o, Matrix &v);
void transformObject(Obj &o, Matrix &m);
light *create_light(float x, float y, float z, color c, float i);
void createSphere(Obj &o, int sectors, int stacks, float radius);
void __renderObject(light *l, Obj &o, Matrix &view, Matrix &perspective, Matrix &from, int w, int h, int onlyVertices);
//void render_vertices(vertex **vertices, int length, float *camera, float *projection, int w, int h);

void initLap();
void startLap();
void endLap(std::string desc);
void meanLap(std::string desc);
