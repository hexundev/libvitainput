# libvitainput
Simple functions for handling input on the Vita

## Usage
- Include `vita_input.h` in your file.
- Call `vitainput_init` or `vitainput_init_advanced` first.
- Call `vitainput_update` in your main loop to get the latest input state.
- Use any of the other functions.
- Calling `vitainput_fini` is not required.

NOTE: Touch functions return screen coordinates (real touch values divided by 2)
