#pragma once

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include "picohttpparser.h"
#include "response_parser.hpp"
#include "cons.h"

using boost::asio::ip::tcp;

class async_client : public std::enable_shared_from_this<async_client> {
public:
	async_client(boost::asio::io_service& io_context) : ios_(io_context), socket_(io_context), resolver_(io_context) {
	}

	void send(const std::string& apikey, const std::string& mobile, const std::string& code) {
		write_message_ = build_message(apikey, mobile, code);
		
		tcp::resolver::query query(SERVER_ADDR, PORT);
		auto self = this->shared_from_this();
		resolver_.async_resolve(query, [this, self](boost::system::error_code ec, const tcp::resolver::iterator& it) {
			if (ec) {
				std::cout << ec.message() << std::endl;
				return;
			}

			boost::asio::async_connect(socket_, it, [this, self](boost::system::error_code ec, const tcp::resolver::iterator&) {
				if (!ec) {
					do_read();

					do_write();
				}
				else {
					std::cout << ec.message() << std::endl;
					close();
				}
			});
		});
	}

	bool has_closed() const {
		return has_closed_;
	}

private:
	std::string build_message(const std::string& apikey, const std::string& mobile, const std::string& code) {
		std::string msg;
		msg.append("apikey=").append(apikey);
		msg.append("&mobile=").append(mobile);
		msg.append("&text=").append(TEMPLATE_TEXT).append(code);

		msg = PRETEXT + "Content-Length: " + std::to_string(msg.size()) + "\r\n\r\n" + msg;
		return msg;
	}

	void do_write() {
		auto self = this->shared_from_this();
		boost::asio::async_write(socket_, boost::asio::buffer(write_message_.data(), write_message_.length()),
			[this, self](boost::system::error_code ec, std::size_t length) {
			if (!ec) {
				std::cout << "send ok " << std::endl;
			}
			else {
				std::cout << "send failed " << ec.message() << std::endl;
				close();
			}
		});
	}

	void close() {
		auto self = this->shared_from_this();
		ios_.dispatch([this, self] {
			std::cout << "close" << std::endl;
			has_closed_ = true;
			boost::system::error_code ec;
			socket_.close(ec);			
		});
	}

	void do_read() {
		auto self = this->shared_from_this();
		socket_.async_read_some(boost::asio::buffer(parser_.buffer(), parser_.left_size()),
			[this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
			if (!ec) {
				auto last_len = parser_.current_size();
				bool at_capacity = parser_.update_size(bytes_transferred);
				if (at_capacity) {
					std::cout << "out of range" << std::endl;
					close();
					return;
				}

				int ret = parser_.parse(last_len);
				if (ret == -2) {
					do_read();
				}
				else if (ret == -1) {//error
					close();
				}
				else {
					if (parser_.has_body()) {
						if (parser_.has_recieved_all())
							handle_response();
						else
							do_read_body();
					}
					else {
						handle_response();
					}
				}
			}
			else {
				close();
			}
		});
	}

	void handle_response() {
		if (parser_.status() != 200) {
			//send sms faild
			std::cout << parser_.message() << " " << parser_.body() << std::endl;
		}

		close();
	}

	void do_read_body() {
		auto self = this->shared_from_this();
		boost::asio::async_read(socket_, boost::asio::buffer(parser_.buffer(), parser_.total_len() - parser_.current_size()),
			[this, self](boost::system::error_code ec, std::size_t length) {
			if (ec) {
				close();
				return;
			}

			handle_body();
		});
	}

	void handle_body() {
		std::cout << parser_.body().length() << std::endl;
		close();
	}

private:
	boost::asio::io_service& ios_;
	tcp::resolver resolver_;
	tcp::socket socket_;
	std::string write_message_;
	response_parser parser_;
	bool has_closed_ = false;
};