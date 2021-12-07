// MIT License
//
// Copyright (c) 2021 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//

#ifndef GU_GU_HPP
#define GU_GU_HPP

#include <iostream>
#include <tuple>

//#if __cplusplus >= 201703L
//    #include <string_view>
//    using string = std::string_view;
//#else
      #include <string>
//    using string = std::string;
//#endif

constexpr unsigned escape = 27;
constexpr unsigned up = 65;
constexpr unsigned down = 66;

namespace {
    void refresh() noexcept { std::cout << "\x1b[2J"; }
    void clear() noexcept { refresh(); std::cout << "\x1b[H"; }
    void to_up() noexcept { std::cout << "\x1b[0A"; }
    void up_to(unsigned n) noexcept { std::cout << "\x1b[" + std::to_string(n) + "A"; }
    void disable_cursor() noexcept { std::cout << "\x1b[?25l"; }
    void enable_cursor() noexcept { std::cout << "\x1b[?25h"; }
}

class gu {
public:
    std::string file_data;

    unsigned __up = 0,
             __down = 0,
             __full_length = 0,
             __h = 0;

    const std::string url = "https://ferhatgec.github.io/land/interesting/gefertean_union/gechains/fpaper/";
public:
    gu() = default; ~gu() = default;

    void init(const std::string gc) noexcept;
    void init_buffer() noexcept;
    void from(bool is_up_key = false) noexcept;

    std::tuple<std::string, bool> check(const std::string url) noexcept;
};

#endif // GU_GU_HPP
