#include "stdafx.h"

#include "logger.hpp"
#include "usr_interrupt_handler.hpp"
#include "microsvc_controller.hpp"
#include "crow.h"
//#include <boost/stacktrace.hpp>

using namespace web;
using namespace cfx;

int wmain(int argc, wchar_t *argv[]) {
	InterruptHandler::hookSIGINT();

	MicroserviceController server;

	try {

		ConfigManager::Load("config.json");

		string address = ConfigManager::Get("server.url");//"https://host_auto_ip4:");
		string call_back_url = ConfigManager::Get("pump.call_back.url");
		string call_back_format = ConfigManager::Get("pump.call_back.format");
		int call_back_enabled = ConfigManager::Get("pump.call_back.enabled", 0);

		string redis_host = ConfigManager::Get("pump.redis.host");
		int redis_port = ConfigManager::Get("pump.redis.port", 6379);
		int redis_enabled = ConfigManager::Get("pump.redis.enabled", 0);

		int pump_enabled = ConfigManager::Get("pump.enabled", 0);
		string pump_host = ConfigManager::Get("pump.manager.host");
		string pump_manager_login = ConfigManager::Get("pump.manager.login");
		string pump_manager_password = ConfigManager::Get("pump.manager.password");

		string init_host = ConfigManager::Get("init.manager.host");
		string init_manager_login = ConfigManager::Get("init.manager.login");
		string init_manager_password = ConfigManager::Get("init.manager.password");
		int ping_seconds = ConfigManager::Get("init.manager.ping_seconds", 30);
		int init_enabled = ConfigManager::Get("init.enabled", 0);		

		logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
		std::ifstream file("logger.ini");
		logging::init_from_stream(file);
		logging::add_common_attributes();

		loginfo << "MetaTrader4 Microservice trying to start at: " << address << endl;

		crow::SimpleApp app;

		CROW_ROUTE(app, "/")([]() {
			return "Hello World";
			});

		app.port(18080).multithreaded().run();

		if (init_enabled) {
			loginfo << "Initing manager..." << endl;
			server.Init(s2ws(init_host), s2ws(init_manager_login), s2ws(init_manager_password), ping_seconds);
		}

		


		InterruptHandler::waitForUserInterrupt();

		loginfo << "MetaTrader4 Microservice is shutdown" << endl;
	}
	catch (std::exception & e) {
		logerror << e.what() << endl;
	}

	catch (...) {
	//	std::cerr << boost::stacktrace::stacktrace() << endl;
	//	logerror << boost::stacktrace::stacktrace() << endl;
	}

	return 0;
}
