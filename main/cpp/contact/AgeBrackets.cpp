#include "AgeBrackets.h"

namespace stride {
namespace AgeBrackets {

unsigned short int ToSize(const AgeBracket& h) {
    return static_cast<unsigned short int>(h);
}

std::string AgeBracketToString(const AgeBracket& ageBracket)
{
    switch(ageBracket){
        case AgeBracket::Daycare:
            return std::string("Daycare");
        case AgeBracket::PreSchool:
            return std::string("PreSchool");
        case AgeBracket::K12School:
            return std::string("K12School");
        case AgeBracket::College:
            return std::string("College");
        case AgeBracket::Workplace:
            return std::string("Workplace");
        case AgeBracket::Retired:
            return std::string("Retired");
        default:
            return std::string("Unknown AgeBracket");
    }
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