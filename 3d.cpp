#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math>
#include <float.h>
#include "3d.h"
#include "llist.h"
#include "mathc.h"


#include <fstream>
#include <string>
#include <iostream>


#define CETTE_CONSTANTE_JUSTE_POUR_AVOIR_UN_Z_BUFFER_POSITIF 32
#define MILLEVINGTQUATRE 1024


// Bronze
//float diffuseLightColor[] = {1.0f, 0.5f, 0.31f}; // white light diffuse
//float specularLightColor[] = { 0.5f, 0.5f, 0.5f };
//float ambient[] = {1.0f, 0.5f, 0.31f};
//float specularStrength = 1.0f;
//int shininess = 52;

// Turquoise
float diffuseLightColor[] = { 0.396f, 0.74151f, 0.69102f };
float specularLightColor[] = { 0.297254f, 0.30829f, 0.306678f };
float ambient[] = { 0.1f, 0.18725f,    0.1745f };
float specularStrength = 1.0f;
int shininess = 26;

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

vec getVertexAsVector(vertex *v)
{
	vec vc(3);
	vc[0] = v->pos[0];
	vc[1] = v->pos[1];
	vc[2] = v->pos[2];
	return vc;
}

vec getVertexNormalsAsVector(vertex *v)
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

vec getObjectColor(object *o)
{
	vec vc(3);
	vc[0] = o->color.r;
	vc[1] = o->color.g;
	vc[2] = o->color.b;
	return vc;
}

vertex *create_vertex(double x, double y, double z)
{
	vertex *v = (vertex *) malloc(sizeof(vertex));

	v->pos[0] = (float) x;
	v->pos[1] = (float) y;
	v->pos[2] = (float) z;
	v->pos[3] = (float) 1;
	v->colour = white;
	return v;
}

vertex *create_vertex_color(double x, double y, double z, color c)
{
	vertex *v = create_vertex(x, y, z);

	v->colour = c;
	// printf("color %d %d %d\n", v->colour.r, v->colour.g, v->colour.b);
	return v;
}

void set_normal(vertex *v, float x, float y, float z)
{
	v->normal[0] = x;
	v->normal[1] = y;
	v->normal[2] = z;
	v->normal[3] = 1.0f;
}


double get_vertex_coord(vertex *v, int i)
{
	return v->pos[i];
}

void print_vertex(vertex *v)
{
	printf(">[%lf %lf %lf %lf]\n", get_vertex_coord(v, 0), get_vertex_coord(v, 1), get_vertex_coord(v, 2), get_vertex_coord(v, 3));
}

void free_vertex(vertex *v)
{
	free(v);
}


face *create_face(int length, ...)
{
	face *f = (face *) malloc(sizeof(face));

	f->length = length;
	f->vertices = (vertex **) malloc(length * sizeof(vertex *));
	va_list valist;

	va_start(valist, length);
	for (int i = 0; i < length; i++) {
		vertex *v = va_arg(valist, vertex *);
		f->vertices[i] = v;
	}
	va_end(valist);
	return f;
}

int add_vertex_to_face(face *f, vertex *v)
{
	f->vertices = (vertex **) realloc(f->vertices, (f->length + 1) * sizeof(vertex *));
	if (f->vertices == NULL)
		return 0;
	f->vertices[f->length] = v;
	f->length++;
	return 1;
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

void free_face(face *f)
{
	for (int i = 0; i < f->length; i++)
		free_vertex(f->vertices[i]);
	free(f->vertices);
	free(f);
}


object *create_object(int length, ...)
{
	object *o = (object *) malloc(sizeof(object));

	o->length = length;
	o->faces = (face **) malloc(length * sizeof(face *));
	o->vertices_list = NULL;
	va_list valist;

	va_start(valist, length);
	for (int i = 0; i < length; i++) {
		face *f = va_arg(valist, face *);
		o->faces[i] = f;
	}
	va_end(valist);
	update_vertices_list(o);

	return o;
}


int add_face_to_object(object *o, face *f)
{
	o->faces = (face **) realloc(o->faces, (o->length + 1) * sizeof(face *));
	if (o->faces == NULL)
		return 0;
	o->faces[o->length] = f;
	o->length++;
	return 1;
}

void update_vertices_list(object *o)
{
	freelist(o->vertices_list);
	o->vertices_list = NULL;
	node *root = o->vertices_list;

	for (int i = 0; i < o->length; i++) {
		face *f = o->faces[i];
		for (int j = 0; j < f->length; j++) {
			vertex *v = f->vertices[j];
			node *nf = find(root, v);
			if (!nf)
				root = addnode(root, v);
		}
	}
	o->vertices_list = root;
}

void translate_object(object *o, vec tv)
{
	for (node *n = o->vertices_list; n != NULL; n = n->next) {
		vertex *vx = (vertex *)n->data;
		vec vcx = getVertexAsVector(vx);
		vcx.mult3(tv);
		vx->pos[0] = vcx[0];
		vx->pos[1] = vcx[1];
		vx->pos[2] = vcx[2];
		vec vcn = getVertexNormalsAsVector(vx);
		vcn.mult3(tv);
		vx->normal[0] = vcn[0];
		vx->normal[1] = vcn[1];
		vx->normal[2] = vcn[2];
	}
}

void transform_object(object *o, mat tm)
{
	for (node *n = o->vertices_list; n != NULL; n = n->next) {
		vertex *vx = (vertex *)n->data;
		vec vcx = getVertexAsVector(vx);
		vcx.multMat4(tm);
		vx->pos[0] = vcx[0];
		vx->pos[1] = vcx[1];
		vx->pos[2] = vcx[2];
		vec vcn = getVertexNormalsAsVector(vx);
		vcn.multMat4(tm);
		vx->normal[0] = vcn[0];
		vx->normal[1] = vcn[1];
		vx->normal[2] = vcn[2];
	}
}

void free_object(object *o)
{
	// MAJ liste des vertices
	// printf("%p\n", (void*) o->vertices_list);
	update_vertices_list(o);

	// Suppression des vertices de la liste créée
	// la liste permet d'éviter les double free
	for (node *n = o->vertices_list; n != NULL; n = n->next)
		free_vertex((vertex *) n->data);

	// Suppression des faces
	for (int i = 0; i < o->length; i++)
		free(o->faces[i]);

	// Suppression de la liste
	freelist(o->vertices_list);

	// Suppression de l'objet
	free(o);
}



void create_sphere(object *o, int sectors, int stacks, float radius)
{
	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, length_inv = 1.0f / radius;   // vertex normal
	//float s, t;                                     // vertex texCoord

	float sector_step = (float) (2 * M_PI / sectors);
	float stack_step = (float) (M_PI / stacks);
	float sector_angle, stack_angle;

	vertex **vertices = (vertex **) malloc((sectors + 1) * (stacks + 1) * sizeof(vertex *));

	printf("malloc %d\n", (sectors + 1) * (stacks + 1));

	int count = 0;

	for (int i = 0; i <= stacks; ++i) {
		stack_angle = (float) M_PI / 2 - i * stack_step;        // starting from pi/2 to -pi/2
		xy = radius * cos/*f*/(stack_angle);                // r * cos(u)
		z = radius * sin/*f*/(stack_angle);                 // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectors; ++j) {
			sector_angle = j * sector_step; // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cos/*f*/(sector_angle);    // r * cos(u) * cos(v)
			y = xy * sin/*f*/(sector_angle);    // r * cos(u) * sin(v)
			vertices[count] = create_vertex(x, y, z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * length_inv;
			ny = y * length_inv;
			nz = z * length_inv;
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
				face *f = create_face(3, vertices[k1], vertices[k2], vertices[k1 + 1]);
				add_face_to_object(o, f);
				compute_normal(f);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1)) {
				face *f = create_face(3, vertices[k1 + 1], vertices[k2], vertices[k2 + 1]);
				add_face_to_object(o, f);
				compute_normal(f);
			}
		}
	}
	update_vertices_list(o);
}


light *create_light(float x, float y, float z, color c, float i)
{
	light *l = (light *) malloc(sizeof(light));

	l->pos[0] = (float) x;
	l->pos[1] = (float) y;
	l->pos[2] = (float) z;
	l->intensity = i;
	l->c = c;
	return l;
}


void create_obj(object *o, char *filename)
{
	FILE *filePointer;
	// int bufferLength = 1024;
	char buffer[MILLEVINGTQUATRE];
	char header[MILLEVINGTQUATRE];
	int vertices_count = 0;
	int normals_count = 0;
	int i = 0;

	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		sscanf(buffer, "%s ", header);
		if (strcmp("v", header) == 0)
			vertices_count++;
		if (strcmp("vn", header) == 0)
			normals_count++;
	}
	fclose(filePointer);

	printf("vertices count :%d\n", vertices_count);
	vertex **vertex_list = (vertex **) malloc(sizeof(vertex *) * vertices_count);

	printf("normals count :%d\n", normals_count);
	float **normals_list = (float **) malloc(sizeof(float *) * normals_count);

	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		sscanf(buffer, "%s ", header);
		if (strcmp("v", header) == 0) {
			float x, y, z;
			sscanf(buffer, "%s %f %f %f", header, &x, &y, &z);
			// printf("v %f %f %f\n", x, y, z);
			vertex *v = create_vertex(x, y, z);
			vertex_list[i] = v;
			i++;
		}
	}
	fclose(filePointer);

	i = 0;
	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		sscanf(buffer, "%s ", header);
		if (strcmp("vn", header) == 0) {
			float x, y, z;
			sscanf(buffer, "%s %f %f %f", header, &x, &y, &z);
			printf("vn %f %f %f\n", x, y, z);
			float *n = (float *) malloc(sizeof(float) * 3);
			n[0] = x;
			n[1] = y;
			n[2] = z;
			normals_list[i] = n;
			i++;
		}
	}
	fclose(filePointer);

	filePointer = fopen(filename, "r");
	int face_indexes[10];
	int normal_indexes[10];
	int k;

	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		sscanf(buffer, "%s ", header);
		if (strcmp("f", header) == 0) {
			i = 0;
			k = 0;
			char *token;
			const char s[2] = " ";
			token = strtok(buffer, s);
			while (token != NULL) {
				if (i > 0) {
					printf("token %d %s\n", i, token);
					int i, j;
					sscanf(token, "%d//%d", &i, &j);
					printf("   %d %d\n", i, j);
					face_indexes[k] = i;
					normal_indexes[k] = j;
					k++;
				}
				token = strtok(NULL, s);
				i++;
			}
			// printf("k %d\n", k);
			face *f = create_face(0);
			for (i = 0; i < k; i++) {
				// printf("face_indexes[%d]=%d\n", i, face_indexes[i]);
				vertex *v = create_vertex(vertex_list[face_indexes[i] - 1]->pos[0],
					vertex_list[face_indexes[i] - 1]->pos[1],
					vertex_list[face_indexes[i] - 1]->pos[2]);
				// print_vertex(v);
				add_vertex_to_face(f, v);
				// printf("normal_indexes[%d]=%d\n", i, normal_indexes[i]);
				set_normal(v, normals_list[normal_indexes[i] - 1][0],
					normals_list[normal_indexes[i] - 1][1],
					normals_list[normal_indexes[i] - 1][2]);
				//  print_vec3(v->normal);
			}
			// printf("len %d\n", f->length);
			add_face_to_object(o, f);
		}
	}
	fclose(filePointer);


	free(vertex_list);
	for (int i = 0; i < normals_count; i++)
		free(normals_list[i]);
	free(normals_list);

	update_vertices_list(o);
}


void cut(char *src, int start, int end, char *target)
{
	int k = 0;

	for (int i = start; i < end; i++) {
		target[k] = src[i];
		k++;
	}
	target[k] = '\0';
}

void split(char *string, char sep, char token_array[][50])
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


void create_object_from_obj_file(object *o, char *filename)
{
	FILE *filePointer;
	// int bufferLength = 1024;
	char buffer[MILLEVINGTQUATRE];
	char header[MILLEVINGTQUATRE];
	int vertices_count = 0;
	int normals_count = 0;
	int uv_count = 0;
	int i = 0;


	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		sscanf(buffer, "%s ", header);
		if (strcmp("v", header) == 0)
			vertices_count++;
		if (strcmp("vn", header) == 0)
			normals_count++;
		if (strcmp("vt", header) == 0)
			uv_count++;
	}
	fclose(filePointer);


	printf("vertices count :%d\n", vertices_count);
	vertex **vertex_list = (vertex **) malloc(sizeof(vertex *) * vertices_count);

	printf("normals count :%d\n", normals_count);
	float **normals_list = (float **) malloc(sizeof(float *) * normals_count);

	printf("uv count :%d\n", uv_count);
	float **uv_list = (float **) malloc(sizeof(float *) * uv_count);

	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		sscanf(buffer, "%s ", header);
		if (strcmp("v", header) == 0) {
			float x, y, z;
			sscanf(buffer, "%s %f %f %f", header, &x, &y, &z);
			vertex *v = create_vertex(x, y, z);
			vertex_list[i] = v;
			i++;
		}
	}
	fclose(filePointer);


	i = 0;
	filePointer = fopen(filename, "r");
	while (fgets(buffer, MILLEVINGTQUATRE, filePointer)) {
		sscanf(buffer, "%s ", header);
		if (strcmp("vn", header) == 0) {
			float x, y, z;
			sscanf(buffer, "%s %f %f %f", header, &x, &y, &z);
			// printf("vn %f %f %f\n", x, y, z);
			float *n = (float *) malloc(sizeof(float) * 3);
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
		sscanf(buffer, "%s ", header);
		if (strcmp("vt", header) == 0) {
			float u, v;
			sscanf(buffer, "%s %f %f", header, &u, &v);
			// printf("vn %f %f %f\n", x, y, z);
			float *t = (float *) malloc(sizeof(float) * 2);
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
		sscanf(buffer, "%s ", header);
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

					sscanf(token_array[0], "%d", &face_index);
					if (strlen(token_array[1]) > 0)
						sscanf(token_array[1], "%d", &uv_index);
					else
						uv_index = 0;
					sscanf(token_array[2], "%d", &normal_index);

					face_indexes[k] = face_index;
					normal_indexes[k] = normal_index;
					uv_indexes[k] = uv_index;

					k++;
				}
				token = strtok(NULL, s);
				i++;
			}


			face *f = create_face(0);
			for (i = 0; i < k; i++) {
				// printf("face_indexes[%d]=%d\n", i, face_indexes[i]);
				vertex *v = create_vertex(vertex_list[face_indexes[i] - 1]->pos[0],
					vertex_list[face_indexes[i] - 1]->pos[1],
					vertex_list[face_indexes[i] - 1]->pos[2]);
				// print_vertex(v);
				add_vertex_to_face(f, v);
				// printf("normal_indexes[%d]=%d\n", i, normal_indexes[i]);
				set_normal(v, normals_list[normal_indexes[i] - 1][0],
					normals_list[normal_indexes[i] - 1][1],
					normals_list[normal_indexes[i] - 1][2]);

				if (uv_indexes[i] - 1 > 0) {
					//                     set_uv(v, 1.0 * uv_list[uv_indexes[i] - 1][0],
					//                         1.0 * uv_list[uv_indexes[i] - 1][1]);
				}
				else {
					//                     set_uv(v, 1.0, 1.0);
				}
				//  print_vec3(v->normal);
			}
			// printf("len %d\n", f->length);
			add_face_to_object(o, f);
		}
	}
	fclose(filePointer);


	free(vertex_list);
	for (int i = 0; i < normals_count; i++)
		free(normals_list[i]);
	free(normals_list);

	update_vertices_list(o);
}


void reflect(vec &out, vec &incident, vec &normal)
{
	float dot = incident.dot3(normal);
	normal = normal * dot;
	normal = normal * 2;
	out = incident - normal;
	out.normalize3();
}

void renderObject(light *l, object *o, mat view, mat perspective, vec from, int w, int h, int onlyVertices)
{
	for (int i = 0; i < o->length; i++) {
		face *f = o->faces[i];
		Fx::Vertex *vertices = new Fx::Vertex[f->length];
		for (int j = 0; j < f->length; j++) {
			vertex *v = f->vertices[j];
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
		grDrawVertexArrayContiguous(GR_POLYGON, f->length, vertices, sizeof(Fx::Vertex));

		delete[] vertices;
	}
}

//void render_vertices(vertex **vertices, int length, float *camera, float *projection, int w, int h)
//{
//}
