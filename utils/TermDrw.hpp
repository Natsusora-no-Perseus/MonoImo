#pragma once

#include <cstdint>
#include <ncurses.h>
#include <cwchar>
#include "DispDum.hpp"
#include <chrono>

class TermDrw
{
    public:

        //Constructor function:
        TermDrw (DispDum &dummy_inst);

        //Rendering:
        void termdisp_refresh(void);
        void main_refresh(void);//Redraws non-static elems

        //Utilities:
        void side_refresh(void);//Redraws bounding box etc. (static elems)
        void termdisp_end(void);


    private:
        DispDum& _dummy_inst;//Instance of DispDum passed as reference

        void _initial_draw(void);
        int _get_elapsed_ms(void);
};
