#ifndef LINEAR_H_
#define LINEAR_H_

#include <algorithm>
#include <cmath>
#include <initializer_list>

template <typename T, unsigned int rows, unsigned int columns>
class Matrix
{
	// Concede all'operatore * (moltiplicazione tra matrici) l'accesso all'array interno.
	template <typename U, unsigned int rs, unsigned int mid, unsigned int cs>
	friend Matrix<U, rs, cs> operator*(const Matrix<U, rs, mid>& mat1, const Matrix<U, mid, cs>& mat2);
	template <typename U, unsigned int rs, unsigned int cs> friend class Matrix;

public:
	static const unsigned int count = rows * columns;

	Matrix() {}
	Matrix(const T& value) {
		fill(value);
	}

	// Inizializza la matrice con una lista di valori, riga per riga. La lista viene eventualmente
	// ripetuta se non ci sono abbastanza elementi.
	Matrix(const std::initializer_list<T>& init) {
		unsigned int i = 0;

		while (i < count) {
			for (auto it = init.begin(); it != init.end() && i < count; ++it, ++i) {
				elements[i] = *it;
			}
		}
	}

    // Simile al precedente ma la dimensione dev'essere quella esatta.
    Matrix(const T (&elements)[rows * columns]) {
        std::copy(std::begin(elements), std::end(elements), std::begin(this->elements));
    }

	// Accesso riga, colonna.
	const T& operator() (unsigned int row, unsigned int column) const {
		return elements[row * columns + column];
	}
	
	// Accesso con indice.
	const T& operator[] (unsigned int idx) const {
		return elements[idx];
	}

	Matrix<T, rows, columns>& operator= (const Matrix<T, rows, columns>& other) {
		for (unsigned int i = 0; i < count; i++) {
			elements[i] = other.elements[i];
		}
		return *this;
	}

	// Cambio di segno.
	Matrix<T, rows, columns> operator-() const {
		Matrix<T, rows, columns> result;

		for (unsigned int i = 0; i < count; i++) {
			result.elements[i] = -elements[i];
		}

		return result;
	}

	// Addizione elemento per elemento.
	Matrix<T, rows, columns> operator+(const Matrix<T, rows, columns>& other) const {
		Matrix<T, rows, columns> result;

		for (unsigned int i = 0; i < count; i++) {
			result.elements[i] = elements[i] + other.elements[i];
		}

		return result;
	}


	// Sottrazione elemento per elemento.
	Matrix<T, rows, columns> operator-(const Matrix<T, rows, columns>& other) const {
		return *this + (- other);
	}

	// Moltiplicazione per uno scalare (da destra).
	Matrix<T, rows, columns> operator*(const T& factor) const {
		Matrix<T, rows, columns> result;

		for (unsigned int i = 0; i < count; i++) {
			result.elements[i] = elements[i] * factor;
		}

		return result;
	}

	// Crea una matrice d'identità.
	static Matrix<T, rows, rows> identity() {
		Matrix<T, rows, rows> result(T(0));

		for (unsigned int i = 0; i < rows; i++) {
			result.elements[i * rows + i] = T(1);
		}

		return result;
	}

	// Crea una matrice di traslazione a partire da un vettore colonna.
	static Matrix<T, rows, rows> translation(const Matrix<T, rows - 1, 1>& offset) {
		Matrix<T, rows, rows> result = identity();

		for (unsigned int i = 0; i < rows - 1; i++) {
			result.elements[i * rows + (rows - 1)] = offset[i];
		}

		return result;
	}


	// Crea una matrice di riscalatura a partire da un vettore colonna.
	static Matrix<T, rows, rows> scale(const Matrix<T, rows - 1, 1>& factor) {
		Matrix<T, rows, rows> result = identity();

		for (unsigned int i = 0; i < rows - 1; i++) {
			result.elements[i * rows + i] = factor[i];
		}

		return result;
	}

	// Crea una matrice di riscalatura a partire da un singolo fattore.
	static Matrix<T, rows, rows> scale(const T& factor) {
		Matrix<T, rows, rows> result = identity();

		for (unsigned int i = 0; i < rows - 1; i++) {
			result.elements[i * rows + i] = factor;
		}

		return result;
	}

	// Crea una matrice trasposta.
	Matrix<T, columns, rows> transpose() const {
		Matrix<T,columns, rows> result;

		for (unsigned int row = 0; row < rows; row++) {
			for (unsigned int column = 0; column < columns; column++) {
				result.elements[column * rows + row] = elements[row * columns + column];
			}
		}

		return result;
	}


	// Dato un vettore colonna, restituisce un vettore di lunghezza unitaria nella stessa
	// direzione.
	Matrix<T, rows, 1> normalized() const {
		Matrix<T, rows, 1> result;
		T length = T(0);

		for (unsigned int row = 0; row < rows; row++) {
			length += elements[row] * elements[row];
		}

		length = std::sqrt(length);

		for (unsigned int row = 0; row < rows; row++) {
			result.elements[row] = elements[row] / length;
		}

		return result;
	}

	// Restituisce un puntatore costante ai dati della matrice per poterla passare a OpenGL.
	const T* data_ptr() const {
		return (const T*) elements;
	}

    size_t size() const {
        return sizeof(elements) / sizeof(elements[0]);
    }

private:
	// Riempie la matrice con lo stesso valore.
	void fill(const T& value) {
		for (unsigned int i = 0; i < rows * columns; i++) {
			elements[i] = value;
		}
	}

	// L'array con gli elementi.
	T elements[rows * columns];
};

// Vettore colonna.
template <typename T, unsigned int N> using Vector = Matrix<T, N, 1>;

// Moltiplicazione righe per colonne.
template <typename T, unsigned int rows, unsigned int mid, unsigned int columns>
Matrix<T, rows, columns> operator*(const Matrix<T, rows, mid>& mat1, const Matrix<T, mid, columns>& mat2)
{
	Matrix<T, rows, columns> result(T(0));

	for (unsigned int row = 0; row < rows; row++) {
		for (unsigned int column = 0; column < columns; column++) {
			for (unsigned int i = 0; i < mid; i++) {
				result.elements[row * columns + column] += mat1(row, i) * mat2(i, column);
			}
		}
	}

	return result;
}

// Moltiplicazione per uno scalare (da sinistra).
template <typename T, unsigned int rows, unsigned int columns>
Matrix<T, rows, columns> operator*(const T& scale, const Matrix<T, rows, columns>& mat)
{
	return mat * scale;
}

// Prodotto vettoriale in 3 dimensioni.
template <typename T> Vector<T, 3> cross3(const Vector<T, 3>& vec1, const Vector<T, 3>& vec2)
{
	Vector<T, 3> result = {
			vec1[1] * vec2[2] - vec1[2] * vec2[1],
			vec1[2] * vec2[0] - vec1[0] * vec2[2],
			vec1[0] * vec2[1] - vec1[1] * vec2[0]
	};

	return result;
}

// Matrice di trasformazione che mostra un oggetto visto da un punto di osservazione
// (3 dimensioni, coordinate omogenee).
template <typename T> Matrix<T, 4, 4> look_at4(
		const Vector<T, 3>& eye, const Vector<T, 3>& target, const Vector<T, 3>& up)
{
	Vector<T, 3> zv = (eye - target).normalized();
	Vector<T, 3> xv = cross3(up, zv).normalized();
    Vector<T, 3> yv = cross3(zv, xv);

	Matrix<T, 4, 4> result = {
            xv[0], xv[1], xv[2], (-(xv.transpose() * eye))[0],
            yv[0], yv[1], yv[2], (-(yv.transpose() * eye))[0],
            zv[0], zv[1], zv[2], (-(zv.transpose() * eye))[0],
			0, 0, 0, 1
	};

	return result;
}

// Una variante più semplice
template <typename T> Matrix<T, 4, 4> camera4(const Vector<T, 3>& eye, float pitch, float yaw)
{
    float cosPitch = cos(pitch),
          sinPitch = sin(pitch),
          cosYaw = cos(yaw),
          sinYaw = sin(yaw);

    Vector<T, 3> xv({cosYaw, 0, -sinYaw}),
                 yv({sinYaw * sinPitch, cosPitch, cosYaw * sinPitch}),
                 zv({sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw});

    Matrix<T, 4, 4> result = {
            xv[0], xv[1], xv[2], (-(xv.transpose() * eye))[0],
            yv[0], yv[1], yv[2], (-(yv.transpose() * eye))[0],
            zv[0], zv[1], zv[2], (-(zv.transpose() * eye))[0],
            0, 0, 0, 1
    };

    return result;
}

// Matrice per la proiezione prospettica in 3 dimensioni (coordinate omogenee).
template <typename T> Matrix<T, 4, 4> perspective4(
		const T& near, const T& far, const T& fov, const T& aspect_ratio)
{
	T s = 1 / std::tan(fov * T(M_PI) / 360);

	Matrix<T, 4, 4> result = {
			s / aspect_ratio, 0, 0, 0,
			0, s, 0, 0,
			0, 0, (far + near) / (near - far), (2 * far * near) / (near - far),
            0, 0, -1, 0
	};

	return result;
}

// Matrice per la proiezione ortogonale in 3 dimensioni (coordinate omogenee).
template <typename T> Matrix<T, 4, 4> ortho4(
		const T& near, const T& far, const T& left, const T& right, const T& bottom, const T& top)
{
	Matrix<T, 4, 4> result = {
			2 / (right - left), 0, 0, (right + left) / (right - left),
			0, 2 / (top - bottom), 0, (top + bottom) / (top - bottom),
			0, 0, 2 / (near - far), (near + far) / (near - far)
	};

	return result;
}

#endif /* LINEAR_H_ */
