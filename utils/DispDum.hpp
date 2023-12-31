#pragma once

#include <stdlib.h>
#include <cstdint>

class DispDum
{
    public:
        //Constructor:
        DispDum(void);

        //Properties
        std::uint8_t get_disp_x(void);
        std::uint8_t get_disp_y(void);

        //SSD1306 Commands:
        void togl_disp(bool disp_pwr);//Disp ON/OFF; 0xAE/0xAF
        void set_disp_src(bool disp_src);//From RAM/Ignore; 0xA4/0xA5
        void set_disp_mode(bool disp_mode);//Normal/Inverse; 0xA6/0xA7

        void set_lo_col(std::uint8_t pg_lo_col);//0x00~0x0F
        void set_hi_col(std::uint8_t pg_hi_col);//0x10~0x1F THIS IS FAULTY! DON'T USE!
        void set_pg_start(std::uint8_t start_pg);//0xB0~0xB7

        void set_disp_ram(std::uint8_t dat_byte);//Write to RAM

        //Facilitating TermDrw:
        std::uint16_t get_braille_int(std::uint8_t x, std::uint8_t y);

        //DEBUG:
        std::uint8_t debug_get_pixel(std::uint8_t x, uint8_t y);

        //Draw pixel modes
        enum DrwPixModes {pix_or=0, pix_and=1, pix_xor=2};

        //Drawing functions:
        void draw_pixel(uint8_t x, uint8_t y);
        //Line-drawing functions:
        void draw_hline(uint8_t x, uint8_t y, uint8_t len);
        void draw_vline(uint8_t x, uint8_t y, uint8_t len);
        void draw_line(uint8_t x_i, uint8_t y_i, uint8_t x_f, uint8_t y_f);

        //Shape-drawing functions:
        void draw_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool filled=false);
        //Add draw_circ, draw_arc in the future

        //Bitmap-drawing functions:
        void draw_bmp(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *xbmp);
        void draw_xbmp(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *xbmp);


        //Mode-setting functions:
        void set_draw_mode(DispDum::DrwPixModes mode);


    private:
        static const uint8_t _disp_x = 128;
        static const uint8_t _disp_y = 64;

        uint8_t _disp_ram[(_disp_y/8)*_disp_x] = {0};//Init display GDDRAM

        bool _disp_pwr, _disp_src, _disp_mode;

        std::uint8_t _pg_lo_col = 0x00, _pg_hi_col = 0x80, _start_pg = 0x00;
        std::uint8_t *_cur_pg_p = nullptr;
        std::uint8_t _cur_col;
        std::uint8_t _flip_bits(std::uint8_t num);

        DispDum::DrwPixModes _cur_drw_mode = pix_or;//Current drawing pixel mode

        void _nochk_draw_pixel(uint8_t x, uint8_t y);//Drawpixel, w/o boundary checking
        void _nochk_draw_hline(uint8_t x, uint8_t y, uint8_t len);

        int (*_pixdrw_arr[3])(uint8_t a, uint8_t b) =//Pixel drawing modes
        {
            [](uint8_t a, uint8_t b){return a | b;},//pix_or
            [](uint8_t a, uint8_t b){return a & b;},//pix_and
            [](uint8_t a, uint8_t b){return a ^ b;} //pix_xor
        };
};
