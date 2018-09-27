/*******************************************************************************
File: Star.h
Name: Edward Jose
*******************************************************************************/


#ifndef STAR_H
#define STAR_H

class Star
{
public:
    Star()
        :x_(rand() % W), y_(rand() % H), w_(2),
        h_(2), r_(rand() % 256), g_(rand() % 256), b_(255),
         state_(0)
    {}

    void run()
    {
        switch(state_)
        {
            case 0:
                ++y_;
                if (y_ > H)
                {
                    state_ = 1;
                }
                break;
            case 1:
                y_ = -5;
                x_ = rand() % W;
                state_ = 0;
        }
    }

    void draw(Surface & surface)
    {
        surface.put_rect(x_, y_, w_, h_, r_, g_, b_);
    }

private:
    int x_, y_, w_, h_;
    int  r_, g_, b_;
    int state_;
};

#endif
