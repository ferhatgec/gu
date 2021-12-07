// MIT License
//
// Copyright (c) 2021 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#include "../include/gu.hpp"

#include <fstream>
#include <sstream>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../libs/termiospp/include/termiospp"
#include "../libs/fpaper/include/fpaper_extract.hpp"
#include "../libs/curl4cpp/curl4.hpp"

void gu::init(std::string gc) noexcept {
    FPaper paper; paper.InitData(gc);
    FPaper_Extract init(paper);

    #if defined(TIOCGSIZE)
        struct ttysize ts;
        if(ioctl(STDIN_FILENO, TIOCGSIZE, &ts) == 0) {
            this->__h = ts.ts_lines;
            init.terminal_width = ts.ts_cols;
        }
    #elif defined(TIOCGWINSZ)
        struct winsize ts;
        if(ioctl(STDIN_FILENO, TIOCGWINSZ, &ts) == 0) {
            this->__h           = ts.ws_row         ;
            init.terminal_width = (ts.ws_col / 1.50);
        }
    #else
        #error "I/O controls are not defined-found."
    #endif

    std::istringstream stream(init.Extract());
    for(std::string temp; std::getline(stream, temp, '\n');
        this->file_data.append(temp + "\n")) { ++this->__down;
    } this->__full_length = this->__down;
}

void gu::init_buffer() noexcept {
    clear(); to_up();
    this->__down = (this->__h / 1.05);
    this->from(false);
    disable_cursor(); char ch;

    Termios new_termio, old_termio; TermiosFlag local_flag;

    term::get_attribute(0, &old_termio); new_termio = old_termio;
    new_termio.set_local_flag(local_flag &= ~TERMIOS_I_CANON);
    new_termio.set_local_flag(local_flag &= ~TERMIOS_ECHO);
    term::set_attribute(0, TERMIOS_SA_NOW, &new_termio);

    while(1) { ch = getchar();
        if(ch == escape) {
            ch = getchar(); ch = getchar();

            switch(ch) {
                case up: {
                    if(this->__up >= 1
                        && this->__up < this->__full_length && this->__full_length > this->__h) {
                            --this->__up; --this->__down;
                            this->from(false);
                    } break;
                }

                case down: {
                    if(this->__down < this->__full_length) {
                        ++this->__down; ++this->__up;
                        this->from(false);
                    } break;
                }

                default: {
                    break;
                }
            } continue;
        }

        if(std::tolower(ch) == 'q') { break; }
    } enable_cursor(); clear();
    term::set_attribute(0, TERMIOS_SA_NOW, &old_termio);
}

void gu::from(bool is_up_key) noexcept {
    std::istringstream stream(this->file_data);
    std::string __new; unsigned i = 0;

    if(is_up_key) {
        for(std::string temp; std::getline(stream, temp); i++) {
            if(i >= this->__up) { __new.append(temp + "\n"); }
        }
    } else {
        for(std::string temp; std::getline(stream, temp); i++) {
            if(i < this->__down) { __new.append(temp + "\n"); }
        }
    } clear(); std::cout << __new; up_to(this->__up);
}

std::tuple<std::string, bool> gu::check(const std::string url) noexcept {
    curl4::CURL4 init = curl4::easy::init(); CURLcode response;
    std::string val;

    init.setopt(CURLOPT_URL, url);
    init.setopt(CURLOPT_WRITEFUNCTION, curl4::easy::writefunc);
    init.setopt(CURLOPT_WRITEDATA, &val);
    response = curl4::easy::perform(init);

    return (response == CURLE_OK) ? std::make_tuple(val, true)
                                  : std::make_tuple(val, false);
}

int main(int argc, char const* const* argv) noexcept {
    if(argc < 2) {
        std::cout << "gu - Gefertean Union reader\n"
                     "-----\n"
                     "Usage:\n" <<
                     argv[0] << " [gechain]\n" <<
                     argv[0] << " 1\n"; return 1;
    } gu init; auto data = init.check(init.url + "gc" + std::string(argv[1]) + ".fpaper");
    if(std::get<0>(data).empty()) {
        std::cout << "gc" << argv[1] << " not found, did you mean " << argv[1] << ", maybe?\n";
        return 1;
    }
    init.init(std::get<0>(data));
    init.init_buffer();
}