#include "TermDrw.hpp"
#include <curses.h>
#include <chrono>

TermDrw::TermDrw(DispDum &dummy_inst) :
    _dummy_inst(dummy_inst)
{
    //Curses setup
    initscr();
    keypad(stdscr, TRUE);
    nonl();
    cbreak();
    leaveok(stdscr, TRUE);
    noecho();
    curs_set(0);//Cursor invisible

    //halfdelay(2);//Delay for 2/10 of a second
    nodelay(stdscr, true);

    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);//Text, bg
        init_pair(2, COLOR_BLACK, COLOR_CYAN);
        init_pair(3, COLOR_WHITE, COLOR_RED);//Error msg
    }
}

void TermDrw::termdisp_refresh(void)
{
    const char fail_str[] = "Space Insuff.";
    if (getmaxx(stdscr) < 70 || getmaxy(stdscr) < 20)
    {
        mvprintw(3, 3, "Space Insuff.");
    }
    else
    {
        //Draw four corners and lines
        attrset(COLOR_PAIR(1));
        mvaddch(2, 2, ACS_ULCORNER);//ULCORNER ch
        hline(ACS_HLINE, 65);//UP line
        mvvline(3, 2, ACS_VLINE, 17);//L line
        mvaddch(2 ,67, ACS_URCORNER);//URCORNER ch
        mvvline(3, 67, ACS_VLINE, 17);//R line
        mvaddch(19, 2, ACS_LLCORNER);//LLCORNER ch
        hline(ACS_HLINE, 65);//LOW line
        mvaddch(19, 67, ACS_LRCORNER);//LRCORNER ch
        refresh();

        //Draw the main display:
        wchar_t c = static_cast<wchar_t>(0x2800);
        for (int i = 0; i < _dummy_inst.get_disp_x() / 2; i ++)//x-coord
        {
            for (int j = 0; j < _dummy_inst.get_disp_y() / 4; j ++)//y-coord
            {
                c = static_cast<wchar_t>(_dummy_inst.get_braille_int(i, j));
                //c = static_cast<wchar_t>((i+10)+(j+10));
                //mvaddch(j+3, i+3, 'T');
                mvaddnwstr(j+3, i+3, &c, 1);
            }
        }

        //Draw info
        attrset(COLOR_PAIR(2));
        mvprintw(2, 4, "SSD1306");
        mvaddwstr(19, 4, L" H←|J↓|K↑|L→ ");
    }
    refresh();
}
void TermDrw::main_refresh(void)//Redraws non-static elems
{
    attrset(COLOR_PAIR(1));//Black bg and white text
    wchar_t c = static_cast<wchar_t>(0x2800);
    for (int i = 0; i < _dummy_inst.get_disp_x() / 2; i ++)//x-coord
    {
        for (int j = 0; j < _dummy_inst.get_disp_y() / 4; j ++)//y-coord
        {
            c = static_cast<wchar_t>(_dummy_inst.get_braille_int(i, j));
            mvaddnwstr(j+3, i+3, &c, 1);
        }
    }

    //refresh();
    wrefresh(stdscr);
}

void TermDrw::side_refresh(void)//Redraws bounding box etc. (static elems)
{
    //Draw the lines:
    attrset(COLOR_PAIR(1));
    mvaddch(2, 2, ACS_ULCORNER);//ULCORNER ch
    hline(ACS_HLINE, 65);//UP line
    mvvline(3, 2, ACS_VLINE, 17);//L line
    mvaddch(2 ,67, ACS_URCORNER);//URCORNER ch
    mvvline(3, 67, ACS_VLINE, 17);//R line
    mvaddch(19, 2, ACS_LLCORNER);//LLCORNER ch
    hline(ACS_HLINE, 65);//LOW line
    mvaddch(19, 67, ACS_LRCORNER);//LRCORNER ch

    //Draw the text
    attrset(COLOR_PAIR(2));
    mvprintw(2, 4, "SSD1306");
    mvaddwstr(19, 4, L" H←|J↓|K↑|L→ ");

    wrefresh(stdscr);
}

void TermDrw::termdisp_end(void)
{
    endwin();
}

