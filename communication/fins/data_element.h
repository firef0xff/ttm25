#pragma once
#include <stdint.h>
#include <memory>
#include <list>

namespace fins
{

class Element
{
public:
    typedef std::unique_ptr< Element > Ptr;
    virtual uint8_t const* Read( uint8_t const* buff, uint8_t size ) = 0;
    virtual uint8_t* Write( uint8_t* buff, uint8_t size ) const = 0;
    virtual uint8_t Size() const = 0;
protected:
    virtual void CheckSize( uint8_t size ) const = 0;
};

typedef std::list< Element::Ptr > Elements;

template <class Type>
class ConcreteElement : public Element
{
public:
    ConcreteElement()
    {
        mData.data = Type();
    }
    ConcreteElement( Type const& v )
    {
        mData.data = v;
    }

    static Element::Ptr Create()
    {
        return Element::Ptr( new ConcreteElement() );
    }
    static Element::Ptr Create( Type const& v )
    {
        return Element::Ptr( new ConcreteElement( v ) );
    }

    void operator = ( Type const& v )
    {
        mData.data = v;
    }


    virtual uint8_t const* Read( uint8_t const* buff, uint8_t size ) override
    {
        //d1:ec:42:c9
        //ec d1 c9 42

        CheckSize( size );
        uint8_t swap_size = (Size()/2)*2;
        bool p = false;
        for ( uint8_t i = 0; i < swap_size; ++i )
        {
            if ( p )
                mData.bytes[ i - 1 ] = *(buff++);
            else
                mData.bytes[ i + 1 ] = *(buff++);
            p = !p;
        }
        for ( uint8_t i = swap_size; i < Size(); ++i )
        {
            mData.bytes[ i ] = *(buff++);
        }

        return buff;
    }
    virtual uint8_t* Write( uint8_t* buff, uint8_t size ) const override
    {
        CheckSize( size );

        uint8_t swap_size = (Size()/2)*2;
        bool p = false;
        for ( uint8_t i = 0; i < swap_size; ++i )
        {
            if ( p )
                *(buff++) = mData.bytes[ i - 1 ];
            else
                *(buff++) = mData.bytes[ i + 1 ];
            p = !p;
        }
        for ( uint8_t i = swap_size; i < Size(); ++i )
        {
            *(buff++) = mData.bytes[ i ];
        }
        return buff;
    }

    Type& Data()
    {
        return mData.data;
    }
    Type const& Data() const
    {
        return mData.data;
    }
    virtual uint8_t Size() const override
    {
        return sizeof( Type );
    }
    virtual void CheckSize( uint8_t size ) const override
    {
        if ( size < Size() )
            throw std::runtime_error( "wrong element buffer size" );
    }
private:
    union Data
    {
        Type data;
        uint8_t bytes[sizeof( Type )];
    } mData;
};

template < class ProxyType, class Type>
void AddElement( fins::Elements& storage, Type ** ptr, Type value )
{
    fins::Element::Ptr item = ProxyType::Create( value );
    auto * real_type = static_cast<ProxyType*>( item.get() );
    *ptr = &real_type->Data();
    storage.push_back( std::move( item ) );
}

typedef ConcreteElement<bool>       BOOL;

typedef ConcreteElement<int16_t>    INT16;
typedef ConcreteElement<uint16_t>   UINT16;
typedef INT16                       INT;
typedef UINT16                      UIND;
typedef UINT16                      WORD;

typedef ConcreteElement<int32_t>    INT32;
typedef ConcreteElement<uint32_t>   UINT32;
typedef INT32                       DINT;
typedef UINT32                      UDINT;
typedef UINT32                      DWORD;

typedef ConcreteElement<int64_t>    INT64;
typedef ConcreteElement<uint64_t>   UINT64;
typedef INT64                       LINT;
typedef UINT64                      ULINT;
typedef UINT64                      LWORD;

typedef ConcreteElement<float>      FLOAT;
typedef FLOAT                       REAL;

typedef ConcreteElement<double>     DOUBLE;
typedef DOUBLE                      LREAL;
}//namespace fins
