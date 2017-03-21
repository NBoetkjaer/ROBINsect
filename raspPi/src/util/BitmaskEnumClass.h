#pragma once
// This file contains templated operator overloads for many bitwise operators that is typically used in combination with 
// enum values.
// It is based on the ideas presented in the following blog: http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
// 
// Background:
// C++11 introduces the scoped enum (enum class), which has the advantange of being type safe, but the drawback is that it will not 
// have standard bitwise operators (eg. &, |, ~, ^, &=, |=, ^=). This file defines a set of bitwise operator overloads for a given
// enum class.
// 
// Usage:
// Define the enum class as you normally would and just add the macro ENABLE_BITMASK_OPERATORS with the requested enum class.
// A special note regarding namespaces: The macro ENABLE_BITMASK_OPERATORS() must be placed in global namespace even if the 
// enum class is placed inside a namespace.
// eg.
// namaspace FileIO
// {
//      enum class Permissions
//      {
//          Readable = 0x4,
//          Writeable = 0x2,
//          Executable = 0x1
//      };
// }
// ENABLE_BITMASK_OPERATORS(FileIO::Permissions)
// 
// The operators are now defined and can be used just like:
// Permissions p = Permissions::Readable | Permissions::Writable;
//
//

#include<type_traits>

// Note: the use of ENABLE_BITMASK_OPERATORS macro must  be placed in global namespace.
#define ENABLE_BITMASK_OPERATORS(x)                     \
template<>                                              \
struct EnableBitMaskOperators<x>                        \
{                                                       \
    static const bool enable = true;                    \
};

//namespace Util
//{
    template<typename Enum>
    struct EnableBitMaskOperators
    {
        static const bool enable = false;
    };
    //////////////////////////////////////////////////////////
    // Bitwise operators (excluding shift operators << and >>)

    // OR
    template<typename Enum >
    typename std::enable_if<EnableBitMaskOperators<Enum >::enable, Enum >::type
        operator|(Enum  lhs, Enum  rhs){
        typedef typename std::underlying_type<Enum >::type underlying;
        return static_cast<Enum>(
            static_cast<underlying>(lhs) |
            static_cast<underlying>(rhs));
    }

    // AND
    template<typename Enum >
    typename std::enable_if<EnableBitMaskOperators<Enum >::enable, Enum >::type
        operator&(Enum  lhs, Enum  rhs){
        typedef typename std::underlying_type<Enum >::type underlying;
        return static_cast<Enum>(
            static_cast<underlying>(lhs)&
            static_cast<underlying>(rhs));
    }

    // XOR
    template<typename Enum >
    typename std::enable_if<EnableBitMaskOperators<Enum >::enable, Enum >::type
        operator^(Enum  lhs, Enum  rhs){
        typedef typename std::underlying_type<Enum >::type underlying;
        return static_cast<Enum>(
            static_cast<underlying>(lhs) ^
            static_cast<underlying>(rhs));
    }

    // NOT
    template<typename Enum >
    typename std::enable_if<EnableBitMaskOperators<Enum >::enable, Enum >::type
        operator~(Enum  lhs){
        typedef typename std::underlying_type<Enum >::type underlying;
        return static_cast<Enum>(~static_cast<underlying>(lhs));
    }

    //////////////////////////////////
    // Compound assignment operators.

    // OR (Compound assignment)
    template<typename Enum >
    typename std::enable_if<EnableBitMaskOperators<Enum >::enable, Enum &>::type
        operator|=(Enum & lhs, Enum  rhs){
        typedef typename std::underlying_type<Enum >::type underlying;
        lhs = static_cast<Enum>(
            static_cast<underlying>(lhs) |
            static_cast<underlying>(rhs));
        return lhs;
    }

    // AND (Compound assignment)
    template<typename Enum >
    typename std::enable_if<EnableBitMaskOperators<Enum >::enable, Enum &>::type
        operator&=(Enum & lhs, Enum  rhs){
        typedef typename std::underlying_type<Enum >::type underlying;
        lhs = static_cast<Enum>(
            static_cast<underlying>(lhs)&
            static_cast<underlying>(rhs));
        return lhs;
    }

    // XOR (Compound assignment)
    template<typename Enum >
    typename std::enable_if<EnableBitMaskOperators<Enum >::enable, Enum &>::type
        operator^=(Enum & lhs, Enum  rhs){
        typedef typename std::underlying_type<Enum >::type underlying;
        lhs = static_cast<Enum>(
            static_cast<underlying>(lhs) ^
            static_cast<underlying>(rhs));
        return lhs;
    }
//} // end of namespace Util
