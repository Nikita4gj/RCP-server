#pragma  once


#include <type_traits>
#include <functional>
#include <concepts>
#include <utility>
#include <tuple>

#include "utils.hpp"

namespace tuple_utils
{
    namespace detail
    {
        template<std::size_t Indx, class Tuple>
        using _tuple_element_forward_t = forward_like_t<Tuple, std::tuple_element_t<Indx, std::remove_reference_t<Tuple>>>;
    }

    template<class Tuple>
    concept is_tuple = requires(Tuple tuple)
    {
        []<typename... Args>(std::tuple<Args...>){}(tuple);
    };
    
    template<class Func, class Tuple>
    concept tuple_for_each_invocable = is_tuple<Tuple> &&
    []<std::size_t... Is>(std::index_sequence<Is...>)
    {
        return (std::invocable<Func, detail::_tuple_element_forward_t<Is, Tuple>> && ...);
    }(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>{});

    template<is_tuple Tuple, tuple_for_each_invocable<Tuple> Func>
    void apply_for_each(Tuple&& tuple, Func&& func)
    {
        std::apply(
            [&func]<typename... Args>(Args&&... targs)
            {
                (std::invoke(func, std::forward<Args>(targs)), ...);
            }, std::forward<Tuple>(tuple)
        );
    }
}

