#ifndef __E4A_CAMERA_ENCODER_H__
#define __E4A_CAMERA_ENCODER_H__

#include "bsp/camera/himax.h"
#include "gaplib/ImgIO.h"
#include "gaplib/jpeg_encoder.h"

#define CAM_WIDTH 324
#define CAM_HEIGHT 244
#define CHANNELS 1
#define IMG_ORIENTATION 0x0101

typedef enum
{
  RAW_ENCODING = 0,
  JPEG_ENCODING = 1
} __attribute__((packed)) StreamerMode_t;

static StreamerMode_t streamerMode = RAW_ENCODING;

pi_task_t capture_task;
jpeg_encoder_t jpeg_encoder;
pi_buffer_t jpeg_buffer;
pi_buffer_t jpeg_data;
pi_buffer_t header;
pi_buffer_t footer;
uint32_t headerSize;
uint32_t footerSize;

int open_camera(struct pi_device *device);
void camera_start_capture(struct pi_device *device, unsigned char *buf, void (*handler)(void*));
void camera_stop_capture(struct pi_device *device);

int setup_jpeg_enc(unsigned char **buf_src);

void crop_img(unsigned char *src, unsigned char *dest, unsigned int nw, unsigned int nh);

#endif