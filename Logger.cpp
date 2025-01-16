#include "Logger.hpp"
#include <fstream>
namespace Logger
{
	// append the msg into log file
	void log(const char* msg, Serverity serverity)
	{
		std::ofstream out(LOGFILE_NAME, std::ios::app);

		out << "\n Serverity: [" << serverityName.at(serverity) << "]\n";
		out << "Message: " << msg << std::endl;
	}
	void log(const std::string& msg, Serverity serverity)
	{
		Logger::log(msg.c_str(), serverity);
	}
}