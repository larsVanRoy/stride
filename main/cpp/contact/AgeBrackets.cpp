#include "AgeBrackets.h"

namespace stride {
namespace AgeBrackets {

unsigned short int ToSize(const AgeBracket& h) {
    return static_cast<unsigned short int>(h);
}

std::string ToString(const AgeBracket& ageBracket)
{
    switch(ageBracket){
        case AgeBracket::Daycare:
            return "Daycare";
        case AgeBracket::PreSchool:
            return "PreSchool";
        case AgeBracket::K12School:
            return "K12School";
        case AgeBracket::College:
            return "College";
        case AgeBracket::Workplace:
            return "Workplace";
        case AgeBracket::Retired:
            return "Retired";
    }
    return "Retired";
}

AgeBracket ToAgeBracket(double age){
    if(Daycare::HasAge(age))
        return AgeBracket::Daycare;
    if(PreSchool::HasAge(age))
        return AgeBracket::PreSchool;
    if(K12School::HasAge(age))
        return AgeBracket::K12School;
    if(College::HasAge(age))
        return AgeBracket::College;
    if(Workplace::HasAge(age))
        return AgeBracket ::Workplace;
    return AgeBracket::Retired;
}

} // namespace AgeBrackets
} // namespace stride