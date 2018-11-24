#ifndef _MATRIX_KEYBOARD_H_
#define _MATRIX_KEYBOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MAX_CMDLEN  100
#define CMDSTATE_IDLE 0

struct cmdBuf {
    int state;
	int expectedBytes;
	int bufferLength;
	uint8_t buf[MAX_CMDLEN];
} ;

void keyboard_setup();

char keyboard_read();

void keyboard_event(void *pvParameter);

extern void uart_parse_command (uint8_t character, struct cmdBuf * cmdBuffer);

#ifdef __cplusplus
}
#endif

#endif