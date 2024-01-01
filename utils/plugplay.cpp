#include "TermDrw.hpp"
#include "DispDum.hpp"
#include "JunkTimer.hpp"
#include <stdio.h>
#include <locale.h>
#include <cwchar>
#include <ctime>
#include <cmath>

#include "test_imgs.h"//Images for testing

#define FRAME_TIME 20

//Use gcc -o plug plugplay.cpp TermDrw.cpp JunkTimer.cpp DispDum.cpp -lncursesw -lstdc++ -lm to compile

int main(void)
{
    DispDum dummy_inst;
    TermDrw term_inst(dummy_inst);
    JunkTimer timer_inst;

    //Create timers
    int scrn_refs_timer = timer_inst.create_timer();
    int disp_draw_timer = timer_inst.create_timer();
    //Start timers
    timer_inst.toggle_timer(scrn_refs_timer, false);
    timer_inst.toggle_timer(disp_draw_timer, false);
    //Value holders
    unsigned long int scrn_timer_val = 0, disp_timer_val = 0;

    setlocale(LC_ALL, "");
    dummy_inst.set_disp_ram(0b10100100);
    const std::uint8_t test_grph[] =
    {0x00, 0x7c, 0x44, 0x44, 0x38, 0x00,
     0x3c, 0x50, 0x50, 0x3c, 0x00,
     0x7c, 0x50, 0x2c, 0x00,
     0x7c, 0x10, 0x28, 0x44, 0x00,
     0x0c, 0x12, 0x12, 0x5c, 0x60, 0x70, 0x00};

    term_inst.termdisp_end();
    dummy_inst.set_disp_mode(false);//Invert
    term_inst.side_refresh();

    wchar_t ch;//Buffer

    //loop()
    for (;;)
    {
        ch = getch();

        //Screen refresh
        //Read timer values
        if (!timer_inst.get_timer_val(scrn_refs_timer, scrn_timer_val))
        {
            printf("Scrn timer err\n");
            term_inst.termdisp_end();
            return 1;
        }
        if (!timer_inst.get_timer_val(disp_draw_timer, disp_timer_val))
        {
            printf("Disp timer error\n");
            term_inst.termdisp_end();
            return 1;
        }

        //Drawing routine
        //Flush display RAM
        const int disp_frame_time = 20, cont_frame_time = 40;

        for (int i = 0; i < 8; i++)
        {
            dummy_inst.set_pg_start(i);
            for (int j = 0; j < 128; j++)
            {
                dummy_inst.set_disp_ram(0x00);
            }
        }
        dummy_inst.set_pg_start(0);
        //Animation frames:
        if (disp_timer_val > cont_frame_time * 100)
        //^ Change "100" to frames in one iteration
        {
            timer_inst.reset_timer(disp_draw_timer);
        }

        //Simulated display refresh:
        if (scrn_timer_val > disp_frame_time)
        {
            term_inst.main_refresh();
            timer_inst.reset_timer(scrn_refs_timer);
        }

        timer_inst.update_timers();
        // ======= End of display functions =======


        if (ch == 'q')
        {
            term_inst.termdisp_end();
            break;
        }
        else if (ch == 'r')
        {
            term_inst.termdisp_refresh();
            printf("scrn: %ld, disp: %ld\n", scrn_timer_val, disp_timer_val);
        }
    }

    return 0;
}

