#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "3d.h"
//#include "llist.h"
//#include "mathc.h"


#include <fstream>
#include <string>
#include <iostream>


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

void __lookAt(Matrix &position, Matrix &target, Matrix &up, Matrix &mat)
{
	// https://github.com/felselva/mathc
	Matrix forward({ target.v[0], target.v[1], target.v[2] }, VEC3);
	forward.vecSubVec(position);
	forward.vec3Normalize();

	Matrix side(VEC3);
	side.copy(forward);
	side.vec3CrossVec3(up);
	side.vec3Normalize();

	mat.matSetAt(0, 0, side.v[0]);
	mat.matSetAt(0, 1, side.v[1]);
	mat.matSetAt(0, 2, side.v[2]);
	mat.matSetAt(0, 3, -side.vec3DotReal(position));

	mat.matSetAt(1, 0, up.v[0]);
	mat.matSetAt(1, 1, up.v[1]);
	mat.matSetAt(1, 2, up.v[2]);
	mat.matSetAt(1, 3, -up.vec3DotReal(position));

	mat.matSetAt(2, 0, -forward.v[0]);
	mat.matSetAt(2, 1, -forward.v[1]);
	mat.matSetAt(2, 2, -forward.v[2]);
	mat.matSetAt(2, 3, forward.vec3DotReal(position));

	mat.matSetAt(3, 0, 0.0f);
	mat.matSetAt(3, 1, 0.0f);
	mat.matSetAt(3, 2, 0.0f);
	mat.matSetAt(3, 3, 1.0f);
}

void __perspective(float fov_y, float aspect, float n, float f, Matrix &mat)
{
	// https://github.com/felselva/mathc
	float tan_half_fov_y = (float)(1.0f / std::tan(fov_y * 0.5f));

	mat.matSetAt(0, 0, 1.0f / aspect * tan_half_fov_y);
	mat.matSetAt(0, 1, 0.0f);
	mat.matSetAt(0, 2, 0.0f);
	mat.matSetAt(0, 3, 0.0f);

	mat.matSetAt(1, 0, 0.0f);
	mat.matSetAt(1, 1, 1.0f / tan_half_fov_y);
	mat.matSetAt(1, 2, 0.0f);
	mat.matSetAt(1, 3, 0.0f);

	mat.matSetAt(2, 0, 0.0f);
	mat.matSetAt(2, 1, 0.0f);
	mat.matSetAt(2, 2, f / (n - f));
	mat.matSetAt(2, 3, -(f * n) / (f - n));

	mat.matSetAt(3, 0, 0.0f);
	mat.matSetAt(3, 1, 0.0f);
	mat.matSetAt(3, 2, -1.0f);
	mat.matSetAt(3, 3, 0.0f);
}

void __rotationX(REAL angle, Matrix &mat)
{
	float cs = std::cos(angle);
	float sn = std::sin(angle);
	mat.clear();
	mat.v[0] = 1;
	mat.v[5] = cs;
	mat.v[6] = -sn;
	mat.v[9] = sn;
	mat.v[10] = cs;
	mat.v[15] = 1;
}

void __rotationY(REAL angle, Matrix &mat)
{
	float cs = std::cos(angle);
	float sn = std::sin(angle);
	mat.clear();
	mat.v[0] = cs;
	mat.v[2] = sn;
	mat.v[5] = 1;
	mat.v[8] = -sn;
	mat.v[10] = cs;
	mat.v[15] = 1;
}

void __rotationZ(REAL angle, Matrix &mat)
{
	float cs = std::cos(angle);
	float sn = std::sin(angle);
	mat.clear();
	mat.v[0] = cs;
	mat.v[1] = -sn;
	mat.v[4] = sn;
	mat.v[5] = cs;
	mat.v[10] = 1;
	mat.v[15] = 1;
}

void __translateObject(Obj &o, Matrix &m)
{
	Matrix vx(VEC3);
	Matrix vn(VEC3);
	for (size_t i = 0; i < o.o.verticesList.size(); i++) {
		_vertex *vertex = o.o.verticesList[i];
		vx.v[0] = vertex->pos[0];
		vx.v[1] = vertex->pos[1];
		vx.v[2] = vertex->pos[2];
		vx.vec4MulMat4(m);
		vertex->pos[0] = vx.v[0];
		vertex->pos[1] = vx.v[1];
		vertex->pos[2] = vx.v[2];
		vn.v[0] = vertex->normal[0];
		vn.v[1] = vertex->normal[1];
		vn.v[2] = vertex->normal[2];
		vn.vec4MulMat4(m);
		vertex->normal[0] = vn.v[0];
		vertex->normal[1] = vn.v[1];
		vertex->normal[2] = vn.v[2];
	}
}

void __transformObject(Obj &o, Matrix &m)
{
	Matrix vx(VEC4);
	Matrix vn(VEC4);
	for (size_t i = 0; i < o.o.verticesList.size(); i++) {
		_vertex *vertex = o.o.verticesList[i];
		vx.v[0] = vertex->pos[0];
		vx.v[1] = vertex->pos[1];
		vx.v[2] = vertex->pos[2];
		vx.vec4MulMat4(m);
		vertex->pos[0] = vx.v[0];
		vertex->pos[1] = vx.v[1];
		vertex->pos[2] = vx.v[2];
		vn.v[0] = vertex->normal[0];
		vn.v[1] = vertex->normal[1];
		vn.v[2] = vertex->normal[2];
		vn.vec4MulMat4(m);
		vertex->normal[0] = vn.v[0];
		vertex->normal[1] = vn.v[1];
		vertex->normal[2] = vn.v[2];
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

		// matAddMat (sectorCount+1) vertices per stack
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


void __reflect(Matrix &out, Matrix &incident, Matrix &normal)
{
	float dot = incident.vec3DotReal(normal);
	normal.vecMulScalar(2 * dot);
	out.copy(incident);
	out.vecSubVec(normal);
	out.vec3Normalize();
}


void __renderObject(light *l, Obj &o, Matrix &view, Matrix &perspective, Matrix &from, int w, int h, int onlyVertices)
{
	//startLap();
	Matrix worldPos(VEC3);
	Matrix worldNorm(VEC3);
	Matrix lightDir(VEC3);
	Matrix viewDir(VEC3);
	Matrix negLightDir(VEC3);
	Matrix reflectDir(VEC3);
	Matrix specular(VEC3);
	Matrix lightColor(VEC3);
	Matrix objectColor(VEC3);
	Matrix c(VEC3);
	Matrix diffuseLightColorV(VEC3);
	Matrix ambientDiffuseSpecular(VEC3);
	Matrix cameraPos(VEC4);

	lightColor.v[0] = l->c.r;
	lightColor.v[1] = l->c.g;
	lightColor.v[2] = l->c.b;

	objectColor.v[0] = o.o.color.r;
	objectColor.v[1] = o.o.color.g;
	objectColor.v[2] = o.o.color.b;

	for (size_t i = 0; i < o.o.faces.size(); i++) {
		_face *f = o.o.faces[i];
		int sz = f->vertices.size();
		Fx::Vertex *vertices = new Fx::Vertex[sz];

		for (int j = 0; j < sz; j++) {
			_vertex *v = f->vertices[j];
			worldPos.v[0] = v->pos[0];
			worldPos.v[1] = v->pos[1];
			worldPos.v[2] = v->pos[2];
			worldNorm.v[0] = v->normal[0];
			worldNorm.v[1] = v->normal[1];
			worldNorm.v[2] = v->normal[2];

			// Gouraud ////////////
			worldNorm.vec3Normalize();
			lightDir.v[0] = l->pos[0];
			lightDir.v[1] = l->pos[1];
			lightDir.v[2] = l->pos[2];
			lightDir.vecSubVec(worldPos);
			lightDir.vec3Normalize();

			float diff = MAX(worldNorm.vec3DotReal(lightDir), 0);
			diffuseLightColorV.v[0] = diffuseLightColor[0];
			diffuseLightColorV.v[1] = diffuseLightColor[1];
			diffuseLightColorV.v[2] = diffuseLightColor[2];
			diffuseLightColorV.vecMulScalar(diff);
			ambientDiffuseSpecular.v[0] = ambient[0];
			ambientDiffuseSpecular.v[1] = ambient[1];
			ambientDiffuseSpecular.v[2] = ambient[2];
			ambientDiffuseSpecular.vecAddVec(diffuseLightColorV);

			// Specular ///////////
			viewDir.v[0] = from.v[0];
			viewDir.v[1] = from.v[1];
			viewDir.v[2] = from.v[2];
			viewDir.vecSubVec(worldPos);
			viewDir.vec3Normalize();
			negLightDir.v[0] = -lightDir.v[0];
			negLightDir.v[1] = -lightDir.v[1];
			negLightDir.v[2] = -lightDir.v[2];
			reflectDir.clear();
			__reflect(reflectDir, negLightDir, worldNorm);
			float spec = (float)std::pow((float)MAX(viewDir.vec3DotReal(reflectDir), 0.0f), (float)shininess);
			specular.v[0] = specularLightColor[0];
			specular.v[1] = specularLightColor[1];
			specular.v[2] = specularLightColor[2];
			specular.vecMulScalar(spec * specularStrength);
			specular.vecMulVec(lightColor);

			// Melt lights with object color ///////////
			ambientDiffuseSpecular.vecAddVec(specular);
			ambientDiffuseSpecular.vecMulVec(objectColor);
			c.copy(ambientDiffuseSpecular);

			// Projection /////////
			cameraPos.v[0] = worldPos.v[0];
			cameraPos.v[1] = worldPos.v[1];
			cameraPos.v[2] = worldPos.v[2];
			cameraPos.v[3] = 1.0f;

			cameraPos.vec4MulMat4(view);
			cameraPos.vec4MulMat4(perspective);
			cameraPos.vecMulScalar(1 / cameraPos.v[3]);

			// Feed 3DFX polygon /////////
			vertices[j].x = (FxFloat)MIN(w - 1, (cameraPos.v[0] + 1) * 0.5 * w);
			vertices[j].y = (FxFloat)MIN(h - 1, (cameraPos.v[1] + 1) * 0.5 * h);
			vertices[j].z = (FxFloat)MIN(65535, (cameraPos.v[2] + 1) * 0.5 * 65536);

			int r = (int)MIN(255, MAX(0, c.v[0]));
			int g = (int)MIN(255, MAX(1, c.v[1]));
			int b = (int)MIN(255, MAX(2, c.v[2]));
			vertices[j].argb = (FxU32)0 | (FxU32)r << 16 | (FxU32)g << 8 | (FxU32)b;
		}

		// 3DFX polygon drawing here /////////
		grCullMode(GR_CULL_NEGATIVE);
		grDrawVertexArrayContiguous(GR_POLYGON, f->vertices.size(), vertices, sizeof(Fx::Vertex));

		delete[] vertices;
	}
	//endLap("render");
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
