#pragma once

class Matrix {
	int* array;
	int m_width;
protected:
	int index(int x, int y) const {
		return x + m_width * y;
	}
public:
	Matrix(int w, int h) : m_width(w), array(new int[w * h]) {} ~Matrix() {
		delete[] array;
	}
	int at(int x, int y) const {
		return array[index(x, y)];
	}
};