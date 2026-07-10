#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define COMMAND_BUFFER_SIZE 64
#define FIRMWARE_NAME "gameboy-printer"
#define FIRMWARE_VERSION "0.2.0"

static void set_led(bool on)
{
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
}

static void handle_command(const char *command)
{
    if (strcmp(command, "PING") == 0)
    {
        printf("PONG\r\n");
    }
    else if (strcmp(command, "INFO") == 0)
    {
        printf("INFO name=%s version=%s board=pico2_w\r\n",
               FIRMWARE_NAME, FIRMWARE_VERSION);
    }
    else if (strcmp(command, "LED ON") == 0)
    {
        set_led(true);
        printf("OK LED ON\r\n");
    }
    else if (strcmp(command, "LED OFF") == 0)
    {
        set_led(false);
        printf("OK LED OFF\r\n");
    }
    else
    {
        printf("ERROR unknown command\r\n");
    }
}

int main(void)
{
    stdio_init_all();

    if (cyw43_arch_init())
    {
        printf("Wi-Fi chip init failed\n");
        return -1;
    }

    set_led(false);
    printf("READY %s %s\r\n", FIRMWARE_NAME, FIRMWARE_VERSION);

    char command[COMMAND_BUFFER_SIZE];
    size_t command_length = 0;
    bool command_overflow = false;

    while (true)
    {
        int character = getchar_timeout_us(0);

        if (character == PICO_ERROR_TIMEOUT)
        {
            tight_loop_contents();
            continue;
        }

        if (character == '\r' || character == '\n')
        {
            if (command_overflow)
            {
                printf("ERROR command too long\r\n");
            }
            else if (command_length > 0)
            {
                command[command_length] = '\0';
                handle_command(command);
            }

            command_length = 0;
            command_overflow = false;
            continue;
        }

        if (command_overflow)
        {
            continue;
        }

        if (command_length < COMMAND_BUFFER_SIZE - 1)
        {
            command[command_length++] = (char)character;
        }
        else
        {
            command_overflow = true;
        }
    }
}
