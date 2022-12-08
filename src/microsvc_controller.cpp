#pragma once

#include "stdafx.h"
#include "microsvc_controller.hpp"
#include "network_utils.hpp"  
#include "crow.h"
#include<iostream>
#include<Windows.h>

using namespace web;

std::mutex APIMutex;


auto MicroserviceController::formatError(int code, const utility::string_t message) {
	web::json::value result = web::json::value::object();

	result[U("message")] = web::json::value::string(message);
	result[U("code")] = web::json::value::number(code);

	return result;
}

auto MicroserviceController::formatError(int code, const char* message) {
	wstring msg(message, message + strlen(message));

	return formatError(code, msg);
}

auto MicroserviceController::formatErrorRequired(utility::string_t field) {
	utility::string_t msg(field);

	msg.append(U(" is required"));

	return formatError(402, msg);
}

void MicroserviceController::Init(const utility::string_t& server, const utility::string_t& login, const utility::string_t& password, const int ping_seconds)
{
	utility::string_t token;

	token = session.CheckGetToken(server, login, password);

	session.SetupPing(ping_seconds);

	loginfo << "Inited " << server << " : " << login << "; token: " << token << "; ping: " << ping_seconds << " seconds" << endl;
}
