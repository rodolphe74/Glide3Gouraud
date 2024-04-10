#pragma once

#include <glide.h>
#include <string>
#include <fstream>
#include <chrono>
#include "globals.h"
#include "mat.h"
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

void _lookAt(vec &position, vec &target, vec &up, mat &mat);
void _perspective(float fov_y, float aspect, float n, float f, mat &mat);
vec getVertexAsVector(_vertex *v);
vec getVertexNormalsAsVector(_vertex *v);
vec getLightPos(light *l);
vec getLightColor(light *l);
vec getObjectColor(Obj &o);

void translateObject(Obj &o, vec &v);
void transformObject(Obj &o, mat &m);

light *create_light(float x, float y, float z, color c, float i);
void createSphere(Obj &o, int sectors, int stacks, float radius);

void renderObject(light *l, Obj &o, mat view, mat perspective, vec from, int w, int h, int onlyVertices);
//void render_vertices(vertex **vertices, int length, float *camera, float *projection, int w, int h);


void startLap();
void endLap(std::string desc);

