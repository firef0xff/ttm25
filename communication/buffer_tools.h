#pragma once
#include <stdint.h>
#include <stddef.h>

template <class T>
uint8_t* ToBuff( T const& data, uint8_t* buff )
{
    size_t size = sizeof(T);
    T* typed_buff = reinterpret_cast<T*>( buff );
    if ( typed_buff )
        *typed_buff = data;
    buff += size;
    return buff;
}

template <class T>
uint8_t const* FromBuff( T & data, uint8_t const* buff )
{
    size_t size = sizeof(T);
    T const* typed_buff = reinterpret_cast<T const*>( buff );
    if ( typed_buff )
        data = *typed_buff;
    buff += size;
    return buff;
}

