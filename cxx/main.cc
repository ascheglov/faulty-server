// cl /std:c++latest -Ic:\_lib\boost_1_70_0 /W4 /EHsc /nologo main.cc

#define _WIN32_WINNT 0x0A00
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_REGEX_NO_LIB

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <memory>

template<class B1, class B2>
void Handle(const boost::beast::http::request<B1>& request, boost::beast::http::response<B2>& response) {
  if (request.method() != boost::beast::http::verb::get) {
    response.result(boost::beast::http::status::bad_request);
    return;
  }

  if (request.target() == "/") {
    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type, "text/html");
    boost::beast::ostream(response.body()) << "<h1>Done</h1>";
    return;
  }

  response.result(boost::beast::http::status::not_found);
  response.set(boost::beast::http::field::content_type, "text/plain");
  boost::beast::ostream(response.body()) << "File not found\r\n";
}

struct Connection : public std::enable_shared_from_this<Connection> {
  explicit Connection(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) {}

  boost::asio::ip::tcp::socket socket_;

  boost::beast::flat_buffer buffer_{8192};
  boost::beast::http::request<boost::beast::http::dynamic_body> request_;

  boost::beast::http::response<boost::beast::http::dynamic_body> response_;
};

void Start(boost::asio::ip::tcp::socket socket) {
  auto self = std::make_shared<Connection>(std::move(socket));
  boost::beast::http::async_read(
      self->socket_,
      self->buffer_,
      self->request_,
      [self](boost::beast::error_code ec, std::size_t bytes_transferred) {
          boost::ignore_unused(bytes_transferred);
          if (ec)
            return;

          self->response_.version(self->request_.version());
          self->response_.keep_alive(false);

          Handle(self->request_, self->response_);

          self->response_.set(boost::beast::http::field::content_length, self->response_.body().size());

          boost::beast::http::async_write(
              self->socket_,
              self->response_,
              [self](boost::beast::error_code ec, std::size_t) {
                  self->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
              });
      });
}

void Serve(boost::asio::ip::tcp::acceptor& acceptor, boost::asio::ip::tcp::socket& socket)
{
  acceptor.async_accept(socket, [&](boost::beast::error_code ec) {
    if (!ec)
      Start(std::move(socket));

    Serve(acceptor, socket);
  });
}

int main() {
  try {
    boost::asio::io_context context{1};

    auto address = boost::asio::ip::make_address("0.0.0.0");
    boost::asio::ip::tcp::endpoint endpoint{address, 80};
    boost::asio::ip::tcp::acceptor acceptor{context, endpoint};
    boost::asio::ip::tcp::socket socket{context};
    Serve(acceptor, socket);

    context.run();
  } catch(std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
