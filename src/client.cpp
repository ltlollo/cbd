#include <iterator>
#include <iostream>
#include <string>
#include "common.h"

int main(int argc, char *argv[]) {
    const auto print_help = [&]() {
        std::cerr << "USAGE:\t" << argv[0]
                  << " action [path]\n"
                     "\taction<string>: s|r|w\n"
                     "\t\tr: get buf\n"
                     "\t\ts|w: set buf\n"
                     "\tpath<string>: socket path (defaults: /tmp/.cb-sock)\n"
                     "SCOPE: send and receve buffers from cbs\n"
                  << std::endl;
    };
    if (argc != 2 && argc != 3) {
        print_help();
        return 1;
    }
    common::Action action;
    std::string actionstr(argv[1]);
    if (actionstr == "s" || actionstr == "w") {
        action = common::Action::Set;
    } else if (actionstr == "r") {
        action = common::Action::Get;
    } else {
        print_help();
        return 1;
    }
    file::Socket sock;
    auto spath = std::string(argc > 2 ? argv[2] : "/tmp/.cb-sock");
    if (action == common::Action::Set) {
        std::cin >> std::noskipws;
        std::istream_iterator<char> begin(std::cin), end;
        auto input = std::vector<char>(begin, end);
        sock.connect(spath);
        sock.send(action);
        sock.send(input.size());
        sock.send(input);
        sock.recv<common::Done>();
    } else {
        sock.connect(spath);
        sock.send(action);
        auto size = sock.recv<size_t>();
        if (!size) { return 0; }
        err::doreturn("exceded size", size > msg::maxsize);
        auto buf = sock.recv<std::vector<char>>(size);
        std::ostream_iterator<char> out (std::cout);
        std::copy(begin(buf), end(buf), out);
    }
    return 0;
}
