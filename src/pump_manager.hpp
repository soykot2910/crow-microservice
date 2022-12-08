#pragma once

#include "stdafx.h"
#include "redis_client.hpp"
#include "network_utils.hpp"

using namespace std;
using namespace cfx;
using namespace web;

static RedisClient redis;
static utility::string_t callback_url;
static utility::string_t callback_format;
static bool call_back_enabled;

class PumpManager {
public:
	void Init(const string manager_host, const string manager_login, const string manager_password,
		const string redis_host, int redis_port, bool redis_enabled_,
		const string call_back_url_, const string call_back_format_, bool call_back_enabled_);
	static void __stdcall PumpingFunc(int code, int type, void* data, void* param);
	void connectManager();
	void connectRedis();
	static void callBack(const utility::string_t key, json::value o) {
		if(call_back_enabled)
			NetworkUtils::CallBack(callback_url, callback_format, key, o);
	}

private:
	string mt4_host;
	int mt4_login;
	string mt4_password;

	static RedisClient* redis;
	string redis_host;
	int redis_port;
	bool redis_enabled;

	static CManagerInterface* pump;
	CManagerFactory* factory;

};