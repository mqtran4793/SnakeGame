#include "snake_game.h"

Snake::Snake()
{
    std::srand(std::time(NULL));
    m_game_speed = 10000;
    m_is_running = true;
    init();
}

void Snake::init()
{
    m_current_direction = Direction::RIGHT;
    m_current_game_state = GameState::GAME_READY;
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

void Snake::draw()
{
    // Draw snake.
    for(auto itr = m_snake.begin(); itr != m_snake.end(); ++itr)
    {
        drawRectangle(COLOR_RED, COLOR_WHITE, itr->position.x, itr->position.y, itr->size.width, itr->size.height);
    }

    // Draw food.
    drawRectangle(COLOR_GREEN, COLOR_WHITE, m_food.position.x, m_food.position.y, DEFAULT_SQUARE_SIDE, DEFAULT_SQUARE_SIDE);
}

void Snake::displayMessage()
{
    XFontStruct* font_info = XLoadQueryFont(m_display, "fixed");
    int text_width;
    int x, y;
    std::string message;

    switch(m_current_game_state)
    {
        case GameState::GAME_READY:
            message = "WELCOME TO SNAKE GAME!";
            text_width = XTextWidth(font_info, message.c_str(), message.size());
            x = (DEFAULT_WINDOW_WIDTH - text_width) / 2;
            y = (DEFAULT_WINDOW_HEIGHT - text_width) / 2 - 50;
            drawText(x, y, COLOR_WHITE, message);
            y += 100;

            message = "Press (SPACE) to start game.";
            break;
        case GameState::GAME_OVER:
            message = "YOU LOSE!";
            text_width = XTextWidth(font_info, message.c_str(), message.size());
            x = (DEFAULT_WINDOW_WIDTH - text_width) / 2;
            y = (DEFAULT_WINDOW_HEIGHT - text_width) / 2 - 50;
            drawText(x, y, COLOR_WHITE, message);
            y += 50;

            message = "Your highest score: " + std::to_string(m_score);
            text_width = XTextWidth(font_info, message.c_str(), message.size());
            x = (DEFAULT_WINDOW_WIDTH - text_width) / 2;
            drawText(x, y, COLOR_WHITE, message);
            y += 50;

            message = "Press (SPACE) to reset game.";
            break;
    }
    text_width = XTextWidth(font_info, message.c_str(), message.size());
    x = (DEFAULT_WINDOW_WIDTH / 2 - text_width) / 2 + DEFAULT_WINDOW_WIDTH / 2;
    drawText(x, y, COLOR_WHITE, message);

    message = "Press (ESC) to quit game.";
    text_width = XTextWidth(font_info, message.c_str(), message.size());
    x = (DEFAULT_WINDOW_WIDTH / 2 - text_width) / 2;
    drawText(x, y, COLOR_WHITE, message);
}

void Snake::displayScore()
{
    XFontStruct* font_info = XLoadQueryFont(m_display, "fixed");
    int text_width;
    std::string message = "Score: " + std::to_string(m_score);

    text_width = XTextWidth(font_info, message.c_str(), message.size());
    drawText(DEFAULT_WINDOW_WIDTH - text_width - 10, DEFAULT_WINDOW_HEIGHT - 10, COLOR_WHITE, message);
}

void Snake::handleKeyPress(XKeyEvent t_key_event)
{
    KeySym key_symbol;
    char buffer[1];
    XLookupString(&t_key_event, buffer, sizeof(buffer), &key_symbol, NULL);
    
    if(m_current_game_state == GameState::GAME_RUNNING)
    {
        switch(key_symbol)
        {
            case XK_Up:
                logger << "KeyPress Event: KEY_UP";
                if(m_current_direction != Direction::DOWN)
                {
                    m_direction_queue.push(Direction::UP);
                }
                break;
            case XK_Down:
                logger << "KeyPress Event: KEY_DOWN";
                if(m_current_direction != Direction::UP)
                {
                    m_direction_queue.push(Direction::DOWN);
                }
                break;
            case XK_Right:
                logger << "KeyPress Event: KEY_RIGHT";
                if(m_current_direction != Direction::LEFT)
                {
                    m_direction_queue.push(Direction::RIGHT);
                }
                break;
            case XK_Left:
                logger << "KeyPress Event: KEY_LEFT";
                if(m_current_direction != Direction::RIGHT)
                {
                    m_direction_queue.push(Direction::LEFT);
                }
                break;
        }
    }
    else
    {
        switch(key_symbol)
        {
            case XK_Escape:
                logger << "KeyPress Event: KEY_ESCAPE";
                m_is_running = false;
                break;
            case XK_space:
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

bool Snake::isBorderCollision()
{
    auto snake_head = m_snake.front();
    int current_head_x = snake_head.position.x;
    int current_head_y = snake_head.position.y;
    Geometry current_window = getWindowGeometry();

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

    auto snake_head = m_snake.front();

    // Find snake_head within given range, iterator to the end is returned if not found.
    return (std::find(std::next(m_snake.begin()), m_snake.end(), snake_head) != m_snake.end());
}

bool Snake::isFoodCollision()
{
    auto snake_head = m_snake.front();

    return (snake_head.position.x == m_food.position.x && snake_head.position.y == m_food.position.y);
}

void Snake::run()
{
    XEvent event;

    while(m_is_running)
    {
        XClearWindow(m_display, m_window);

        if(XPending(m_display))
        {
            XNextEvent(m_display, &event);
            if(event.type == KeyPress)
            {
                handleKeyPress(event.xkey);
            }
        }

        switch(m_current_game_state)
        {
            case GameState::GAME_READY:
                logger << "GAME READY";
                displayMessage();
                break;
            case GameState::GAME_RUNNING:
                logger << "GAME RUNNING";
                draw();
                displayScore();
                moveSnake();
                
                if(isBorderCollision() || isSelfCollision())
                {
                    m_current_game_state = GameState::GAME_OVER;
                }
                else if(isFoodCollision())
                {
                    m_snake.push_front({m_food.position.x, m_food.position.y});
                    spawnFood();
                    m_score++;
                }
                break;
            case GameState::GAME_OVER:
                logger << "GAME OVER";
                displayMessage();
                break;
        }
        
        XFlush(m_display);
        usleep(m_game_speed);
    }
}

void Snake::moveSnake()
{
    if(!m_direction_queue.empty())
    {
        m_current_direction = m_direction_queue.front();
        m_direction_queue.pop();
    }

    auto snake_head = m_snake.front();
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