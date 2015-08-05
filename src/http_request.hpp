/*
* HTTP Request
*
* http://tylerobrien.com
* https://github.com/TylerOBrien/Cpp-HTTP-Request
*
* Copyright (c) 2015 Tyler O'Brien
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* */

#ifndef _TYLEROBRIEN_HTTP_REQUEST_HPP_
#define _TYLEROBRIEN_HTTP_REQUEST_HPP_

#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind/bind.hpp>
#include <boost/format.hpp>

namespace http {

const std::size_t BUFFER_NBYTES = 32768;

enum error {
    RESOLVE = -1, CONNECT = -2, SEND = -3, READ = -4
};

/*
 * Function callback types.
 * */

typedef void(*ResponseFn)(std::string);
typedef void(*ErrorFn)(int,boost::system::error_code);
typedef void(*ResponseRefFn)(std::string&);
typedef void(*ErrorRefFn)(int,boost::system::error_code&);
typedef void(*ResponseConstRefFn)(const std::string&);
typedef void(*ErrorConstRefFn)(int,const boost::system::error_code&);

/*
 * Type shortcuts.
 * */

typedef boost::asio::streambuf Buffer;
typedef boost::asio::ip::tcp::socket Socket;
typedef boost::asio::ip::tcp::resolver Resolver;
typedef boost::asio::ip::tcp::resolver::query Query;

/*
 * HTTP Request Templates
 * */

const char RAW_REQUEST_BASIC[] =
"%s HTTP/1.1\r\n\
Host: %s\r\n\
Accept: %s\r\n\
Connection: close\r\n\r\n";

const char RAW_REQUEST_CONTENT[] =
"%s HTTP/1.1\r\n\
Host: %s\r\n\
Accept: %s\r\n\
Connection: close\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
Content-Length: %u\r\n\r\n%s\r\n\r\n";

/*
 * BasicErrorHandler
 * */

struct BasicErrorHandler {
    void operator () (int, const boost::system::error_code&) {}
};

/*
 * Handler
 * */

template<
    class ResponseHdl,
    class ErrorHdl>
struct Handler {
    ResponseHdl response;
    ErrorHdl error;

    Handler(
        const ResponseHdl& response_handler,
        const ErrorHdl& error_handler)
    : response(response_handler),
        error(error_handler)
    {}
};

/*
 * Request
 * */

template <
    class ResponseHdl,
    class ErrorHdl>
struct Request
{
    typedef Handler<ResponseHdl,ErrorHdl> Handler;

    Resolver resolver;
    Query query;
    Socket socket;
    std::string str;
    Handler handler;

    Request(
        boost::asio::io_service& ioservice,
        const std::string& host,
        const std::string& command,
        const std::string& accept,
        const Handler& handler)
    : resolver(ioservice),
      socket(ioservice),
      query(Query(host,"80")),
      handler(handler)
    {
        str = boost::str(
            boost::format(RAW_REQUEST_BASIC)
            % command % host % accept
        );
    }

    Request(
        boost::asio::io_service& ioservice,
        const std::string& host,
        const std::string& command,
        const std::string& content,
        const std::string& accept,
        const Handler& handler)
    : resolver(ioservice),
      socket(ioservice),
      query(Query(host,"80")),
      handler(handler)
    {
        str = boost::str(
            boost::format(RAW_REQUEST_CONTENT)
            % command % host % accept % content.size() % content
        );
    }
};

/*
 * make_buffer() returns foo
 *
 * Input:
    * nbytes    Size of the buffer.
 *
 * Creates a new buffer, on the heap, and returns a pointer to it.
 * */
inline boost::shared_ptr<Buffer> make_buffer(
    std::size_t nbytes)
{
    return boost::shared_ptr<Buffer>(new Buffer(nbytes));
}

/*
 * make_handler() returns foo
 *
 * Input:
    *
 *
 * */
template<
    class ResponseHdl>
inline Handler<ResponseHdl,BasicErrorHandler> make_handler(
    const ResponseHdl& response)
{
    return Handler<ResponseHdl,BasicErrorHandler>(response, BasicErrorHandler());
}

/*
 * make_handler() returns foo
 *
 * Input:
    *
 *
 * */
template<
    class ResponseHdl,
    class ErrorHdl>
inline Handler<ResponseHdl,ErrorHdl> make_handler(
    const ResponseHdl& response,
    const ErrorHdl& error)
{
    return Handler<ResponseHdl,ErrorHdl>(response, error);
}

/*
 * TODO:
 * Fix the error being caused by these two overloads.
 */

/*
 * make_request()
 *
 * Input:
    *
 *
 * */
/*template <
    class ResponseHdl,
    class ErrorHdl>
inline boost::shared_ptr<Request<ResponseHdl,ErrorHdl>> make_request(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const Handler<ResponseHdl,ErrorHdl>& handler)
{
    return boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>(
        new Request<ResponseHdl,ErrorHdl>(
            ioservice, host, command, "text/html", handler
        )
    );
}*/

/*
 * make_request()
 *
 * Input:
    *
 *
 * */
/*template <
    class ResponseHdl,
    class ErrorHdl>
inline boost::shared_ptr<Request<ResponseHdl,ErrorHdl>> make_request(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const std::string& accept,
    const Handler<ResponseHdl,ErrorHdl>& handler)
{
    return boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>(
        new Request<ResponseHdl,ErrorHdl>(
            ioservice, host, command, accept, handler
        )
    );
}*/

/*
 * make_request() returns foo
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl>
inline boost::shared_ptr<Request<ResponseHdl,BasicErrorHandler>> make_request(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const std::string& content,
    const ResponseHdl& response_hdl)
{
    return boost::shared_ptr<Request<ResponseHdl,BasicErrorHandler>>(
        new Request<ResponseHdl,BasicErrorHandler>(
            ioservice, host, command, content, "text/html", make_handler(response_hdl,BasicErrorHandler())
        )
    );
}

/*
 * make_request() returns foo
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
inline boost::shared_ptr<Request<ResponseHdl,ErrorHdl>> make_request(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const ResponseHdl& response_hdl,
    const ErrorHdl& error_hdl)
{
    return boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>(
        new Request<ResponseHdl,ErrorHdl>(
            ioservice, host, command, "text/html", make_handler(response_hdl,error_hdl)
        )
    );
}

/*
 * make_request() returns foo
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
inline boost::shared_ptr<Request<ResponseHdl,ErrorHdl>> make_request(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const std::string& accept,
    const ResponseHdl& response_hdl,
    const ErrorHdl& error_hdl)
{
    return boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>(
        new Request<ResponseHdl,ErrorHdl>(
            ioservice, host, command, accept, make_handler(response_hdl,error_hdl)
        )
    );
}

/*
 * make_request() returns foo
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
inline boost::shared_ptr<Request<ResponseHdl,ErrorHdl>> make_request(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const std::string& content,
    const std::string& accept,
    const ResponseHdl& response_hdl,
    const ErrorHdl& error_hdl)
{
    return boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>(
        new Request<ResponseHdl,ErrorHdl>(
            ioservice, host, command, content, accept, make_handler(response_hdl,error_hdl)
        )
    );
}

/*
 * read() returns nothing
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
void read(
    const boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>& request,
    const boost::shared_ptr<std::string>& response
);

/*
 * handle_read() returns nothing
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
void handle_read(
    const boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>& request,
    boost::shared_ptr<std::string>& response,
    boost::shared_ptr<Buffer> buffer,
    const boost::system::error_code& errcode)
{
    if (!errcode || errcode.value() == 2) {
        Buffer::const_buffers_type data = buffer->data();
        *response += std::string(
            boost::asio::buffers_begin(data), boost::asio::buffers_begin(data) + buffer->size()
        );
        if (!errcode) {
            read(request, response);
        } else {
            request->handler.response(*response);
        }
    } else {
        request->handler.error(error::READ, errcode);
    }
}

/*
 * read() returns nothing
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
void read(
    const boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>& request,
    const boost::shared_ptr<std::string>& response
    )
{
    boost::shared_ptr<Buffer> buffer = make_buffer(BUFFER_NBYTES);
    boost::asio::async_read(request->socket, *buffer,
        boost::bind(&handle_read<ResponseHdl,ErrorHdl>,
            request, response, buffer, boost::asio::placeholders::error
        )
    );
}

/*
 * handle_send() returns nothing
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
inline void handle_send(
    const boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>& request,
    const boost::system::error_code& errcode)
{
    if (!errcode) {
        read<ResponseHdl,ErrorHdl>(request, boost::shared_ptr<std::string>(new std::string));
    } else {
        request->handler.error(error::SEND, errcode);
    }
}

/*
 * handle_connect() returns nothing
 *
 * Input:
    * request   HTTP request object.
    * error     Container of potential error.
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
inline void handle_connect(
    const boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>& request,
    const boost::system::error_code& errcode)
{
    if (!errcode) {
        boost::asio::async_write(request->socket,
            boost::asio::buffer(request->str.c_str(), request->str.size()),
            boost::bind(&handle_send<ResponseHdl,ErrorHdl>,
                request, boost::asio::placeholders::error
            )
        );
    } else {
        request->handler.error(error::CONNECT, errcode);
    }
}

/*
 * handle_resolve() returns nothing
 *
 * Input:
    * request    HTTP request object.
    * itr        Result of host being resolved.
    * error      Container of potential error.
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
inline void handle_resolve(
    const boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>& request,
    const Resolver::iterator& itr,
    const boost::system::error_code& errcode)
{
    if (!errcode) {
        boost::asio::async_connect(request->socket, itr,
            boost::bind(&handle_connect<ResponseHdl,ErrorHdl>,
                request, boost::asio::placeholders::error
            )
        );
    } else {
        request->handler.error(error::RESOLVE, errcode);
    }
}

/*
 * send() returns nothing
 *
 * Input:
    * request   HTTP request object.
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
inline void send(
    const boost::shared_ptr<Request<ResponseHdl,ErrorHdl>>& request)
{
    request->resolver.async_resolve(request->query,
        boost::bind(&handle_resolve<ResponseHdl,ErrorHdl>,
            request, boost::asio::placeholders::iterator, boost::asio::placeholders::error
        )
    );
}

/*
 * send() returns nothing
 *
 * Input:
    *
 *
 * */
inline void send(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const ResponseFn& response_handler)
{
    send<ResponseFn, BasicErrorHandler>(
        make_request(
            ioservice, host, command, "text/html",
            response_handler, BasicErrorHandler()
        )
    );
}

/*
 * send() returns nothing
 *
 * Input:
    *
 *
 * */
inline void send(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const ResponseRefFn& response_handler)
{
    send<ResponseRefFn, BasicErrorHandler>(
        make_request(
            ioservice, host, command, "text/html",
            response_handler, BasicErrorHandler()
        )
    );
}

/*
 * send() returns nothing
 *
 * Input:
    *
 *
 * */
inline void send(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const ResponseConstRefFn& response_handler)
{
    send<ResponseConstRefFn, BasicErrorHandler>(
        make_request(
            ioservice, host, command, "text/html",
            response_handler, BasicErrorHandler()
        )
    );
}

/*
 * send() returns nothing
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl>
inline void send(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command)
{
    send<ResponseHdl, BasicErrorHandler>(
        make_request(
            ioservice, host, command, "text/html",
            ResponseHdl(), BasicErrorHandler()
        )
    );
}

/*
 * send() returns nothing
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
inline void send(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command)
{
    send<ResponseHdl, ErrorHdl>(
        make_request(
            ioservice, host, command, "text/html",
            ResponseHdl(), ErrorHdl()
        )
    );
}

/*
 * send() returns nothing
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl>
inline void send(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const ResponseHdl& response_handler)
{
    send<ResponseHdl, BasicErrorHandler>(
        make_request(
            ioservice, host, command, "text/html",
            response_handler, BasicErrorHandler()
        )
    );
}

/*
 * send() returns nothing
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl,
    class ErrorHdl>
inline void send(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const ResponseHdl& response_handler,
    const ErrorHdl& error_handler)
{
    send<ResponseHdl, ErrorHdl>(
        make_request(
            ioservice, host, command, "text/html",
            response_handler, error_handler
        )
    );
}

    /*
 * send() returns nothing
 *
 * Input:
    *
 *
 * */
template <
    class ResponseHdl>
inline void send(
    boost::asio::io_service& ioservice,
    const std::string& host,
    const std::string& command,
    const std::string& content,
    const ResponseHdl& response_handler)
{
    send<ResponseHdl, BasicErrorHandler>(
        make_request(
            ioservice, host, command, content, "text/html",
            response_handler, BasicErrorHandler()
        )
    );
}

}

#endif