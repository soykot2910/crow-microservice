#include "stdafx.h"
#include "pump_manager.hpp"
#include "types.hpp"
#include "network_utils.hpp"

using namespace web;
using namespace cfx;
using namespace http;

CManagerInterface* PumpManager::pump = NULL;
RedisClient* PumpManager::redis = NULL;
CManagerInterface* man;

static int prev_call = 0;
static int prev_call2 = 0;
static int prev_call3 = 0;

void __stdcall PumpManager::PumpingFunc(int code, int type, void* data, void* param)
{
	//cout << "code: " << code << endl;
	//cout << "type: " << type << endl;
	UserRecord user;
	TradeRecord trade;
	json::value o;
	jSymbol s;
	ConSymbol symbol;
	wstring grp;
	utility::string_t t;
	int total = 0;
	int call_time = 0;
	int call_time2 = 0;
	int call_time3 = 0;
	TradeRecord* trades = NULL;
	MarginLevel* margins = NULL;


	switch (code) {
	case PUMP_UPDATE_ONLINE:
	case PUMP_UPDATE_GROUPS:
	case PUMP_START_PUMPING:
	case PUMP_STOP_PUMPING:
		break;
	case PUMP_UPDATE_BIDASK:
	{


		call_time2 = GetTickCount();

		//ucout << "Tick Count " << call_time2 << " Prev Tick" << prev_call2 << " Substraction " << abs(call_time2 - prev_call2) << endl;

		if (abs(call_time2 - prev_call2) > 2000) // 2 seconds
		{
			//ucout << "" << endl;
		}
		else {
			return;
		}
		prev_call2 = call_time2;

		pump->Ping();


		try {

			loginfo << "Set  User Margins Process starts" << endl;

			margins = pump->MarginsGet(&total);

			for (int i = 0; i < total; i++) {

				int login = margins[i].login;

				int res;
				UserRecord user = { 0 };

				res = pump->UserRecordGet(login, &user);

				if (res != RET_OK) {
					throw ManagerException(res, pump->ErrorDescription(res));
				}

				redis->CacheSet("margin:" + std::to_string(margins[i].login), ws2s(JMarginLevel::FromMarginLevel(margins[i], user).AsJSON().serialize()), false);
			}

			call_time3 = GetTickCount();
			loginfo << "Margin Set Ends" << endl;

			if (abs(call_time3 - prev_call3) > 1000000) // 1000 seconds
			{
				ucout << "Deleting orders from cache " << endl;
				prev_call3 = call_time3;
				for (int i = 0; i < total; i++) {
					ucout << "DEL orders:" << margins[i].login << ":working" << endl;
					redis->CacheClear("orders:" + std::to_string(margins[i].login) + ":working");
				}
			}

			if (total > 0)
				redis->CacheCommit();
			pump->MemFree(margins);
			margins = NULL;
			total = 0;

		}
		catch (const std::exception& ex) {
			ucout << ex.what() << endl;
			logerror << ex.what() << endl;
		}

		call_time = GetTickCount();


		if (abs(call_time - prev_call) > 100000) // 100 seconds
		{
			ucout << "============= Setting All users margin ========== " << endl;

			int allTotal = 1;
			UserRecord* users = NULL;
			users = pump->UsersGet(&allTotal);


			if (users != NULL) {
				for (int i = 0; i < allTotal; i++) {
					int login = users[i].login;
					int res;
					UserRecord user = { 0 };
					res = pump->UserRecordGet(login, &user);
					if (res != RET_OK) {
						throw ManagerException(res, pump->ErrorDescription(res));
					}


					MarginLevel level = { 0 };
					JMarginLevel jLevel;
					int resMargin = pump->MarginLevelGet(login, user.group, &level);

					if (resMargin == RET_OK) {
						jLevel = JMarginLevel::FromMarginLevel(level, user);
					}

					web::json::value result = web::json::value::object();

					if (jLevel.equity != 0) {
						result[U("equity")] = web::json::value::number(jLevel.equity);
					}
					else {
						result[U("equity")] = web::json::value::number(user.balance);
					}

					result[U("balance")] = web::json::value::number(user.balance);
					result[U("last_day_balance")] = web::json::value::number(user.prevbalance);
					result[U("last_month_balance")] = web::json::value::number(user.prevmonthbalance);

					redis->CacheSet("margin:" + std::to_string(users[i].login), ws2s(result.serialize()), false);


				}
				cout << "========================== Margin updated done ====================" << endl;

			}
			else {
				cout << "user not" << endl;
			}


		}
		else {
			return;
		}
		prev_call = call_time;


		try {
			trades = pump->TradesGet(&total);

			auto time = std::time(nullptr);
			//wcout << std::put_time(std::gmtime(&time), L"%F %T") << L" :: PUMP_UPDATE_BIDASK. found " << total << L" trades" << endl;
			for (int i = 0; i < total; i++) {
				redis->CacheSet("orderpl:" + std::to_string(trades[i].order), std::to_string(trades[i].profit), false);
				redis->CacheSAdd("orders:" + std::to_string(trades[i].login) + ":working", std::to_string(trades[i].order));
			}

			if (total > 0)
				redis->CacheCommit();
			pump->MemFree(trades);
			trades = NULL;
			total = 0;
			//smembers orders:19989624:working

		}
		catch (const std::exception& ex) {
			logerror << ex.what() << endl;
		}


	}

	break;
	case PUMP_UPDATE_SYMBOLS:
		if (data == NULL)
			return;
		try {
			symbol = *(ConSymbol*)data;
			ucout << L"PUMP_UPDATE_SYMBOLS " << symbol.symbol << endl;

			s = jSymbol::FromConSymbol(symbol);
			o = s.AsJSON();

			redis->CacheSet("symbol:" + ws2s(s.name), ws2s(o.serialize()));
			callBack(L"symbol", o);
		}
		catch (const std::exception& ex) {
			logerror << ex.what() << endl;
		}
		break;
	case PUMP_UPDATE_USERS:
		if (data == NULL)
			return;
		try {
			user = *(UserRecord*)data;
			loginfo << L"PUMP_UPDATE_USERS " << user.login << endl;
			loginfo << "url: " << callback_url << endl;
			loginfo << "format: " << callback_format << endl;

			o = JUser::FromUserRecord(user).AsJSON();

			redis->CacheSet("user:" + std::to_string(user.login), ws2s(o.serialize()));
			callBack(L"user", o);
		}
		catch (const std::exception& ex) {
			logerror << ex.what() << endl;
		}

		break;
	case PUMP_UPDATE_TRADES:
		if (data == NULL)
			return;

		try {
			total = 0;

			margins = pump->MarginsGet(&total);

			//for (int i = 0; i < total; i++) {
				//ucout << L"margin:" << std::to_wstring(margins[i].login) << jMargin::FromMarginLevel(margins[i]).AsJSON().serialize() << endl;
			//}

			pump->MemFree(margins);
			margins = NULL;
			total = 0;

			trade = *(TradeRecord*)data;
			loginfo << "PUMP_UPDATE_TRADES" << trade.order << trade.symbol << endl;

			o = jTrade::FromTradeRecord(trade).AsJSON();

			redis->CacheSet("order:" + std::to_string(trade.order), ws2s(o.serialize()));

			ucout << "Trade cmd " << trade.cmd << "trade state " << trade.state << endl;

			if (trade.cmd == OP_BUY || trade.cmd == OP_SELL) {
				ucout << "Buy or Sell order placed -> ticket " << std::to_wstring(trade.order) << endl;

				if (trade.state == TS_OPEN_NORMAL || trade.state == TS_OPEN_REMAND || trade.state == TS_OPEN_RESTORED) {
					ucout << "TS OPEN Normal -> add to working order" << endl;
					redis->CacheSAdd("orders:" + std::to_string(trade.login) + ":working", std::to_string(trade.order));
				}
				if (trade.state == TS_CLOSED_NORMAL || trade.state == TS_CLOSED_PART || trade.state == TS_CLOSED_BY || trade.state == TS_DELETED) {
					ucout << " add to smembers" << endl;

					redis->CacheSAdd("orders:" + std::to_string(trade.login) + ":closed", std::to_string(trade.order));
					vector<string> v;
					v.push_back("orders:" + std::to_string(trade.login) + ":working");
					v.push_back("orders:" + std::to_string(trade.login) + ":closed");

					vector<string> inter = redis->CacheSInter(v);
					if (inter.size() > 0)
						ucout << "delete order from :working" << endl;

					redis->CacheSDel("orders:" + std::to_string(trade.login) + ":working", inter);
					//cacheDel("orderpl:" + trade.order);
				}
			}

			callBack(L"order", o);
		}
		catch (const std::exception& ex) {
			ucout << ex.what() << endl;
			logerror << ex.what() << endl;
		}

		break;
	case PUMP_UPDATE_ACTIVATION:
		break;
	case PUMP_UPDATE_MARGINCALL:
		break;
	case PUMP_UPDATE_REQUESTS:
		break;
	case PUMP_UPDATE_PLUGINS:
		break;
	case PUMP_UPDATE_NEWS:
		break;
	case PUMP_UPDATE_MAIL:
		break;
	default: break;
	}
	//---
}

void PumpManager::Init(const string mt4_host_, const string mt4_login_, const string mt4_password_,
	const string redis_host_, int redis_port_, bool redis_enabled_,
	const string call_back_url_, const string call_back_format_, bool call_back_enabled_)
{

	mt4_host = mt4_host_;
	mt4_login = atoi(mt4_login_.c_str());
	mt4_password = mt4_password_;

	callback_url = s2ws(call_back_url_);
	callback_format = s2ws(call_back_format_);
	call_back_enabled = call_back_enabled_;

	redis_host = redis_host_;
	redis_port = redis_port_;
	redis_enabled = redis_enabled_;

	connectRedis();
	connectManager();
}

void PumpManager::connectManager()
{
	int res;

	factory = new CManagerFactory("mtmanapi64.dll");

	if (factory->WinsockStartup() != RET_OK) {
		throw exception("WinsockStartup failed");
	}

	if (factory->IsValid() == FALSE) {
		throw exception("Failed to create factory");
	}

	pump = factory->Create(ManAPIVersion);

	if (!pump) {
		throw exception("Failed to create manager with pump");
	}

	if (pump->IsConnected())
		pump->Disconnect();

	if ((res = pump->Connect(mt4_host.c_str())) == RET_OK) {
		res = pump->Login(mt4_login, mt4_password.c_str());
		if (res != RET_OK) {
			throw ManagerException(res, pump->ErrorDescription(res));
		}
	}
	else {
		throw ManagerException(res, pump->ErrorDescription(res));
	}

	if ((res = pump->PumpingSwitchEx(PumpManager::PumpingFunc, CLIENT_FLAGS_HIDENEWS | CLIENT_FLAGS_HIDEMAIL, 0)) != RET_OK) {
		throw ManagerException(res, pump->ErrorDescription(res));
	}

}

void PumpManager::connectRedis()
{
	redis = new RedisClient();

	if (!redis_enabled)
		return;

	if (redis->Connect(redis_host, redis_port, "") == false) {
		throw exception("Failed connect to redis");
	}
}
