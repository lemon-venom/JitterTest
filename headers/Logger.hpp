#pragma once

#include <iostream>
#include <string>
#include <sstream>

class Logger
{
public:

	Logger();
	virtual ~Logger();

	void	log(std::string);
	void	toggleLog();

private:

	bool loggingOn_;
};

extern Logger logger;
