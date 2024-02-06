
#include "pmsis.h"
#include "e4a_collision.h"
#include "cpx.h"
#include "bsp/bsp.h"
#include <stdio.h>
#include <stdarg.h>

#include "eco_type.h"

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s 

AT_HYPERFLASH_FS_EXT_ADDR_TYPE e4a_collision_L3_Flash = 0;

#define NUM_CLASSES 1
#define AT_INPUT_SIZE 	(AT_INPUT_WIDTH*AT_INPUT_HEIGHT*AT_INPUT_COLORS)

typedef signed char NETWORK_OUT_TYPE;
// typedef int8_t NETWORK_OUT_TYPE;
L2_MEM NETWORK_OUT_TYPE *ResOut;


static CPXPacket_t txp;

void console_print(char *format, ...) {
    va_list args;
    va_start(args, format);
#ifdef GVSOC
    vprintf(format, args);
#else
    cpxPrintToConsole(LOG_TO_CRTP, format, args);
#endif
    va_end(args);
}

static CP curr_cp = S4;
static CP cp_ret;

static void RunNetwork() {
    
    cp_ret = e4a_collisionCNN(ResOut, curr_cp);
}

tensor intermediates[] = {
    { .c=1,  .h=200, .w=200, .offset=0     }, // NO
    { .c=4,  .h=100, .w=100, .offset=0     }, // S4
    { .c=4,  .h=100, .w=100, .offset=40000 }, // S7
    { .c=8,  .h=50,  .w=50,  .offset=0     }, // S10
    { .c=8,  .h=50,  .w=50,  .offset=20000 }, // S13 
    { .c=16, .h=25,  .w=25,  .offset=10000 }, // S16
    { .c=16, .h=25,  .w=25,  .offset=0     },  // S19
    { .c=1,  .h=1,   .w=1,   .offset=0     }  // ALL
};

char *cp_str[] = {
    "NO", "S4", "S7", "S10", "S13", "S16", "S19", "ALL"
};

signed char *get_inter_data(CP cp) {
    
    return cp == ALL ? ResOut : (signed char *)(e4a_collision_L2_Memory_Dyn+intermediates[cp].offset);
}

int body(void) {

	// Open the cluster
	struct pi_device cluster_dev;
	struct pi_cluster_conf conf;
	pi_cluster_conf_init(&conf);

	conf.cc_stack_size = STACK_SIZE;

	pi_open_from_conf(&cluster_dev, (void *)&conf);
	pi_cluster_open(&cluster_dev);

    /* Set Frequency */
	pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC*1000*1000);
	pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL*1000*1000);
	// pi_freq_set(PI_FREQ_DOMAIN_PERIPH, FREQ_PE*1000*1000);
	console_print("Set FC Frequency = %d MHz, CL Frequency = %d MHz, PERIPH Frequency = %d MHz\n",
			pi_freq_get(PI_FREQ_DOMAIN_FC), pi_freq_get(PI_FREQ_DOMAIN_CL), pi_freq_get(PI_FREQ_DOMAIN_PERIPH));

    ResOut = (NETWORK_OUT_TYPE *) AT_L2_ALLOC(0, NUM_CLASSES*sizeof(NETWORK_OUT_TYPE));
    if (ResOut==0) {
		console_print("Failed to allocate Memory for Result (%ld bytes)\n", NUM_CLASSES*sizeof(NETWORK_OUT_TYPE));
  	    pmsis_exit(-1);
	}
    
    /* Network Constructor */
    int err_const = AT_CONSTRUCT();
    if (err_const)
    {
        console_print("Graph constructor exited with error: %d\n", err_const);
        pmsis_exit(-2);
    }

    struct pi_cluster_task* task = (struct pi_cluster_task*)pi_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = &RunNetwork;
    task->stack_size = STACK_SIZE;             // defined in makefile
    task->slave_stack_size = SLAVE_STACK_SIZE; // "
    task->arg = NULL;

    // char *ImageName = __XSTR(AT_IMAGE);
    char ImageName[255];
    char ImageSave[255];
    console_print("Model:\t%s\n\n", __XSTR(TRAINED_MODEL));
    for (int i = 0; i <= 353; i++) {

    // FAKE INPUT
        // sprintf(ImageName, "%s/../calibration/good/%05d.ppm", __XSTR(CURR_DIR), i);
        sprintf(ImageName, "%s/calibration/good/00000.ppm", __XSTR(CURR_DIR));
        //Reading Image from Bridge
        img_io_out_t type = IMGIO_OUTPUT_CHAR;
        if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, AT_INPUT_COLORS, Input_1, AT_INPUT_SIZE*sizeof(char), type, 0)) {
            console_print("Failed to load image %s\n", ImageName);
            pmsis_exit(-1);
        }
        // console_print("Finished reading image\n");

        // for (int i=0; i<AT_INPUT_SIZE; i++)
        //     Input_1[i] = ((unsigned char *)Input_1)[i] / 2;

        // static pi_evt_t end_task_nn;

        // pi_cluster_task(task, (void (*)(void *))&RunNetwork, NULL);

        // pi_cluster_enqueue_task_async(&cluster_dev, task, NULL);
        pi_cluster_send_task_to_cl(&cluster_dev, task);

        console_print("Model ran to %s Layer, tensor (CxHxW): %ux%ux%u first bytes = %d\n",
            cp_str[cp_ret],
            intermediates[cp_ret].c, intermediates[cp_ret].h, intermediates[cp_ret].w, 
            get_inter_data(cp_ret)[0]);
        for(int i = 0; i < 20; i++) {
            console_print("%x ", get_inter_data(cp_ret)[i]);
        }
        console_print("\n");
        if(curr_cp <= ALL) {

            sprintf(ImageSave, "%s/saved/inter_layer_%s.dat", __XSTR(CURR_DIR), cp_str[cp_ret]);
            WriteImageToFile(ImageSave, intermediates[cp_ret].w, intermediates[cp_ret].h, intermediates[cp_ret].c, get_inter_data(cp_ret), 13);
        }
        // printf("Waiting\n");
        //pi_evt_wait(&end_task_print);
        // pi_evt_wait(&end_task_nn);

        console_print("output = %d\n\n", *ResOut);
        curr_cp++;
    }

    console_print("\n\n\t *** END E4A ***\n");
    return 0;
}

int start(void) {
    int ret = 0;
    
    console_print("\n\n\t *** E4A Collision Detection on GVSOC ***\n\n");
    console_print("Model Informations: \n");
    console_print("\tInput Size         :\t%d B\n", AT_INPUT_SIZE);
    console_print("\tOutput Size        :\t%d B\n", NUM_CLASSES*sizeof(NETWORK_OUT_TYPE));
    console_print("\tN CORE             :\t%d\n", NUM_CORES);
    console_print("\tN STACK_SIZE       :\t%d\n", STACK_SIZE);
    console_print("\tN SLAVE_STACK_SIZE :\t%d\n", SLAVE_STACK_SIZE);
    console_print("\n\n");


    cpxInit();
    // cpxInitRoute(CPX_T_GAP8, CPX_T_STM32, CPX_F_APP, &txp.route);

    // memcpy(txp.data, "HELLO\0", 6);
    // txp.dataLength = 6;
    // cpxSendPacketBlocking(&txp);

    ret =  body();

    while (1){
        pi_yield();
    }

    return ret;
}

int main(void) {

    pi_bsp_init();
    return pmsis_kickoff((void *)start);
}