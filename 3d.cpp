#include "3d.h"
#include <float.h>
#include <fstream>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>


#define MILLEVINGTQUATRE 1024
#define VEC4MULMAT4 vec4MulMat4Mmx

static std::chrono::steady_clock::time_point beginTime;
static std::chrono::steady_clock::time_point endTime;
static double countLap = 0;
static double sumLap = 0;

// default base material for WF objects
float ambient[] = { 0.2f, 0.2f, 0.2f };				// default ambient for exported WF objects
float specularStrength = 1.0f;


void lookAt(Matrix &position, Matrix &target, Matrix &up, Matrix &mat)
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

void perspective(float fov_y, float aspect, float n, float f, Matrix &mat)
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

void rotationX(REAL angle, Matrix &mat)
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

void rotationY(REAL angle, Matrix &mat)
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

void rotationZ(REAL angle, Matrix &mat)
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

void translateObject(Obj &o, Matrix &m)
{
	Matrix vx(VEC3);
	for (size_t i = 0; i < o.vertices.size(); i++) {
		Vertex *vertex = o.vertices[i];
		vx.v[0] = vertex->pos.x;
		vx.v[1] = vertex->pos.y;
		vx.v[2] = vertex->pos.z;
		vx.VEC4MULMAT4(m);
		vertex->pos.x = vx.v[0];
		vertex->pos.y = vx.v[1];
		vertex->pos.z = vx.v[2];
	}

	std::map<std::string, Object *>::iterator it;
	for (it = o.objects.begin(); it != o.objects.end(); it++) {
		for (size_t i = 0; i < it->second->faces.size(); i++) {
			Face *face = it->second->faces[i];
			for (size_t j = 0; j < face->normals.size(); j++) {
				vx.v[0] = face->normals[j].x;
				vx.v[1] = face->normals[j].y;
				vx.v[2] = face->normals[j].z;
				vx.VEC4MULMAT4(m);
				face->normals[j].x = vx.v[0];
				face->normals[j].y = vx.v[1];
				face->normals[j].z = vx.v[2];
			}
		}
	}
}

void transformObject(Obj &o, Matrix &m)
{
	Matrix vx(VEC4);
	for (size_t i = 0; i < o.vertices.size(); i++) {
		Vertex *vertex = o.vertices[i];
		vx.v[0] = vertex->pos.x;
		vx.v[1] = vertex->pos.y;
		vx.v[2] = vertex->pos.z;
		vx.VEC4MULMAT4(m);
		vertex->pos.x = vx.v[0];
		vertex->pos.y = vx.v[1];
		vertex->pos.z = vx.v[2];
	}

	std::map<std::string, Object *>::iterator it;
	for (it = o.objects.begin(); it != o.objects.end(); it++) {
		for (size_t i = 0; i < it->second->faces.size(); i++) {
			Face *face = it->second->faces[i];
			for (size_t j = 0; j < face->normals.size(); j++) {
				vx.v[0] = face->normals[j].x;
				vx.v[1] = face->normals[j].y;
				vx.v[2] = face->normals[j].z;
				vx.VEC4MULMAT4(m);
				face->normals[j].x = vx.v[0];
				face->normals[j].y = vx.v[1];
				face->normals[j].z = vx.v[2];
			}
		}
	}
}

//void compute_normal(face *f)
//{
//	//if (f->length > 1) {
//	//	float v1[3];
//	//	float v2[3];
//	//	float *c = (float *) malloc(sizeof(float) * VEC3_SIZE);
//	//	vec3_subtract(v1, f->vertices[1]->pos, f->vertices[0]->pos);
//	//	vec3_subtract(v2, f->vertices[f->length - 1]->pos, f->vertices[0]->pos);
//
//	//	vec3_cross(c, v1, v2);
//
//	//	for (int i = 0; i < f->length; i++)
//	//		set_normal(f->vertices[i], c[0], c[1], -c[2]);
//	//	free(c);
//	//}
//}

void createSphere(Obj &o, int sectors, int stacks, float radius)
{
	//float x, y, z, xy;                              // vertex position
	//float nx, ny, nz, lengthInv = 1.0f / radius;   // vertex normal
	////float s, t;                                     // vertex texCoord

	//float sectorStep = (float)(2 * M_PI / sectors);
	//float stackStep = (float)(M_PI / stacks);
	//float sectorAngle, stackAngle;

	//Vertex **vertices = (Vertex **)malloc((sectors + 1) * (stacks + 1) * sizeof(vertex *));

	//printf("malloc %d\n", (sectors + 1) * (stacks + 1));

	//int count = 0;

	//for (int i = 0; i <= stacks; ++i) {
	//	stackAngle = (float)M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
	//	xy = radius * cos/*f*/(stackAngle);                // r * cos(u)
	//	z = radius * sin/*f*/(stackAngle);                 // r * sin(u)

	//	// matAddMat (sectorCount+1) vertices per stack
	//	// the first and last vertices have same position and normal, but different tex coords
	//	for (int j = 0; j <= sectors; ++j) {
	//		sectorAngle = j * sectorStep; // starting from 0 to 2pi

	//		// vertex position (x, y, z)
	//		x = xy * cos/*f*/(sectorAngle);    // r * cos(u) * cos(v)
	//		y = xy * sin/*f*/(sectorAngle);    // r * cos(u) * sin(v)
	//		vertices[count] = o.createVertex(x, y, z);

	//		// normalized vertex normal (nx, ny, nz)
	//		nx = x * lengthInv;
	//		ny = y * lengthInv;
	//		nz = z * lengthInv;
	//		// set_normal(vertices[count], nx, ny, nz);

	//		count++;
	//	}
	//}

	//// generate CCW index list of sphere triangles
	//// k1--k1+1
	//// |  / |
	//// | /  |
	//// k2--k2+1
	//int k1, k2;

	//for (int i = 0; i < stacks; ++i) {
	//	k1 = i * (sectors + 1); // beginning of current stack
	//	k2 = k1 + sectors + 1;  // beginning of next stack

	//	for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
	//		if (i != 0) {
	//			Face *f = o.createFace(3, vertices[k1], vertices[k2], vertices[k1 + 1]);
	//			o.addFace(f);
	//			o.computeNormal(f);
	//		}

	//		// k1+1 => k2 => k2+1
	//		if (i != (stacks - 1)) {
	//			Face *f = o.createFace(3, vertices[k1 + 1], vertices[k2], vertices[k2 + 1]);
	//			o.addFace(f);
	//			o.computeNormal(f);
	//		}
	//	}
	//}
}


Light *createLight(float x, float y, float z, Color c, float i)
{
	Light *l = (Light *)malloc(sizeof(Light));
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


void renderObject(Light *l, Obj &o, Matrix &view, Matrix &perspective, Matrix &from, int w, int h, int onlyVertices)
{
	//std::ofstream out("positions.log", std::ios_base::app);
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
	Material *currentMaterial;

	lightColor.v[0] = l->c.r;
	lightColor.v[1] = l->c.g;
	lightColor.v[2] = l->c.b;

	objectColor.v[0] = o.color.r;
	objectColor.v[1] = o.color.g;
	objectColor.v[2] = o.color.b;

	std::map<std::string, Object *>::iterator it;
	for (it = o.objects.begin(); it != o.objects.end(); it++) {

		currentMaterial = &it->second->material;

		for (size_t i = 0; i < it->second->faces.size(); i++) {
			Face *f = it->second->faces[i];
			int sz = f->vertices.size();
			Fx::Vertex *vertices = new Fx::Vertex[sz];

			for (int j = 0; j < sz; j++) {
				Vertex *v = f->vertices[j];
				worldPos.v[0] = v->pos.x;
				worldPos.v[1] = v->pos.y;
				worldPos.v[2] = v->pos.z;
				worldNorm.v[0] = f->normals[j].x;
				worldNorm.v[1] = f->normals[j].y;
				worldNorm.v[2] = f->normals[j].z;

				// Gouraud ////////////
				worldNorm.vec3Normalize();
				lightDir.v[0] = l->pos[0];
				lightDir.v[1] = l->pos[1];
				lightDir.v[2] = l->pos[2];
				lightDir.vecSubVec(worldPos);
				lightDir.vec3Normalize();

				float diff = MAX(worldNorm.vec3DotReal(lightDir), 0);
				diffuseLightColorV.v[0] = currentMaterial->diffuseLightColor[0];
				diffuseLightColorV.v[1] = currentMaterial->diffuseLightColor[1];
				diffuseLightColorV.v[2] = currentMaterial->diffuseLightColor[2];
				diffuseLightColorV.vecMulScalar(diff);
				ambientDiffuseSpecular.v[0] = currentMaterial->ambient[0];
				ambientDiffuseSpecular.v[1] = currentMaterial->ambient[1];
				ambientDiffuseSpecular.v[2] = currentMaterial->ambient[2];
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
				float spec = (float)std::pow((float)MAX(viewDir.vec3DotReal(reflectDir), 0.0f), currentMaterial->shininess /*(float)shininess*/);
				specular.v[0] = currentMaterial->specularLightColor[0];
				specular.v[1] = currentMaterial->specularLightColor[1];
				specular.v[2] = currentMaterial->specularLightColor[2];
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

				cameraPos.VEC4MULMAT4(view);
				cameraPos.VEC4MULMAT4(perspective);
				cameraPos.vecMulScalar(1 / cameraPos.v[3]);

				// Feed 3DFX polygon /////////
				// 1.333 to compensate 3dfx resolution ratio
				vertices[j].x = (FxFloat)MIN(w - 1, (cameraPos.v[0] + 1) * 0.5 * w);
				vertices[j].y = (FxFloat)MIN(h - 1, (cameraPos.v[1] * 1.333 + 1) * 0.5 * h);
				vertices[j].z = (FxFloat)MIN(65535, (cameraPos.v[2] + 1) * 0.5 * 65536);

				int r = (int)MIN(255, MAX(0, c.v[0]));
				int g = (int)MIN(255, MAX(1, c.v[1]));
				int b = (int)MIN(255, MAX(2, c.v[2]));
				vertices[j].argb = (FxU32)0 | (FxU32)r << 16 | (FxU32)g << 8 | (FxU32)b;

				// Log
				//out << i << "," << j << "," << r << "," << g << "," << b << std::endl;
			}

			// 3DFX polygon drawing here /////////
			grCullMode(GR_CULL_NEGATIVE);
			grDrawVertexArrayContiguous(GR_POLYGON, f->vertices.size(), vertices, sizeof(Fx::Vertex));

			delete[] vertices;
		}
	}
}

void initLap()
{
	countLap = 0;
	sumLap = 0;
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
	countLap++;
	sumLap += std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime).count();
}

void meanLap(std::string desc)
{
	std::ofstream out("3d.log", std::ios_base::app);
	out << "Mean time " << desc << " = " << (sumLap / countLap) << " units" << std::endl;
}



//void render_vertices(vertex **vertices, int length, float *camera, float *projection, int w, int h)
//{
//}
