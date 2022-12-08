#include "stdafx.h"
#include "redis_client.hpp"

const std::string RedisClient::formatKey(const string& key) {
	string t(prefix);

	t.append(key);

	return t;
}

const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	localtime_s(&tstruct, &now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

bool RedisClient::Connect(const string &server, size_t port, const string &prefix_) {

	winsock_initializer winsock_init;
	prefix = prefix_;

	cpp_redis::active_logger = unique_ptr<cpp_redis::logger>(new cpp_redis::logger);
	client_.connect(server, port,
		[&](const string &host, size_t port, cpp_redis::client::connect_state status) {
		if (status == cpp_redis::client::connect_state::dropped) {
			loginfo << " redis disconnected from " << host << ":" << port << ". reconnecting" << endl;
		}

		if (status == cpp_redis::client::connect_state::start) {
			loginfo << "redis connecting to " << host << ":" << port << endl;
		}

		if (status == cpp_redis::client::connect_state::failed || status == cpp_redis::client::connect_state::lookup_failed) {
			logerror << "redis failed connect to " << host << ":" << port << endl;
		}

		if (status == cpp_redis::client::connect_state::ok) {
			loginfo << "redis connected to " << host << ":" << port << endl;

			connect_succ.notify_all();
		}
	}, 10000, 1000, 1000);

	std::unique_lock<std::mutex> lk(mtx_connect);
	return connect_succ.wait_for(lk, std::chrono::seconds(30), [this] {
		return client_.is_connected();
	});
}

void RedisClient::Disconnect() {
	client_.disconnect();
}

void RedisClient::Send(const string &channel, const string &json) {

	try {

		if (!client_.is_connected())
			return;
			
		client_.publish(formatKey(channel), json, [](cpp_redis::reply &r) {
			//		cout << "reply: " << r.as_string() << endl;

			if (r.is_error()) {
				logerror << "redis publish error " << r.error() << endl;
			}

		});

		client_.commit();
	} 
	catch (const cpp_redis::redis_error &e) {
		logerror << "redis publish error: " << e.what() << endl;
	}
}

void RedisClient::CacheSet(const string key, const string value, bool commit) {

	if (!client_.is_connected())
		return;

	client_.set(formatKey(key), value);
	if (commit)
		client_.commit();
}

void RedisClient::CacheClear(const string key) {
	if (!client_.is_connected())
		return;

	vector<string> v;
	v.push_back(key);
	client_.del(v);

	client_.commit();
}


void RedisClient::CacheCommit() {
	if (!client_.is_connected())
		return;
	client_.commit();
}

void RedisClient::CacheSAdd(const string key, const string value)
{
	if (!client_.is_connected())
		return;

	vector<string> v;
	v.push_back(value);
	client_.sadd(key, v);
	client_.commit();

}

void RedisClient::CacheSDel(const string key, const vector<string>& v)
{
	if (!client_.is_connected())
		return;

	client_.srem(key, v);
	client_.commit();
}

vector<string> RedisClient::CacheSInter(const vector<string>& keys)
{
	if (!client_.is_connected())
		return vector<string>();

	auto r = client_.sinter(keys);

	client_.commit();

	auto arr = r.get().as_array();
	vector<string> v;

	for (auto iter = arr.begin(); iter != arr.end(); iter++) {
		v.push_back(iter->as_string());
	}

	return v;
}

void RedisClient::ÑacheSAdd(const string& key, const vector<string>& v)
{
	if (!client_.is_connected())
		return;

	client_.sadd(key, v);
	client_.commit();
}

