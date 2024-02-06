/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

// #include "pmsis.h"
#include "stdio.h"
#include "Gap.h"
#include "task.h"
#include <pmsis.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "e4a_collisionKernels.h"

#include "e4a_collision.h"
#include "wifi.h"

#include "ans.h"
#include "cpx.h"
#include "camenc.h"

#include <stdlib.h>

#define NUM_CLASSES 1
typedef signed char NETWORK_OUT_TYPE;

static int wifiConnected = 0;
static int wifiClientConnected = 0;

static CPXPacket_t rxp;
static CPXPacket_t rx_res_p;
static CPXPacket_t tx_wifi_p;

static SemaphoreHandle_t data_sem;
static SemaphoreHandle_t capture_sem;

static struct pi_device camera;
static struct pi_device cluster_dev;
static struct pi_cluster_task *infer_task;


static unsigned char *cameraBuffer;
static unsigned char *croppedBuffer;

static uint8_t curr_cp;

/* Final output of the inference */
L2_MEM NETWORK_OUT_TYPE *ResOut;

AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

/* All cutting point and it Ouptput info for one model */
tensor inter_info[] = {
    { .name="NO",  .c=1,  .h=200, .w=200, .offset=0     }, // 0: NO
    { .name="S4",  .c=4,  .h=100, .w=100, .offset=40000 }, // 1: S4
    { .name="S7",  .c=4,  .h=100, .w=100, .offset=0     }, // 2: S7
    { .name="S10", .c=8,  .h=50,  .w=50,  .offset=40000 }, // 3: S10
    { .name="S13", .c=8,  .h=50,  .w=50,  .offset=0     }, // 4: S13 
    { .name="S16", .c=16, .h=25,  .w=25,  .offset=20000 }, // 5: S16
    { .name="S19", .c=16, .h=25,  .w=25,  .offset=64    }, // 6: S19
    { .name="S22", .c=1,  .h=1,   .w=64,  .offset=0     }, // 7: S22
    { .name="ALL", .c=1,  .h=1,   .w=1,   .offset=0     }  // 8: ALL
};

char *AT_GraphNodeNames[9] = {
	"NO",
	"S4",
	"S7",
	"S10",
	"S13",
	"S16",
	"S19",
	"S22",
	"ALL"
};

void rx_wifi_task(void *parameters) {

  while (1)
  {
    cpxReceivePacketBlocking(CPX_F_WIFI_CTRL, &rxp);

    WiFiCTRLPacket_t * wifiCtrl = (WiFiCTRLPacket_t*) rxp.data;

    switch (wifiCtrl->cmd)
    {
      case WIFI_CTRL_STATUS_WIFI_CONNECTED:
        cpxPrintToConsole(LOG_TO_CRTP, "Wifi connected (%u.%u.%u.%u)\n",
                          wifiCtrl->data[0], wifiCtrl->data[1],
                          wifiCtrl->data[2], wifiCtrl->data[3]);
        wifiConnected = 1;
        break;
      case WIFI_CTRL_STATUS_CLIENT_CONNECTED:
        cpxPrintToConsole(LOG_TO_CRTP, "Wifi client connection status: %u\n", wifiCtrl->data[0]);
        wifiClientConnected = wifiCtrl->data[0];
        break;
      case WIFI_CTRL_DATA_TRANSFER:
        /* New data from Server release data semaphore to get inference result */
        *ResOut = wifiCtrl->data[0];
        xSemaphoreGive(data_sem);
        break;
      default:
        break;
    }
  }
}

/* return half inference data buffer */
int8_t *get_inter_data(uint8_t cp) {

  return cp == ALL ? ResOut : (int8_t *)(e4a_collision_L2_Memory_Dyn+inter_info[cp].offset);
}

void sendIntermediateData(CPXPacket_t * packet, uint8_t * buffer, int cp) {

  InterHeader *dataHeader = (InterHeader *) packet->data;
  dataHeader->c=inter_info[cp].c;
  dataHeader->h=inter_info[cp].h;
  dataHeader->w=inter_info[cp].w;
  dataHeader->cp=cp;
  packet->dataLength = sizeof(InterHeader);
  
  cpxSendPacketBlocking(packet);
  // if(cpxSendPacket(packet, 2000) != pdTRUE) {

  //   cpxPrintToConsole(LOG_TO_CRTP, "Send to server TIMEOUT\n");
  // }

  // cpxPrintToConsole(LOG_TO_CRTP, "Send to server WITH TIMEOUT\n");
  sendBufferViaCPXBlocking(packet, (uint8_t *)get_inter_data(cp),
      (inter_info[cp].c*inter_info[cp].h*inter_info[cp].w));
  // if(sendBufferViaCPX(packet, (uint8_t *)get_inter_data(cp),
  //     (inter_info[cp].c*inter_info[cp].h*inter_info[cp].w)) != pdTRUE) {
  
  //   // cpxPrintToConsole(LOG_TO_CRTP, "-------------------------- Server TIMEOUT\n");
  //   xSemaphoreGive(data_sem);
  // }
  // cpxPrintToConsole(LOG_TO_CRTP, "AFTER Send to server WITH TIMEOUT\n");
}

static void RunNetwork() {
  
  __PREFIX(CNN)(croppedBuffer, ResOut, curr_cp);

  // transform 127|0 to 1|0 (useless)
  *ResOut = (*ResOut + 1) >> 7;
}

void do_edge_inference(uint8_t cp){
  
  curr_cp = cp;

  /* Run inference */
  pi_cluster_send_task_to_cl(&cluster_dev, infer_task);
  vTaskDelay((cp + 1) * 25); // Simulation delay
}

void do_full_edge_inference(uint8_t cp){
  
  curr_cp = cp;

  /* Run inference */
  pi_cluster_send_task_to_cl(&cluster_dev, infer_task);
  vTaskDelay((cp + 1) * 25); // Simulation delay
}

int do_server_inference(uint8_t cp){
  
  int start_tx_inf, end_tx_inf, time;

  if(wifiClientConnected) {
    
    /* Send intermediate data to inference server */
    start_tx_inf = xTaskGetTickCount();
    sendIntermediateData(&tx_wifi_p, NULL, cp);
    end_tx_inf = xTaskGetTickCount();
    time = end_tx_inf - start_tx_inf;

    /* Block until response from Inference server */
    if(xSemaphoreTake(data_sem, ( TickType_t ) (TickType_t)portMAX_DELAY) == pdTRUE) {

    } else {
      cpxPrintToConsole(LOG_TO_CRTP, "Receive from server TIMEOUT\n");
      *ResOut = -1;
    }

  } else {
    vTaskDelay(10);
  }

  return time;
}


static void cam_handler(void *arg) {

  camera_stop_capture(&camera);
  xSemaphoreGive(capture_sem);
}

void start_eco(void *parameters) {

  int ret;
  bool ans_done = 0;
  cpxPrintToConsole(LOG_TO_CRTP, "\n\n\t *** Starting ANS ***\n\n");

  /* Init acquisition Buffer */
  cameraBuffer = (unsigned char *)pmsis_l2_malloc(CAM_WIDTH*CAM_HEIGHT*CHANNELS * sizeof(unsigned char));    
  if (cameraBuffer == NULL) {
      cpxPrintToConsole(LOG_TO_CRTP, "Failed Allocated memory for camera buffer\n");
      return;
  }

  /* Init Inference Input Buffer */
  croppedBuffer = (unsigned char *)pmsis_l2_malloc(AT_INPUT_WIDTH*AT_INPUT_HEIGHT * sizeof(unsigned char));    
  if (croppedBuffer == NULL) {
      cpxPrintToConsole(LOG_TO_CRTP, "Failed Allocated memory for camera buffer\n");
      return;
  }


  /* Allocate L2 memory for inference output */
  ResOut = (NETWORK_OUT_TYPE *) AT_L2_ALLOC(0, NUM_CLASSES*sizeof(NETWORK_OUT_TYPE));
  if (ResOut == NULL) {
    cpxPrintToConsole(LOG_TO_CRTP, "Failed to allocate Memory for Result (%ld bytes)\n", NUM_CLASSES*sizeof(NETWORK_OUT_TYPE));
    pmsis_exit(-1);
	}

  /* Setup camera */
  if (open_camera(&camera)) {
    cpxPrintToConsole(LOG_TO_CRTP, "Failed to open camera\n");
    return ;
  }
  cpxPrintToConsole(LOG_TO_CRTP,"Camera is open\n");

    /* Allocate task for inference */
  infer_task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
  if (!infer_task)
  {  
    cpxPrintToConsole(LOG_TO_CRTP, "failed to allocate memory for infer_task\n");
  }
  cpxPrintToConsole(LOG_TO_CRTP,"Allocated memory for infer_task\n");

  memset(infer_task, 0, sizeof(struct pi_cluster_task));
  infer_task->entry = &RunNetwork;
  infer_task->stack_size = STACK_SIZE;             // defined in makefile
  infer_task->slave_stack_size = SLAVE_STACK_SIZE; // "                 "
  infer_task->arg = NULL;

  /* Construct CNN */
  ret = AT_CONSTRUCT();
  if (ret) {
    cpxPrintToConsole(LOG_TO_CRTP,"Failed to construct CNN with %d\n", ret);
    pmsis_exit(-5);
  }
  cpxPrintToConsole(LOG_TO_CRTP,"Constructed CNN\n");

  /* Init ANS */
  cpxPrintToConsole(LOG_TO_CRTP, "Init ANS\n");
  init_ans();

  cpxPrintToConsole(LOG_TO_CRTP, "DO ANS\n");

  /* Start first capture */
  camera_start_capture(&camera, cameraBuffer, cam_handler);


  /* Main loop */
  do {
    // vTaskDelay(300);
    
    /* Wait until image acquisition is done by camera */
    xSemaphoreTake(capture_sem, portMAX_DELAY);

    /* Preprocess: crop image to 200x200 */
    crop_img(cameraBuffer, croppedBuffer, AT_INPUT_WIDTH, AT_INPUT_HEIGHT);

    /* Do ANS things and inference only if connected to server */
    if(wifiClientConnected) {

      ans_done = do_ans(do_full_edge_inference, do_edge_inference, do_server_inference);
      // cpxPrintToConsole(LOG_TO_CRTP, "Inference Result: %d\n", *ResOut);

      // cpxPrintToConsole(LOG_TO_CRTP, "Last front inference was to : %s (%d)\n", AT_GraphNodeNames[curr_cp], curr_cp);
      // cpxPrintToConsole(LOG_TO_CRTP, "-----------------------------\n");

    } else {

      cpxPrintToConsole(LOG_TO_CRTP, "Not connected to Inference Server\n");
    }


    /* Start new capture */
    camera_start_capture(&camera, cameraBuffer, cam_handler);

  } while(!ans_done);
    

  free_ans();
  
  cpxPrintToConsole(LOG_TO_CRTP, "--------------------- END -----------------------\n");
  return;
}


int e4a_collision()
{

  struct pi_cluster_conf conf;
  struct pi_cluster_task *task;
  static struct pi_cluster_conf cluster_conf;
  
  int ret = 0;

  /* Configure task cluster */
  pi_cluster_conf_init(&cluster_conf);
  pi_open_from_conf(&cluster_dev, (void *)&cluster_conf);
  pi_cluster_open(&cluster_dev);

  pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC*1000*1000);
  pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL*1000*1000);

  cpxInit();
  cpxEnableFunction(CPX_F_WIFI_CTRL);

  cpxInitRoute(CPX_T_GAP8, CPX_T_WIFI_HOST, CPX_F_APP, &tx_wifi_p.route);


  cpxPrintToConsole(LOG_TO_CRTP, "\n\n\t *** E4A Collision Detection on GAP8 ***\n\n");
  cpxPrintToConsole(LOG_TO_CRTP, "Model Informations: \n");
  cpxPrintToConsole(LOG_TO_CRTP, "Input Size: %d B\n", AT_INPUT_WIDTH*AT_INPUT_HEIGHT);
  cpxPrintToConsole(LOG_TO_CRTP, "Output Size: %d B\n", NUM_CLASSES);
  cpxPrintToConsole(LOG_TO_CRTP, "N CORE: %d\n", gap_ncore());
  cpxPrintToConsole(LOG_TO_CRTP, "N STACK_SIZE: %d\n", STACK_SIZE);
  cpxPrintToConsole(LOG_TO_CRTP, "N SLAVE_STACK_SIZE: %d\n\n", SLAVE_STACK_SIZE);
  cpxPrintToConsole(LOG_TO_CRTP, "TXQ_SIZE: %d\n\n", TXQ_SIZE);
  cpxPrintToConsole(LOG_TO_CRTP, "RXQ_SIZE: %d\n\n", RXQ_SIZE);
  
  
  /* Semaphore to know when data frome Inference Server is available */
  data_sem = xSemaphoreCreateBinary();
  
  /* Semaphore to know when Image acquisition is done from camera */
  capture_sem = xSemaphoreCreateBinary();


  /* Setup wifi task */
  BaseType_t xTask;
  xTask = xTaskCreate(rx_wifi_task, "rx_wifi_task", configMINIMAL_STACK_SIZE,
                      NULL, tskIDLE_PRIORITY + 2, NULL);

  if (xTask != pdPASS) {
    cpxPrintToConsole(LOG_TO_CRTP, "RX wifi task did not start !\n");
    pmsis_exit(-1);
  }

  /* Setup Main task less priority than wifi */
  BaseType_t main_xtask;
  main_xtask = xTaskCreate(start_eco, "overload_task", configMINIMAL_STACK_SIZE * 2,
                      NULL, tskIDLE_PRIORITY + 1, NULL);

  if (main_xtask != pdPASS) {
    cpxPrintToConsole(LOG_TO_CRTP, "Main task did not start !\n");
    pmsis_exit(-1);
  }



  while (1) {
    pi_yield();
  }

  return 0;
}

int main(void)
{
  pi_bsp_init();
  return pmsis_kickoff((void *)e4a_collision);
}
