#pragma once
/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2019, Jan Broeckhove.
 */

#include <iterator>
#include <string>

namespace stride {

/**
 * AgeBrackets that determine participation in type of ContactPool.
 */
namespace AgeBrackets {

enum class AgeBracket : unsigned int
{
    Daycare         = 0U,
    PreSchool       = 1U,
    K12School       = 2U,
    College         = 3U,
    Workplace       = 4U,
    Retired         = 5U,
};

/// Convert AgeBracket to unsigned short int
unsigned short int ToSize(const AgeBracket& h);

/// Number of ContactPool types.
inline constexpr unsigned int NumOfAgeBrackets() { return 6U; }

/// AgeBracket to string
std::string AgeBracketToString(const AgeBracket& ageBracket);

/// Keeps all available AgeBrackets in a template pack and exposes it as a std::initialize_list
template <AgeBracket... AgeBrackets>
struct IDPack
{
    constexpr static std::initializer_list<AgeBracket> AsInitializerList = {AgeBrackets...}; ///< Exposed as std::initializer_list
};

/// Placed separately to please swig and avoid syntax errors there
using AgeBracketPack_t =
IDPack<AgeBracket::Daycare, AgeBracket::PreSchool, AgeBracket::K12School, AgeBracket::College, AgeBracket::Workplace, AgeBracket::Retired>;

/// A constexpr global variable that gives access to the available AgeBrackets
constexpr AgeBracketPack_t AgeBracketPack;

/// To allow iteration over the type ids.
constexpr std::initializer_list<AgeBracket> AgeBracketList = AgeBracketPack_t::AsInitializerList;

/// DayCare with lower (inclusive) and upper (exclusive) age bracket..
struct Daycare
{
        constexpr static inline double m_lower = 0.0; // if changed, activate test in HasAge!
        constexpr static inline double m_upper = 3.0;

        static bool HasAge(double age) { return /*age >= m_lower && */ age < m_upper; }
};

/// Preschool with lower (inclusive) and upper (exclusive) age bracket.
struct PreSchool
{
        constexpr static inline double m_lower = 3.0;
        constexpr static inline double m_upper = 6.0;

        static bool HasAge(double age) { return age >= m_lower && age < m_upper; }
};

/// K12school with lower (inclusive) and upper (exclusive) age bracket.
struct K12School
{
        constexpr static inline double m_lower = 6.0;
        constexpr static inline double m_upper = 18.0;

        static bool HasAge(double age) { return age >= m_lower && age < m_upper; }
};

/// College with lower (inclusive) and upper (exclusive) age bracket.
struct College
{
        constexpr static inline double m_lower = 18.0;
        constexpr static inline double m_upper = 26.0;

        static bool HasAge(double age) { return age >= m_lower && age < m_upper; }
};

/// College with lower (inclusive) and upper (exclusive) age bracket.
struct Workplace
{
        constexpr static inline double m_lower = 18U;
        constexpr static inline double m_upper = 65u;

        static bool HasAge(double age) { return age >= m_lower && age < m_upper; }
};

AgeBracket ToAgeBracket(double age);

} // namespace AgeBrackets
} // namespace stride
