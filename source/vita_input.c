#include "vita_input.h"

#include <string.h> // memset

typedef struct _vitainput_button_state
{
	int wasPressed;
	int pressed;
	int holdDuration;
} _vitainput_button_state;

static SceCtrlButtons get_sce_button(vitainput_button button)
{
	switch (button)
	{
	case VI_TRIANGLE:
		return SCE_CTRL_TRIANGLE;
	case  VI_CIRCLE:
		return SCE_CTRL_CIRCLE;
	case VI_CROSS:
		return SCE_CTRL_CROSS;
	case VI_SQUARE:
		return SCE_CTRL_SQUARE;

	case  VI_UP:
		return SCE_CTRL_UP;
	case VI_RIGHT:
		return SCE_CTRL_RIGHT;
	case VI_DOWN:
		return SCE_CTRL_DOWN;
	case VI_LEFT:
		return SCE_CTRL_LEFT;

	case VI_START:
		return SCE_CTRL_START;
	case  VI_SELECT:
		return SCE_CTRL_SELECT;

	case VI_L:
		return SCE_CTRL_LTRIGGER;
	case VI_R:
		return SCE_CTRL_RTRIGGER;

	default:
		break;
	}

	return SCE_CTRL_CROSS;
}

static void update_button_state(_vitainput_button_state* st, int pressed)
{
	st->wasPressed = st->pressed;
	st->pressed = pressed;
	if (pressed)
	{
		st->holdDuration++;
	}
	else
	{
		st->holdDuration = 0;
	}
}

static int get_max_report_num(vitainput_touch_port port)
{
	switch (port)
	{
	case VI_TOUCH_PORT_FRONT:
		return VI_TOUCH_FRONT_MAX_REPORT;

	case VI_TOUCH_PORT_BACK:
		return VI_TOUCH_BACK_MAX_REPORT;

	default:
		break;
	}

	return 0;
}

static void set_touch_port_state(SceUInt32 port, int enable, int enableForce)
{
	sceTouchSetSamplingState(port, enable ? SCE_TOUCH_SAMPLING_STATE_START : SCE_TOUCH_SAMPLING_STATE_STOP);

	if (enableForce)
	{
		sceTouchEnableTouchForce(port);
	}
	else
	{
		sceTouchDisableTouchForce(port);
	}
}

static float normalize_stick(int value)
{
	return (((float)value) - 127.0f) / 128.0f;
}

static int initialized = 0;
static SceCtrlData controlData;
static SceTouchData touchData[SCE_TOUCH_PORT_MAX_NUM];
static _vitainput_button_state buttonStates[VI_BTN_MAX];

/* Public functions */

int vitainput_init()
{
	return vitainput_init_advanced(1, 1, 0);
}
int vitainput_init_advanced(int enableAnalogSampling, int enableFrontPort, int enableBackPort)
{
	if (initialized)
	{
		return 0;
	}

	vitainput_clear();

	sceCtrlSetSamplingMode(enableAnalogSampling ? SCE_CTRL_MODE_ANALOG : SCE_CTRL_MODE_DIGITAL);
	set_touch_port_state(SCE_TOUCH_PORT_FRONT, enableFrontPort, 0);
	set_touch_port_state(SCE_TOUCH_PORT_BACK, enableBackPort, 0);

	initialized = 1;
	return 1;
}

int vitainput_fini()
{
	if (!initialized)
	{
		return 0;
	}

	vitainput_clear();

	initialized = 0;
	return 1;
}


void vitainput_update()
{
	// Read state
	sceCtrlPeekBufferPositive(0, &controlData, 1);

	// Update buttons
	for (int i = 0; i < VI_BUTTONS_END; ++i)
	{
		int pressed = controlData.buttons & get_sce_button((vitainput_button)i);
		int index = (vitainput_button)i;
		update_button_state(&buttonStates[index], pressed);
	}

	// Update touch
	for (int i = 0; i < VI_TOUCH_PORT_MAX; ++i)
	{
		// Read state
		sceTouchPeek(i, &touchData[i], 1);

		int pressed = vitainput_touch_get_report_count((vitainput_touch_port)i) > 0;
		int index = (vitainput_button)(VI_TOUCH_FRONT_PRESS + i);
		update_button_state(&buttonStates[index], pressed);
	}
}

void vitainput_clear()
{
	memset(&controlData, 0, sizeof(SceCtrlData));
	memset(&touchData, 0, sizeof(touchData));
	memset(&buttonStates, 0, sizeof(buttonStates));
}

/* Button functions */

int vitainput_btn_pressed(vitainput_button button)
{
	if (button == VI_NONE || button >= VI_BTN_MAX)
	{
		return 0;
	}

	return buttonStates[(int)button].pressed && !buttonStates[(int)button].wasPressed;
}

int vitainput_btn_down(vitainput_button button)
{
	if (button == VI_NONE || button >= VI_BTN_MAX)
	{
		return 0;
	}

	return buttonStates[(int)button].pressed;
}

int vitainput_btn_released(vitainput_button button)
{
	if (button == VI_NONE || button >= VI_BTN_MAX)
	{
		return 0;
	}

	return !buttonStates[(int)button].pressed && buttonStates[(int)button].wasPressed;
}

int vitainput_btn_get_hold_duration(vitainput_button button)
{
	if (button == VI_NONE || button >= VI_BTN_MAX)
	{
		return 0;
	}

	return buttonStates[(int)button].holdDuration;
}

#define btn_down(b) ((controlData.buttons & b)?1:0)

int vitainput_dpad_get_float(float* x, float* y)
{
	if (x == NULL || y == NULL)
	{
		return 0;
	}

	*x = (float)(btn_down(SCE_CTRL_RIGHT) - btn_down(SCE_CTRL_LEFT));
	*y = (float)(btn_down(SCE_CTRL_DOWN) - btn_down(SCE_CTRL_UP));

	return 1;
}

/* Analog stick functions */

int vitainput_analog_get(vitainput_analog stick, int* x, int* y)
{
	switch (stick)
	{
	case VI_LS:
		*x = (int)controlData.lx;
		*y = (int)controlData.ly;
		return 1;

	case VI_RS:
		*x = (int)controlData.rx;
		*y = (int)controlData.ry;
		return 1;

	default:
		break;
	}

	*x = 0;
	*y = 0;
	return 0;
}

int vitainput_analog_get_float(vitainput_analog stick, float* x, float* y)
{
	switch (stick)
	{
	case VI_LS:
		*x = normalize_stick((int)controlData.lx);
		*y = normalize_stick((int)controlData.ly);
		return 1;

	case VI_RS:
		*x = normalize_stick((int)controlData.rx);
		*y = normalize_stick((int)controlData.ry);
		return 1;

	default:
		break;
	}

	*x = 0.0f;
	*y = 0.0f;
	return 0;
}

/* Touch functions */

int vitainput_touch_get_report_count(vitainput_touch_port port)
{
	if (port >= VI_TOUCH_PORT_MAX)
	{
		return 0;
	}

	return touchData[port].reportNum;
}

int vitainput_touch_get_report(vitainput_touch_port port, int touchReportIndex, int* x, int* y)
{
	if (port >= VI_TOUCH_PORT_MAX ||
		touchReportIndex >= get_max_report_num(port) ||
		touchData[port].reportNum == 0)
	{
		*x = 0;
		*y = 0;
		return 0;
	}

	// Divided by 2 to get screen coordinates
	*x = touchData[port].report[touchReportIndex].x / 2;
	*y = touchData[port].report[touchReportIndex].y / 2;

	return 1;
}

int vitainput_touch_front_get_first(int* x, int* y)
{
	return vitainput_touch_get_report(VI_TOUCH_PORT_FRONT, 0, x, y);
}

int vitainput_touch_back_get_first(int* x, int* y)
{
	return vitainput_touch_get_report(VI_TOUCH_PORT_BACK, 0, x, y);
}