#pragma once

#include <string>
#include <utility>
#include <type_traits>

/* concept check {
 *   bool operator()(char32_t) noexcept;
 *  };
 */

template<class _1, class... _2>
struct all_of {
    static_assert(std::is_trivial_v<_1>);

    bool operator()(char32_t c) noexcept { return _1()(c) && all_of<_2...>()(c); }
};

template<class _1, class _2>
struct all_of<_1, _2> {
    static_assert(std::is_trivial_v<_1>);
    static_assert(std::is_trivial_v<_2>);

    bool operator()(char32_t c) noexcept { return _1()(c) && _2()(c); }
};

template<class _1, class... _2>
struct any_of {
    static_assert(std::is_trivial_v<_1>);

    bool operator()(char32_t c) noexcept { return _1()(c) || any_of<_2...>()(c); }
};

template<class _1, class _2>
struct any_of<_1, _2> {
    static_assert(std::is_trivial_v<_1>);
    static_assert(std::is_trivial_v<_2>);

    bool operator()(char32_t c) noexcept { return _1()(c) || _2()(c); }
};

template<class _1>
struct check_not {
    static_assert(std::is_trivial_v<_1>);

    bool operator()(char32_t c) noexcept { return !_1()(c); }
};

template<char32_t l, char32_t r>
struct in_range {
    bool operator()(char32_t c) noexcept { return c >= l && c <= r; }
};

template<char32_t l>
struct is {
    bool operator()(char32_t c) noexcept { return c == l; }
};

namespace ascii {
    using is_char = in_range<0, 127>;
    using is_digit = in_range<'0', '9'>;
    using is_lower_case = in_range<'a', 'z'>;
    using is_upper_case = in_range<'A', 'Z'>;
    using is_letter = all_of<is_lower_case, is_upper_case>;
    using is_special = all_of<in_range<0, 31>, is<127>>;
    using is_symbol = all_of<is_char, check_not<all_of<is_special, is_letter, is_digit>>>;
}

namespace unicode {
    using is_newline = any_of<is<0x000A>, is<0x000B>, is<0x000C>, is<0x000D>, is<0x0085>, is<0x2028>, is<0x2029>>;
    using is_id_char = any_of<is<0x00A8>, is<0x00AA>, is<0x00AD>, is<0x00AF>, is<0x2054>,
            in_range<0x00B2, 0x00B5>, in_range<0x00B7, 0x00BA>, in_range<0x00BC, 0x00BE>, in_range<0x00C0, 0x00D6>,
            in_range<0x00D8, 0x00F6>, in_range<0x00F8, 0x00FF>, in_range<0x0100, 0x167F>, in_range<0x1681, 0x180D>,
            in_range<0x180F, 0x1FFF>, in_range<0x200B, 0x200D>, in_range<0x202A, 0x202E>, in_range<0x203F, 0x2040>,
            in_range<0x2060, 0x206F>, in_range<0x2070, 0x218F>, in_range<0x2460, 0x24FF>, in_range<0x2776, 0x2793>,
            in_range<0x2C00, 0x2DFF>, in_range<0x2E80, 0x2FFF>, in_range<0x3004, 0x3007>, in_range<0x3021, 0x302F>,
            in_range<0x3031, 0x303F>, in_range<0x3040, 0xD7FF>, in_range<0xF900, 0xFD3D>, in_range<0xFD40, 0xFDCF>,
            in_range<0xFDF0, 0xFE44>, in_range<0xFE47, 0xFFFD>, in_range<0x10000, 0x1FFFD>, in_range<0x20000, 0x2FFFD>,
            in_range<0x30000, 0x3FFFD>, in_range<0x40000, 0x4FFFD>, in_range<0x50000, 0x5FFFD>, in_range<0x60000, 0x6FFFD>,
            in_range<0x70000, 0x7FFFD>, in_range<0x80000, 0x8FFFD>, in_range<0x90000, 0x9FFFD>, in_range<0xA0000, 0xAFFFD>,
            in_range<0xB0000, 0xBFFFD>, in_range<0xC0000, 0xCFFFD>, in_range<0xD0000, 0xDFFFD>, in_range<0xE0000, 0xEFFFD>>;
}

template <class d, class m>
struct token_type {
    using determine = d;
    using match = m;
};

template <class _1, class ..._2>
struct match_sequence {
    static_assert(std::is_trivial_v<_1>);

    template <class iter>
    bool operator()(iter& in) noexcept {
        const auto first = _1()(in);
        const auto second = match_sequence<_2...>()(in);
        return first && second;
    }
};

template<class _1, class _2>
struct match_sequence<_1, _2> {
    static_assert(std::is_trivial_v<_1>);
    static_assert(std::is_trivial_v<_2>);

    template <class iter>
    bool operator()(iter& in) noexcept {
        const auto first = _1()(in);
        const auto second = _2()(in);
        return first && second;
    }
};

template <class c>
struct match_once {
    static_assert(std::is_trivial_v<c>);

    template <class iter>
    bool operator()(iter& in) noexcept {
        const auto res = c()(*in);
        if (res) {
            ++in;
        }
        return res;
    }
};

template <class c>
struct match_any {
    static_assert(std::is_trivial_v<c>);

    template <class iter>
    bool operator()(iter& in) noexcept {
        while (c()(*in)) { ++in; }
        return true;
    }
};

template <class c>
using match_once_or_more = match_sequence<match_once<c>, match_any<c>>;

namespace identifier {
    using is_start = any_of<is<'_'>, ascii::is_letter, unicode::is_id_char>;
    using is_char = any_of<is_start, ascii::is_digit>;
    using token = token_type<is_start, match_sequence<match_once<is_start>, match_any<is_char>>>;
}

namespace number {
    using is_hex = any_of<in_range<'0', '9'>, in_range<'a', 'f'>, in_range<'A', 'F'>>;
    using is_octal = in_range<'0', '7'>;
    using is_bin = any_of<is<'0'>, is<'1'>>;
    using hex_digit_sequence = match_once_or_more<is_hex>;
    using octal_digit_sequence = match_once_or_more<is_octal>;
    using bin_digit_sequence = match_once_or_more<is_bin>;
    using digit_sequence = match_once_or_more<ascii::is_digit>;
}

class u32_iterator {
public:
    enum class exit_reason : int {
        none, eof, decode_error, io_error
    };

    virtual bool move_next() noexcept = 0;

    virtual exit_reason exit_status() const noexcept = 0;

    virtual char32_t current() const noexcept = 0;
};

struct token {
    int type_id;
    std::u32string token;
};
