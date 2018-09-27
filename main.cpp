/****************************************************************************
 Yihsiang Liow
 Copyright
 ****************************************************************************/
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <string>

#include "Includes.h"
#include "Constants.h"
#include "compgeom.h"
#include "Surface.h"
#include "Event.h"
#include "Star.h"

int ALIEN_SPEED = 2;
int SHIP_SPEED = 5;
int score = 0;
int num_dead_aliens = 0;


class AlienLaser
{
public:
    AlienLaser(int x = 0, int y = 0)
        :x_(x), y_(y), state_(-1), alive_(0)
    {}    

    void x(int x){ x_ = x; }

    void y(int y){ y_ = y; }

    void alive(bool alive){ alive_ = alive; }

    bool alive(){ return alive_; }

    Rect & rect() { return rect_; }

    int state() { return state_; }

    void state(int laser_state) { state_ = laser_state; }
    
    void run()
    {
        if (alive_)
        {
            switch (state_)
            {
                case -1:
                    rect_.x = x_ + 15;
                    rect_.y = y_ + 32;
                    rect_.w = 2;
                    rect_.h = 10;
                    state_ = 0;
                case 0:
                    rect_.y += 2;
                    if (rect_.y >= H)
                    {
                        state_ = 1;
                    }
                    break;
                case 1:
                    alive_ = 0;
                    state_ = -1;
                    break;
            }
        }
    }

    void draw(Surface & surface)
    {
        if (alive_)
        {
            surface.put_rect(rect_.x, rect_.y, rect_.w, rect_.h, 248, 248, 255);
        }
    }
    
private:
    int x_, y_;
    int dy_;
    int state_;
    bool alive_;
    Rect rect_;
};


class Alien
{
public:
    Alien(int x, int y, Surface & surface)
        :x_(x), y_(y), surface_(surface), dx_(ALIEN_SPEED), dy_(ALIEN_SPEED),
         state_(-1), alive_(1)
    {}

    void state(int alien_state) { state_ = alien_state; }

    int state() { return state_; }

    Rect & rect() { return rect_; }

    bool alive() { return alive_; }
    
    void alive(bool alive_state)
    {
        alive_ = alive_state;
        state_ = 0;
    }
    
    void fire(AlienLaser & alien_laser)
    {
        alien_laser.x(rect_.x);
        alien_laser.y(rect_.y);
        alien_laser.alive(1);
    }    
    
    void direction()
    {
        if (alive_)
        {
            if (home_position_.x + rect_.w >= W)
            {
                fleet_move_ = -1;
            }
            else if (home_position_.x <= 0)
            {
                fleet_move_ = 1;
            }
        }
    }
    
    virtual void run()
    {
        switch (state_)
        {
            case -1:
                rect_ = image().getRect();
                rect_.x = x_;
                rect_.y = y_;
                home_position_.x = rect_.x;
                home_position_.y = rect_.y;
                                        
                state_ = 0;
            case 0:
                rect_.x += dx_ * fleet_move_;
                home_position_.x = rect_.x;
                break;
            case 1:
                rect_.y += dy_;
                    
                home_position_.x += dx_ * fleet_move_;
                    
                if (rect_.y + rect_.h >= H)
                {
                    rect_.y = -40;
                    state_ = 2;
                }
                break;
            case 2:
                home_position_.x += dx_ * fleet_move_;
                                        
                if (home_position_.y == rect_.y)
                {
                    rect_.x = home_position_.x;
                    state_ = 0;
                }
                else
                {
                    if (home_position_.x != rect_.x && rect_.x >= 0)
                    {
                        if (rect_.x < home_position_.x)
                        {
                            rect_.x += dx_;
                        }
                        else if (rect_.x > home_position_.x &&
                                 rect_.x + rect_.w <= W)
                        {
                            rect_.x -= dx_;
                        }
                    }
                    if (home_position_.y != rect_.y)
                    {
                        rect_.y += dy_;
                    }
                }
                   
                break;
            case 3:
                alive_ = 0;
                state_ = 0;
                rect_.x = home_position_.x;
                rect_.y = home_position_.y;
        }
    }

    void draw()
    {
        if(alive_)
        {
            surface_.put_image(image(), rect_);
        }
    }
    
    virtual Image & image() = 0;

    virtual void death(int current_state) = 0;
    
private:
    bool alive_;
    int x_, y_;
    int dx_, dy_;
    int state_;
    Surface & surface_;
    Rect rect_;
    Rect home_position_;
    static int fleet_move_;
    bool change_state_ = 0;
};


class AquaAlien : public Alien
{
public:
    AquaAlien(int x, int y, Surface & surface)
        :Alien(x, y, surface)
    {}

    Image & image()
    {
        return image_;
    }

    void death(int current_state)
    {
        if (current_state == 0)
        {
            score += 30;
        }
        else if (current_state == 1 || current_state == 2)
        {
            score += 60;
        }            
    }
    
private:
    static Image image_;
};


class RedAlien : public Alien
{
public:
    RedAlien(int x,  int y, Surface & surface)
        :Alien(x, y, surface)
    {}

    Image & image()
    {
        return image_;
    }

    void death(int current_state)
    {
        if (current_state == 0)
        {
            score += 40;
        }
        else if (current_state == 1 || current_state == 2)
        {
            score += 80;
        }            
    }
    
private:
    static Image image_;
};


class PurpleAlien : public Alien
{
public:
    PurpleAlien(int x, int y, Surface & surface)
        :Alien(x, y, surface)
    {}

    Image & image()
    {
        return image_;
    }

    void death(int current_state)
    {
        if (current_state == 0)
        {
            score += 50;
        }
        else if (current_state == 1 || current_state == 2)
        {
            score += 100;
        }            
    }
    
private:
    static Image image_;
};


class FlagShip : public Alien
{
public:
    FlagShip(int x, int y, Surface & surface)
        :Alien(x, y, surface)
    {}

    Image & image()
    {
        return image_;
    }

    void death(int current_state)
    {
        if (current_state == 0)
        {
            score += 30;
        }
        else if (current_state == 1 || current_state == 2)
        {
            score += 150;
        }            
    }
        
private:
    static Image image_;
};


class PlayerLaser
{
public:
    PlayerLaser(int x = 0, int y = 0)
        :x_(x), y_(y), state_(-1), alive_(0)
    {}    

    void x(int x){ x_ = x; }

    void y(int y){ y_ = y; }

    void alive(bool alive){ alive_ = alive; }

    bool alive(){ return alive_; }

    Rect & rect() { return rect_; }
    
    int state() { return state_; }

    void state(int laser_state) { state_ = laser_state; }
        
    void run()
    {
        if (alive_)
        {
            switch (state_)
            {
                case -1:
                    rect_.x = x_ + 15;
                    rect_.y = y_ - 10;
                    rect_.w = 2;
                    rect_.h = 10;
                    state_ = 0;
                case 0:
                    rect_.y -= 2;
                    if (rect_.y <= 0)
                    {
                        state_ = 1;
                    }
                    break;
                case 1:
                    alive_ = 0;
                    state_ = -1;
            }
        }
    }

    void draw(Surface & surface)
    {
        if (alive_)
        {
            surface.put_rect(rect_.x, rect_.y, rect_.w, rect_.h, 255, 255, 0);
        }
    }
    
private:
    int x_, y_;
    int dy_;
    int state_;
    bool alive_;
    Rect rect_;
};


class PlayerShip
{
public:
    PlayerShip(int x, int y, Surface & surface)
        :x_(x), y_(y), surface_ (surface), state_(-1), alive_(1),
         dx_(SHIP_SPEED), player_move_(-1), invuln_(0)
    {
        rect_ = image_.getRect();
        rect_.x = x_;
        rect_.y = y_;
    }

    void state(int player_state) { state_ = player_state; }

    int state() { return state_; }
    
    Rect & rect() { return rect_; }
    
    bool alive() { return alive_; }

    void alive(bool alive_status) { alive_ = alive_status; } 

    bool invuln() { return invuln_; }

    void invuln(bool invuln_state) { invuln_ = invuln_state; }
    
    void move_left()
    {
        rect_.x -= dx_;
        if (rect_.x <= 0)
        {
            rect_.x = 0;
        }
    }

    void move_right()
    {
        rect_.x += dx_;
        if (rect_.x + rect_.w >= W)
        {
            rect_.x = W - rect_.w;
        }
    }

    void fire(PlayerLaser & player_laser)
    {
        player_laser.x(rect_.x);
        player_laser.y(rect_.y);
        player_laser.alive(1);
    }    
    
    void draw()
    {
        if (alive_)
        {
            surface_.put_image(image_, rect_);
        }
    }

    void death(int lives_left)
    {
        if (lives_left >= 0)
        {
            rect_.x = W / 2;
            invuln_ = 1;
        }
        else
        {
            alive_ = 0;
        }
    }
    
private:
    static Image image_;
    int x_, y_, dx_;
    int state_;
    Rect rect_;
    int player_move_;
    bool alive_;
    Surface & surface_;
    bool invuln_;
};


Image AquaAlien::image_("images/galaxian/GalaxianAquaAlien.gif");
Image RedAlien::image_("images/galaxian/GalaxianRedAlien.gif");
Image PurpleAlien::image_("images/galaxian/GalaxianPurpleAlien.gif");
Image FlagShip::image_("images/galaxian/GalaxianFlagship.gif");
Image PlayerShip::image_("images/galaxian/GalaxianGalaxip.gif");
int Alien::fleet_move_(1);


bool check_collision(Rect & good_guy, Rect & bad_guy)
{
    bool ret = false;

    int x0 = good_guy.x;
    int x1 = good_guy.x + good_guy.w;
    int y0 = good_guy.y;
    int y1 = good_guy.y + good_guy.h;
    
    int x0p = bad_guy.x;
    int x1p = bad_guy.x + bad_guy.w;
    int y0p = bad_guy.y;
    int y1p = bad_guy.y + bad_guy.h;
    
    if ((x0p >= x0 && x0p <= x1) || (x1p >= x0 && x1p <= x1))
    {
        if((y0p >= y0 && y0p <= y1) || (y1p >= y0 && y1p <= y1))
        {
            ret = true;
        }
    }
    
    return ret;
}


void test_game()
{
    Surface surface(W, H);
    Event event;
    const int PLAYER_LIVES = 2;
    const int STAR_SIZE = 500;
    const int PLAYER_LASER_SIZE = 1000;
    const int ALIEN_LASER_SIZE = 100;
    const int AQUA_SIZE = 30;
    const int PURPLE_SIZE = 8;
    const int RED_SIZE = 6;
    const int FLAG_SIZE = 2;
    const int TYPES_OF_ALIEN = 4;
    int FLEET_SIZE = AQUA_SIZE + PURPLE_SIZE + RED_SIZE + FLAG_SIZE;
    
    Star star[STAR_SIZE];

    Alien ** a;
    a = new Alien * [FLEET_SIZE];
    int j = 0;

    Alien * b[TYPES_OF_ALIEN];
    b[0] = new FlagShip(0, 150, surface);
    b[1] = new RedAlien(0, 210, surface);
    b[2] = new PurpleAlien(0, 270, surface);
    b[3] = new AquaAlien(0, 330, surface);

    
    PlayerLaser player_laser[PLAYER_LASER_SIZE];
    AlienLaser alien_laser[ALIEN_LASER_SIZE];

    // a[0] - a[45] points to aliens
    for (int i = 0; i < FLAG_SIZE; ++i)
    {
        a[i] = new FlagShip(120 + 40 * 3 * j, 0, surface);
        ++j;  
    }

    j = 0;
    for (int i = FLAG_SIZE; i < FLAG_SIZE + RED_SIZE; ++i)
    {
        a[i] = new RedAlien(80 + 40 * j, 40, surface);
        ++j;
    }

    j = 0;
    for (int i = FLAG_SIZE + RED_SIZE;
         i < FLAG_SIZE + RED_SIZE + PURPLE_SIZE; ++i)
    {
        a[i] = new PurpleAlien(40 + 40 * j, 80, surface);
        ++j;
    }

    j = 0;
    for (int i = FLAG_SIZE + RED_SIZE + PURPLE_SIZE;
         i < FLAG_SIZE + RED_SIZE + PURPLE_SIZE + AQUA_SIZE; ++i)
    {
        a[i] = new AquaAlien(40 * (j % 10), 120 + (j / 10) * 40, surface);
        ++j;
    }

    PlayerShip player_ship(W / 2, H - 50, surface);

    PlayerShip * player_lives[PLAYER_LIVES];
    for (int i = 0; i < PLAYER_LIVES; ++i)
    {
        player_lives[i] = new PlayerShip(0 + 40 * i, H - 30, surface);
    }

    int lives_left = PLAYER_LIVES;
    int invuln_timer = 0;
    int num_fleets_destroyed = 0;
    int state = 0;
    int level_timer = -1;
    int shoot_timer = 0;
    int end_level_timer = 150;
    int death_timer = 150;
    bool descend = 0;
    int descend_timer = 0;
    
    while (1)
    {
        int start = getTicks();
        /********************************************************************
                             CHECK USER INPUTS
        ********************************************************************/
        if (event.poll() && event.type() == QUIT) break;

        KeyPressed keypressed = get_keypressed();
        if (state == 0)
        {
            if (keypressed[SPACE])
            {
                level_timer = 150;
            }
            
            if (level_timer > 0)
            {
                --level_timer;
            }
            else if(level_timer == 0)
            {
                state = 1;
                descend_timer = 150;
                level_timer = -1;
            }
        }
        else if (state == 1)
        {
            if (keypressed[LEFTARROW])
            {
                player_ship.move_left();
            }
            else if (keypressed[RIGHTARROW])
            {
                player_ship.move_right();
            }

            if (shoot_timer == 0)
            {
                shoot_timer = 15;
                if (keypressed[SPACE])
                {
                    if (player_ship.alive())
                    {
                        {
                            for (int i = 0; i < PLAYER_LASER_SIZE; ++i)
                            {
                                if (player_laser[i].alive() == 0)
                                {
                                    player_ship.fire(player_laser[i]);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            --shoot_timer;
        }
        
        /********************************************************************
                                 GAME LOGIC
        ********************************************************************/
        if(state)
        {
            // logic to tell the aliens when to shoot while in state 0
            if (rand() % 100 == 0)
            {
                int chosen_alien = rand() % FLEET_SIZE;
                if (a[chosen_alien]->alive())
                { for (int i = 0; i < ALIEN_LASER_SIZE; ++i)
                    {
                        if (alien_laser[i].alive() == 0)
                        {
                            if (a[chosen_alien]->state() == 0)
                            {
                                a[chosen_alien]->fire(alien_laser[i]);
                                break;
                            }
                        }
                    }
                }
            }

            // logic to tell the aliens to shoot while in state 1
            if (rand() % 50 == 0)
            {
                int chosen_alien = rand() % FLEET_SIZE;
                if (a[chosen_alien]->alive())
                {
                    for (int i = 0; i < ALIEN_LASER_SIZE; ++i)
                    {
                        if (alien_laser[i].alive() == 0)
                        {
                            if (chosen_alien == 0 || chosen_alien == 2 ||
                                chosen_alien == 3 || chosen_alien == 4)
                            {
                                a[rand() % 3 + 2]->fire(alien_laser[i]);
                            }
                            else if (chosen_alien == 1 || chosen_alien == 5 ||
                                     chosen_alien == 6 || chosen_alien == 7)
                            {
                                a[rand() % 3 + 5]->fire(alien_laser[i]);
                            }
                            else
                            {
                                a[chosen_alien]->fire(alien_laser[i]);
                            }
                            break;
                        }
                    }
                }
            }
        
            // logic to tell when to have the aliens descend
            // i.e. move to state 1        
            if (rand() % 100 == 1)
            {
                int chosen_alien = rand() % FLEET_SIZE;
                if (a[chosen_alien]->alive())
                {
                    if (a[chosen_alien]->state() == 0 && descend)
                    {
                        if (chosen_alien == 0 || chosen_alien == 2 ||
                            chosen_alien == 3 || chosen_alien == 4)
                        {
                            a[0]->state(1);
                            a[2]->state(1);
                            a[3]->state(1);
                            a[4]->state(1);
                        }
                        else if (chosen_alien == 1 || chosen_alien == 5 ||
                                 chosen_alien == 6 || chosen_alien == 7)
                        {
                            a[1]->state(1);
                            a[5]->state(1);
                            a[6]->state(1);
                            a[7]->state(1);
                        }
                        else
                        {
                            a[chosen_alien]->state(1);
                        }
                    }
                }
            }

            num_dead_aliens = 0;
            for (int i = 0; i < FLEET_SIZE; ++i)
            {
                if (a[i]->alive() == 0)
                {
                    ++num_dead_aliens;
                }
            }
            
            // logic to switch levels
            if (num_dead_aliens >= FLEET_SIZE)
            {
                if (end_level_timer == 0)
                {
                    for (int i = 0; i < FLEET_SIZE; ++i)
                    {
                        a[i]->alive(1);
                    }
                    num_dead_aliens = 0;
                    ++num_fleets_destroyed;
                    end_level_timer = 150;
                }
                else
                {
                    --end_level_timer;
                }
            }

            // logic when player dies
            if (lives_left < 0)
            {
                --death_timer;
                if (death_timer == 0)
                {
                    state = 0;
                    lives_left = 2;
                    player_ship.alive(1);

                    for (int i = 0; i < PLAYER_LIVES; ++i)
                    {
                        player_lives[i]->alive(1);
                    }

                    for (int i = 0; i < FLEET_SIZE; ++i)
                    {
                        a[i]->state(3);
                        descend = false;
                    }
                }
            }
        }
        
        /********************************************************************
                            UPDATE EACH OBJECT
        ********************************************************************/
        for (int i = 0; i < STAR_SIZE; ++i)
        {
            star[i].run();
        }

        if (state == 0)
        {
            for (int i = 0; i < TYPES_OF_ALIEN; ++i)
            {
                b[i]->direction();
            }

            for (int i = 0; i < TYPES_OF_ALIEN; ++i)
            {
                b[i]->run();
            }
        }
        else if (state == 1)
        {
            if (descend_timer == 0)
            {
                descend = true;
            }

            --descend_timer;
            
            for (int i = 0; i < FLEET_SIZE; ++i)
            {
                a[i]->direction();
            }


            for (int i = 0; i < FLEET_SIZE; ++i)
            {
                a[i]->run();
            }

            for (int i = 0; i < PLAYER_LASER_SIZE; ++i)
            {
                player_laser[i].run();
            }

            for (int i = 0; i < ALIEN_LASER_SIZE; ++i)
            {
                alien_laser[i].run();
            }
        }
        /********************************************************************
                             COMPUTE COLLISIONS AND RESOLUTION
        ********************************************************************/
        if (state)
        {
            if (player_ship.invuln())
            {
                --invuln_timer;
                if (!invuln_timer)
                {
                    player_ship.invuln(0);
                }
            }

            // player laser rects v alien ship rects
            for (int i = 0; i < PLAYER_LASER_SIZE; ++i)
            {
                if (player_laser[i].alive())
                {
                    for (int j = 0; j < FLEET_SIZE; ++j)
                    {
                        if (a[j]->alive())
                        {
                            if (a[j]->state() != -1)
                            {
                                if(check_collision(player_laser[i].rect(),
                                                   a[j]->rect()))
                                {
                                    a[j]->death(a[j]->state());
                                    a[j]->state(3);
                                    player_laser[i].state(1);
                                }
                            }
                        }
                    }
                }
            }
        
            // check the player ship rect against the rects of every alien
            if (player_ship.alive())
            {
                if (!player_ship.invuln())
                {
                    for (int i = 0; i < FLEET_SIZE; ++i)
                    {
                        if (a[i]->alive())
                        {
                            if (a[i]->state() != -1)
                            {
                                if(check_collision(player_ship.rect(),
                                                   a[i]->rect()))
                                {
                                    a[i]->death(a[i]->state());
                                    a[i]->state(3);
                                    --lives_left;
                                    if (lives_left >= 0)
                                    {
                                        player_lives[lives_left]->alive(0);
                                    }
                                    player_ship.death(lives_left);
                                    invuln_timer = 100;
                                }
                            }
                        }
                    }
                }
            }

            // player ship rect vs alien_laser rects
            if (player_ship.alive())
            {
                if (!player_ship.invuln())
                {
                    for (int i = 0; i < ALIEN_LASER_SIZE; ++i)
                    {
                        if (alien_laser[i].alive())
                        {
                            if (alien_laser[i].state() != -1)
                            {
                                if(check_collision(player_ship.rect(),
                                                   alien_laser[i].rect()))
                                {
                                    alien_laser[i].state(1);
                                    --lives_left;
                                    if (lives_left >= 0)
                                    {
                                        player_lives[lives_left]->alive(0);
                                    }
                                    player_ship.death(lives_left);
                                    invuln_timer = 100;
                                }
                            }
                        }
                    }
                }
            }
        }

        // create the strings for the title
        Font press_space_font("fonts/FreeSans.ttf", 30);
        Image press_space_image(press_space_font.render("press space to start",
                                                        255, 0, 0));
        Rect press_space_rect = press_space_image.getRect();
        press_space_rect.x = 200;
        press_space_rect.y = 0;

        Font galaxian_font("fonts/FreeSans.ttf", 75);
        Image galaxian_image(galaxian_font.render("GALAXIAN",
                                                  255, 255, 255));
        Rect galaxian_rect = galaxian_image.getRect();
        galaxian_rect.x = 130;
        galaxian_rect.y = 15;

        Font flagship1_font("fonts/FreeSans.ttf", 30);
        Image flagship1_image(flagship1_font.render("60",
                                                  255, 255, 0));
        Rect flagship1_rect = flagship1_image.getRect();
        flagship1_rect.x = 200;
        flagship1_rect.y = 175;

        Font flagship2_font("fonts/FreeSans.ttf", 30);
        Image flagship2_image(flagship2_font.render("150",
                                                  255, 255, 0));
        Rect flagship2_rect = flagship2_image.getRect();
        flagship2_rect.x = 350;
        flagship2_rect.y = 175;
        
        Font red1_font("fonts/FreeSans.ttf", 30);
        Image red1_image(red1_font.render("50",
                                                  255, 0, 0));
        Rect red1_rect = red1_image.getRect();
        red1_rect.x = 200;
        red1_rect.y = 235;

        Font red2_font("fonts/FreeSans.ttf", 30);
        Image red2_image(red2_font.render("100",
                                                  255, 0, 0));
        Rect red2_rect = red2_image.getRect();
        red2_rect.x = 350;
        red2_rect.y = 235;

        Font purple1_font("fonts/FreeSans.ttf", 30);
        Image purple1_image(purple1_font.render("40",
                                                  148, 0, 210));
        Rect purple1_rect = purple1_image.getRect();
        purple1_rect.x = 200;
        purple1_rect.y = 295;

        Font purple2_font("fonts/FreeSans.ttf", 30);
        Image purple2_image(purple2_font.render("80",
                                                  148, 0, 210));
        Rect purple2_rect = purple2_image.getRect();
        purple2_rect.x = 350;
        purple2_rect.y = 295;

        Font aqua1_font("fonts/FreeSans.ttf", 30);
        Image aqua1_image(aqua1_font.render("30",
                                                  0, 255, 255));
        Rect aqua1_rect = aqua1_image.getRect();
        aqua1_rect.x = 200;
        aqua1_rect.y = 355;

        Font aqua2_font("fonts/FreeSans.ttf", 30);
        Image aqua2_image(aqua2_font.render("60",
                                                  0, 255, 255));
        Rect aqua2_rect = aqua2_image.getRect();
        aqua2_rect.x = 350;
        aqua2_rect.y = 355;
        
        // create the score string
        std::string string_score = std::to_string(score);
        char c_score[string_score.size()];
        
        for (int i = 0; i < string_score.size(); ++i)
        {
            c_score[i] = string_score[i];
        }

        c_score[string_score.size()] = '\0';
        
        Font score_font("fonts/FreeSans.ttf", 20);
        Image score_image(score_font.render(c_score,
                                            255, 0, 0));
        Rect score_rect = score_image.getRect();
        score_rect.x = W - (10 + string_score.size() * 10);
        score_rect.y = 0;

        // create the fleet string
        std::string string_fleet = std::to_string(num_fleets_destroyed);
        char c_fleet[string_fleet.size()];
        
        for (int i = 0; i < string_fleet.size(); ++i)
        {
            c_fleet[i] = string_fleet[i];
        }

        c_fleet[string_fleet.size()] = '\0';
        
        Font fleet_font("fonts/FreeSans.ttf", 15);
        Image fleet_image(fleet_font.render(c_fleet,
                                            255, 0, 0));
        Rect fleet_rect = fleet_image.getRect();
        fleet_rect.x = W - (10 + string_fleet.size() * 10);
        fleet_rect.y = H - 30;
                    
        
        /********************************************************************
                                 DRAW EACH OBJECT
        ********************************************************************/
        surface.lock();
        surface.fill(BLACK);
        for (int i = 0; i < 500; ++i)
        {
            star[i].draw(surface);
        }

        if (state == 0)
        {
            for (int i = 0; i < TYPES_OF_ALIEN; ++i)
            {
                b[i]->draw();
            }
            
            surface.put_image(press_space_image, press_space_rect);
            surface.put_image(galaxian_image, galaxian_rect);
            surface.put_image(flagship1_image, flagship1_rect);
            surface.put_image(flagship2_image, flagship2_rect);
            surface.put_image(red1_image, red1_rect);
            surface.put_image(red2_image, red2_rect);
            surface.put_image(purple1_image, purple1_rect);
            surface.put_image(purple2_image, purple2_rect);
            surface.put_image(aqua1_image, aqua1_rect);
            surface.put_image(aqua2_image, aqua2_rect);
        }
        else if (state == 1)
        {
            for (int i = 0; i < FLEET_SIZE; ++i)
            {
                a[i]->draw();
            }

            for (int i = 0; i < PLAYER_LASER_SIZE; ++i)
            {
                player_laser[i].draw(surface);
            }

            for (int i = 0; i < ALIEN_LASER_SIZE; ++i)
            {
                alien_laser[i].draw(surface);
            }

            for (int i = 0; i < PLAYER_LIVES; ++i)
            {
                player_lives[i]->draw();
            }
                
            player_ship.draw();

            surface.put_image(score_image, score_rect);
            surface.put_image(fleet_image, fleet_rect);
        }
        surface.unlock();
        surface.flip();
        
        
        int end = getTicks();
        int dt = end - start;
        int timeframe = 1000 / 30;
        delay(10);
    }

    for (int i = 0; i < TYPES_OF_ALIEN; ++i)
    {
        delete b[i];
    }
    
    for (int i = 0; i < PLAYER_LIVES; ++i)
    {
        delete player_lives[i];
    }
    
    for (int i = 0; i < FLEET_SIZE; ++i)
    {
        delete [] a[i];
    }

    delete [] a;
}


int main(int argc, char* argv[])
{
    srand((unsigned int) time(NULL));
    test_game();
	
    return 0;
}
