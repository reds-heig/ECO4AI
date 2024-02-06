#ifndef ANS_H
#define ANS_H


#define NB_CP 9

/**
 * @brief init ANS matrix and variables
 * 
 */
void init_ans();

/**
 * @brief release memory from used by ANS
 * 
 */
void free_ans();

/**
 * @brief Do Ans and inference with giver callbacks
 * 
 * @param do_f_inference Edge inference function
 * @param do_e_inference Server inference function
 * @return int 0 if OK
 */
int do_ans(void(*do_f_inference)(int), void(*do_e_inference)(int));

#endif