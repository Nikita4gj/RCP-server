#pragma  once


#include <type_traits>

namespace rpc::utils
{
    template<class Pattern, class Type>
    using forward_like_t = std::conditional_t<
        std::is_lvalue_reference_v<Pattern>,
        std::add_lvalue_reference_t<Type>,
        std::add_rvalue_reference_t<Type>
        >;
}