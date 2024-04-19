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
	float ambient[3];				// Ka	(blender = metallic)
	float specularStrength = 1.0f;
	int shininess;					// Ns   (from 1.0 - 0.0 Principled BSDF range to 0.0 - 900.0 OBJ specular exponent range)	(blender = (1.0 - roughness) * 30)
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



static Color WHITE = { 255, 255, 255 };
static Color GRAY = { 128, 128, 128 };
static Color BLUE = { 0, 0, 255 };
static Color GREEN = { 0, 255, 0 };
static Color YELLOW = { 255, 255, 0 };
static Color CYAN = { 0, 255, 255 };
static Color RED = { 255, 0, 0 };

static Material BRONZE = { { 1.0f, 0.5f, 0.31f }, { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.5f, 0.31f }, 1.0f, 52 };
static Material JADE = { { 0.54f, 0.89f, 0.63f }, { 0.316228f, 0.316228f, 0.316228f}, { 0.135f, 0.2225f, 0.1575f }, 1.0f, 13 };
static Material TURQUOISE = { { 0.396f, 0.74151f, 0.69102f }, { 0.297254f, 0.30829f, 0.306678f }, { 0.1f, 0.18725f, 0.1745f }, 1.0f, 26 };
static Material BLACK_RUBBER = { { 0.01f, 0.01f, 0.01f }, { 0.4f, 0.4f, 0.4f }, { 0.02f, 0.02f, 0.02f }, 1.0f, 20 };
static Material CHROME = { { 0.4f, 0.4f, 0.4f }, { 0.774597f, 0.774597f, 0.774597f }, { 0.25f, 0.25f, 0.25f }, 1.0f, 76 };
static Material CYAN_PLASTIC = { { 0.0f, 0.50980392f, 0.50980392 }, { 0.50196078f, 0.50196078f, 0.50196078  }, { 0.0f, 0.1f, 0.06 }, 1.0f, 32 };


class Obj
{
private:
	void cut(char *src, int start, int end, char *target);
	void split(char *string, char sep, char token_array[][50]);

public:
	//Object o;
	Color color;
	std::vector<Vertex *> vertices;
	std::map<std::string, Material> materials;
	std::map<std::string, Object *> objects;
	Obj();
	Obj(int length, ...);
	Obj(const char *filename);
	void loadObjects(const char *filename);
	void loadMaterials(const char *filename);
	void applyMaterials();
	void applyMaterial(Object *o, Material *m);
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

