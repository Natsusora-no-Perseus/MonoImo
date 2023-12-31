#include "DispDum.hpp"
#include <cstdint>

DispDum::DispDum(void)
{
    DispDum::_disp_pwr = true;//Display ON
    DispDum::_disp_src = true;//From RAM
    DispDum::_disp_mode = true;//Normal

    DispDum::_cur_col = DispDum::_pg_lo_col;

    DispDum::_cur_pg_p = &DispDum::_disp_ram[DispDum::_start_pg*_disp_x + DispDum::_pg_lo_col];
}

std::uint8_t DispDum::get_disp_x(void)
{
    return DispDum::_disp_x;
}

std::uint8_t DispDum::get_disp_y(void)
{
    return DispDum::_disp_y;
}

void DispDum::togl_disp(bool disp_pwr)
{
    DispDum::_disp_pwr = disp_pwr;
}

void DispDum::set_disp_src(bool disp_src)
{
    DispDum::_disp_src = disp_src;
}

void DispDum::set_disp_mode(bool disp_mode)
{
    DispDum::_disp_mode = disp_mode;
}

void DispDum::set_lo_col(std::uint8_t pg_lo_col)
{
    DispDum::_pg_lo_col = pg_lo_col;
}

void DispDum::set_hi_col(std::uint8_t pg_hi_col)
{
    DispDum::_pg_hi_col = pg_hi_col;
}

void DispDum::set_pg_start(std::uint8_t start_pg)
{
    DispDum::_start_pg = start_pg;
    //Will also reset column pointer
    DispDum::_cur_col = DispDum::_pg_lo_col;
    DispDum::_cur_pg_p = &DispDum::_disp_ram[DispDum::_start_pg * DispDum::_disp_x + DispDum::_pg_lo_col];
}

void DispDum::set_disp_ram(std::uint8_t dat_byte)
{
    if (DispDum::_cur_col >= DispDum::_pg_hi_col)//Last write in a row
    {
        *DispDum::_cur_pg_p = dat_byte;
        DispDum::_cur_pg_p = &DispDum::_disp_ram[DispDum::_start_pg * DispDum::_disp_x + DispDum::_pg_lo_col];//Go back to first col
        DispDum::_cur_col = DispDum::_pg_lo_col;
    }
    else
    {
        *DispDum::_cur_pg_p = dat_byte;
        DispDum::_cur_pg_p ++;//Increment col
        DispDum::_cur_col ++;
    }
}

std::uint8_t DispDum::_flip_bits(std::uint8_t num)
{
    num = (num & 0xF0) >> 4 | (num & 0x0F) << 4;
    num = (num & 0xCC) >> 2 | (num & 0x33) << 2;
    num = (num & 0xAA) >> 1 | (num & 0x55) << 1;
    return num;
}

std::uint16_t DispDum::get_braille_int(std::uint8_t x, std::uint8_t y)
{
    if (!DispDum::_disp_pwr)
    {return 0x2800;}//If screen OFF, display nothing
    else if (!DispDum::_disp_src)
    {return 0x28FF;}//If ignoring RAM, all pixels on

    uint8_t brai_buf = 0;//Initialize buffer
    //The two bytes in GDDRAM corresponding to the character
    //GDDRAM has 128 cols and 8 rows (for SSD1306)
    uint8_t datbyte1 = DispDum::_disp_ram[(y/2)*DispDum::_disp_x+(x*2)];
    uint8_t datbyte2 = DispDum::_disp_ram[(y/2)*DispDum::_disp_x+(x*2+1)];

    if (y % 2 == 0)//Use upper half of a page
    {
        brai_buf = (datbyte1 & 0b11110000) | ((datbyte2 & 0b11110000) >> 4);
    }
    else//Use lower half
    {
        brai_buf = ((datbyte1 & 0b00001111) << 4) | (datbyte2 & 0b00001111);
    }

    if (!DispDum::_disp_mode)//Invert it
    {
        brai_buf = ~brai_buf;
    }

    //Convert to unicode:
    uint16_t ret = 0;
    //[1][4]
    //[2][5]
    //[3][6]
    //[7][8]
    ret += brai_buf & 0b11100001;
    ret += (brai_buf & 0b00001110) << 1;
    ret += (brai_buf & 0b00010000) >> 3;
    ret = DispDum::_flip_bits(ret);

    return ret+0x2800;
}

std::uint8_t DispDum::debug_get_pixel(std::uint8_t x, uint8_t y)
{
    return DispDum::_disp_ram[(DispDum::_disp_x * y) + x];
}

void DispDum::_nochk_draw_pixel(uint8_t x, uint8_t y)
{
    uint8_t *target_blk = &DispDum::_disp_ram[(y/8) * DispDum::_disp_x + x];
    *target_blk = _pixdrw_arr[DispDum::_cur_drw_mode](*target_blk, (0x80 >> (y%8)));
}

//Draws a pixel at (x, y); note: 0<=x<=127, 0<=y<=63
void DispDum::draw_pixel(uint8_t x, uint8_t y)
{
    if (x >= DispDum::_disp_x || y >= DispDum::_disp_y)
    {return;}

    DispDum::_nochk_draw_pixel(x, y);
}

//Draws a horizontal line from (x, y) with length len
void DispDum::_nochk_draw_hline(uint8_t x, uint8_t y, uint8_t len)
{
    uint8_t *start_blk = &DispDum::_disp_ram[(y/8) * DispDum::_disp_x + x];
    uint8_t pos = y % 8;
    for (int i = 0; i < len; i++)
    {
        *start_blk = _pixdrw_arr[DispDum::_cur_drw_mode](*start_blk, (0x80 >> pos));
        start_blk ++;
    }
}
void DispDum::draw_hline(uint8_t x, uint8_t y, uint8_t len)
{
    if (x >= DispDum::_disp_x || y >= DispDum::_disp_y)
    {return;}

    if (x+len >= DispDum::_disp_x)
    {
        len = DispDum::_disp_x - x - 1;//Auto truncate len
    }
    DispDum::_nochk_draw_hline(x, y, len);
}

//Draw a vertical line from (x, y) with length len
void DispDum::draw_vline(uint8_t x, uint8_t y, uint8_t len)
{
    if (x >= DispDum::_disp_x || y >= DispDum::_disp_y)
    {return;}

    len = y+len >= DispDum::_disp_y ? DispDum::_disp_y - y - 1 : len;//Auto truncate len

    uint8_t *start_blk = &DispDum::_disp_ram[(y/8) * DispDum::_disp_x + x];

    //First (non-full) block
    *start_blk = _pixdrw_arr[DispDum::_cur_drw_mode](*start_blk,
            (0xFF >> y%8) & (0xFF << (y%8 + len < 8 ? 8 - y%8 - len : 0)));
    if (y%8 + len <= 8)
    {return;}

    len -= 8 - y % 8;

    while (len >= 8)
    {
        start_blk += DispDum::_disp_x;//Advance to next block (in y-direction)
        *start_blk = _pixdrw_arr[DispDum::_cur_drw_mode](*start_blk, 0xFF);
        len -= 8;
    }
    //Final (non-full) block
    if (len > 0)
    {
        start_blk += DispDum::_disp_x;
        *start_blk = _pixdrw_arr[DispDum::_cur_drw_mode](*start_blk, ~(0xFF >> len%8));
    }
}

void DispDum::draw_line(uint8_t x_i, uint8_t y_i, uint8_t x_f, uint8_t y_f)
{
    //Drawing using Bresnham line drawing algorithm
    //For detailed explaination refer to the following links:
    //https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html
    //https://zingl.github.io/Bresenham.pdf

    //Bound checking
    if (x_i >= DispDum::_disp_x || x_f >= DispDum::_disp_x)
    {return;}
    if (y_i >= DispDum::_disp_y || y_f >= DispDum::_disp_y)
    {return;}

    int16_t dx = std::abs(x_f - x_i), dy = -std::abs(y_f - y_i);
    int8_t sx = x_i < x_f ? 1 : -1, sy = y_i < y_f ? 1 : -1;//Stepping direction
    int16_t err = dx + dy, e2 = 0;//Prepare the errors

    for (;;)
    {
        DispDum::draw_pixel(x_i, y_i);
        if (x_i == x_f && y_i == y_f)
        {break;}
        e2 = 2 * err;//Avoid floating point math
        if (e2 >= dy)//Effectively (dxy >= -dx), diagonal value in sx direction
        {err += dy; x_i += sx;}//... so move in sx direction
        if (e2 <= dx)
        {err += dx; y_i += sy;}
    }
}

void DispDum::draw_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool filled)
{
    if (x >= DispDum::_disp_x || y >= DispDum::_disp_y)
    {return;}

    w = x + w >= DispDum::_disp_x ? DispDum::_disp_x - x - 1 : w;
    h = y + h >= DispDum::_disp_y ? DispDum::_disp_y - y - 1 : h;

    DispDum::_nochk_draw_hline(x, y, w);
    DispDum::_nochk_draw_hline(x, y+h-1, w);
    if (filled)
    {
        for (uint8_t i = y+1; i < y+h-1; i ++)
        {
            DispDum::_nochk_draw_hline(x, i, w);
        }
    }
    else
    {
        DispDum::draw_vline(x, y, h);
        DispDum::draw_vline(x+w, y, h);
    }
}

void DispDum::draw_bmp(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *xbmp)
{
    if (x >= DispDum::_disp_x || y >= DispDum::_disp_y)
    {return;}

    //Trucate:
    uint8_t w_c = x + w >= DispDum::_disp_x ? DispDum::_disp_x - x - 1 : w;
    uint8_t h_c = y + h >= DispDum::_disp_y ? DispDum::_disp_y - y - 1 : h;

    uint8_t hi_offset = y % 8, lo_offset = 8 - y % 8;

    uint8_t d_row = y/8, b_row  = 0;

    //First row
    /*
    for (uint8_t b_col = 0; b_col < w_c; b_col ++)
    {
        DispDum::_disp_ram[DispDum::_disp_x * d_row + x + b_col] =
            _pixdrw_arr[DispDum::_cur_drw_mode]
            (xbmp[b_row * w + b_col] >> hi_offset,
             DispDum::_disp_ram[DispDum::_disp_x * d_row + x + b_col]);
    }
    */

    while (h_c >= 8)
    {
        h_c -= 8;
        for (uint8_t b_col = 0; b_col < w_c; b_col ++)
        {
            DispDum::_disp_ram[DispDum::_disp_x * d_row + x + b_col] =
                _pixdrw_arr[DispDum::_cur_drw_mode]
                ((xbmp[(b_row-1) * w + b_col] << lo_offset) |
                 (xbmp[b_row * w + b_col] >> hi_offset),
                 DispDum::_disp_ram[DispDum::_disp_x * d_row + x + b_col]);
        }
        d_row ++, b_row ++;
    }
}

void DispDum::draw_xbmp(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *xbmp)
{
    //Refer to the following link for a complete specification of XBMP format:
    //https://www.fileformat.info/format/xbm/egff.htm

    if (x >= DispDum::_disp_x || y >= DispDum::_disp_y)
    {return;}

    uint8_t w_c = x + w >= DispDum::_disp_x ? DispDum::_disp_x - x - 1 : w;
    uint8_t h_c = y + h >= DispDum::_disp_y ? DispDum::_disp_y - y - 1 : h;

    uint8_t ign_pix = 8 - w % 9;//TODO: figure out if this is correct or not

    for (uint8_t i = 0; i < h_c; i ++)
    {
        for (uint8_t j = 0; j < w_c; j ++)
        {
            if((xbmp[(i * w + j + i * ign_pix) / 8] & (0x01 << (j) % 8)))
            {
                DispDum::_nochk_draw_pixel(x + j, y + i);
            }
        }
    }
}




void DispDum::set_draw_mode(DispDum::DrwPixModes mode)
{
    DispDum::_cur_drw_mode = mode;
}
