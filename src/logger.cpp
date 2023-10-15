#include "logger.h"

#ifdef DEBUG
Logger::Logger() {}

void Logger::log()
{
    std::cout << m_message_stream.str() << std::endl;
    // Clear the internal stringstream for the next log
    m_message_stream.str(std::string());
}

#endif // DEBUG