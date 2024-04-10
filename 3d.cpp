#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "3d.h"
#include "llist.h"
#include "mathc.h"


#include <fstream>
#include <string>
#include <iostream>


#define CETTE_CONSTANTE_JUSTE_POUR_AVOIR_UN_Z_BUFFER_POSITIF 32
#define MILLEVINGTQUATRE 1024

static std::chrono::steady_clock::time_point beginTime;
static std::chrono::steady_clock::time_point endTime;


// Bronze
float diffuseLightColor[] = { 1.0f, 0.5f, 0.31f }; // white light diffuse
float specularLightColor[] = { 0.5f, 0.5f, 0.5f };
float ambient[] = { 1.0f, 0.5f, 0.31f };
float specularStrength = 1.0f;
int shininess = 52;

// Turquoise
//float diffuseLightColor[] = { 0.396f, 0.74151f, 0.69102f };
//float specularLightColor[] = { 0.297254f, 0.30829f, 0.306678f };
//float ambient[] = { 0.1f, 0.18725f,    0.1745f };
//float specularStrength = 1.0f;
//int shininess = 26;

// Black rubber
//float diffuseLightColor[] = { 0.01f, 0.01f, 0.01f };
//float specularLightColor[] = { 0.4f, 0.4f, 0.4f };
//float ambient[] = { 0.02f, 0.02f, 0.02f };
//float specularStrength = 1.0f;
//int shininess = 20;

// Chrome
//float diffuseLightColor[] = { 0.4f, 0.4f, 0.4f };
//float specularLightColor[] = { 0.774597f, 0.774597f, 0.774597f };
//float ambient[] = { 0.25f, 0.25f, 0.25f };
//float specularStrength = 1.0f;
//int shininess = 150;

void _lookAt(vec &position, vec &target, vec &up, mat &mat)
{
	// https://github.com/felselva/mathc
	vec forward{ 0, 0, 0 };
	forward = target - position;
	forward.normalize3();

	vec side = forward.cross3(up);
	side.normalize3();

	mat = {
		{side[0], up[0], -forward[0], 0.0f},
		{side[1], up[1], -forward[1], 0.0f},
		{side[2], up[2], -forward[2], 0.0f},
		{-side.dot3(position), -up.dot3(position), forward.dot3(position), 1.0f}
	};

}

void _perspective(float fov_y, float aspect, float n, float f, mat &mat)
{
	// https://github.com/felselva/mathc
	float tan_half_fov_y = (float)(1.0f / std::tan(fov_y * 0.5f));

	mat = {
		{1.0f / aspect * tan_half_fov_y, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f / tan_half_fov_y, 0.0f, 0.0f},
		{0.0f, 0.0f, f / (n - f), -1.0f},
		{0.0f, 0.0f, -(f * n) / (f - n), 0.0f}
	};
}

vec getVertexAsVector(_vertex *v)
{
	vec vc(3);
	vc[0] = v->pos[0];
	vc[1] = v->pos[1];
	vc[2] = v->pos[2];
	return vc;
}

vec getVertexNormalsAsVector(_vertex *v)
{
	vec vc(3);
	vc[0] = v->normal[0];
	vc[1] = v->normal[1];
	vc[2] = v->normal[2];
	return vc;
}

vec getLightPos(light *l)
{
	vec vc(3);
	vc[0] = l->pos[0];
	vc[1] = l->pos[1];
	vc[2] = l->pos[2];
	return vc;
}

vec getLightColor(light *l)
{
	vec vc(3);
	vc[0] = l->c.r;
	vc[1] = l->c.g;
	vc[2] = l->c.b;
	return vc;
}

vec getObjectColor(Obj &o)
{
	vec vc(3);
	vc[0] = o.o.color.r;
	vc[1] = o.o.color.g;
	vc[2] = o.o.color.b;
	return vc;
}

vec getObjectColor(object *o)
{
	vec vc(3);
	vc[0] = o->color.r;
	vc[1] = o->color.g;
	vc[2] = o->color.b;
	return vc;
}

void translateObject(Obj &o, vec &v)
{
	for (size_t i = 0; i < o.o.verticesList.size(); i++) {
		_vertex *vx = o.o.verticesList[i];
		vec vcx = getVertexAsVector(vx);
		vcx.mult3(v);
		vx->pos[0] = vcx[0];
		vx->pos[1] = vcx[1];
		vx->pos[2] = vcx[2];
		vec vcn = getVertexNormalsAsVector(vx);
		vcn.mult3(v);
		vx->normal[0] = vcn[0];
		vx->normal[1] = vcn[1];
		vx->normal[2] = vcn[2];
	}
}

void transformObject(Obj &o, mat &m)
{
	for (size_t i = 0; i < o.o.verticesList.size(); i++) {
		_vertex *vx = o.o.verticesList[i];
		vec vcx = getVertexAsVector(vx);
		vcx.multMat4(m);
		vx->pos[0] = vcx[0];
		vx->pos[1] = vcx[1];
		vx->pos[2] = vcx[2];
		vec vcn = getVertexNormalsAsVector(vx);
		vcn.multMat4(m);
		vx->normal[0] = vcn[0];
		vx->normal[1] = vcn[1];
		vx->normal[2] = vcn[2];
	}
}

void compute_normal(face *f)
{
	//if (f->length > 1) {
	//	float v1[3];
	//	float v2[3];
	//	float *c = (float *) malloc(sizeof(float) * VEC3_SIZE);
	//	vec3_subtract(v1, f->vertices[1]->pos, f->vertices[0]->pos);
	//	vec3_subtract(v2, f->vertices[f->length - 1]->pos, f->vertices[0]->pos);

	//	vec3_cross(c, v1, v2);

	//	for (int i = 0; i < f->length; i++)
	//		set_normal(f->vertices[i], c[0], c[1], -c[2]);
	//	free(c);
	//}
}

void createSphere(Obj &o, int sectors, int stacks, float radius)
{
	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;   // vertex normal
	//float s, t;                                     // vertex texCoord

	float sectorStep = (float)(2 * M_PI / sectors);
	float stackStep = (float)(M_PI / stacks);
	float sectorAngle, stackAngle;

	_vertex **vertices = (_vertex **)malloc((sectors + 1) * (stacks + 1) * sizeof(vertex *));

	printf("malloc %d\n", (sectors + 1) * (stacks + 1));

	int count = 0;

	for (int i = 0; i <= stacks; ++i) {
		stackAngle = (float)M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cos/*f*/(stackAngle);                // r * cos(u)
		z = radius * sin/*f*/(stackAngle);                 // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectors; ++j) {
			sectorAngle = j * sectorStep; // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cos/*f*/(sectorAngle);    // r * cos(u) * cos(v)
			y = xy * sin/*f*/(sectorAngle);    // r * cos(u) * sin(v)
			vertices[count] = o.createVertex(x, y, z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			// set_normal(vertices[count], nx, ny, nz);

			count++;
		}
	}

	// generate CCW index list of sphere triangles
	// k1--k1+1
	// |  / |
	// | /  |
	// k2--k2+1
	int k1, k2;

	for (int i = 0; i < stacks; ++i) {
		k1 = i * (sectors + 1); // beginning of current stack
		k2 = k1 + sectors + 1;  // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
			if (i != 0) {
				_face *f = o.createFace(3, vertices[k1], vertices[k2], vertices[k1 + 1]);
				o.addFace(f);
				o.computeNormal(f);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1)) {
				_face *f = o.createFace(3, vertices[k1 + 1], vertices[k2], vertices[k2 + 1]);
				o.addFace(f);
				o.computeNormal(f);
			}
		}
	}
}


light *create_light(float x, float y, float z, color c, float i)
{
	light *l = (light *)malloc(sizeof(light));
	if (l) {
		l->pos[0] = (float)x;
		l->pos[1] = (float)y;
		l->pos[2] = (float)z;
		l->intensity = i;
		l->c = c;
	}
	return l;

}

void reflect(vec &out, vec &incident, vec &normal)
{
	float dot = incident.dot3(normal);
	normal = normal * dot;
	normal = normal * 2;
	out = incident - normal;
	out.normalize3();
}

void renderObject(light *l, Obj &o, mat view, mat perspective, vec from, int w, int h, int onlyVertices)
{
	startLap();
	for (size_t i = 0; i < o.o.faces.size(); i++) {
		_face *f = o.o.faces[i];
		int sz = f->vertices.size();
		Fx::Vertex *vertices = new Fx::Vertex[sz];
		
		for (int j = 0; j < sz; j++) {
			_vertex *v = f->vertices[j];
			vec worldPos = getVertexAsVector(v);
			vec worldNorm = getVertexNormalsAsVector(v);

			// Gouraud ////////////
			worldNorm.normalize3();
			vec lightPos = getLightPos(l);
			vec lightDir = (lightPos - worldPos).asVec();
			lightDir.normalize3();
			float diff = MAX(worldNorm.dot3(lightDir), 0);
			vec *_diffuseLightColor = new vec({ diffuseLightColor[0], diffuseLightColor[1], diffuseLightColor[2] });
			mat diffuse = *_diffuseLightColor * diff;
			vec *_ambient = new vec({ ambient[0], ambient[1], ambient[2] });
			vec ambientDiffuseSpecularVec = (*_ambient + diffuse).asVec();

			// Specular ///////////
			vec viewDir = from;
			viewDir = viewDir - worldPos;
			viewDir.normalize3();
			vec *_negLightDir = new vec({ -lightDir[0], -lightDir[1], -lightDir[2] });
			vec *_reflectDir = new vec(3);
			reflect(*_reflectDir, *_negLightDir, worldNorm);
			float spec = (float)std::pow((float)MAX(viewDir.dot3(*_reflectDir), 0.0f), (float)shininess);
			vec *_specular = new vec({ specularLightColor[0], specularLightColor[1], specularLightColor[2] });
			*_specular = *_specular * spec;
			*_specular = *_specular * specularStrength;
			vec lightColorVec = getLightColor(l);
			_specular->mult3(lightColorVec);

			// Melt lights with object color ///////////
			vec objectColorVec = getObjectColor(o);
			vec c(3);
			ambientDiffuseSpecularVec.add3(*_specular);
			ambientDiffuseSpecularVec.mult3(objectColorVec);
			c = ambientDiffuseSpecularVec;

			// Projection /////////
			vec cameraPosVec = { worldPos[0], worldPos[1], worldPos[2], 1 };
			cameraPosVec.multMat4(view);
			cameraPosVec.multMat4(perspective);
			cameraPosVec = cameraPosVec * (1 / cameraPosVec[3]);

			// Feed 3DFX polygon /////////
			vertices[j].x = (FxFloat)MIN(w - 1, (cameraPosVec[0] + 1) * 0.5 * w);
			vertices[j].y = (FxFloat)MIN(h - 1, (cameraPosVec[1] + 1) * 0.5 * h);
			vertices[j].z = (FxFloat)(cameraPosVec[2] + CETTE_CONSTANTE_JUSTE_POUR_AVOIR_UN_Z_BUFFER_POSITIF);

			int r = (int)MIN(255, MAX(0, c[0]));
			int g = (int)MIN(255, MAX(1, c[1]));
			int b = (int)MIN(255, MAX(2, c[2]));
			vertices[j].argb = (FxU32)0 | (FxU32)r << 16 | (FxU32)g << 8 | (FxU32)b;

			delete _diffuseLightColor;
			delete _ambient;
			delete _negLightDir;
			delete _reflectDir;
			delete _specular;
		}
		

		// 3DFX polygon drawing here /////////
		grCullMode(GR_CULL_NEGATIVE);
		grDrawVertexArrayContiguous(GR_POLYGON, f->vertices.size(), vertices, sizeof(Fx::Vertex));

		delete[] vertices;
	}
	endLap("render");
}

void startLap()
{
	beginTime = std::chrono::steady_clock::now();
}


void endLap(std::string desc)
{
	endTime = std::chrono::steady_clock::now();
	std::ofstream out("3d.log", std::ios_base::app);
	out << "Time " << desc << " = " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime).count() << " units" << std::endl;
}



//void render_vertices(vertex **vertices, int length, float *camera, float *projection, int w, int h)
//{
//}
