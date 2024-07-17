#pragma once
#include <memory>
#include <variant>
/**
 * @brief A file containing the workings of a full binary-syntax-tree library.
 * 
 */

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
    SELF_MOD
};

template<typename... _Tys>
struct known_variant
{
    int T = NULL;
};

template<typename _N>
struct bst_node
{
    std::shared_ptr<known_variant<_N, bst_node<_N>> _Left = nullptr;
    bst_operator _Operator;
    std::shared_ptr<known_variant<_N, bst_node<_N>> _Right = nullptr;
};