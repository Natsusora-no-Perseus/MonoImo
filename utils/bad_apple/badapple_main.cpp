#include "TermDrw.hpp"
#include "DispDum.hpp"
#include "JunkTimer.hpp"
#include <stdio.h>
#include <locale.h>
#include <cwchar>
#include <ctime>
#include <cmath>

#include "test_imgs.h"//Images for testing
#include "out_seq.h"

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

    //Initial test image draw
    /*
    for (int i = 0; i < 140; i++)
    {
        if (i % 27 == 0)
        {
            dummy_inst.set_pg_start(i/27);
        }
        dummy_inst.set_disp_ram(test_grph[i%27]);
    }
    */

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
        //Padding
        for (int i = 0;
                i < (std::cos(
                    ((float)disp_timer_val / (float)cont_frame_time / 100.0)
                    * (2 * 3.14)
                    ) + 1) * (50);
                i ++)//Varies from 0 to 100
        {
            //dummy_inst.set_disp_ram(0x00);
            //^ This is the original testing padding function



            //TEST: draw_pixel; result: success.
            /*
            dummy_inst.draw_pixel(i, 32);
            dummy_inst.draw_pixel(i, 34);
            dummy_inst.draw_pixel(i, 42);
            dummy_inst.draw_pixel(i, 64);//y-cood overflow test; success.
            */

            //TEST: draw_hline
            /*
            dummy_inst.draw_hline(0, 63, i+55);//x+len overflow test; pass.
            dummy_inst.draw_hline(130, 17, 30);//x-cood overflow test; pass.
            */

            //TEST: draw_vline
            /*
            dummy_inst.draw_vline(12, 9, i);//y-cood overflow test; pass.
            dummy_inst.draw_vline(2, 0, i/2);//Border case test; pass.
            */

            //TEST: draw_line (under experiment)
            /*
            dummy_inst.draw_line(2, 62, 126, 1);
            dummy_inst.draw_line(126, 62, 1, 1);
            dummy_inst.draw_line(4, 4, 126, 4);
            dummy_inst.draw_line(6, 4, 6, 62);
            */
            //dummy_inst.draw_box(15, 15, i/4, i/4, true);
            //dummy_inst.draw_line(50, 31, 100-i, 8);
            //dummy_inst.draw_line(50, 32, i, 60);
        }
        //Draw image
        /*
        for (int i = 0; i < 27; i++)
        {
            dummy_inst.set_disp_ram(test_grph[i]);
        }
        */
        //dummy_inst.draw_bmp(2, 9, CHESS_W, CHESS_H, chessboard_bits);
        /*
        dummy_inst.draw_xbmp(40+std::cos(
                    (((float)disp_timer_val / (float)cont_frame_time / 100.0)
                    * (2 * 3.14)
                    ) + 1) * (30)
        , 9, KITA_W, KITA_H, kita_bits);
        */
        dummy_inst.draw_xbmp(2, 2, BADAPPLE_W, BADAPPLE_H,
                badapple[(disp_timer_val/cont_frame_time)%FRAME_CNT]);

        //TEST: draw_box
        //dummy_inst.draw_box(2, 9, disp_timer_val/cont_frame_time,
        //      disp_timer_val/cont_frame_time/2, false);
        //TEST: draw_xbmp
        //dummy_inst.draw_xbmp(disp_timer_val/cont_frame_time/10,
        //      8, 27, 8, test_grph);
        //Restart if reached end time
        if (disp_timer_val > cont_frame_time * FRAME_CNT)
        {
            timer_inst.reset_timer(disp_draw_timer);
        }

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

