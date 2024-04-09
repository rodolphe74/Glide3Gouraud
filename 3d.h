#ifndef threed_h
#define threed_h

#include <glide.h>
//#include "mathc.h"
#include "globals.h"
#include "llist.h"
#include "mat.h"


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define M_PI 3.14159265358979323846
#define TO_RADIAN(x) (x * (M_PI / 180))
#define VEC3_SIZE 3
#define VEC4_SIZE 4

//typedef struct color {
//	unsigned char	r;
//	unsigned char	g;
//	unsigned char	b;
//} color;

static color white = { 255, 255, 255 };
static color gray = { 128, 128, 128 };
static color blue = { 0, 0, 255 };
static color green = { 0, 255, 0 };
static color yellow = { 255, 255, 0 };
static color cyan = { 0, 255, 255 };
static color red = { 255, 0, 0 };

namespace Fx {
	typedef struct {
		FxFloat x, y;	// (x,y) coordinates
		FxFloat z;
		FxU32 argb;		// color of each vertex
	} Vertex;
}


//typedef struct vertex {
//	float pos[VEC4_SIZE];
//	float normal[VEC4_SIZE];
//	color colour;
//} vertex;

//typedef struct face {
//	vertex **vertices;
//	int		length;
//} face;

//typedef struct object {
//	face **faces;
//	int	length;
//	node *vertices_list;
//	color color;
//} object;

//typedef struct light {
//	float	pos[VEC4_SIZE];
//	float	intensity;
//	color		c;
//} light;


void _lookAt(vec &position, vec &target, vec &up, mat &mat);
void _perspective(float fov_y, float aspect, float n, float f, mat &mat);
vec getVertexAsVector(vertex *v);
vec getVertexNormalsAsVector(vertex *v);
vec getLightPos(light *l);
vec getLightColor(light *l);
vec getObjectColor(object *o);

vertex *create_vertex(double x, double y, double z);
vertex *create_vertex_color(double x, double y, double z, color c);
void set_normal(vertex *v, float x, float y, float z);
void free_vertex(vertex *v);
void print_vertex(vertex *v);
double get_vertex_coord(vertex *v, int i);

face *create_face(int length, ...);
int add_vertex_to_face(face *f, vertex *v);
void compute_normal(face *f);
void free_face(face *f);

object *create_object(int length, ...);
int add_face_to_object(object *o, face *f);
void update_vertices_list(object *o);
void translate_object(object *o, vec tv);
void transform_object(object *o, mat tm);
void free_object(object *object);

light *create_light(float x, float y, float z, color c, float i);

//void print_mat4(float *m);
//void print_vec4(float *m);
//void print_vec3(float *m);

void create_sphere(object *o, int sectors, int stack, float radius);
void create_object_from_obj_file(object *o, char *filename);

void renderObject(light *l, object *o, mat view, mat perspective, vec from, int w, int h, int onlyVertices);
//void render_vertices(vertex **vertices, int length, float *camera, float *projection, int w, int h);

#endif
