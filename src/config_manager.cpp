#include "stdafx.h"
#include "config_manager.hpp"

property_tree::ptree ConfigManager::_pt;

string ConfigManager::Get(const string &key) {
	return _pt.get<string>(key);
}

int ConfigManager::Get(const string &key, int def) {
	return _pt.get<int>(key);
}

bool ConfigManager::Load(const char *file) {
	try {

		ifstream ifs(file);

		property_tree::read_json(ifs, _pt);

		ifs.close();
	}
	catch (const std::exception &ex) {
		ucout << ex.what() << endl;
		return false;
	}

	return true;
}

