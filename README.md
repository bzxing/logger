# Logger
## Highlights
* High-performance non-blocking network IO (Boost::ASIO)
* Safe concurrent client access
* Highly modular design, polymorphism used when appropraite
* C++11/14 compliance for thread and memory safety as well as performance (application of the smart pointers, move semantics and RAII-style locks)
* Persistent database (under construction)
## Dependencies
* Linux
* gcc
* make
* boost::asio
## Build
* `git clone https://github.com/xza218/logger.git`
* `cd logger/server/src`
* `make`
## Tutorial
* Assuming pwd is at logger root
* Start server first: `./server/build/server`
* Start client next: `./client/client.py`
* On client command line interface:
  * First try add a new log: `new_log JohnDoe info hello world I'm here!!!`, and press Enter to send it out, then press Enter AGAIN with an empty command so that the client reads the response from the server.
    * On the client window you'll receive server's response: `Ok`
  * Add another log at different priority level: `new_log BlackSmith error Panic!! I can't handle this I think I might as well just die.`. Press Enter twice like before.
    * You'll see the `Ok` response again on the client side.
      * Tips: Five log levels are: `debug`, `info`, `warning`, `critical` and `error`, from least severe to most severe. If you use anything other than those provided, you'll receive an error instead of `Ok`. Try it out!
  * Try to request a read from server. Enter `dump_all debug` then press Enter twice.
    * Aside from `Ok`, you'll receive the 2 messages you just sent
  * Try increase the debug level in the dump. Type `dump_all warning` and press Enter twice.
    * You'll see only the more severe message now!
  * Type `delete_all` and press Enter twice, then `dump_all` and press Enter twice, tell me what happened.
  * Try a wrong command name and press Enter twice. See what happened?
