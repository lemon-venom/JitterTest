#include "..\headers\Logger.hpp"

Logger logger;

Logger::Logger()
{
	loggingOn_ = false;
}

Logger::~Logger()
{
}

void Logger::log(std::string message)
{
	if (loggingOn_) {
		std::cout << message << std::endl;
	}
}

void Logger::toggleLog()
{
	loggingOn_ = !loggingOn_;
}