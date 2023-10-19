#ifndef SNAKE_H
#define SNAKE_H

#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
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
        Pixmap m_buffer_window;
        GameState m_current_game_state;
        Direction m_current_direction;
        std::list<Square> m_snake;
        Square m_food;
        unsigned int m_food_value;
        std::queue<Direction> m_direction_queue;
        bool m_is_running;
        unsigned int m_frame_rate;
        unsigned int m_score;
        std::thread inputThread;
        std::thread gameThread;
        std::mutex m_mutex;
        std::condition_variable m_cv;
        

        void init();
        void reset();
        void handleKeyInput();
        void updateGame();
        void draw();
        void displayMessage();
        void displayScore();
        void moveSnake();
        void spawnFood();
        bool isBorderCollision();
        bool isSelfCollision();
        bool isFoodCollision();

    public:
        Snake();
        ~Snake();
        void run();
};

#endif // SNAKE_H