
#include <pmsis.h>
#include <math.h>
#include "Gap.h"

#include "ans.h"
#include "matrix_utils.h"
#include "cpx.h"

Matrix *A_inv;
Matrix *theta_t;
Matrix *theta_t_transpose;
Matrix *delay_prediction;
Matrix *dl_conf_matrix_1;
Matrix *dl_conf_matrix_2;
Matrix *xp;
Matrix *xp_transpose;
Matrix *update_A;

Matrix *X;
Matrix *A;
Matrix *B;

double g_alpha;
// double g_beta;
double g_T_e;
int g_T_c;

double g_last_d_e_pt;
double g_last_d_e_pt_hat;
int g_inference_count;
int g_correct_pred_counter;
double g_d_e_hat[NB_CP];
double g_d_f[NB_CP];
// double g_d_e[NB_CP-1];
double g_d_hat[NB_CP];
int g_d_p_estimation_argsort[NB_CP];
int g_p_t_;
double g_d_e_pt;



/*----------------- Simulation Only -----------------*/
/* Client inference time */

/* Server inference + transfert time */
double D_E_Pt_simulated[NB_CP] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0 // P=8
};

double D_F_simulated[NB_CP] = {
  1, 2, 3, 4, 5, 6, 7, 8, 59
};

double D_F_time[5][NB_CP] = {
  {5e-3, 5e-3, 5e-3, 5e-3, 1e-3, 5e-3, 5e-3, 5e-3, 32e-3}, // P=8
  {1e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 32e-3}, // P=5
  {48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 32e-3},  // P=2
  {48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 48e-3, 48e-3, 32e-3},
  {48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 48e-3, 48e-3, 48e-3, 32e-3}
};
double D_E_time[5][NB_CP] = {
  {5e-3, 5e-3, 5e-3, 5e-3, 1e-3, 5e-3, 5e-3, 5e-3, 0}, // P=8
  {1e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 0}, // P=5
  {48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 0},  // P=2
  {48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 48e-3, 48e-3, 0},
  {48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 48e-3, 48e-3, 48e-3, 0}
};


/*---------------------------------------------------*/

void calc_d_e(double *d_e_hat, double alpha) {
  double delay_confidence = 0;
  
  /* Invert matrix A*/
  // scale_matrix(A, (double)0.9, A);
  invert_matrix(A, A_inv);
  // cpxPrintToConsole(LOG_TO_CRTP, "ANS after invert_matrix\n");

  /* Multiplie A_inv and B*/
  mult_matrix(A_inv, B, theta_t);
  // cpxPrintToConsole(LOG_TO_CRTP, "ANS after mult_matrix\n");

  /* Tranpose matrix theta*/
  transpose_matrix(theta_t, theta_t_transpose); // TO check
  // cpxPrintToConsole(LOG_TO_CRTP, "ANS after transpose_matrix\n");


  for(int p = 0; p < NB_CP; p++) {
    //TODOOOOOOO: SERT A RIEN CAR PAS PYTHON 
    memcpy(xp->data, (X->data+(p*X->col)), X->col*sizeof(double));
    // cpxPrintToConsole(LOG_TO_CRTP, "ANS after memcpy\n");
    

    /* Define delay prediction*/
    mult_matrix(theta_t_transpose, xp, delay_prediction);
    // cpxPrintToConsole(LOG_TO_CRTP, "ANS after mult_matrix\n");
    
    /* a_inv * xp */
    mult_matrix(A_inv, xp, dl_conf_matrix_1);
    // cpxPrintToConsole(LOG_TO_CRTP, "ANS after mult_matrix\n");

    /* Tranpose xp*/
    transpose_matrix(xp, xp_transpose);
    // cpxPrintToConsole(LOG_TO_CRTP, "ANS after transpose_matrix\n");

    /* xp.T A_inv xp*/
    mult_matrix(xp_transpose, dl_conf_matrix_1, dl_conf_matrix_2);
    // cpxPrintToConsole(LOG_TO_CRTP, "ANS after mult_matrix\n");
    
    delay_confidence = alpha * sqrt(dl_conf_matrix_2->data[0]);
    d_e_hat[p] = delay_prediction->data[0] - delay_confidence;
  }
}

void update(int p_t_, double d_e_pt, Matrix *A, Matrix *B, Matrix *X) {
  
  memcpy(xp->data, (X->data+(p_t_*X->col)), X->col*sizeof(double));
  
  transpose_matrix(xp, xp_transpose);

  // print_matrix(xp_transpose);
  mult_matrix(xp, xp_transpose, update_A);

  add_matrix(A, update_A, A);
  
  // B = B + xp*d_e_pt;
  scale_matrix(xp, d_e_pt, xp);

  add_matrix(B, xp, B);
}

static int cpt_rand = 0;
void randomize_D_E() {
  // cpxPrintToConsole(LOG_TO_CRTP, "----- RANDOMIZE NEW VAL = serv_time[%d]\n NEW VAL: ", cpt_rand);
  
  memcpy(D_E_Pt_simulated, D_E_time[cpt_rand], NB_CP*sizeof(double));
  memcpy(D_F_simulated, D_F_time[cpt_rand], NB_CP*sizeof(double));

  // for(int i = 0; i< NB_CP; i++) {
  //   cpxPrintToConsole(LOG_TO_CRTP, "%f, ", D_E_Pt_simulated[i]);
  // }

  // cpxPrintToConsole(LOG_TO_CRTP, "\n");
  cpt_rand = (++cpt_rand) % 6;
}

void init_ans() {

  X = init_matrix(NB_CP, 7);
  A = init_matrix(X->col, X->col);
  B = init_matrix(X->col, 1);

  set_matrix_identity(A);

  A_inv = init_matrix(A->row, A->col);
  theta_t = init_matrix(A->row, 1);
  theta_t_transpose = init_matrix(1, A->row);
  xp = init_matrix(X->col, 1);
  xp_transpose = init_matrix(1, X->col);
  delay_prediction = init_matrix(1, 1);
  dl_conf_matrix_1 = init_matrix(A->row, 1);
  dl_conf_matrix_2 = init_matrix(1, 1);
  update_A = init_matrix(A->row, A->col);
  

  set_matrix(X,
    19200000.0/19200000.0*1000+1, 640064.0/19200000.0*1000+1, 140064.0,  6.0, 2.0, 7.0, 40000.0, 
    15200000.0/19200000.0*1000+1, 640064.0/19200000.0*1000+1, 100064.0,  5.0, 2.0, 6.0, 40000.0, 
    13760000.0/19200000.0*1000+1, 640064.0/19200000.0*1000+1, 60064.0,   4.0, 2.0, 5.0, 40000.0, 
    10880000.0/19200000.0*1000+1, 640064.0/19200000.0*1000+1, 40064.0,   3.0, 2.0, 4.0, 20000.0, 
    9440000.0/19200000.0*1000+1,  640064.0/19200000.0*1000+1, 20064.0,   2.0, 2.0, 3.0, 20000.0, 
    1440000.0/19200000.0*1000+1,  640064.0/19200000.0*1000+1, 10064.0,   1.0, 2.0, 2.0, 10000.0, 
    0.0+1,                        640064.0/19200000.0*1000+1, 64.0,      0.0, 2.0, 1.0, 10000.0, 
    0.0+1,                        64.0/19200000.0*1000+1,     0.0,       0.0, 1.0, 0.0, 64.0, 
    0.0+1,                        0.0+1,                      0.0,       0.0, 0.0, 0.0, 0.0
  );
  print_matrix(X);
  // return;
  g_alpha = 0.1;
//   beta = 1.0;
  g_T_e = 5e-2; //a prediction is considered good enough if (d_e_pt - d_e_pt_hat) < T_e
  g_T_c = 10; //passed this number of good predictions, the system isn't updated anymore until it is wrong again

  /*--------------------------------*/
  g_last_d_e_pt = -1.0;
  g_last_d_e_pt_hat = -1.0;
  g_inference_count = 0;
  g_correct_pred_counter = 0;
  g_d_e_pt = -1;
}

int start_ans = 0;
int end_ans = 0;
int time_ans = 0;

int do_ans(void(*do_f_inference)(int), void(*do_e_inference)(int)) {

  start_ans = xTaskGetTickCount();
  int rand = 0;

  int start_f_inf = 0;
  int end_f_inf = 0;
  int time_f_inf = 0;

  int start_e_inf = 0;
  int end_e_inf = 0;
  int time_e_inf = 0;
    
  // Estimate back inference time
  calc_d_e(g_d_e_hat, g_alpha);

  // Estimate total inference time
  memcpy(g_d_f, D_F_simulated, NB_CP*sizeof(double));

  for(int i  = 0; i < NB_CP; i++) {
    g_d_hat[i] = g_d_f[i] + g_d_e_hat[i];
  }

  // Choose partition p point for min estimated total time
  argsort(g_d_hat, g_d_p_estimation_argsort, NB_CP);
  g_p_t_ = g_d_p_estimation_argsort[0];

  // Every 1/4 times, randomize decision for p if selected partition point is set to NO partial inference
  if (g_p_t_ == NB_CP-1 && g_inference_count % 400 == 0){

    rand = (xTaskGetTickCount() + (int)B->data[0]) % (NB_CP - 1);
    g_p_t_ = g_d_p_estimation_argsort[rand];
  }

  // Run inference
  if(g_p_t_ == NB_CP-1) {

      start_f_inf = xTaskGetTickCount();
      do_f_inference(g_p_t_);
      end_f_inf = xTaskGetTickCount();

      // Full edge, we do not update UCB
      g_d_f[g_p_t_] = D_F_simulated[g_p_t_]; // Measured front inference time

  } else {
    /* Do and measuring th time of edge inference */
    start_f_inf = xTaskGetTickCount();
    do_f_inference(g_p_t_);
    end_f_inf = xTaskGetTickCount();

    /* Do and measuring th time of server inference */
    start_e_inf = xTaskGetTickCount();
    do_e_inference(g_p_t_);
    end_e_inf = xTaskGetTickCount();

    g_d_f[g_p_t_] = D_F_simulated[g_p_t_]; // Measured front inference time
    g_d_e_pt = D_E_Pt_simulated[g_p_t_]; // Measured back inference time

    // keep count of the number of times the estimation was "close enough" to the measured value
    if (g_last_d_e_pt >= 0 && g_last_d_e_pt_hat >= 0 && fabs(g_last_d_e_pt - g_last_d_e_pt_hat) < g_T_e) {
        g_correct_pred_counter++;
    } else {
        g_correct_pred_counter = 0;
    }
    g_last_d_e_pt = g_d_e_pt;
    g_last_d_e_pt_hat = min_array(g_d_e_hat, NB_CP);

    if(g_correct_pred_counter < g_T_c) {
        update(g_p_t_, g_d_e_pt, A, B, X);
    }
  }

  // cpxPrintToConsole(LOG_TO_CRTP, "END   duration: %d\n", time_e_inf);

  // 
  if(!(g_inference_count % 200)) {
    // TODO change simulation time
    cpxPrintToConsole(LOG_TO_CRTP, "--------------------------------------------\n");
    if(cpt_rand == 5) {
      return -1;
    }
    randomize_D_E(g_inference_count);
  }

  g_inference_count++;

  end_ans = xTaskGetTickCount();
  
  time_f_inf = end_f_inf - start_f_inf;
  time_e_inf = end_e_inf - start_e_inf;
  time_ans = end_ans - start_ans;
  // cpxPrintToConsole(LOG_TO_CRTP, "FRONT      duration: %d [TICKS]\n", time_f_inf);
  cpxPrintToConsole(LOG_TO_CRTP, "END        duration: %d [TICKS]\n", time_e_inf);
  // cpxPrintToConsole(LOG_TO_CRTP, "ANS no inf duration: %d [TICKS]\n", (time_ans - time_e_inf- time_f_inf));
  cpxPrintToConsole(LOG_TO_CRTP, "ANS        duration: %d [TICKS]\n", time_ans);


  return 0;

  // free_matrix(A);
  // free_matrix(B);
  // free_matrix(X);

}

void free_ans() {
  free_matrix(A);
  free_matrix(B);
  free_matrix(X);
  free_matrix(A_inv);
  free_matrix(theta_t);
  free_matrix(theta_t_transpose);
  free_matrix(xp);
  free_matrix(xp_transpose);
  free_matrix(delay_prediction);
  free_matrix(dl_conf_matrix_1);
  free_matrix(dl_conf_matrix_2);
  free_matrix(update_A);
}