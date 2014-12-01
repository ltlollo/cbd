#include <thread>
#include <mutex>
#include "common.h"

void job(file::Socket&& ms, std::vector<char>& buf, std::mutex& m) {
    try {
        std::lock_guard<std::mutex> lock(m);
        file::Socket sock{std::move(ms)};
        sock.time(5);
        auto action = sock.recv<common::Action>();
        if (action == common::Get) {
            sock.send(buf.size());
            if (buf.size()) {
                sock.send(buf);
            }
        } else {
            auto size = sock.recv<std::size_t>();
            err::doreturn("exceded size", size > msg::maxsize);
            buf = sock.recv<std::vector<char>>(size);
        }
    } catch (std::exception& e) {
        printf("%s", e.what());
    }
}


class Server {
    std::mutex m;
    file::Socket sock;
    file::Bind sockbind;
    std::vector<char> buf;
public:
    Server() : sock{}, sockbind("/tmp/.cb-sock", sock) {
        err::donotfail_errno("sigaction", ansi::signal, ansi::sigint,
                             [](int, siginfo_t*, void*){
            ansi::unlink("/tmp/.cb-sock");
            ansi::exit(0);
        });
    }
    [[ noreturn ]] void run() {
        sock.listen();
        fun::loop([&](){
            std::thread(job, sock.clone(), ref(buf), ref(m)).detach();
        });
    }
};

int main(int argc, char *argv[]) {
    const auto print_help = [&]() {
        printf("USAGE:\t%s\n"
               "SCOPE: send and receve buffers on request\n",
               argv[0]);
    };
    if (argc > 1) {
        print_help();
        return 1;
    }
    if (!ansi::exp::dfork()) {
        return 0;
    }
    try {
        Server().run();
    } catch (std::runtime_error& e) {
        printf("%s\n", e.what());
        return 1;
    } catch (...) {
        return 1;
    }
}
