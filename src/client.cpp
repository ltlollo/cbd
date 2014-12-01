#include <iterator>
#include <iostream>
#include "common.h"

int main(int argc, char *argv[]) {
    const auto print_help = [&]() {
        std::cerr << "USAGE:\t" << argv[0]
                  << " action\n"
                     "\taction<string>: r|w\n"
                     "\t\tr: get buf\n"
                     "\t\ts: set buf\n"
                     "SCOPE: send and receve buffers from cbs\n"
                  << std::endl;
    };
    if (argc < 2) {
        print_help();
        return 1;
    }
    common::Action action;
    std::string actionstr(argv[1]);
    if (actionstr == "s") {
        action = common::Action::Set;
    } else if (actionstr == "r") {
        action = common::Action::Get;
    } else {
        print_help();
        return 1;
    }
    file::Socket sock;
    sock.connect("/tmp/.cb-sock");
    sock.send(action);
    if (action == common::Action::Set) {
        std::cin >> std::noskipws;
        std::istream_iterator<char> begin(std::cin), end;
        auto input = std::vector<char>(begin, end);
        sock.send(input.size());
        sock.send(input);
    } else {
        auto size = sock.recv<size_t>();
        if (!size) return 0;
        err::doreturn("exceded size", size > msg::maxsize);
        auto buf = sock.recv<std::vector<char>>(size);
        std::ostream_iterator<char> out (std::cout);
        std::copy(begin(buf), end(buf), out);
    }
    return 0;
}
