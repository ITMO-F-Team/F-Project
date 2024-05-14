#include "flang/pp/print_token.hpp"

namespace flang
{
std::ostream& operator<<(std::ostream& os, Token const& token)
{
    std::string token_type_representation;
    switch (token.type()) {
        case tkLPAREN:
            token_type_representation = "tkLPAREN";
            break;
        case tkRPAREN:
            token_type_representation = "tkRPAREN";
            break;
        case tkQUOTE:
            token_type_representation = "tkQUOTE";
            break;
        case tkQUOTEMARK:
            token_type_representation = "tkQUOTEMARK";
            break;
        case tkSETQ:
            token_type_representation = "tkSETQ";
            break;
        case tkFUNC:
            token_type_representation = "tkFUNC";
            break;
        case tkLAMBDA:
            token_type_representation = "tkLAMBDA";
            break;
        case tkPROG:
            token_type_representation = "tkPROG";
            break;
        case tkCOND:
            token_type_representation = "tkCOND";
            break;
        case tkWHILE:
            token_type_representation = "tkWHILE";
            break;
        case tkRETURN:
            token_type_representation = "tkRETURN";
            break;
        case tkBREAK:
            token_type_representation = "tkBREAK";
            break;
        case tkIDENTIFIER:
            token_type_representation = "tkIDENTIFIER";
            break;
        case tkINTEGER:
            token_type_representation = "tkINTEGER";
            break;
        case tkREAL:
            token_type_representation = "tkREAL";
            break;
        default:
            throw std::runtime_error("Token print rules are not excessive!");
    }
    os << token_type_representation;
    return os;
}
} // namespace flang
