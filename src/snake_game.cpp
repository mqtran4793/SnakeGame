#include "snake_game.h"

Snake::Snake()
{
    std::srand(std::time(NULL));
    m_is_running = true;
    m_buffer_window = XCreatePixmap(m_display, m_window, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, DefaultDepth(m_display, DefaultScreen(m_display)));

    init();
}

Snake::~Snake()
{
    XFreePixmap(m_display, m_buffer_window);
}

void Snake::init()
{
    m_current_direction = Direction::RIGHT;
    m_current_game_state = GameState::GAME_READY;
    /*
    NOTE
    - frame rate -> snake movement speed
    - food value -> score, snake growth rate
    TODO New features.
    - Snake movement speed based on level/score.
    - Level based on score. Ex: Level 1 - Score 0 - 100, etc...
    - Food value based on level. Ex: Level 2 - Food value 2, etc... 
    */
    m_frame_rate = 15;
    m_food_value = 5;
    m_score = 0;
    
    // Spawn snake, head at x: 20, y: 10, length is 2 segments.
    for(auto i = 20; i > 0; i -= DEFAULT_SQUARE_SIDE)
    {
        Square square = {{i, DEFAULT_SQUARE_SIDE}};
        m_snake.push_front(square);
    }

    // Spawn food.
    spawnFood();
}

void Snake::reset()
{
    m_snake.clear();
    init();
}

void Snake::handleKeyInput()
{
    while(m_is_running)
    {
        XEvent event;
        KeySym key_symbol;
        char buffer[1];
        XNextEvent(m_display, &event);

        if(event.type == KeyPress)
        {
            XLookupString(&event.xkey, buffer, sizeof(buffer), &key_symbol, NULL);
    
            if(m_current_game_state == GameState::GAME_RUNNING)
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this]{return m_direction_queue.empty();});
                switch(key_symbol)
                {
                    case XK_Up:
                    {
                        logger << "KeyPress Event: KEY_UP";
                        if(m_current_direction != Direction::DOWN)
                        {
                            m_direction_queue.push(Direction::UP);
                        }
                        break;
                    }
                    case XK_Down:
                    {
                        logger << "KeyPress Event: KEY_DOWN";
                        if(m_current_direction != Direction::UP)
                        {
                            m_direction_queue.push(Direction::DOWN);
                        }
                        break;
                    }
                    case XK_Right:
                    {
                        logger << "KeyPress Event: KEY_RIGHT";
                        if(m_current_direction != Direction::LEFT)
                        {
                            m_direction_queue.push(Direction::RIGHT);
                        }
                        break;
                    }
                    case XK_Left:
                    {
                        logger << "KeyPress Event: KEY_LEFT";
                        if(m_current_direction != Direction::RIGHT)
                        {
                            m_direction_queue.push(Direction::LEFT);
                        }
                        break;
                    }
                }
                lock.unlock();
            }
            else
            {
                switch(key_symbol)
                {
                    case XK_Escape:
                    {
                        logger << "KeyPress Event: KEY_ESCAPE";
                        m_is_running = false;
                        break;
                    }
                    case XK_space:
                    {
                        logger << "KeyPress Event: KEY_SPACE";
                        if(m_current_game_state == GameState::GAME_OVER)
                        {
                            reset();
                        }
                        m_current_game_state = GameState::GAME_RUNNING;
                        break;
                    }
                }
            }
        }
    }
    
}

void Snake::updateGame()
{
    while(m_is_running)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::chrono::milliseconds frame_duration(1000 / m_frame_rate);

        switch(m_current_game_state)
        {
            case GameState::GAME_READY:
            {
                logger << "GAME READY";
                displayMessage();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                break;
            }
            case GameState::GAME_RUNNING:
            {
                logger << "GAME RUNNING";
                auto start = std::chrono::high_resolution_clock::now();
                
                draw();
                moveSnake();
                
                if(isBorderCollision() || isSelfCollision())
                {
                    m_current_game_state = GameState::GAME_OVER;
                }
                else if(isFoodCollision())
                {
                    for(auto i = 0; i < m_food_value; i++)
                    {
                        m_snake.push_front({m_food.position.x, m_food.position.y});
                    }
                    spawnFood();
                    m_score += m_food_value;
                }
                lock.unlock();
                m_cv.notify_one();

                auto end = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
                // Calculate the remaining time to meet the desired frame duration
                auto remaining_time = frame_duration - elapsed;
                // If the frame was processed faster than the desired frame duration, sleep for the remaining time
                if(remaining_time > std::chrono::milliseconds(0))
                {
                    std::this_thread::sleep_for(remaining_time);
                }
                break;
            }
            case GameState::GAME_OVER:
            {
                logger << "GAME OVER";
                displayMessage();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                break;
            }
        }
    }
}

void Snake::draw()
{
    XSetForeground(m_display, m_graphic_context, DEFAULT_BACKGROUND_COLOR);
    XFillRectangle(m_display, m_buffer_window, m_graphic_context, 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

    // Display score.
    displayScore();

    // Draw snake.
    for(auto itr = m_snake.begin(); itr != m_snake.end(); ++itr)
    {
        drawRectangle(m_buffer_window, COLOR_RED, COLOR_WHITE, itr->position.x, itr->position.y, itr->size.width, itr->size.height);
    }

    // Draw food.
    drawRectangle(m_buffer_window, COLOR_GREEN, COLOR_WHITE, m_food.position.x, m_food.position.y, DEFAULT_SQUARE_SIDE, DEFAULT_SQUARE_SIDE);

    XCopyArea(m_display, m_buffer_window, m_window, m_graphic_context, 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, 0);
    XFlush(m_display);
}

void Snake::displayMessage()
{
    Geometry current_window = getWindowGeometry();
    XFontStruct* font_info = XLoadQueryFont(m_display, "fixed");
    int text_width;
    int x, y;
    std::string message;

    XClearWindow(m_display, m_window);

    switch(m_current_game_state)
    {
        case GameState::GAME_READY:
            message = "WELCOME TO SNAKE GAME!";
            text_width = XTextWidth(font_info, message.c_str(), message.size());
            x = (current_window.width - text_width) / 2;
            y = (current_window.height) / 2 - 100;
            drawText(m_window, x, y, COLOR_WHITE, message);
            y += 100;

            message = "Press (SPACE) to start game.";
            break;
        case GameState::GAME_OVER:
            message = "YOU LOSE!";
            text_width = XTextWidth(font_info, message.c_str(), message.size());
            x = (current_window.width - text_width) / 2;
            y = (current_window.height) / 2 - 100;
            drawText(m_window, x, y, COLOR_WHITE, message);
            y += 50;

            message = "Your highest score: " + std::to_string(m_score);
            text_width = XTextWidth(font_info, message.c_str(), message.size());
            x = (current_window.width - text_width) / 2;
            drawText(m_window, x, y, COLOR_WHITE, message);
            y += 50;

            message = "Press (SPACE) to reset game.";
            break;
    }
    text_width = XTextWidth(font_info, message.c_str(), message.size());
    x = (current_window.width / 2 - text_width) / 2 + current_window.width / 2;
    drawText(m_window, x, y, COLOR_WHITE, message);

    message = "Press (ESC) to quit game.";
    text_width = XTextWidth(font_info, message.c_str(), message.size());
    x = (current_window.width / 2 - text_width) / 2;
    drawText(m_window, x, y, COLOR_WHITE, message);
}

void Snake::displayScore()
{
    Geometry current_window = getWindowGeometry();
    XFontStruct* font_info = XLoadQueryFont(m_display, "fixed");
    int text_width;
    std::string message = "Score: " + std::to_string(m_score);

    text_width = XTextWidth(font_info, message.c_str(), message.size());
    drawText(m_buffer_window, current_window.width - text_width - 10, current_window.height - 10, COLOR_WHITE, message);
}

void Snake::moveSnake()
{
    if(!m_direction_queue.empty())
    {
        m_current_direction = m_direction_queue.front();
        m_direction_queue.pop();
    }

    Square snake_head = m_snake.front();
    switch(m_current_direction)
    {
        case Direction::UP:
            logger << "Moving: UP";
            snake_head.position.y -= (int) DEFAULT_MOVE_DISTANCE;
            break;
        case Direction::DOWN:
            logger << "Moving: DOWN";
            snake_head.position.y += (int) DEFAULT_MOVE_DISTANCE;
            break;
        case Direction::RIGHT:
            logger << "Moving: RIGHT";
            snake_head.position.x += (int) DEFAULT_MOVE_DISTANCE;
            break;
        case Direction::LEFT:
            logger << "Moving: LEFT";
            snake_head.position.x -= (int) DEFAULT_MOVE_DISTANCE;
            break;
    };

    m_snake.push_front(snake_head);
    m_snake.pop_back();
}

void Snake::spawnFood()
{
    m_food.position.x = (std::rand() % DEFAULT_WINDOW_WIDTH) / DEFAULT_SQUARE_SIDE * DEFAULT_SQUARE_SIDE;
    m_food.position.y = (std::rand() % DEFAULT_WINDOW_HEIGHT) / DEFAULT_SQUARE_SIDE * DEFAULT_SQUARE_SIDE;
}

bool Snake::isBorderCollision()
{
    Geometry current_window = getWindowGeometry();
    Square snake_head = m_snake.front();
    int current_head_x = snake_head.position.x;
    int current_head_y = snake_head.position.y;

    return (current_head_x < 0 || current_head_x >= current_window.width
            || current_head_y < 0 || current_head_y >= current_window.height);
}

bool Snake::isSelfCollision()
{
    // Snake can't collide with itself if it has less than 5 segments.
    if (m_snake.size() < 5)
    {
        return false;
    }

    Square snake_head = m_snake.front();

    // Find snake_head within given range, iterator to the end is returned if not found.
    return (std::find(std::next(m_snake.begin()), m_snake.end(), snake_head) != m_snake.end());
}

bool Snake::isFoodCollision()
{
    Square snake_head = m_snake.front();

    return (snake_head.position.x == m_food.position.x && snake_head.position.y == m_food.position.y);
}

void Snake::run()
{
    inputThread = std::thread(&Snake::handleKeyInput, this);
    gameThread = std::thread(&Snake::updateGame, this);

    inputThread.join();
    gameThread.join();
}