#pragma once

#include <array>
#include <cstdint>

// ApEk,
// NoAvailableAlias
// this code is public domain

using PointElement = std::uint32_t;
using Point = std::array<PointElement, 2>;

#define USE_PREMATURE_OPTIMIZATION

template <typename PT> struct morton
{
    inline bool operator()(PT const& lhs, PT const& rhs) const
    {
#ifndef USE_PREMATURE_OPTIMIZATION
        std::size_t x = 0;
        std::size_t j = 0;

        for(std::size_t k = 0; k < lhs.size(); ++k)
        {
            std::size_t y = lhs[k] ^ rhs[k];

            if (x < y && x < (x ^ y))
            {
                j = k;
                x = y;
            }
        }
        return lhs[j] < rhs[j];
#else
        std::size_t x = lhs[0] ^ rhs[0];
        std::size_t y = lhs[1] ^ rhs[1];
        auto k = (x < y) && x < (x ^ y);
        return lhs[k] < rhs[k];
#endif
    }
};

//-----------------------------------------------------------------------------
// following code derived from: (after much vex)
// https://gist.github.com/s-l-teichmann/4014673
// https://gist.github.com/s-l-teichmann/4014664

namespace
{

const std::size_t SIZE = sizeof(PointElement) * CHAR_BIT;
const std::size_t BITS = SIZE / 2;
const std::size_t MSB = 2 * BITS - 1;
const std::size_t HI_MASK = 1 << (BITS + 1);

const std::size_t _000_ = 0;
const std::size_t _001_ = 1;
const std::size_t _010_ = 2;
const std::size_t _011_ = 3;
const std::size_t _100_ = 4;
const std::size_t _101_ = 5;

const std::size_t MASK = 0xAAAAAAAAAAAAAAAA; // hhhhhhhhh
const std::size_t FULL = 0xFFFFFFFFFFFFFFFF; // uuuuuuuuu

std::size_t setbits(PointElement p, PointElement v)
{
    PointElement mask = (MASK >> (MSB - p)) & (~(FULL << p) & FULL);
    return (v | mask) & ~(1 << p) & FULL;
}
std::size_t usetbit(PointElement p, PointElement v)
{
    PointElement mask = ~(MASK >> (MSB - p)) & FULL;
    return (v & mask) | (1 << p);
}

std::size_t z_encode(Point p)
{
    std::size_t x = p[0];
    std::size_t y = p[1];

    std::size_t r = 0;

    for (std::size_t b = 1, m = 1; m != HI_MASK; m <<= 1)
    {
        if (y & m) r |= b; b <<= 1;
        if (x & m) r |= b; b <<= 1;
    }
    return r;
}
Point z_decode(std::size_t r)
{
    std::size_t x = 0;
    std::size_t y = 0;

    for (std::size_t b = 1, m = 1; m != HI_MASK; m <<= 1)
    {
        if (r & b) y |= m; b <<= 1;
        if (r & b) x |= m; b <<= 1;
    }
    return { static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y) };
}

} // anonymous namespace ------------------------------------------------------

template <typename PT> PT bigmin(PT const& min, PT const& max, PT const& rhs)
{
    std::size_t minz = z_encode(min);
    std::size_t maxz = z_encode(max);
    std::size_t code = z_encode(rhs);

    std::size_t retv = maxz;

    for(PointElement p = MSB, mask = 1 << MSB; mask != 0; mask >>= 1, --p)
    {
        PointElement v = (code & mask) ? _100_ : _000_;

        if (minz & mask) v |= _010_;
        if (maxz & mask) v |= _001_;

        switch (v) // hotspot at 46%
        {
            case _001_:
                retv = usetbit(p, minz);
                maxz = setbits(p, maxz);
        break;
            case _011_:
                return z_decode(minz);
            case _100_:
                return z_decode(retv);
            case _101_:
                minz = usetbit(p, minz);
        }
    }
    return z_decode(retv);
}
