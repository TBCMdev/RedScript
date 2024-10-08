/**
 * @brief A file containing the workings of a full binary-syntax-tree library.
 */
#pragma once
#include <memory>
#include <variant>
#include <string>

enum class bst_operator
{
    ADD,
    SELF_ADD,
    SUB,
    SELF_SUB,
    MUL,
    SELF_MUL,
    DIV,
    SELF_DIV,
    MOD,
    SELF_MOD,

    NONE
};
inline std::string bst_operator_str(bst_operator op)
{
    switch(op)
    {
        case bst_operator::ADD: return "+"; 
        case bst_operator::SELF_ADD: return "+="; 
        case bst_operator::SUB: return "-"; 
        case bst_operator::SELF_SUB: return "-="; 
        case bst_operator::MUL: return "*"; 
        case bst_operator::SELF_MUL: return "*="; 
        case bst_operator::DIV: return "/"; 
        case bst_operator::SELF_DIV: return "/="; 
        case bst_operator::MOD: return "%";
        case bst_operator::SELF_MOD: return "%=";  
    }
    return "";
}


// Utility to find the index of a type in a variadic template pack
// template<typename T, typename... Ts>
// struct index_of;

// template<typename T, typename First, typename... Rest>
// struct index_of<T, First, Rest...>
// {
//     static constexpr int value = std::is_same_v<T, First> ? 0 : 1 + index_of<T, Rest...>::value;
// };

// template<typename T>
// struct index_of<T>
// {
//     static_assert(sizeof(T) == 0, "Type not found in parameter pack of known_variant.");
// };

// template<typename... _Tys>
// struct known_variant
// {
//     int t_index = -1;
//     std::variant<_Tys...> _Value;

//     template<typename _Tx>
//     inline constexpr known_variant(_Tx&& v)
//     {
//         t_index = index_of<std::decay_t<_Tx>, _Tys...>::value;
//         _Value = v;
//     }
//     template<typename _NewT>
//     inline void change(_NewT&& n)
//     {
//         _Value.
//         t_index = index_of<std::decay_t<_NewT>, _Tys...>::value;
//         _Value = v;
//     }


// };


template<typename _N>
struct bst_node
{ 
    std::shared_ptr<std::variant<_N, bst_node<_N>>> _Left;
    // root operator is used when adding two root nodes.
    bst_operator _Operator = bst_operator::NONE, _RootOperator = bst_operator::NONE;
    std::shared_ptr<std::variant<_N, bst_node<_N>>> _Right;


    // Constructor to initialize the shared_ptr members
    bst_node() : _Left(nullptr), _Right(nullptr) {}
};
template<typename _N>
using binary_syntax_tree = bst_node<_N>;
template<typename _N>
using bst = bst_node<_N>;

