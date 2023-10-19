#include "game_display.h"

Logger logger;

GameDisplay::GameDisplay()
{
    XInitThreads();
    m_display = XOpenDisplay(NULL);

    if(m_display == NULL)
    {
        logger << "Failed to open display!";
        exit(1);
    }

    int screen = DefaultScreen(m_display);

    m_window = XCreateSimpleWindow(m_display,
                                   RootWindow(m_display, screen),
                                   0,
                                   0,
                                   DEFAULT_WINDOW_WIDTH,
                                   DEFAULT_WINDOW_HEIGHT,
                                   1,
                                   BlackPixel(m_display, screen),
                                   DEFAULT_BACKGROUND_COLOR);

    XSelectInput(m_display, m_window, KeyPressMask | ExposureMask);
    XMapWindow(m_display, m_window);

    m_graphic_context = XCreateGC(m_display, m_window, 0, NULL);
}

GameDisplay::~GameDisplay()
{
    XFreeGC(m_display, m_graphic_context);
    XDestroyWindow(m_display, m_window);
    XCloseDisplay(m_display);
}

Geometry GameDisplay::getWindowGeometry() const
{
    Window root_window;
    int x, y;
    unsigned int height, width, border_width, depth;
    XGetGeometry(m_display, m_window, &root_window, &x, &y, &width, &height, &border_width, &depth);

    Geometry current_window;
    current_window.x = x;
    current_window.y = y;
    current_window.width = width;
    current_window.height = height;

    return current_window;
}

void GameDisplay::drawRectangle(Window t_window, unsigned long t_fill_color, unsigned long t_border_color, int t_x, int t_y, int t_width, int t_height) const
{
    // Set fill color.
    XSetForeground(m_display, m_graphic_context, t_fill_color);
    XFillRectangle(m_display, t_window, m_graphic_context, t_x, t_y, t_width, t_height);
    // Set border color.
    XSetForeground(m_display, m_graphic_context, t_border_color);
    XDrawRectangle(m_display, t_window, m_graphic_context, t_x, t_y, t_width, t_height);
}

void GameDisplay::drawText(Window t_window, int t_x, int t_y, unsigned long t_text_color, const std::string &t_string) const
{
    // Set text color.
    XSetForeground(m_display, m_graphic_context, t_text_color);
    XDrawString(m_display, t_window, m_graphic_context, t_x, t_y, t_string.c_str(), t_string.size());
}