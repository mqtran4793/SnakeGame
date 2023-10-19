#ifndef GAME_DISPLAY_H
#define GAME_DISPLAY_H

#include <list>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <string>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "logger.h"

const unsigned long DEFAULT_BACKGROUND_COLOR    = 0x112016;
const unsigned int DEFAULT_WINDOW_WIDTH         =      800;
const unsigned int DEFAULT_WINDOW_HEIGHT        =      600;
const unsigned int DEFAULT_SQUARE_SIDE          =       10;
extern Logger logger;

struct Geometry
{
    int x;
    int y;
    unsigned int width;
    unsigned int height;
};

struct Position
{
    int x;
    int y;
};

struct Size
{
    unsigned int width;
    unsigned int height;
};

struct Square
{
    Position position;
    Size size {DEFAULT_SQUARE_SIDE, DEFAULT_SQUARE_SIDE};

    bool operator==(const Square& t_other) const
    {
        return (position.x == t_other.position.x && position.y == t_other.position.y);
    }
};

class GameDisplay
{
    protected:
        Display *m_display;
        Window m_window;
        GC m_graphic_context;
        
        Geometry getWindowGeometry() const;
        void drawRectangle(Window t_window, unsigned long t_fill_color, unsigned long t_border_color, int t_x, int t_y, int t_width, int t_height) const;
        void drawText(Window t_window, int t_x, int t_y, unsigned long t_text_color, const std::string &t_string) const;

    public:
        GameDisplay();
        ~GameDisplay();
};

#endif // GAME_DISPLAY_H