#include "obj.h"
#include <stdarg.h>

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
		_face *f = va_arg(valist, _face *);
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

	o.verticesList.reserve(vertices_count);
	float **normals_list = new float *[normals_count];
	float **uv_list = new float *[uv_count];

	//printf("vertices count :%d\n", vertices_count);
	//printf("normals count :%d\n", normals_count);
	//printf("uv count :%d\n", uv_count);

	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		memset(header, 0, sizeof(header));
		int howMany = sscanf(buffer, "%s ", header);
		if (strcmp("v", header) == 0) {
			float x, y, z;
			howMany = sscanf(buffer, "%s %f %f %f", header, &x, &y, &z);
			_vertex *v = createVertex(x, y, z);
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


			_face *f = createFace(0);
			f->vertices.reserve(k);
			for (i = 0; i < k; i++) {
				_vertex *v = o.verticesList[face_indexes[i] - 1];
				addVertexToFace(f, v);
				setNormal(v, normals_list[normal_indexes[i] - 1][0], normals_list[normal_indexes[i] - 1][1], normals_list[normal_indexes[i] - 1][2]);

				// TODO
				if (uv_indexes[i] - 1 > 0) {
					//  set_uv(v, 1.0 * uv_list[uv_indexes[i] - 1][0],
					//  1.0 * uv_list[uv_indexes[i] - 1][1]);
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
#pragma warning(pop) 

_vertex *Obj::createVertex(double x, double y, double z)
{
	_vertex *v = new _vertex;
	v->pos[0] = (float)x;
	v->pos[1] = (float)y;
	v->pos[2] = (float)z;
	v->pos[3] = (float)1;
	v->colour = _white;
	v->referencesCount = 0;
	o.verticesList.push_back(v);
	return v;
}

_vertex *Obj::createVertexColor(double x, double y, double z, _color c)
{
	_vertex *v = createVertex(x, y, z);
	v->colour = c;
	return v;
}

void Obj::setNormal(_vertex *v, float x, float y, float z)
{
	v->normal[0] = x;
	v->normal[1] = y;
	v->normal[2] = z;
	v->normal[3] = 1.0f;
}

void Obj::freeVertex(_vertex *v)
{
	delete v;
}

void Obj::printVertex(_vertex *v)
{
	printf(">[%lf %lf %lf %lf]\n", getVertexCoord(v, 0), getVertexCoord(v, 1), getVertexCoord(v, 2), getVertexCoord(v, 3));
}

double Obj::getVertexCoord(_vertex *v, int i)
{
	return v->pos[i];
}

_face *Obj::createFace(int length, ...)
{
	_face *f = new _face;
	va_list valist;
	va_start(valist, length);
	for (int i = 0; i < length; i++) {
		_vertex *v = va_arg(valist, _vertex *);
		f->vertices.push_back(v);
	}
	va_end(valist);
	return f;
}

int Obj::addVertexToFace(_face *f, _vertex *v)
{
	v->referencesCount++;
	f->vertices.push_back(v);
	return 1;
}

void Obj::computeNormal(_face *f)
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

void Obj::freeFace(_face *f)
{
	for (size_t i = 0; i < f->vertices.size(); i++) {
		_vertex *v = f->vertices[i];
		v->referencesCount = (v->referencesCount > 0 ? --v->referencesCount : 0);
	}
	delete f;
}

int Obj::addFace(_face *f)
{
	o.faces.push_back(f);
	return 1;
}

void Obj::freeUselessVertices()
{
	for (size_t i = 0; i < o.verticesList.size(); i++) {
		if (o.verticesList[i]->referencesCount == 0) {
			freeVertex(o.verticesList[i]);
		}
	}
}

Obj::~Obj()
{
	for (size_t i = 0; i < o.faces.size(); i++) {
		freeFace(o.faces[i]);
	}

	for (size_t i = 0; i < o.verticesList.size(); i++) {
		if (o.verticesList[i]->referencesCount == 0) {
			freeVertex(o.verticesList[i]);
		}
	}
}
