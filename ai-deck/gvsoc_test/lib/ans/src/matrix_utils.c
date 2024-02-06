
#include "matrix_utils.h"
#include "stdio.h"
#include "Gap.h"
#include "task.h"
#include <stdarg.h>


void printBits(int const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    // printf("%.16lf:\t", *(double *)ptr);
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            // printf("%u", byte);            
        }
    }
    // puts("");
}

void free_matrix(Matrix *m) {
  // pi_l2_malloc_dump();
  pmsis_l2_malloc_free(m->data, m->row*m->col*sizeof(double));
  pmsis_l2_malloc_free(m, sizeof(Matrix));
}

Matrix *init_matrix(int row, int col){

  Matrix *m = (Matrix *)pmsis_l2_malloc(sizeof(Matrix));
  if(m == NULL) {
    // printf("[ERROR] Failed to allocate matrix struct, size: %d at %x\n", row, col, sizeof(Matrix), m);
    return NULL;
  }

  m->data = (double *)pmsis_l2_malloc(row*col*sizeof(double));
  if(m->data == NULL) {
    // printf("[ERROR] Failed to allocate data matrix (%dx%d), size: %d at %x\n", row, col, row*col*sizeof(double), m->data);
    return NULL;
  }

  memset(m->data, 0, row*col*sizeof(double));
  m->row = row;
  m->col = col;
  
  return m;
}

void set_matrix(Matrix *m, ...){

  va_list args;
  va_start(args, m);

  for(int i = 0; i < m->row*m->col; i++) {
    m->data[i] = (double)va_arg(args, double);
  }
  va_end(args);
}

void print_matrix(Matrix *m) {

  // printf("\nMatrix (%dx%d):\n", m->row, m->col);
  for(int i = 0; i < m->row; i++) {
    for(int j = 0; j < m->col; j++) {
      // printf("%.9f ", m->data[m->col * i + j]);
    }
    // printf("\n");
  }
}

void print_matrix_bin(Matrix *m) {

  // printf("\nMatrix (%dx%d):\n", m->row, m->col);
  for(int i = 0; i < m->row; i++) {
    for(int j = 0; j < m->col; j++) {
      printBits(sizeof(double), &(m->data[m->col * i + j]));
    }
    // printf("----------------------------\n");
  }
}


/**
 * @brief 
 *  may cause an error if:
 *    m->m1->row % NB_CORE > NB_CORE
 * @param arg 
 */
int compute_parallelize(int *start, int *end, int *batch_size, Matrix *m) {

  int remain = NB_CORE < m->row ? m->row % NB_CORE : 0;

  /* Compute batch to parallelize */
  if(NB_CORE > m->row) {
    
    (*batch_size) = 1;
    if(pi_core_id() > (unsigned)(m->row - 1)) {
      pi_cl_team_barrier();
      return -1;
    }
  } else {

    (*batch_size) = m->row / NB_CORE;
  }

  (*start) = pi_core_id() * (*batch_size);

  if(remain && (int)pi_core_id() > (NB_CORE - remain - 1)) {
    // printf("augment batch, %d, %d\n", NB_CORE, remain);
    (*batch_size)++;
    (*start) += pi_core_id() - (NB_CORE - remain);
  }
  (*end) = (*start) + (*batch_size);


  // printf("CORE %ld, batch: %d, start: %d, end %d\n", pi_core_id(), (*batch_size), (*start), (*end));
  pi_cl_team_barrier();
  return 0;
}


void _mult_matrix(void *arg) {
  OpArg *m = (OpArg *)arg;


  int start;
  int end;
  int batch_size = 0;
  double d = 0;

  if(compute_parallelize(&start, &end, &batch_size, m->m1)) {
    pi_cl_team_barrier();
    return;
  }

  // printf("Result: ");
  for(int i = start ; i < end; i++) {
    for(int j = 0 ; j < m->m2->col; j++) {
      
      m->mr->data[i * m->mr->col + j] = 0;
      d = 0;
      
      for(int k = 0; k < m->m1->col; k++) {
        d += m->m1->data[i * m->m1->col + k] * m->m2->data[k * m->m2->col + j];
        m->mr->data[i * m->mr->col + j] += m->m1->data[i * m->m1->col + k] * m->m2->data[k * m->m2->col + j];
      }
    }
  }

  pi_cl_team_barrier();
}

void mult_matrix(Matrix *a, Matrix *b, Matrix *r) {

  double d = 0;
  
  // pi_cl_team_fork(NB_CORE, _mult_matrix, &mult_arg);
  for(int i = 0 ; i < a->row; i++) {
    for(int j = 0 ; j < b->col; j++) {
      
      r->data[i * r->col + j] = 0;
      d = 0;
      
      for(int k = 0; k < a->col; k++) {
        d += a->data[i * a->col + k] * b->data[k * b->col + j];
        r->data[i * r->col + j] += a->data[i * a->col + k] * b->data[k * b->col + j];
      }
    }
  }
}

void augment_identity(Matrix *ident) {
  int n = ident->col/2;
  for(int i = 0; i < ident->row; i++) {
    for(int j = 0; j < n; j++) {
      if(i == j) {
        ident->data[i*ident->col + j + (n)] = 1;
      } else {

        ident->data[i*ident->col + j + (n)] = 0;
      }
    }
  }
}

int gauss_jordan(Matrix *ident) {

  double r;
  int n = ident->col/2;

  for(int i = 0; i < ident->row; i++) {
    if(ident->data[i*ident->col+i] == 0) {
      // printf("MATH ERROR, index: [%d][%d], val: %.2f !\n",i,i,ident->data[i*ident->col+i]);
      return -1;
    }

    for(int j = 0; j < n; j++) {
      if(i != j) {
        r = ident->data[j*ident->col+i]/ident->data[i*ident->col+i];

        for(int k = 0; k < ident->col; k++) {
          ident->data[j*ident->col+k] -= (r * ident->data[i*ident->col+k]);
        }
      }
    }
  }
  return 0;
}

void convert_diag(Matrix *ident) {
  
  int n = ident->col/2;

  for(int i = 0; i < ident->row; i++) {
    for(int j = n; j < ident->col; j++) {
      ident->data[i*ident->col+j] = ident->data[i*ident->col+j]/ident->data[i*ident->col+i];
    }
  }
}


void invert_matrix(Matrix *a, Matrix *r) {

  Matrix *ident = init_matrix(a->row, 2*a->col);
  /* Copy matrix in augmented */
  for(int i = 0; i < a->row; i++) {
    for(int j = 0; j < a->col; j++) {
      ident->data[i*ident->col+j] = a->data[i*a->col+j]; 
    }
  }

  /* calcul identity */
  augment_identity(ident);

  /* Apply Gauss Jordan Elimination */
  if(gauss_jordan(ident)) {

    free_matrix(ident);
    return;
  }

  /* Convert Principal Diagonal to 1 */
  convert_diag(ident);

  /* Extract inverted from augmented matrix */
  for(int i = 0; i < r->row; i++) {
    for(int j = 0; j < r->col; j++) {
      r->data[i*r->col+j] = ident->data[i*ident->col+j+r->col];
    }
  }

  free_matrix(ident);
}

void transpose_matrix(Matrix *a, Matrix *r) {

  for(int i = 0; i< a->row; i++) {
    for(int j = 0; j< a->col; j++) {
    
      r->data[j*r->col+i] = a->data[i*a->col+j];
    }
  }
}

void add_matrix(Matrix *a, Matrix *b, Matrix *r) {

  for(int i = 0; i< a->row; i++) {
    for(int j = 0; j< b->col; j++) {
    
      r->data[i*r->col+j] = a->data[i*a->col+j]+b->data[i*b->col+j];
    }
  }
}

void scale_matrix(Matrix *a, double scale, Matrix *r) {

  for(int i = 0; i< a->row; i++) {
    for(int j = 0; j< a->col; j++) {
      r->data[i*r->col+j] = a->data[i*a->col+j]*scale;
    }
  }
}

void set_matrix_identity(Matrix *a) {

  for(int i = 0; i< a->row; i++) {
    for(int j = 0; j< a->col; j++) {
      if(i == j) {
        a->data[i*a->col+j] = 1.0;
      } else {
        a->data[i*a->col+j] = 0.0;
      }
    }
  }
}

void argsort(double *arr, int *indices, int size) {

  for (int i = 0; i < size; i++) {
    indices[i] = i;
  }

  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[indices[j]] > arr[indices[j + 1]]) {
        int temp = indices[j];
        indices[j] = indices[j + 1];
        indices[j + 1] = temp;
      }
    }
  }
}

double min_array(double *arr, int size) {
  double min = arr[0];

  for (int i = 1; i < size; i++) {
    if (arr[i] < min) {
      min = arr[i]; 
    }
  }

  return min;
}