#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

typedef struct {
  double *data;
  int row;
  int col;
} Matrix;

typedef struct {
  Matrix *m1; // 1st op
  Matrix *m2; // 2nd op
  Matrix *mr; // result
} OpArg;

#define NB_CORE 1

/**
 * @brief Free ressources used by a matrix
 * 
 * @param m 
 */
extern void free_matrix(Matrix *m);

/**
 * @brief Init matrix and set data array to 0
 * 
 * @param row 
 * @param col 
 * @return Matrix* 
 */
extern Matrix *init_matrix(int row, int col);

/**
 * @brief Set data field of matrix m
 * 
 * @param m 
 * @param ... double values
 */
extern void set_matrix(Matrix *m, ...);

/**
 * @brief Print matrix m
 * 
 * @param m 
 */
extern void print_matrix(Matrix *m);

/**
 * @brief Multiplie 2 matrix a and b
 * Return in r
 * The multiplication is parallelized
 * 
 * @param a 
 * @param b 
 * @param r 
 */
void mult_matrix(Matrix *a, Matrix *b, Matrix *r);

/**
 * @brief Invert Matrix a using Gauss Jordan Elimination
 * return result in r
 * 
 * @param a 
 * @param r 
 */
void invert_matrix(Matrix *a, Matrix *r);

/**
 * @brief Transpose matrix a
 * return result ind r
 * 
 * @param a 
 * @param r 
 */
void transpose_matrix(Matrix *a, Matrix *r);

/**
 * @brief Add matrix a and b
 * return result in r
 * 
 * @param a 
 * @param b 
 * @param r 
 */
void add_matrix(Matrix *a, Matrix *b, Matrix *r);

/**
 * @brief Scale matrix a by value 'scale'
 * return result in r
 */
void scale_matrix(Matrix *a, double scale, Matrix *r);

/**
 * @brief Set the matrix a as identity matrix
 * 
 * @param a 
 */
void set_matrix_identity(Matrix *a);

void printBits(int const size, void const * const ptr);

/**
 * @brief return a sorted array of indices 
 * 
 * @param arr 
 * @param indices 
 * @param size 
 */
void argsort(double *arr, int *indices, int size);

/**
 * @brief return the min value of a double array
 * 
 * @param arr 
 * @param size 
 * @return double 
 */
double min_array(double *arr, int size);

#endif
