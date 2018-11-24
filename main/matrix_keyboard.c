#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "matrix_keyboard.h"
#include "keylayouts.h"

static char tag[] = "keyboard";

const char keyboard_characters[4][4] = {
    { '1', '2', '3', 'A'},
    { '4', '5', '6', 'B'},
    { '7', '8', '9', 'C'},
    { '*', '0', '#', 'D'}
};

const uint8_t keyboard_input_pins[] = {
    GPIO_NUM_26,
    GPIO_NUM_25,
    GPIO_NUM_33,
    GPIO_NUM_32
};

const uint8_t keyboard_output_pins[] = {
    GPIO_NUM_13,
    GPIO_NUM_12,
    GPIO_NUM_14,
    GPIO_NUM_27
};

void keyboard_setup(){
    for (uint8_t index = 0; index < 4; index++) 
    {
        gpio_pad_select_gpio(keyboard_output_pins[index]);
        gpio_set_direction(keyboard_output_pins[index], GPIO_MODE_OUTPUT);
    }
    for (uint8_t index = 0; index < 4; index++)
    {
        gpio_pad_select_gpio(keyboard_input_pins[index]);
        gpio_set_direction(keyboard_input_pins[index], GPIO_MODE_INPUT);
        gpio_set_pull_mode(keyboard_input_pins[index], GPIO_PULLDOWN_ONLY);
    }
}

char keyboard_read(){
    uint8_t output = 0, input = 0;
    for(output = 0; output < 4; output++)
    {
        gpio_set_level(keyboard_output_pins[output], 1);
        for(input = 0; input < 4; input++){
            if(gpio_get_level(keyboard_input_pins[input]) == 1)
            {
                int start = xTaskGetTickCount()*portTICK_RATE_MS;
                while(gpio_get_level(keyboard_input_pins[input]) == 1)
                {
                    if(((xTaskGetTickCount()*portTICK_RATE_MS)-start)>300)  break;
                }
                // vTaskDelay(200/portTICK_RATE_MS);
                gpio_set_level(keyboard_output_pins[output], 0);
                return (char)keyboard_characters[output][input];
            }
        }
        gpio_set_level(keyboard_output_pins[output], 0);
    }
    return 0;
}

void keyboard_event(void *pvParameter)
{
    // QueueHandle_t stringParam = (QueueHandle_t)pvParameter;
    keyboard_setup();
    struct cmdBuf cmdBuffer;
    cmdBuffer.state=CMDSTATE_IDLE;

    while (1) 
    {
        char aux = keyboard_read();

        if(aux != 0)
        {
            ESP_LOGI(tag, "Character pressed: %c", aux);
            if(aux == 'A' || aux == 'D') // 'A' de "Apaga" ou 'D' de "Delete"
            {
                uart_parse_command(127, &cmdBuffer); // 127 BACKSPACE
            }
            else if(aux == 'B') // 'B' será TAB
            {
                uart_parse_command(0x0B, &cmdBuffer); // 0x0B VERTICAL TAB
            }
            else if(aux == 'C') // 'C' será para "Confirmar", ou seja, ENTER
            {
                uart_parse_command('\n', &cmdBuffer);
            }
            else
            {
                uart_parse_command(aux, &cmdBuffer);
            }
        }
        vTaskDelay(10);
    }
}
