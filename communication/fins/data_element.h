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
        CheckSize( size );
        for ( uint8_t i = Size(); i > 0; --i )
        {
            mData.bytes[ i - 1  ] = *(buff++);
        }
        return buff;
    }
    virtual uint8_t* Write( uint8_t* buff, uint8_t size ) const override
    {
        CheckSize( size );

        for ( uint8_t i = Size(); i > 0; --i )
        {
            *(buff++) = mData.bytes[ i - 1  ];
        }
        return buff;
    }

    Type& Data()
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

typedef ConcreteElement<bool>       BOOL_ELEMENT;
typedef ConcreteElement<int16_t>    INT16_ELEMENT;
typedef ConcreteElement<uint16_t>   UINT16_ELEMENT;
}//namespace fins
