#pragma once

#include <cpp_redis/cpp_redis>
#include <cpp_redis/misc/macro.hpp>

using namespace std;

class RedisClient
{
public:
	RedisClient() {	
	}
	bool Connect(const string &server, size_t port, const string &prefix );
	void Disconnect();
	void Send(const string &channel, const string &json);
	void CacheSet(const string key, const string value, bool commit = true);
	void CacheClear(const string key);
	void CacheCommit();
	void ÑacheSAdd(const string& key, const vector<string>& v);
	void CacheSAdd(const string key, const string value);
	void CacheSDel(const string key, const vector<string>& v);

	vector<string> CacheSInter(const vector<string>& keys);

private:
	const string formatKey(const string& key);

private:

	cpp_redis::client client_;
	string prefix;
	std::mutex mtx_connect;
	std::condition_variable connect_succ;
};
