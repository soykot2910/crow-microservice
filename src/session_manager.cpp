#include "stdafx.h"
#include "session_manager.hpp"

ManagersDatabase managerDB;
std::mutex DBMutex;

void __stdcall SessionManager::PumpingFunc(int code, int type, void * data, void *param) {
//	if(code == PUMP_UPDATE_ONLINE)
//		cout << "user online." << endl;
}

void SessionManager::ping(const boost::system::error_code& e, boost::asio::deadline_timer* t, const int ping_seconds)
{

	unique_lock<mutex> lock{ DBMutex };

	for (const auto& item : managerDB) {
		auto end = chrono::steady_clock::now();
		auto sec = end - item.second.inited_at;

		const auto hrs = chrono::duration_cast<chrono::hours>(sec);
		const auto mins = chrono::duration_cast<chrono::minutes>(sec - hrs);
		const auto secs = chrono::duration_cast<chrono::seconds>(sec - hrs - mins);
		const auto ms = chrono::duration_cast<chrono::milliseconds>(sec - hrs - mins - secs);

		loginfo << "Ping " << item.second.server << "@" << item.second.login << ". Up for: " << hrs.count() << " hours, " << mins.count() << " mins, " << secs.count() << " secs" << endl;

		if (item.second.man) {
			int res = item.second.man->Ping();

			loginfo << "  Dealer - " << item.second.man->ErrorDescription(res) << endl;

		}


		if (item.second.pump) {
			int res = item.second.pump->Ping();

			loginfo << "  Pump - " << item.second.pump->ErrorDescription(res) << endl;
		}

	}	

	t->expires_at(t->expires_at() + boost::posix_time::seconds(ping_seconds));
	t->async_wait(boost::bind(&SessionManager::ping, boost::asio::placeholders::error, t, ping_seconds));
}

void SessionManager::SetupPing(const int ping_seconds_) {

	if (is_ping_setup) {
		return;
	}

	ping_seconds = ping_seconds_;

	timer = new boost::asio::deadline_timer(io_timer, boost::posix_time::seconds(ping_seconds));
	timer->async_wait(boost::bind(&SessionManager::ping,
		boost::asio::placeholders::error, timer, ping_seconds));

	thread_timer = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_timer));

	is_ping_setup = true;
}

utility::string_t SessionManager::CheckGetToken(const utility::string_t server, const utility::string_t login, const utility::string_t password) {

	unique_lock<mutex> lock{ DBMutex };

	CManagerInterface *man = NULL;
	CManagerInterface *pump = NULL;
	ManagerInformation info;

	utility::string_t token;

	token.append(login);

	if (managerDB.find(token) != managerDB.end()) {
		return token;
	}

	int res;

	factory = new CManagerFactory("mtmanapi64.dll");

	if (factory->WinsockStartup() != RET_OK) {
		throw std::exception("WinsockStartup failed");
	}

	if (factory->IsValid() == FALSE) {
		throw std::exception("Failed to create factory");
	}

	man = factory->Create(ManAPIVersion);

	if (!man) {
		throw std::exception("Failed to create manager");
	}

	if (man->IsConnected())
		man->Disconnect();

	pump = factory->Create(ManAPIVersion);

	if (!pump) {
		throw std::exception("Failed to create manager with pump");
	}

	if (pump->IsConnected())
		pump->Disconnect();

	size_t i;
	char _server[256];
	int _login;
	char _password[17];

	wcstombs_s(&i, _server, server.length() + 1, server.c_str(), server.length());
	wcstombs_s(&i, _password, password.length() + 1, password.c_str(), password.length());
	_login = std::stoi(login);

	if ((res = man->Connect(_server)) == RET_OK) {
		res = man->Login(_login, _password);
		if (res != RET_OK) {
			throw ManagerException(res, man->ErrorDescription(res));
		}
	}
	else {
		throw ManagerException(res, man->ErrorDescription(res));
	}

	if ((res = pump->Connect(_server)) == RET_OK) {
		res = pump->Login(_login, _password);
		if (res != RET_OK) {
			throw ManagerException(res, pump->ErrorDescription(res));
		}
	}
	else {
		throw ManagerException(res, pump->ErrorDescription(res));
	}

	if ((res = pump->PumpingSwitchEx(SessionManager::PumpingFunc, CLIENT_FLAGS_HIDENEWS | CLIENT_FLAGS_HIDEMAIL, 0)) != RET_OK) {
		throw ManagerException(res, man->ErrorDescription(res));
	}

	//cout << "getting mnager rights" << endl;
	//ConManager rights;
	//if ((res = man->ManagerRights(&rights)) != RET_OK) {
	//	throw ManagerException(res, man->ErrorDescription(res));
	//}

	//cout << "manager rights: online=" << rights.online << ";" << endl;

//	man->MemFree(rights);

	info.server = server;
	info.login = _login;
	info.password = password;
	info.man = man;
	info.pump = pump;
	info.inited_at = chrono::steady_clock::now();

	managerDB.insert( std::pair<utility::string_t, ManagerInformation>(token, info));

	return token;
}

CManagerInterface* SessionManager::CheckGetManager( const utility::string_t token) {
	unique_lock<mutex> lock{ DBMutex };

	if (managerDB.find(token) != managerDB.end()) {
		auto i = managerDB[token];
		
		checkConnection(i.man, i.server, i.password, i.login);

		return i.man;
	}

	return NULL;
}

CManagerInterface* SessionManager::CheckGetPump(const utility::string_t token) {
	unique_lock<mutex> lock{ DBMutex };

	if (managerDB.find(token) != managerDB.end()) {
		auto i = managerDB[token];

		checkConnection(i.pump, i.server, i.password, i.login);

		return i.pump;
	}

	return NULL;
}

void SessionManager::Clean() {
//	CManagerInterface *man;
//	std::map<utility::string_t, ManagerInformation>::iterator it;

	//for (it = managerDB.begin(); it != managerDB.end(); it++)
	//{

	//	man = it->second().man;


	//	if (man) {
	//		if (man->IsConnected()) {
	//			man->Disconnect();
	//		}
	//		man->Release();
	//		man = NULL;
	//	}
	//}

	if (factory)
		factory->WinsockCleanup();
}

void SessionManager::checkConnection(CManagerInterface* man, const utility::string_t server, const utility::string_t password, const int login) {
	int res;
	size_t k;

	//man->Ping();

	if (man->IsConnected())
		return;
	
	//man->Disconnect();

	char _server[256];
	char _password[17];

	wcstombs_s(&k, _server, server.length() + 1, server.c_str(), server.length());
	wcstombs_s(&k, _password, password.length() + 1, password.c_str(), password.length());

	if ((res = man->Connect(_server)) == RET_OK) {
		res = man->Login(login, _password);
		if (res != RET_OK) {
			throw ManagerException(res, man->ErrorDescription(res));
		}
	}
	else {
		throw ManagerException(res, man->ErrorDescription(res));
	}

}