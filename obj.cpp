#include "obj.h"
#include <stdarg.h>

std::map<std::string, Material> Obj::materials;

void Obj::cut(char *src, int start, int end, char *target)
{
	int k = 0;

	for (int i = start; i < end; i++) {
		target[k] = src[i];
		k++;
	}
	target[k] = '\0';
}

void Obj::split(char *string, char sep, char token_array[][50])
{
	// Decoupe en 10 tokens max de 50 cars
	int sep_count = 0;
	int idx1 = 0, idx2 = 0;
	char token[50];
	char *first_car = string;

	while (*string) {
		if (sep == *string) {
			cut(first_car, idx1, idx2, token);
			strcpy(token_array[sep_count], token);
			idx1 = idx2 + 1;
			sep_count++;
		}

		string++;
		idx2++;
	}
	cut(first_car, idx1, idx2, token);
	strcpy(token_array[sep_count], token);
}

Obj::Obj()
{
}

Obj::Obj(int length, ...)
{
	va_list valist;
	va_start(valist, length);
	for (int i = 0; i < length; i++) {
		Face *f = va_arg(valist, Face *);
		o.faces[i] = f;
	}
	va_end(valist);
}

#pragma warning(push)
#pragma warning(disable : 6054)
Obj::Obj(const char *filename)
{
	FILE *filePointer;
	// int bufferLength = 1024;
	char buffer[MILLEVINGTQUATRE];
	char header[MILLEVINGTQUATRE + 1];
	int vertices_count = 0;
	int normals_count = 0;
	int uv_count = 0;
	int i = 0;

	o.color = white;

	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("v", header) == 0)
			vertices_count++;
		if (strcmp("vn", header) == 0)
			normals_count++;
		if (strcmp("vt", header) == 0)
			uv_count++;
	}
	fclose(filePointer);

	o.vertices.reserve(vertices_count);
	float **normals_list = new float *[normals_count];
	float **uv_list = new float *[uv_count];

	//printf("vertices count :%d\n", vertices_count);
	//printf("normals count :%d\n", diffuseCount);
	//printf("uv count :%d\n", specularCount);

	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("v", header) == 0) {
			float x, y, z;
			howMany = sscanf(buffer, "%s %f %f %f", header, &x, &y, &z);
			Vertex *v = createVertex(x, y, z);
			i++;
		}
	}
	fclose(filePointer);

	i = 0;
	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("vn", header) == 0) {
			float x, y, z;
			howMany = sscanf(buffer, "%s %f %f %f", header, &x, &y, &z);
			float *n = new float[3];
			n[0] = x;
			n[1] = y;
			n[2] = z;
			normals_list[i] = n;
			i++;
		}
	}
	fclose(filePointer);

	i = 0;
	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("vt", header) == 0) {
			float u, v;
			howMany = sscanf(buffer, "%s %f %f", header, &u, &v);
			float *t = new float[2];
			t[0] = u;
			t[1] = v;
			uv_list[i] = t;
			i++;
		}
	}
	fclose(filePointer);


	filePointer = fopen(filename, "r");
	int face_indexes[10];
	int normal_indexes[10];
	int uv_indexes[10];
	char token_array[3][50];

	int k;

	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("f", header) == 0) {
			i = 0;
			k = 0;
			char *token;
			const char s[2] = " ";
			token = strtok(buffer, s);
			while (token != NULL) {
				if (i > 0) {
					split(token, '/', token_array);
					int face_index, normal_index, uv_index;

					howMany = sscanf(token_array[0], "%d", &face_index);
					if (strlen(token_array[1]) > 0)
						howMany = sscanf(token_array[1], "%d", &uv_index);
					else
						uv_index = 0;
					howMany = sscanf(token_array[2], "%d", &normal_index);

					face_indexes[k] = face_index;
					normal_indexes[k] = normal_index;
					uv_indexes[k] = uv_index;

					k++;
				}
				token = strtok(NULL, s);
				i++;
			}


			Face *f = createFace(0);
			f->vertices.resize(k);
			f->normals.resize(k);
			for (i = 0; i < k; i++) {
				Vertex *v = o.vertices[face_indexes[i] - 1];
				setVertexToFace(f, i, v);
				
				// create an independ list of normals for the face
				setNormal(f, i, normals_list[normal_indexes[i] - 1][0], normals_list[normal_indexes[i] - 1][1], normals_list[normal_indexes[i] - 1][2]);

				// TODO
				if (uv_indexes[i] - 1 > 0) {
					//  set_uv(v, 1.0 * specularList[uv_indexes[i] - 1][0],
					//  1.0 * specularList[uv_indexes[i] - 1][1]);
				}
				else {
					//  set_uv(v, 1.0, 1.0);
				}
			}
			addFace(f);
		}
	}
	fclose(filePointer);

	for (int i = 0; i < normals_count; i++) {
		delete[] normals_list[i];
	}
	delete[] normals_list;

	for (int i = 0; i < uv_count; i++) {
		delete[] uv_list[i];
	}
	delete[] uv_list;
}

void Obj::loadMaterials(const char *filename)
{
	FILE *filePointer;
	char buffer[MILLEVINGTQUATRE];
	char header[MILLEVINGTQUATRE + 1];
	int materialsCount = 0;
	int i = 0;

	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("newmtl", header) == 0)
			materialsCount++;
	}
	fclose(filePointer);

	o.vertices.reserve(materialsCount);
	char **materialNameList = new char *[materialsCount];
	float **diffuseList = new float *[materialsCount];
	float **specularList = new float *[materialsCount];
	float **ambientList = new float *[materialsCount];
	float **shininessList = new float *[materialsCount];

	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("newmtl", header) == 0) {
			char* materialName = new char[256];
			memset(materialName, 0, 256);
			*(materialNameList + i) = materialName;
			i++;
		}
	}
	fclose(filePointer);

	i = 0;
	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("Kd", header) == 0) {
			float x, y, z;
			howMany = sscanf(buffer, "%s %f %f %f", header, &x, &y, &z);
			float *n = new float[3];
			n[0] = x;
			n[1] = y;
			n[2] = z;
			*(diffuseList + i) = n;
			i++;
		}
	}
	fclose(filePointer);

	i = 0;
	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("Ks", header) == 0) {
			float x, y, z;
			howMany = sscanf(buffer, "%s %f %f %f", header, &x, &y, &z);
			float *n = new float[3];
			n[0] = x;
			n[1] = y;
			n[2] = z;
			*(specularList + i) = n;
			i++;
		}
	}
	fclose(filePointer);

	i = 0;
	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("Ka", header) == 0) {
			float x, y, z;
			howMany = sscanf(buffer, "%s %f %f %f", header, &x, &y, &z);
			float *n = new float[3];
			n[0] = x;
			n[1] = y;
			n[2] = z;
			*(ambientList + i) = n;
			i++;
		}
	}
	fclose(filePointer);

	i = 0;
	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("Ns", header) == 0) {
			float x;
			howMany = sscanf(buffer, "%s %f", header, &x);
			float *n = new float[1];
			n[0] = x;
			*(shininessList + i) = n;
			i++;
		}
	}
	fclose(filePointer);

	// feed materials dictionary
	for (int i = 0; i < materialsCount; i++) {
		if (materialNameList[0]) {
			Material m;
			m.diffuseLightColor[0] = diffuseList[i][0];
			m.diffuseLightColor[1] = diffuseList[i][1];
			m.diffuseLightColor[2] = diffuseList[i][2];

			m.specularLightColor[0] = specularList[i][0];
			m.specularLightColor[1] = specularList[i][1];
			m.specularLightColor[2] = specularList[i][2];

			m.ambient[0] = ambientList[i][0];
			m.ambient[1] = ambientList[i][1];
			m.ambient[2] = ambientList[i][2];

			m.shininess = (int)(*shininessList[i]);

			m.specularStrength = 1.0f;

			std::string materialNameString = std::string(materialNameList[0]);
			materials.insert(std::pair<std::string, Material>(materialNameString, m));
		}
	}

	for (int i = 0; i < materialsCount; i++) {
		delete[] materialNameList[i];
	}
	for (int i = 0; i < materialsCount; i++) {
		delete[] diffuseList[i];
	}
	delete[] diffuseList;

	for (int i = 0; i < materialsCount; i++) {
		delete[] specularList[i];
	}
	for (int i = 0; i < materialsCount; i++) {
		delete[] shininessList[i];
	}
	delete[] specularList;
}
#pragma warning(pop) 

Vertex *Obj::createVertex(double x, double y, double z)
{
	Vertex *v = new Vertex;
	v->pos.x = (float)x;
	v->pos.y = (float)y;
	v->pos.z = (float)z;
	v->pos.w = (float)1;
	v->colour = white;
	v->referencesCount = 0;
	o.vertices.push_back(v);
	return v;
}

Vertex *Obj::createVertexColor(double x, double y, double z, Color c)
{
	Vertex *v = createVertex(x, y, z);
	v->colour = c;
	return v;
}

void Obj::setNormal(Face* f, int i, float x, float y, float z)
{
	f->normals[i].x = x;
	f->normals[i].y = y;
	f->normals[i].z = z;
	f->normals[i].w = 1.0f;
}

void Obj::freeVertex(Vertex *v)
{
	delete v;
}

void Obj::printVertex(Vertex *v)
{
	printf(">[%lf %lf %lf %lf]\n", getVertexCoord(v, 0), getVertexCoord(v, 1), getVertexCoord(v, 2), getVertexCoord(v, 3));
}

double Obj::getVertexCoord(Vertex *v, int i)
{
	switch (i)
	{
	case 0:
		return v->pos.x;
		break;
	case 1:
		return v->pos.y;
		break;
	case 2:
		return v->pos.z;
		break;
	case 3:
		return v->pos.w;
		break;
	default:
		return 0;
		break;
	}
}

Face *Obj::createFace(int length, ...)
{
	Face *f = new Face;
	va_list valist;
	va_start(valist, length);
	for (int i = 0; i < length; i++) {
		Vertex *v = va_arg(valist, Vertex *);
		f->vertices.push_back(v);
	}
	va_end(valist);
	return f;
}

int Obj::addVertexToFace(Face *f, Vertex *v)
{
	v->referencesCount++;
	f->vertices.push_back(v);
	return 1;
}

int Obj::setVertexToFace(Face* f, int i, Vertex* v)
{
	f->vertices[i] = v;
	return 1;
}

void Obj::computeNormal(Face *f)
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

void Obj::freeFace(Face *f)
{
	for (size_t i = 0; i < f->vertices.size(); i++) {
		Vertex *v = f->vertices[i];
		v->referencesCount = (v->referencesCount > 0 ? --v->referencesCount : 0);
	}
	delete f;
}

int Obj::addFace(Face *f)
{
	o.faces.push_back(f);
	return 1;
}

void Obj::freeUselessVertices()
{
	for (size_t i = 0; i < o.vertices.size(); i++) {
		if (o.vertices[i]->referencesCount == 0) {
			freeVertex(o.vertices[i]);
		}
	}
}

void Obj::freeMaterials()
{
}

Obj::~Obj()
{
	for (size_t i = 0; i < o.faces.size(); i++) {
		freeFace(o.faces[i]);
	}

	for (size_t i = 0; i < o.vertices.size(); i++) {
		if (o.vertices[i]->referencesCount == 0) {
			freeVertex(o.vertices[i]);
		}
	}
}
