#ifndef __E4A_WIFI_H__
#define __E4A_WIFI_H__

#include <stdint.h>
#include "cpx.h"

typedef enum {
  WIFI_CTRL_SET_SSID                = 0x10,
  WIFI_CTRL_SET_KEY                 = 0x11,

  WIFI_CTRL_WIFI_CONNECT            = 0x20,

  WIFI_CTRL_STATUS_WIFI_CONNECTED   = 0x31,
  WIFI_CTRL_STATUS_CLIENT_CONNECTED = 0x32,

  WIFI_CTRL_DATA_TRANSFER           = 0x33,
} __attribute__((packed)) WiFiCTRLType;

typedef struct {
  WiFiCTRLType cmd;
  uint8_t data[50];
} __attribute__((packed)) WiFiCTRLPacket_t;


void sendBufferViaCPXBlocking(CPXPacket_t * packet, uint8_t * buffer, uint32_t bufferSize) {
  uint32_t offset = 0;
  uint32_t size = 0;
  // cpxPrintToConsole(LOG_TO_CRTP, "------- sendBufferViaCPXBlocking Begin\n");
  do {
    size = sizeof(packet->data);
    if (offset + size > bufferSize)
    {
      size = bufferSize - offset;
    }
    memcpy(packet->data, &buffer[offset], sizeof(packet->data));
    packet->dataLength = size;
    cpxSendPacketBlocking(packet);
    // cpxPrintToConsole(LOG_TO_CRTP, "sendBufferViaCPXBlocking Sent %d/%d\n", (offset + size), bufferSize);
    offset += size;
  } while (size == sizeof(packet->data));
  // cpxPrintToConsole(LOG_TO_CRTP, "------- sendBufferViaCPXBlocking End\n");
}

int sendBufferViaCPX(CPXPacket_t * packet, uint8_t * buffer, uint32_t bufferSize) {
  uint32_t offset = 0;
  uint32_t size = 0;
  do {
    size = sizeof(packet->data);
    if (offset + size > bufferSize)
    {
      size = bufferSize - offset;
    }
    memcpy(packet->data, &buffer[offset], sizeof(packet->data));
    packet->dataLength = size;
    // cpxSendPacketBlocking(packet);
    if(cpxSendPacket(packet, 2000) != pdTRUE) {

      // cpxPrintToConsole(LOG_TO_CRTP, "cpxSendPacket TIMEOUT\n");
      return pdFALSE;
    }
    // cpxPrintToConsole(LOG_TO_CRTP, "cpxSendPacket send %d\n", offset);
    offset += size;
  } while (size == sizeof(packet->data));

  return pdTRUE;
}

#endif