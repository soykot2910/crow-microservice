#pragma once

using namespace std;
using namespace utility;
using namespace boost;

class ConfigManager
{
public:
	static bool Load(const char *file_name);
	static string Get(const string &key);
	static int Get(const string &key, int def );
private:
	static property_tree::ptree _pt;
};