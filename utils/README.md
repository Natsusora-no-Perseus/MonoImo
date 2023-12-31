# Development/Testing Utilities
Files here are created for development or testing utilities.
Please refer to the following for the purpose of them.
## Main Function
- See `plugplay.cpp`
- Designed to test the modules in testing utilities.
## TUI Simulator
- Consists of: `TermDrw.hpp`, `TermDrw.cpp`
- Utilizes the `ncurses` library to create a TUI simulating a screen output, as well as the driving methods for real displays.
    - Currently only simulates a SSD1306 chip driving a 128*64 OLED display, with only page addressing mode.
## Display Dummy Driver
- Consists of: `DispDum.hpp`, `DispDum.cpp`
- An implementation of higher-level drawing functions, resembling those used on a real display.
## Junk Timer
- Consists of: `JunkTimer.hpp`, `JunkTimer.cpp`
- A very janky simulation of timer calls on MCUs. Used for refreshing the display and animations.

--------

### Additional Elements
- `./bad_apple` is a small demo utilizing the dummy driver's xbmp-drawing capabilities to draw a *Bad Apple!* video onto the TUI simulator.
    - This directory contains some scripts used for creating this demo.
    - `create_sequence.sh` generates a sequence of `.xbm` frames.
    - `combine.py` merges these frames into a `.h` file that contains these frames and can be used by `DispDum::draw_xbmp` to draw the frame sequence.
    - `badapple_main.cpp` is a version of the `plugplay.cpp` main function used to create this demo.
