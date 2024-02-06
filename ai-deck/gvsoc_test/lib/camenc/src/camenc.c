#include "camenc.h"
#include "bsp/bsp.h"
#include "pmsis.h"
#include "cpx.h"



int open_camera(struct pi_device *device) {

    struct pi_himax_conf cam_conf;

    pi_himax_conf_init(&cam_conf);

    cam_conf.format = PI_CAMERA_QVGA;

    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    return -1;

    pi_camera_control(device, PI_CAMERA_CMD_START, 0);
    uint8_t set_value = 3;
    uint8_t reg_value;
    pi_camera_reg_set(device, IMG_ORIENTATION, &set_value);
    pi_time_wait_us(1000000);
    pi_camera_reg_get(device, IMG_ORIENTATION, &reg_value);

    if (set_value != reg_value)
    {
        cpxPrintToConsole(LOG_TO_CRTP,"Failed to rotate camera image\n");
        return -1;
    }
                
    pi_camera_control(device, PI_CAMERA_CMD_STOP, 0);

    pi_camera_control(device, PI_CAMERA_CMD_AEG_INIT, 0);
    return 0;
}



void camera_start_capture(struct pi_device *device, unsigned char *buf, void (*handler)(void*)) {

  pi_camera_control(device, PI_CAMERA_CMD_STOP, 0);
  pi_camera_capture_async(device, buf, CAM_WIDTH * CAM_HEIGHT, pi_task_callback(&capture_task, handler, NULL));
  pi_camera_control(device, PI_CAMERA_CMD_START, 0);
}



void camera_stop_capture(struct pi_device *device) {

  pi_camera_control(device, PI_CAMERA_CMD_AEG_INIT, 0);
  pi_camera_control(device, PI_CAMERA_CMD_STOP, 0);
}



int setup_jpeg_enc(unsigned char **buf_src) {
  struct jpeg_encoder_conf enc_conf;
  jpeg_encoder_conf_init(&enc_conf);
  enc_conf.width = 200;
  enc_conf.height = 200;
  enc_conf.flags = 0; // Move this to the cluster

  if (jpeg_encoder_open(&jpeg_encoder, &enc_conf))
  {
    cpxPrintToConsole(LOG_TO_CRTP, "Failed initialize JPEG encoder\n");
    return -1;
  }

  pi_buffer_init(&jpeg_buffer, PI_BUFFER_TYPE_L2, *buf_src);
  pi_buffer_set_format(&jpeg_buffer, 200, 200, 1, PI_BUFFER_FORMAT_GRAY);

  header.size = 1024;
  header.data = pmsis_l2_malloc(1024);

  footer.size = 10;
  footer.data = pmsis_l2_malloc(10);

  // This must fit the full encoded JPEG
  jpeg_data.size = 1024 * 15;
  jpeg_data.data = pmsis_l2_malloc(1024 * 15);

  if (header.data == 0 || footer.data == 0 || jpeg_data.data == 0) {
    cpxPrintToConsole(LOG_TO_CRTP, "Could not allocate memory for JPEG image\n");
    return -1;
  }

  jpeg_encoder_header(&jpeg_encoder, &header, &headerSize);
  jpeg_encoder_footer(&jpeg_encoder, &footer, &footerSize);

  return 0;
}



void crop_img(unsigned char *src, unsigned char *dest, unsigned int nw, unsigned int nh) {

  for(unsigned i = 0; i < nw; i++) {
    for(unsigned j = 0; j < nh; j++) {
      dest[i * nw + j] = ((unsigned char)src[(i + (CAM_HEIGHT/2) - (nh/2)) * CAM_WIDTH + (j + (CAM_WIDTH/2) - (nw/2))]);
    }
  }
}