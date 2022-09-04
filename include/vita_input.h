#ifndef VITAINPUT_H
#define VITAINPUT_H

#include <psp2/types.h>
#include <psp2/touch.h>
#include <psp2/ctrl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Note from SDK: SCE_TOUCH_MAX_REPORT 8	//!< FIXME 6 on front | 4 on back
// 8 is struct array capacity
#define VI_TOUCH_FRONT_MAX_REPORT 6
#define VI_TOUCH_BACK_MAX_REPORT 4

typedef enum vitainput_button
{
    VI_NONE,
    VI_TRIANGLE,
    VI_CIRCLE,
    VI_CROSS,
    VI_SQUARE,

    VI_UP,
    VI_RIGHT,
    VI_DOWN,
    VI_LEFT,

    VI_START,
    VI_SELECT,

    VI_L,
    VI_R,

    VI_BUTTONS_END,

    // Treat touch press as a button
    VI_TOUCH_FRONT_PRESS,
    VI_TOUCH_BACK_PRESS,

    VI_BTN_MAX,
}
vitainput_button;

typedef enum vitainput_analog
{
    VI_LS,
    VI_RS,
    VI_ANALOG_MAX
} 
vitainput_analog;

typedef enum vitainput_touch_port
{
    VI_TOUCH_PORT_FRONT = SCE_TOUCH_PORT_FRONT,
    VI_TOUCH_PORT_BACK = SCE_TOUCH_PORT_BACK,
    VI_TOUCH_PORT_MAX = SCE_TOUCH_PORT_MAX_NUM,
}
vitainput_touch_port;

int vitainput_init(); // Default is analog and front touch enabled, back disabled
int vitainput_init_advanced(int enableAnalogSampling, int enableFrontPort, int enableBackPort);
int vitainput_fini();

void vitainput_update();
void vitainput_clear();

int vitainput_btn_pressed(vitainput_button button);
int vitainput_btn_down(vitainput_button button);
int vitainput_btn_released(vitainput_button button);
int vitainput_btn_get_hold_duration(vitainput_button button);

int vitainput_dpad_get_float(float* x, float* y); // values: -1 to 1, neutral is ~0

int vitainput_analog_get(vitainput_analog stick, int* x, int* y); // values: 0-255, neutral is ~127
int vitainput_analog_get_float(vitainput_analog stick, float* x, float* y); // values: -1 to 1, neutral is ~0

int vitainput_touch_get_report_count(vitainput_touch_port port); // call this before touch get
int vitainput_touch_get_report(vitainput_touch_port, int touchReportIndex, int* x, int* y); // returns 1 if there was a touch, (x,y) are in (960,544)
int vitainput_touch_front_get_first(int* x, int* y); // returns 1 if there was any touch in the front, (x,y) are in (960,544)
int vitainput_touch_back_get_first(int* x, int* y); // returns 1 if there was any touch in the front, (x,y) are in (960,544)

#ifdef __cplusplus
}
#endif

#endif
