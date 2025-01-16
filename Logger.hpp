#pragma once
#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>
#include <map>
namespace Logger
{
	const std::string LOGFILE_NAME = "log.txt";
	enum Serverity
	{
		HIGH,
		MEDIUM,
		LOW,
		INFO,
		WARNING
	};
	const std::map<Serverity, const std::string> serverityName
	{
		{HIGH, "HIGH"},
		{MEDIUM,"MEDIUM"},
		{LOW,"LOW"},
		{INFO,"INFO"},
		{WARNING, "WARNING"}
	};
	void log(const char* msg, Serverity serverity = INFO);
	void log(const std::string& msg, Serverity serverity = INFO);

}

#endif