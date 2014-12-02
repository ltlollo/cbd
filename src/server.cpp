#include "common.h"

static const char* globpath;

void job(file::Socket&& ms, std::vector<char>& buf) {
    try {
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
    file::Socket sock;
    file::Bind sockbind;
    std::vector<char> buf;
public:
    Server() : sock{}, sockbind(globpath, sock) {
        err::donotfail_errno("sigaction", ansi::signal, ansi::sigint,
                             [&](int, siginfo_t*, void*) {
                                     ansi::unlink(globpath);
                                     ansi::exit(0);
                                 });
    }
    [[ noreturn ]] void run() {
        sock.listen(1);
        fun::loop([&](){
            job(sock.clone(), ref(buf));
        });
    }
};

int main(int argc, char *argv[]) {
    const auto print_help = [&]() {
        printf("USAGE:\t%s [path]\n"
               "\tpath<string>: socket path (defaults: /tmp/.cb-sock)\n"
               "SCOPE: send and receve buffers on request\n",
               argv[0]);
    };
    if (argc == 2 && argv[1] == std::string("-h")) {
        print_help();
        return 0;
    }
    if (argc > 2) {
        print_help();
        return 1;
    }
    if (!ansi::exp::dfork()) {
        return 0;
    }
    try {
        globpath = argc > 1 ? argv[1] : "/tmp/.cb-sock";
        Server().run();
    } catch (std::runtime_error& e) {
        printf("%s\n", e.what());
        return 1;
    } catch (...) {
        return 1;
    }
}
