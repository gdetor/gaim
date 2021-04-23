/* Test functions cpp of GAIM package 
 * Copyright (C) 2019  Georgios Detorakis (gdetor@protonmail.com)
 *                     Andrew Burton (ajburton@uci.edu)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// $Id$
/**
 * @file test_functions.cpp
 * Furnishes a small collection of standard test objective functions
 * typically used as benchmarks in numerical optimization.
 */
// $Log$
#include "gaim.h"
#include <cmath>


/**
 *  @brief Sphere cost function
 *
 *  Sphere cost function 
 *  \f[ f({\bf x}) = \sum_{i=0}^{n}x_i^2 \f]
 *
 *  @param[in] x Vector
 *  @return A number indicating the cost
 */
REAL_ sphere(std::vector<REAL_>& x)
{
    REAL_ mysum = 0;
    for (auto &i : x) {
        mysum += i * i;
    }
    return -mysum;
}


/**
 *  @brief Rastrigin cost function
 *
 *  Rastrigin cost function
 *  \f$ f({\bf x}) = An + \sum{i=0}{n}x_i^2 - A \cos(2\pi x_i) \f$ 
 *
 *  @param[in] x Vector
 *  @return A number indicating the cost
 */
REAL_ rastrigin(std::vector<REAL_>& x)
{
    REAL_ mysum = 0.0;
    for (auto &i : x) {
        mysum += i*i - 10.0 * cos(2. * M_PI *i);
    }
    return -(10.0 * x.size() + mysum);
}


/**
 *  @brief Schwefel cost function
 *
 *  Schwefel cost function
 *  \f[ f({\bf x}) = 418.9829 n - \sum_{i=0}^{n} x_i \sin(\sqrt{|x_i|}) \f]
 *
 *  @param[in] x Vector
 *  @return A number indicating the cost
 */
REAL_ schwefel(std::vector<REAL_>& x)
{
    REAL_ mysum = 0.0;
    for (auto &i : x) {
        mysum += i * sin(sqrt(abs(i)));
    }
    return -(418.9829 * x.size() - mysum);
}


/**
 *  @brief Griewangk cost function
 *
 *  Griewangk cost function
 *  \f[ f({\bf x}) =  1 + \frac{1}{4000} \sum{i=0}^{n} x_i^2 -
 *          \prod_{i=0}^{n} \cos(\frac{x_i}{\sqrt{i+1}}) \f]
 *
 *  @param[in] x Vector
 *  @return A number indicating the cost
 */
REAL_ griewank(std::vector<REAL_>& x)
{
    REAL_ myprod = 1.0, mysum = 0.0;
    size_t ii = 0;
    for(auto &i : x) {
        myprod *= cos(i / sqrt(ii + 1));
        mysum += i * i;
        ii++;
    }
    return -(1 + mysum / 4000.0 - myprod);  
}


int distance(int x, int y)
{
    if (x == 1 && y == 2) {
        return 10;
    } else if (x == 1 && y == 3) {
        return 15;
    } else if (x == 2 && y == 3) {
        return 35;
    } else if (x == 2 && y == 4) {
        return 25;
    } else if (x == 2 && y == 1) {
        return 10;
    } else if (x == 1 && y == 4) {
        return 20;
    } else if (x == 4 && y == 1) {
        return 20;
    } else if (x == 3 && y == 1) {
        return 15;
    } else if (x == 3 && y == 2) {
        return 35;
    } else if (x == 3 && y == 4) {
        return 30;
    } else if (x == 4 && y == 2) {
        return 25;
    } else if (x == 4 && y == 3) {
        return 30;
    } else {
        return 100;
    }
}


REAL_ tsm(std::vector<REAL_>& x)
{
    REAL_ mysum = 0;
    mysum = 0;
    for (size_t i = 0; i < x.size()-1; ++i) {
        mysum += distance(x[i], x[i+1]);
    }
    return -mysum;
}
