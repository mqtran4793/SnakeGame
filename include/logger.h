#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>

#ifdef DEBUG
class Logger
{
    private:
        std::ostringstream m_message_stream;
        void log();
    public:
        Logger();
        // Overload the << operator to append messages to the internal stream
        template <typename T>
        Logger &operator<<(const T &t_message)
        {
            m_message_stream << t_message;
            log();
            return *this;
        }
};
#else
class Logger
{
    public:
        template <typename T>
        Logger &operator<<(const T &t_message)
        {
            return *this;
        }
};
#endif // DEBUG

#endif // LOGGER_H