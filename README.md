C++ HTTP Request
---------

Example
---------
```cpp
#include "http_request.hpp"
#include <iostream>

void handle_response(const std::string& data) {
    std::cout << data << std::endl;
}

int main() {
    boost::asio::io_service ioservice;

    http::send(
        ioservice, "google.com", "GET /something_that_doesnt_exist", handle_response
    );

    ioservice.run();
}
```
