#pragma once 

#include <locale>
#include <codecvt>
#include "session_manager.hpp"
#include "types.hpp"
#include "winsock_initializer.h"
#include "config_manager.hpp"
#include <pplx/pplxtasks.h>

#include <direct.h>
#define GetCurrentDir _getcwd


using namespace std;
using namespace utility;

static SessionManager session;

static std::string GetCurrentWorkingDir()
{
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
}

class MicroserviceController{
public:
	~MicroserviceController() {
		session.Clean();
	}
	void Init(const utility::string_t& host, const utility::string_t& login, const utility::string_t& password, const int ping_seconds);
	auto formatError(int code, const utility::string_t message);
	auto formatError(int code, const char* message);
	auto formatErrorRequired(utility::string_t field);

private:
	int sleep;
};
