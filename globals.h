#pragma once

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define M_PI 3.14159265358979323846
#define TO_RADIAN(x) (x * (M_PI / 180))
#define VEC3_SIZE 3
#define VEC4_SIZE 4

typedef struct node {
	void *data;
	struct node *next;
} node;

typedef struct color {
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
} color;

typedef struct vertex {
	float pos[VEC4_SIZE];
	float normal[VEC4_SIZE];
	color colour;
} vertex;

typedef struct face {
	vertex **vertices;
	int		length;
} face;

typedef struct object {
	face **faces;
	int	length;
	node *vertices_list;
	color color;
} object;

typedef struct light {
	float	pos[VEC4_SIZE];
	float	intensity;
	color		c;
} light;
