// ConsoleApplication41.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
//#include <map>
#include <string>
#include <exception>
#include <array>
#include <functional>
#include <tuple>
#include <cassert>
//
struct Base;
//
//using object_type = std::tuple< std::string, std::function<Base*()> >;
using object_type = std::tuple< std::string, Base*(*)() >;

//
//
class FactoryException : public std::exception
{
private:
	std::string what_;
public:
	FactoryException(std::string const& str) : what_(str)
	{
	}
	const char* what() const
	{
		return what_.c_str();
	}
};

/** \class DefaultFactoryError
*   \brief
*/
struct DefaultFactoryErrorPolicy
{
	template <typename ObjectIdentifierType>
	static void UnknownType(ObjectIdentifierType)
	{
		throw FactoryException("factory exception: unknown type");
	}

    static void StorageOverflow( bool val )
    {
        if( val )
            throw FactoryException("factory exception: storage overflow");
    }
};
//
struct NoExceptionFactoryErrorPolicy
{
    template <typename ObjectIdentifierType>
    static void UnknownType(ObjectIdentifierType)
    {
        assert( !"unknown type" );
    }

    static void StorageOverflow(bool val)
    {
        if (val)
            assert(!"storage overflow");
    }
};

//
template
<
    typename TObjectType,
    typename TObjectIdentifierType,
    typename TFactoryErrorPolicy = DefaultFactoryErrorPolicy,
    size_t StorageSize = 128
>
struct DefaultFactoryStorage
{
    using ObjectTypeId = std::tuple< TObjectIdentifierType, TObjectType*(*)() >;
    //
    DefaultFactoryStorage() : index_(0)
    {
        std::fill_n(
            std::begin(storage_), StorageSize, ObjectTypeId{}
        );
    }

    void add( ObjectTypeId& ot )
    {
        TFactoryErrorPolicy::StorageOverflow( index_ + 1 > StorageSize );
        storage_[ index_++ ] = std::move(ot);
    }

    auto get( TObjectIdentifierType const& id )
    {
        for (size_t i = 0; i < index_; ++i)
        {
            if (id == std::get<0>( storage_[i]) )
            {
                return (std::get<1>( storage_[i] ));
            }
        }
        TFactoryErrorPolicy::UnknownType( id );
        return (std::get<1>( ObjectTypeId{} ));
    }

    size_t index_;
    std::array< ObjectTypeId, StorageSize > storage_;
};
//
template
<
	typename TObjectType,
	typename TObjectIdentifierType,
    typename TFactoryStorage = DefaultFactoryStorage< TObjectType, TObjectIdentifierType >
>
class Factory
{
public:
    using ObjectTypeId = std::tuple< TObjectIdentifierType, TObjectType*(*)() >;
    //
    Factory() = default;

	void Register( ObjectTypeId& ot )
	{
		impl_.add( ot );
	}

	void Register( std::initializer_list< ObjectTypeId > ots )
	{
        for ( auto i: ots)
            Register( i );
    }
/*
	void UnRegister(ObjectIdentifierType const& id)
	{
        impl_.remove( id );
	}
*/
	auto CreateObject( TObjectIdentifierType const& id) 
	{
        return ( impl_.get( id ) )();
	}

private:
    TFactoryStorage impl_;
};

struct Base
{
	Base()
	{
		std::cout << "ctor base\n";
	}
};
//
auto make_object_type(std::string const& id, Base*(*fn)() )
{
    return std::make_tuple(id, fn);
}
//
void print_id( const object_type& v )
{
	std::cout << std::get<0>(v) << '\n';
}

void print_tuple( const std::string& v, std::function<Base*()> )
{
	std::cout << v << '\n';
}

void Register( std::initializer_list< object_type > const& ots )
{
	std::initializer_list< object_type >::iterator i = ots.begin(), e = ots.end();
	for( ; i != e; ++i )
		std::apply( print_tuple, (*i) );
}

int main()
{
//	auto types_for_register = {
//		make_object_type("Base",  []() { return new Base(); } ),
//		make_object_type("Base1", []() { return new Base(); } ),
//	};

//	for( const auto& args: types_for_register )
//		std::apply( print_tuple, args );
/*
	Register(
		{
			make_object_type("Base", []() { return new Base(); }),
			make_object_type("Base1", []() { return new Base(); })
		}
	);
*/

	Factory< Base, std::string > fct;
    try
    {
        fct.Register(
            {
                make_object_type("Base",  []() { return new Base(); }),
                make_object_type("Base1", []() { return new Base(); })
            }
        );
    	//
		Base* b = fct.CreateObject( std::string("Base") );
		Base* b1 = fct.CreateObject(std::string("Base1") );
		Base* b2 = fct.CreateObject(std::string("Base2") );
	}
	catch (std::exception& e)
	{
		std::cout << "ops:" << e.what() << '\n';
	}
	//
    return 0;
}

