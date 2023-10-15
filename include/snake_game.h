#ifndef SNAKE_H
#define SNAKE_H

#include <unistd.h>
#include <algorithm>
#include "game_display.h"

const unsigned long COLOR_RED               = 0xFF0000;
const unsigned long COLOR_GREEN             = 0x008000;
const unsigned long COLOR_WHITE             = 0xFFFFFF;
const unsigned int DEFAULT_MOVE_DISTANCE    =       10;

enum class GameState
{
    GAME_READY,
    GAME_RUNNING,
    GAME_OVER
};

enum class Direction
{
    UP,
    DOWN,
    RIGHT,
    LEFT
};

class Snake : public GameDisplay
{
    private:
        GameState m_current_game_state;
        Direction m_current_direction;
        std::list<Square> m_snake;
        Square m_food;
        std::queue<Direction> m_direction_queue;
        bool m_is_running;
        unsigned int m_game_speed;
        unsigned int m_score;

        void init();
        void reset();
        void draw();
        void displayMessage();
        void displayScore();
        void handleKeyPress(XKeyEvent t_key_event);
        bool isBorderCollision();
        bool isSelfCollision();
        bool isFoodCollision();
        void moveSnake();
        void spawnFood();

    public:
        Snake();
        void run();
};

#endif // SNAKE_H