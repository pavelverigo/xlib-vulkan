#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "engine.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define WIDTH 600
#define HEIGHT 600

#define FPS_MEASURE_LEN 120

typedef struct FPSCounter {
    float data[FPS_MEASURE_LEN];
    float prev;
    int i;
} FPSCounter;

float fps_append_and_measure(FPSCounter *f, float m) {
    float sum_prev = f->prev - f->data[f->i];
    f->data[f->i] = m;
    f->i++;
    if (f->i == FPS_MEASURE_LEN) {
        f->i = 0;
    }
    float new_sum = sum_prev + m;
    f->prev = new_sum;
    return new_sum / FPS_MEASURE_LEN;
}

float diff_time_ms(struct timespec *t1) {
    struct timespec t2;
    clock_gettime(CLOCK_MONOTONIC, &t2);

    float time_spent = (t2.tv_sec - t1->tv_sec) * 1000.0 + 
                       (t2.tv_nsec - t1->tv_nsec) / 1000000.0;
    
    *t1 = t2;

    return time_spent;
}

int main() {
    // I want to see output before segmentation fault
    setbuf(stdout, NULL);

    Display *display = XOpenDisplay(NULL);

    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    Window root = DefaultRootWindow(display);

    // Create an X11 window
    XSetWindowAttributes attributes;
    attributes.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask
                            | PointerMotionMask | EnterWindowMask | LeaveWindowMask;

    Window window = XCreateWindow(display, root, 0, 0, WIDTH, HEIGHT, 1, CopyFromParent,
                                  InputOutput, CopyFromParent, CWEventMask, &attributes);

    XMapWindow(display, window);
    XStoreName(display, window, "Vulkan Window");

    Atom WM_PROTOCOLS = XInternAtom(display, "WM_PROTOCOLS", False);
    Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1);

    Engine engine;
    engine_init_xlib(&engine, WIDTH, HEIGHT, display, window);    

    struct timespec delta_timer, debug_timer;
    clock_gettime(CLOCK_MONOTONIC, &delta_timer);
    clock_gettime(CLOCK_MONOTONIC, &debug_timer); 

    // struct timespec sleep_t = {
    //     .tv_nsec = 1 * 1000000,
    // };

    FPSCounter counter = {0};

    int running = 1;

    int width = WIDTH;
    int height = HEIGHT;

    float min_cycle_ms = 500;
    float max_cycle_ms = 5000;

    float accum_cycle = 0;
    float cur_cycle = max_cycle_ms;

    int mouse_inside = 0;
    int mouse_x = 0;
    int mouse_y = 0;

    char window_title[32];

    while (running) {
        float delta_ms = diff_time_ms(&delta_timer);
        // float render_ms = diff_time_ms(&debug_timer);
        // printf("render_ms %.2f ms\n", render_ms);
        // printf("Time elapsed: %.2f ms\n", delta_ms);

        {
            XTextProperty title;
            float val = fps_append_and_measure(&counter, 1000.0f / delta_ms);
            snprintf(window_title, sizeof(window_title), "FPS: %.2f", (double) val);
            char *list[] = {window_title};
            XStringListToTextProperty(list, 1, &title);
            XSetWMName(display, window, &title);
            XFree(title.value);
        }

        // oval distance
        {
            float alpha;

            if (mouse_inside) {
                float dx = width / 2.0f - mouse_x;
                float dy = height / 2.0f - mouse_y;
                float maxdx = (width / 2.0f) * (width / 2.0f);
                float maxdy = (height / 2.0f) * (width / 2.0f);
                float diag = dx * dx / maxdx + dy * dy / maxdy;

                alpha = fmin(1.0f, diag);
            } else {
                alpha = 1.0f;
            }

            cur_cycle = min_cycle_ms + (max_cycle_ms - min_cycle_ms) * alpha;
        }

        accum_cycle += delta_ms / cur_cycle;
        if (accum_cycle > 1.0f) {
            accum_cycle -= 1.0f;
        }

        while (XPending(display) > 0) {
            XEvent event;
            XNextEvent(display, &event);

            if (event.type == Expose) {
                // TODO
            } else if (event.type == KeyPress) {
                running = 0;
                break;
            } else if (event.type == ConfigureNotify) {
                if (!(width == event.xconfigure.width &&
                     height == event.xconfigure.height)) {
                    width = event.xconfigure.width;
                    height = event.xconfigure.height;
                    engine_signal_resize(&engine, width, height);
                }
            } else if (event.type == ClientMessage) {
                if (event.xclient.message_type == WM_PROTOCOLS && (Atom)event.xclient.data.l[0] == WM_DELETE_WINDOW) {
                    running = 0;
                    break;
                }
            } else if (event.type == MotionNotify) {
                mouse_x = event.xmotion.x;
                mouse_y = event.xmotion.y;
            } else if (event.type == EnterNotify) {
                mouse_inside = 1;
            } else if (event.type == LeaveNotify) {
                mouse_inside = 0;
            }
        }

        if (!running) {
            break;
        }
        
        // float x_ms = diff_time_ms(&debug_timer);
        // printf("x_ms %.2f ms\n", x_ms);

        engine_draw(&engine, accum_cycle);

        // nanosleep(&sleep_t, NULL);
    }

    engine_deinit(&engine);

    // Clean up
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}