#pragma once

#include <iostream>

namespace SDL_GUI {
/** 2 dimensional point */
class Position {
public:
    int _x = 0; /**< horizontal position */
    int _y = 0; /**< vertical position */

    /** Default constructor */
    Position() = default;

    /**
     * Constructor
     * @param x horizontal position
     * @param y vertical position
     */
    Position(int x, int y) : _x(x), _y(y) {}

    /**
     * memberwise addition of two Positions
     * @param p position to add
     * @return this + p
     */
    Position operator+(const Position &p) const;

    /**
     * memberwise substraction of two Positions
     * @param p position to substract
     * @return this - p
     */
    Position operator-(const Position &p) const;

    Position operator*(const int &s) const;
    Position operator/(const int &s) const;

    /**
     * Unary Addition
     * @param p position to add
     * @return this + p
     */
    Position &operator+=(const Position &p);

    /**
     * Unary Substraction
     * @param p position to substract
     * @return this - p
     */
    Position &operator-=(const Position &p);

    Position &operator*=(const int &s);
    Position &operator/=(const int &s);

    bool operator==(const Position &p) const;

    friend std::ostream& operator<<(std::ostream& os, const Position& pos);
};

/**
 * Print out the coordinates of this.
 * @param os out stream
 * @param pos position to output
 * @return output stream
 */
std::ostream& operator<<(std::ostream& os, const Position& pos);
}
