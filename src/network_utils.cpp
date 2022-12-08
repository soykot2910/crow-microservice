#include "stdafx.h"
#include "network_utils.hpp"


namespace cfx {

	HostInetInfo NetworkUtils::queryHostInetInfo() {
		io_service ios;
		tcp::resolver resolver(ios);
		tcp::resolver::query query(host_name(), "");
		return resolver.resolve(query);
	}

	utility::string_t NetworkUtils::hostIP(unsigned short family) {
		auto hostInetInfo = queryHostInetInfo();
		tcp::resolver::iterator end;
		while (hostInetInfo != end) {
			tcp::endpoint ep = *hostInetInfo++;
			sockaddr sa = *ep.data();
			if (sa.sa_family == family) {
				return utility::conversions::to_string_t(ep.address().to_string());
			}
		}
		return nullptr;
	}

	void NetworkUtils::CallBack(const utility::string_t url, const utility::string_t format, const utility::string_t key, json::value o) {
		Concurrency::task<web::http::http_response> task;
		http_client callback_client(url);

		try {
			//ucout << key << ": " << o.serialize() << endl;

			if (format == L"json") {

				auto req = json::value::object();
				req[U("type")] = json::value::string(key);
				req[U("object")] = o;

				task = callback_client.request(methods::POST, L"", req);
			}
			else {
				http_request request(methods::POST);
				request.headers().add(L"Content-Type", L"application/x-www-form-urlencoded; charset=UTF-8");
				request.set_body(key + L"=" + o.serialize());
				task = callback_client.request(request);
			}

			task.then([](http_response response) {
				//ucout << response.to_string();
				if (response.status_code() == status_codes::OK) {
					auto body = response.extract_string().get();
					ucout << body << std::endl;
				}
				}).wait();
		}
		catch (const web::http::http_exception& e) {
			ucout << e.error_code() << std::endl;
		}
		catch (const std::exception& ex) {
			ucout << ex.what() << std::endl;
		}
		catch (...) {
		}


	}
}
