#pragma once

#include "types.hpp"
#include "MT4ManagerAPI.h"
#include <map>
#include <boost/uuid/detail/md5.hpp>

typedef struct {
	utility::string_t server;
	int login;
	utility::string_t password;
	std::chrono::steady_clock::time_point inited_at;

	CManagerInterface *man;
	CManagerInterface *pump;
} ManagerInformation;


using ManagersDatabase = std::map<utility::string_t, ManagerInformation>;

class SessionManager {
public:
	utility::string_t CheckGetToken(const utility::string_t server, const utility::string_t login, const utility::string_t password);
	CManagerInterface* CheckGetManager(const utility::string_t key);
	CManagerInterface* CheckGetPump(const utility::string_t key);
	void     Clean();

	static void __stdcall PumpingFunc(int code, int type, void * data, void *param);
	static void ping(const boost::system::error_code& /*e*/, boost::asio::deadline_timer* /*t*/, const int ping_seconds);
	void SetupPing(const int ping_seconds);

private:
	void checkConnection(CManagerInterface* man, const utility::string_t server, const utility::string_t password, const int login);

private:
	CManagerFactory  *factory;
	bool is_ping_setup;
	boost::asio::io_service io_timer;

	boost::asio::deadline_timer *timer;

	boost::thread *thread_timer;
	int ping_seconds;
};