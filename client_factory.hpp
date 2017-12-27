#pragma once
#include <mutex>
#include <list>
#include <boost/asio.hpp>
#include "async_client.hpp"

class client_factory {
public:
	static client_factory & instance() {
		static client_factory instance;
		return instance;
	}

	auto new_client() {
		auto client = std::make_shared<async_client>(ios_);

		{
			std::unique_lock<std::mutex> lock(mtx_);
			v_.push_back(client);
		}

		return client;
	}

	void run() {
		ios_.run();
	}

	void stop() {
		timer_.cancel();
		ios_.stop();
	}

private:
	client_factory() : work_(ios_), timer_(ios_){
		check();
	}

	void check() {
		timer_.expires_from_now(boost::posix_time::seconds(10*60));
		timer_.async_wait([this](boost::system::error_code const& ec) {
			if (ec) {
				return;
			}

			{
				std::unique_lock<std::mutex> lock(mtx_);
				if (!v_.empty())
					v_.remove_if([](auto client) { return client->has_closed(); });
			}

			check();
		});
	}

	~client_factory() { ios_.stop(); }

	client_factory(const client_factory&) = delete;
	client_factory& operator=(const client_factory&) = delete;
	client_factory(client_factory&&) = delete;
	client_factory& operator=(client_factory&&) = delete;

	boost::asio::io_service ios_;
	boost::asio::io_service::work work_;

	std::list<std::shared_ptr<async_client>> v_;
	std::mutex mtx_;

	boost::asio::deadline_timer timer_;
};
