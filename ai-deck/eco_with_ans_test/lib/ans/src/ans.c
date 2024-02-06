
#include <pmsis.h>
#include <math.h>
#include "Gap.h"

#include "ans.h"
#include "matrix_utils.h"
#include "cpx.h"

#if 0
#define ANS_SIM
#endif

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
double D_F_simulated[NB_CP] = {
  0.06+25, 2.32+50, 5.5+75, 6.6+100, 8.08+125, 9.74+150, 10.96+175, 15.22+200, 15.22+225
};

#ifdef ANS_SIM
/* Server inference + transfert time */
double D_E_Pt_simulated[NB_CP] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0 // P=8
};


double D_F_time[5][NB_CP] = {
  {1e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 32e-3}, // P=0
  // {48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 32e-3},  // P=7
  {48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 48e-3, 48e-3, 32e-3}, // P=5
  {48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 1e-3}, // P=8
  {48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 48e-3, 48e-3, 48e-3, 32e-3}, // P=4
  {1e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 32e-3}, // P=5
};
double D_E_time[5][NB_CP] = {
  {1e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 0}, // P=0
  // {48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 0},  // P=7
  {48e-3, 48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 48e-3, 48e-3, 0},
  {5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 0}, // P=8
  {48e-3, 48e-3, 48e-3, 48e-3, 10e-3, 48e-3, 48e-3, 48e-3, 0},
  {1e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 5e-3, 0}, // P=0
};
#endif
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

#ifdef ANS_SIM
static int cpt_rand = 0;
void randomize_D_E() {
  
  cpxPrintToConsole(LOG_TO_CRTP, "-------------------------------------------- %d\n", cpt_rand);

  memcpy(D_E_Pt_simulated, D_E_time[cpt_rand], NB_CP*sizeof(double));
  memcpy(D_F_simulated, D_F_time[cpt_rand], NB_CP*sizeof(double));
  
  cpt_rand = (++cpt_rand) % 6;
}
#endif

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
  

  int MAC_UNIT_MAP_RANGE = 10;

  set_matrix(X,
    19200000.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 640064.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 140064.0,  6.0, 2.0, 7.0, 40000.0, 
    15200000.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 640064.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 100064.0,  5.0, 2.0, 6.0, 40000.0, 
    13760000.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 640064.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 60064.0,   4.0, 2.0, 5.0, 40000.0, 
    10880000.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 640064.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 40064.0,   3.0, 2.0, 4.0, 20000.0, 
    9440000.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0,  640064.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 20064.0,   2.0, 2.0, 3.0, 20000.0, 
    1440000.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0,  640064.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 10064.0,   1.0, 2.0, 2.0, 10000.0, 
    0.0+1.0,                        640064.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0, 64.0,      0.0, 2.0, 1.0, 10000.0, 
    0.0+1.0,                        64.0/19200000.0*MAC_UNIT_MAP_RANGE+1.0,     0.0,       0.0, 1.0, 0.0, 64.0, 
    0.0+1.0,                        0.0+1.0,                      0.0,       0.0, 0.0, 0.0, 0.0
  );
  print_matrix(X);
  // return;
  g_alpha = 0.1;
//   beta = 1.0;
  g_T_e = 10; // a prediction is considered good enough if (d_e_pt - d_e_pt_hat) < T_e
  g_T_c = 30; // passed this number of good predictions, the system isn't updated anymore until it is wrong again

  /*--------------------------------*/
  g_last_d_e_pt = -1.0;
  g_last_d_e_pt_hat = -1.0;
  g_inference_count = 0;
  g_correct_pred_counter = 0;
  g_d_e_pt = -1;

  // Estimate total inference time
  memcpy(g_d_f, D_F_simulated, NB_CP*sizeof(double));
}


// static int back_timing[NB_CP];
// static int front_timing[NB_CP];
// static int tx_timing[NB_CP];

static bool is_learning = false;

static int rand = 0;

static int start_f_inf = 0;
static int end_f_inf = 0;

static int start_e_inf = 0;
static int end_e_inf = 0;
    
static int start_ans = 0;
static int end_ans = 0;
static int time_ans = 0;

static int time_f_inf = 0;
static int time_e_inf = 0;
static int tx_time = 0;

int do_ans(void(*do_full_f_inference)(uint8_t), void(*do_f_inference)(uint8_t), int(*do_e_inference)(uint8_t)) {

  start_ans = xTaskGetTickCount();

  // Estimate back inference time
  calc_d_e(g_d_e_hat, g_alpha);

  for(int i  = 0; i < NB_CP; i++) {
    g_d_hat[i] = g_d_f[i] + g_d_e_hat[i];
  }

  // Choose partition p point for min estimated total time
  argsort(g_d_hat, g_d_p_estimation_argsort, NB_CP);
  g_p_t_ = g_d_p_estimation_argsort[0];

  /* Don't allow to take 8 as cuttign point */
  // if(g_p_t_ == NB_CP-1) {
  //   g_p_t_ = g_d_p_estimation_argsort[1];
  // }

#if 0
  // Every 1/4 times, randomize decision for p if selected partition point is set to NO partial inference
  if (g_p_t_ == NB_CP-1 && g_inference_count % 50 == 0){

    do {

      rand = (xTaskGetTickCount() + (int)B->data[0]) % (NB_CP - 1);
    } while(rand == g_p_t_);

    g_p_t_ = g_d_p_estimation_argsort[rand];
  }
#endif

  if(is_learning) {
    cpxPrintToConsole(LOG_TO_CRTP,"ANS Learning phase");
    g_p_t_ = (g_p_t_ + 1) % NB_CP;

    if(g_inference_count == NB_CP*2) {
      is_learning = false;
      cpxPrintToConsole(LOG_TO_CRTP,"ANS Learning phase finish");
    }
  }

  // Run inference
  if(g_p_t_ == NB_CP-1) {

      start_f_inf = xTaskGetTickCount();
      // do_f_inference(g_p_t_);
      do_full_f_inference(g_p_t_);
      end_f_inf = xTaskGetTickCount();
      time_f_inf = end_f_inf - start_f_inf;

      // Full edge, we do not update UCB

      g_d_f[g_p_t_] = (double)time_f_inf;

  } else {
    /* Do and measuring th time of edge inference */
    start_f_inf = xTaskGetTickCount();
    do_f_inference(g_p_t_);
    end_f_inf = xTaskGetTickCount();
    time_f_inf = end_f_inf - start_f_inf;

    /* Do and measuring th time of server inference */
    start_e_inf = xTaskGetTickCount();
    tx_time = do_e_inference(g_p_t_);
    end_e_inf = xTaskGetTickCount();
    time_e_inf = end_e_inf - start_e_inf;


    g_d_f[g_p_t_] = (double)time_f_inf;

    /* keep last server time if new measurment is too big */
    if((end_e_inf - start_e_inf) > 2000) {

      cpxPrintToConsole(LOG_TO_CRTP, "WARNING: keep last server time < 2000\n");
      // g_d_e_pt = g_d_e_pt;
    } else {

      // time_e_inf = end_e_inf - start_e_inf;  
      g_d_e_pt = (double)time_e_inf;
    }


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
  if(!(g_inference_count % 100)) {
#ifdef ANS_SIM
    // TODO change simulation time
    // cpxPrintToConsole(LOG_TO_CRTP, "--------------------------------------------\n");
    if(cpt_rand == 5) {
      return -1;
    }
    randomize_D_E(g_inference_count);
#endif
  }

  g_inference_count++;

  end_ans = xTaskGetTickCount();
  

  time_ans = end_ans - start_ans;
  cpxPrintToConsole(LOG_TO_CRTP, "DATA0:%d,%d,%.0f,%d\n",
    g_p_t_, time_f_inf, g_d_e_pt, tx_time);

  cpxPrintToConsole(LOG_TO_CRTP, "DATA1:%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
    g_d_e_hat[0],g_d_e_hat[1],g_d_e_hat[2],g_d_e_hat[3],g_d_e_hat[4],g_d_e_hat[5],g_d_e_hat[6],g_d_e_hat[7],g_d_e_hat[8]);


  // start_f_inf = xTaskGetTickCount();
  // // do_f_inference(g_p_t_);
  // do_full_f_inference(NB_CP-1);
  // end_f_inf = xTaskGetTickCount();
  // time_f_inf = end_f_inf - start_f_inf;
  // front_timing[NB_CP-1] = time_f_inf;
  // tx_timing[NB_CP-1] = 0;

  // for(int i = 0; i < NB_CP-1; i++) {

  //   /* Do and measuring th time of edge inference */
  //   start_f_inf = xTaskGetTickCount();
  //   do_f_inference(i);
  //   end_f_inf = xTaskGetTickCount();

  //   /* Do and measuring th time of server inference */
  //   start_e_inf = xTaskGetTickCount();
  //   tx_time = do_e_inference(i);
  //   end_e_inf = xTaskGetTickCount();

  //   time_f_inf = end_f_inf - start_f_inf;
  //   front_timing[i] = time_f_inf;

  //   time_e_inf = end_e_inf - start_e_inf;
  //   back_timing[i] = time_e_inf;

  //   tx_timing[i] = tx_time;

  // }


  // cpxPrintToConsole(LOG_TO_CRTP, "DATA1:"
  // "%d,%d,%d,%d,%d,%d,%d,%d,%d,"
  // "%d,%d,%d,%d,%d,%d,%d,%d,%d,"
  // "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
  // front_timing[0],front_timing[1],front_timing[2],front_timing[3],front_timing[4],front_timing[5],front_timing[6],front_timing[7],front_timing[8],
  // back_timing[0],back_timing[1],back_timing[2],back_timing[3],back_timing[4],back_timing[5],back_timing[6],back_timing[7],back_timing[8],
  // tx_timing[0],tx_timing[1],tx_timing[2],tx_timing[3],tx_timing[4],tx_timing[5],tx_timing[6],tx_timing[7],tx_timing[8]);

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