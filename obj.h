#pragma once

#include <vector>
#include <map>
#include <string>
#include "globals.h"

#define MILLEVINGTQUATRE 1024
#define DEUXCENTCINQUANTESIX 256

typedef struct _material {
	float diffuseLightColor[3];		// Kd
	float specularLightColor[3];	// Ks
	float ambient[3];				// Ka
	float specularStrength = 1.0f;
	int shininess;					// Ns
} Material;

typedef struct _color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Color;

typedef struct _vec4 {
	float x, y, z, w;
} Vec4;

typedef struct _vertex {
	Vec4 pos;
	Color colour;
	int referencesCount = 0;
	Material material;
} Vertex;

typedef struct _face {
	std::vector<Vertex *> vertices;
	std::vector<Vec4> normals;
} Face;

typedef struct _object {
	std::vector<Face *> faces;
	Color color;
	char materialName[DEUXCENTCINQUANTESIX];
	Material material;
} Object;

typedef struct _light {
	float pos[4];
	float intensity;
	Color c;
} Light;



static Color white = { 255, 255, 255 };
static Color gray = { 128, 128, 128 };
static Color blue = { 0, 0, 255 };
static Color green = { 0, 255, 0 };
static Color yellow = { 255, 255, 0 };
static Color cyan = { 0, 255, 255 };
static Color red = { 255, 0, 0 };


class Obj
{
private:
	void cut(char *src, int start, int end, char *target);
	void split(char *string, char sep, char token_array[][50]);

public:
	//Object o;
	Color color;
	std::vector<Vertex *> vertices;
	static std::map<std::string, Material> materials;
	static std::map<std::string, Object *> objects;
	Obj();
	Obj(int length, ...);
	Obj(const char *filename);
	void loadObjects(const char *filename);
	void loadMaterials(const char *filename);
	void applyMaterials();
	Vertex *createVertex(double x, double y, double z);
	Vertex *createVertexColor(double x, double y, double z, Color c);
	void setNormal(Face* f, int i, float x, float y, float z);
	void freeVertex(Vertex *v);
	void printVertex(Vertex *v);
	double getVertexCoord(Vertex *v, int i);
	Face *createFace(Object *o, int length, ...);
	int addVertexToFace(Face *f, Vertex *v);
	int setVertexToFace(Face* f, int i, Vertex* v);
	void computeNormal(Face *f);
	void freeFace(Face *f);
	int addFace(Object *o, Face *f);
	void freeUselessVertices();
	void freeMaterials();
	Object *createObject();
	~Obj();
};

