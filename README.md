# libvitainput
Simple functions for handling input on the Vita

## Usage

- Call `vitainput_init` or `vitainput_init_advanced` first.
- Call `vitainput_update` in your main loop to get the latest input state.
- Use any of the other functions.
- Calling `vitainput_fini` is not required.
