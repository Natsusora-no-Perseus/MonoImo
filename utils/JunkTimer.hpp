#pragma once
#include <ctime>
#include <vector>

#define INIT_TIMER_CAP 3
#define MIN_TIME_STEP 10000//Somewhere around 1ms

struct JTimer
{
    unsigned long int counter_value = 0;
    bool is_paused = true;
};

class JunkTimer
{
    public:
        //Constructor
        JunkTimer(void);

        //Creating timer
        int create_timer(void);

        void update_timers(void);//Call this to update values of the timers

        //Accessing
        //Returns false if no timer with value found
        bool get_timer_val(int index, unsigned long int &ret);
        bool toggle_timer(int index, bool is_paused);
        bool reset_timer(int index);//This does NOT reset pausing state

    private:
        std::vector<JTimer> _jtimers;//Holds list of timers
        int _cur_timer_cnt = 0;
        clock_t _last_time;
};
