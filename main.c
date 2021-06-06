// Copyright 2021 Jonne Kokkonen
// Released under the MIT licence, https://opensource.org/licenses/MIT

#include <SDL2/SDL.h>
#include <libserialport.h>
#include <signal.h>
#include <unistd.h>

#include "command.h"
#include "input.h"
#include "render.h"
#include "serial.h"
#include "slip.h"
#include "write.h"

// maximum amount of bytes to read from the serial in one read()
#define serial_read_size 1024

uint8_t run = 1;

// Handles CTRL+C / SIGINT
void intHandler(int dummy) { run = 0; }

int main(int argc, char *argv[]) {
    char l_s[]="SDL_NOMOUSE=1";
    putenv(l_s);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("could not initialize sdl2: %s\n", SDL_GetError());
        return -1;
    }

    if (SDL_NumJoysticks() > 0) {
        // Open joystick
        SDL_Joystick *joy = SDL_JoystickOpen(0);
        if (joy) {
            SDL_Log("Opened Joystick 0\n");
            SDL_Log("Name: %s\n", SDL_JoystickNameForIndex(0));
            SDL_Log("Number of Axes: %d\n", SDL_JoystickNumAxes(joy));
            SDL_Log("Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
            SDL_Log("Number of Balls: %d\n", SDL_JoystickNumBalls(joy));
        } else {
            SDL_Log("Couldn't open Joystick 0\n");
        }
    }

    // allocate memory for serial buffer
    uint8_t *serial_buf = malloc(serial_read_size);

    static uint8_t slip_buffer[serial_read_size]; // SLIP command buffer

    // settings for the slip packet handler
    static const slip_descriptor_s slip_descriptor = {
        .buf = slip_buffer,
        .buf_size = sizeof(slip_buffer),
        .recv_message = process_command, // the function where complete slip
        // packets are processed further
    };

    static slip_handler_s slip;

    signal(SIGINT, intHandler);
    signal(SIGTERM, intHandler);

    slip_init(&slip, &slip_descriptor);

    struct sp_port *port;

    port = init_serial();
    if (port == NULL)
        return -1;

    if (enable_and_reset_display(port) == -1)
        run = 0;

    if (initialize_sdl() == -1)
        run = 0;

    int ci = initialize_game_controllers();
    SDL_Log("ci: %d\n", ci);

    uint8_t prev_input = 0;

    // main loop
    while (run) {

        // get current inputs
        input_msg_s input = get_input_msg();

        switch (input.type) {
            case normal:
                if (input.value != prev_input) {
                    prev_input = input.value;
                    send_msg_controller(port, input.value);
                }
                break;
            case keyjazz:
                if (input.value != prev_input) {
                    prev_input = input.value;
                    if (input.value != 0) {
                        send_msg_keyjazz(port, input.value, 0xFF);
                    } else {
                        send_msg_keyjazz(port, 0, 0);
                    }
                }
                break;
            case special:
                switch (input.value) {
                    case msg_quit:
                        run = 0;
                        break;
                }
                break;
        }    

        // read serial port
        size_t bytes_read = sp_nonblocking_read(port, serial_buf, serial_read_size);
        if (bytes_read < 0) {
            SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Error %d reading serial. \n",
                    (int)bytes_read);
            run = 0;
        }
        if (bytes_read > 0) {
            for (int i = 0; i < bytes_read; i++) {
                uint8_t rx = serial_buf[i];
                // process the incoming bytes into commands and draw them
                int n = slip_read_byte(&slip, rx);
                if (n != SLIP_NO_ERROR) {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SLIP error %d\n", n);
                }
            }
            usleep(10);
        } else {
            render_screen();
            usleep(100);
        }
    }

    // exit, clean up
    SDL_Log("Shutting down\n");
    close_game_controllers();
    close_renderer();
    disconnect(port);
    sp_close(port);
    sp_free_port(port);
    free(serial_buf);

    return 0;
}
