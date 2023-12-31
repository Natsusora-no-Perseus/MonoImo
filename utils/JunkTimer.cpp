#include "JunkTimer.hpp"
#include <ctime>
#include <iostream>

JunkTimer::JunkTimer(void)
{
    JunkTimer::_last_time = clock();
}

int JunkTimer::create_timer(void)
{
    JTimer new_timer = {0, true};
    if (JunkTimer::_jtimers.capacity() < JunkTimer::_cur_timer_cnt+1)
    {
        JunkTimer::_jtimers.resize(JunkTimer::_cur_timer_cnt + 3);
    }
    JunkTimer::_jtimers[JunkTimer::_cur_timer_cnt] = new_timer;
    return (JunkTimer::_cur_timer_cnt ++);
}

void JunkTimer::update_timers(void)
{
    clock_t cur_time = clock();
    unsigned long int time_diff = cur_time - JunkTimer::_last_time;
    if (time_diff < MIN_TIME_STEP)
    {return;}
    time_diff *= 1000;
    time_diff /= CLOCKS_PER_SEC;
    //std::cout << time_diff << std::endl;
    for (int i=0; i < JunkTimer::_cur_timer_cnt; i ++)
    {
        if (!JunkTimer::_jtimers[i].is_paused)
        {
            JunkTimer::_jtimers[i].counter_value += time_diff;
        }
    }
    JunkTimer::_last_time = cur_time;
}


bool JunkTimer::get_timer_val(int index, unsigned long int &ret)
{
    if (JunkTimer::_cur_timer_cnt <= index)
    {return false;}
    ret = JunkTimer::_jtimers[index].counter_value;
    return true;
}

bool JunkTimer::toggle_timer(int index, bool is_paused)
{
    if (JunkTimer::_cur_timer_cnt <= index)
    {return false;}
    JunkTimer::_jtimers[index].is_paused = is_paused;
    return true;
}

bool JunkTimer::reset_timer(int index)
{
    if (JunkTimer::_cur_timer_cnt <= index)
    {return false;}
    JunkTimer::_jtimers[index].counter_value = 0;
    return true;
}

