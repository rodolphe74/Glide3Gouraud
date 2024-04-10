#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>

#define MAT4SZ 4
#define MAT3SZ 3
#define VEC4SZ 4
#define VEC3SZ 3

typedef float REAL;
typedef REAL MEMORY[16];
enum VTYPE { VEC3, VEC4, MAT3, MAT4 };

size_t lines[];

class Matrix
{
private:
	MEMORY v = { 0 };
	MEMORY x = { 0 };	// used to cache
	MEMORY y = { 0 };	// used to hold transposed

	// msvc ensure array is aligned
	float __declspec(align(16)) mmxA[4];
	float __declspec(align(16)) mmxB[4];
	float __declspec(align(16)) mmxR[4];

	float __declspec(align(32)) avxA[8];
	float __declspec(align(32)) avxB[8];
	float __declspec(align(32)) avxR[8];

	VTYPE t = VTYPE::MAT4;
	int r = MAT4SZ;
	int c = MAT4SZ;
	
	Matrix();

	void retain() {
		memcpy(x, v, sizeof(REAL) * 16);
	}

	void leave() {
		memcpy(v, x, sizeof(REAL) * 16);
	}



public:
	Matrix(VTYPE t)
	{
		this->t = t;
		switch (t)
		{
		case VEC3:
			r = 1;
			c = 3;
			break;
		case VEC4:
			r = 1;
			c = 4;
			break;
		case MAT3:
			r = 3;
			c = 3;
			break;
		case MAT4:
			r = 4;
			c = 4;
			break;
		default:
			break;
		}
	}

	Matrix(std::initializer_list<REAL> lst, VTYPE t)
	{
		this->t = t;
		switch (t)
		{
		case VEC3:
			r = 1;
			c = 3;
			break;
		case VEC4:
			r = 1;
			c = 4;
			break;
		case MAT3:
			r = 3;
			c = 3;
			break;
		case MAT4:
			r = 4;
			c = 4;
			break;
		default:
			break;
		}
		int x = 0, y = 0;
		for (REAL l : lst) {
			*(v + y * 4 + x ) = l;
			x++;
			if (x == c) {
				x = 0;
				y++;
			}
		}
	}

	void setType(VTYPE t) {
		this->t = t;
		switch (t)
		{
		case VEC3:
			r = 1;
			c = 3;
			break;
		case VEC4:
			r = 1;
			c = 4;
			break;
		case MAT3:
			r = 3;
			c = 3;
			break;
		case MAT4:
			r = 4;
			c = 4;
			break;
		default:
			break;
		}
	}

	inline void copy(Matrix &w) {
		memcpy(v, w.v, sizeof(float) * 16);
	}

	void clear() {
		memset(v, 0, sizeof(REAL) * 16);
	}


	void vecSetAt(int x, REAL value)
	{
		*(v + x) = value;
	}

	void matSetAt(int x, int y, REAL value)
	{
		*(v + y * 4 + x) = value;
	}

	REAL vecGetAt(int x)
	{
		return  *(v + x);
	}

	REAL matGetAt(int x, int y) {
		return *(v + y * 4 + x);
	}

	void getColumn(int index, float *result)
	{
		for (int y = 0; y < 4; y++) {
			result[y] = *(v + y * 4 + index);
		}
	}

	void storeTransposed() {
		int i, j;
		for (i = 0; i < r; i++)
			for (j = 0; j < c; j++)
				*(y + i * 4 + j) = *(v + j * 4 + i);
	}

	void add(Matrix &w);
	void matMul(Matrix &w);
	void matMulMmx(Matrix &w);

	friend std::ostream &operator<<(std::ostream &os, const Matrix &m);
};

