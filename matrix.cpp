#include "matrix.h"
#include <xmmintrin.h>
#include <intrin.h>

size_t lines[] = {0, 4, 8, 12};

std::ostream &operator<<(std::ostream &os, const Matrix &m)
{
    for (int y = 0; y < m.r; y++) {
        for (int x = 0; x < m.c; x++) {
            os << *(m.v + y * 4 + x) << " ";
        }
        os << std::endl;
    }
    return os;
}

void Matrix::matAddMat(Matrix &w)
{
    for (int y = 0; y < r; y++) {
        for (int x = 0; x < c; x++) {
            *(v + y * 4 + x) += *(w.v + y * 4 + x);
        }
    }

}

void Matrix::matSubMat(Matrix &w)
{
    for (int y = 0; y < r; y++) {
        for (int x = 0; x < c; x++) {
            *(v + y * 4 + x) -= *(w.v + y * 4 + x);
        }
    }
}

void Matrix::matMulMat(Matrix &w)
{
    // !! MANDATORY !! c == w.r
    retain();

    for (int i = 0; i < r; i++) {
        for (int j = 0; j < w.c; j++) {
            *(v + i * 4 + j) = 0;
            for (int k = 0; k < w.r; k++) {
                *(v + i * 4 + j) += *(x + i * 4 + k) * *(w.v + k * 4 + j);
            }
        }
    }
}


void Matrix::matMulMatMmx(Matrix &w)
{
    __m128 a, b, s;
    retain();
    // w.storeTransposed(); // must be called before matMulMatMmx

    for (int i = 0; i < r; i++) {
        memcpy(mmxA, x + i * 4, 16);
        a = _mm_load_ps(mmxA);
        for (int j = 0; j < w.c; j++) {
            memcpy(mmxB, w.y + j * 4, 16);
            b = _mm_load_ps(mmxB);
            s = _mm_mul_ps(a, b);

            // s = { s3, s2, s1, s0 }
            s = _mm_hadd_ps(s, s);
            // s = {s3+s2, s1+s0, s3+s2, s1+s0}
            s = _mm_hadd_ps(s, s);
            // s = {s2+s3+s1+s0, s3+s2+s1+s0, s3+s2+s1+s0, s3+s2+s1+s0}

            _mm_store_ps(mmxR, s);
            *(v + i * 4 + j) = mmxR[0];
        }
    }
}

void Matrix::vecAddVec(Matrix &w)
{
    for (int x = 0; x < c; x++) {
        *(v + x) += *(w.v + x);
    }
}

void Matrix::vecSubVec(Matrix &w)
{
    for (int x = 0; x < c; x++) {
        *(v + x) -= *(w.v + x);
    }
}

void Matrix::vecMulVec(Matrix &w)
{
    for (int x = 0; x < c; x++) {
        *(v + x) *= *(w.v + x);
    }
}

void Matrix::vec4MulMat4(Matrix &w)
{
    retain();
    *(v) = *(w.v) * *(x) + *(w.v + 4) * *(x + 1) + *(w.v + 8) * *(x + 2) + *(w.v + 12) * *(x + 3);
    *(v + 1) = *(w.v + 1) * *(x) + *(w.v + 5) * *(x + 1) + *(w.v + 9) * *(x + 2) + *(w.v + 13) * *(x + 3);
    *(v + 2) = *(w.v + 2) * *(x) + *(w.v + 6) * *(x + 1) + *(w.v + 10) * *(x + 2) + *(w.v + 14) * *(x + 3);
    *(v + 3) = *(w.v + 3) * *(x) + *(w.v + 7) * *(x + 1) + *(w.v + 11) * *(x + 2) + *(w.v + 15) * *(x + 3);
}

void Matrix::vec4MulMat4Mmx(Matrix& w)
{
	retain();
	clear();
	__m128 a, s;
    __m128 b;
    memcpy(mmxB, v, 16);
    b = _mm_load_ps(mmxB);
	for (int i = 0; i < 4; i++) {
		memcpy(mmxA, w.v + i * 4, 16);
		a = _mm_load_ps(mmxA);
		s = _mm_mul_ps(a, _mm_set1_ps(*(x + i)));
        b = _mm_add_ps(b, s);
	}
    _mm_store_ps(v, b);
}

void Matrix::vecMulScalar(REAL s)
{
    *(v) *= s;
    *(v + 1) *= s;
    *(v + 2) *= s;
    *(v + 3) *= s;
}

void Matrix::vec3CrossVec3(Matrix &w)
{
    retain();
    *(v) = *(x + 1) * *(w.v + 2) - *(w.v + 1) * *(x + 2);
    *(v + 1) = -(*(x) * *(w.v + 2) - *(w.v) * *(x + 2));
    *(v + 2) = *(x) * *(w.v + 1) - *(w.v) * *(x + 1);
}

void Matrix::vec3Normalize()
{
	float m = std::sqrt(*(v) * *(v) + *(v + 1) * *(v + 1) + *(v + 2) * *(v + 2));
    *(v) = *(v) / m;
    *(v + 1) = *(v + 1) / m;
    *(v + 2) = *(v + 2) / m;
}

REAL Matrix::vec3DotReal(Matrix &w)
{
    return *(v) * *(w.v) + *(v + 1) * *(w.v + 1) + *(v + 2) * *(w.v + 2);
}

