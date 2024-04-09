#ifndef MATRIX_MAT_HPP
#define MATRIX_MAT_HPP

// https://github.com/pavel-shvetsov

#include <iostream>

// Available in all types of builds
// Do not use with heavy calculations
#define ASSERT(expr, msg) { if (!(expr)) { \
    std::cerr << "ASSERTION FAILED: " << msg \
              << "\nSOURCE: " << __FILE__ << ", line " << __LINE__ << std::endl; exit(1); } }

// Active only in debug builds for heavy processing assertions
// Automatically disabled in release builds
#ifndef NDEBUG
#define DBG_ASSERT(expr, msg) ASSERT(expr, msg)
#define CREATE_INC count_creations++;
#define DELETE_INC count_deletions++;
#else
#define DBG_ASSERT(...)
#define CREATE_INC
#define DELETE_INC
#endif

class vec;


class mat {
public:
	// Zero value ctor
	mat(int row_num, int col_num);
	mat(int row_num);

	// Initializer list ctor
	mat(std::initializer_list<std::initializer_list<float>> lst);
	mat(std::initializer_list<float> lst);

	// Copy ctor
	mat(const mat &that);

	// Move constructor / assignment
	mat(mat &&that) noexcept;
	mat &operator=(mat &&that) noexcept;

	// Dtor
	virtual ~mat() { delete[] data; DELETE_INC }

	int rows() const { return r; }

	int cols() const { return c; }

	// Matrix subscript operator
	float *operator[](int index) const {
		return data + index * c;
	}

	float *getData() {
		return data;
	}

	// Copy-assignment
	mat &operator=(const mat &that);

	friend mat operator*(float scalar, const mat &mat_right);
	friend mat operator*(const mat &mat_left, float scalar);
	friend mat operator*(const mat &lhs, const mat &rhs);
	friend mat operator+(const mat &lhs, const mat &rhs);
	friend mat operator-(const mat &lhs, const mat &rhs);
	friend std::ostream &operator<<(std::ostream &os, const mat &m);

	// rotations
	void rotationX(float angle);
	void rotationY(float angle);
	void rotationZ(float angle);

	// translation
	void translation(vec &v);

	// first dim
	vec asVec();

#ifndef NDEBUG
	static unsigned get_creations() { return count_creations; }
	static unsigned get_deletions() { return count_deletions; }
#endif

protected:
	int r, c;
	float *data;

#ifndef NDEBUG
	static unsigned count_creations;
	static unsigned count_deletions;
#endif
};

// Vector class
class vec : public mat {
	using mat::mat;
public:
	using mat::operator=;
	float &operator[](int index) {
		ASSERT(index < r * c, "Wrong index for [] operator")
			return data[index];
	}
	
	vec cross3(vec other) {
		vec result = { 0,0,0 };

		result[0] = data[1] * other[2] - other[1] * data[2];
		result[1] = -(data[0] * other[2] - other[0] * data[2]);
		result[2] = data[0] * other[1] - other[0] * data[1];

		return result;
	}

	void normalize3() {
		float m = std::sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
		data[0] = data[0] / m;
		data[1] = data[1] / m;
		data[2] = data[2] / m;
	}

	float dot3(vec other) {
		return data[0] * other[0] + data[1] * other[1] + data[2] * other[2];
	}

	void add3(vec other) {
		data[0] = data[0] + other[0];
		data[1] = data[1] + other[1];
		data[2] = data[2] + other[2];
	}

	void mult3(vec other) {
		data[0] = data[0] * other[0];
		data[1] = data[1] * other[1];
		data[2] = data[2] * other[2];
	}

	void mult4(vec other) {
		data[0] = data[0] * other[0];
		data[1] = data[1] * other[1];
		data[2] = data[2] * other[2];
		data[3] = data[3] * other[3];
	}


	void multMat4(mat other) {
		float x = data[0];
		float y = data[1];
		float z = data[2];
		float w = data[3];

		data[0] = other.getData()[0] * x + other.getData()[4] * y + other.getData()[8] * z + other.getData()[12] * w;
		data[1] = other.getData()[1] * x + other.getData()[5] * y + other.getData()[9] * z + other.getData()[13] * w;
		data[2] = other.getData()[2] * x + other.getData()[6] * y + other.getData()[10] * z + other.getData()[14] * w;
		data[3] = other.getData()[3] * x + other.getData()[7] * y + other.getData()[11] * z + other.getData()[15] * w;
	}

};

#endif //MATRIX_MAT_HPP