#pragma once
#include <string>


class S3ErrorManager
{
public:
	std::string errMsg;
	
	S3ErrorManager(std::string msg)
	{
		errMsg = msg;
	};
};
