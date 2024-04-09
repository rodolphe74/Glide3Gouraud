#pragma once

#include <vector>

#define MILLEVINGTQUATRE 1024

typedef struct _color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} _color;

typedef struct _vertex {
	float pos[/*VEC4_SIZE*/4];
	float normal[/*VEC4_SIZE*/4];
	_color colour;
	int referencesCount = 0;
} _vertex;

typedef struct _face {
	std::vector<_vertex *> vertices;
} _face;

typedef struct _object {
	std::vector<_face *> faces;
	std::vector<_vertex*> verticesList;
	_color color;
} _object;

typedef struct _light {
	float pos[/*VEC4_SIZE*/4];
	float intensity;
	_color c;
} _light;


static _color _white = { 255, 255, 255 };
static _color _gray = { 128, 128, 128 };
static _color _blue = { 0, 0, 255 };
static _color _green = { 0, 255, 0 };
static _color _yellow = { 255, 255, 0 };
static _color _cyan = { 0, 255, 255 };
static _color _red = { 255, 0, 0 };


class Obj
{
private:
	_object o;
	void cut(char *src, int start, int end, char *target);
	void split(char *string, char sep, char token_array[][50]);


public:
	Obj();
	Obj(int length, ...);
	Obj(const char *filename);
	_vertex *createVertex(double x, double y, double z);
	_vertex *createVertexColor(double x, double y, double z, _color c);
	void setNormal(_vertex *v, float x, float y, float z);
	void freeVertex(_vertex *v);
	void printVertex(_vertex *v);
	double getVertexCoord(_vertex *v, int i);
	_face *createFace(int length, ...);
	int addVertexToFace(_face *f, _vertex *v);
	void computeNormal(_face *f);
	void freeFace(_face *f);
	int addFace(_face *f);
	void freeUselessVertices();
	~Obj();
};

