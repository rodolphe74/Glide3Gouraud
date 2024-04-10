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

void Matrix::add(Matrix &w)
{
    for (int y = 0; y < r; y++) {
        for (int x = 0; x < c; x++) {
            *(v + y * 4 + x) += *(w.v + y * 4 + x);
        }
    }

}

void Matrix::matMul(Matrix &w)
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


void Matrix::matMulMmx(Matrix &w)
{
    __m128 a, b, s;
    retain();
    // w.storeTransposed(); // must be called before matMulMmx

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

