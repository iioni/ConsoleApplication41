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
struct DefaultFactoryError
{
	template <typename ObjectIdentifierType>
	static auto UnknownType(ObjectIdentifierType)
	{
		throw FactoryException("factory exception: unknown type");
		return nullptr;
	}
};
//
template
<
    typename TObjectType,
    typename TObjectIdentifierType,
    size_t StorageSize = 128
>
struct DefaultFactoryStorage
{
    using ObjectTypeId = std::tuple< TObjectIdentifierType, TObjectType*(*)() >;
    //
    /*
    std::fill_n(
    std::begin(idToObject_), idToObject_.size(),
    std::make_tuple(ObjectIdentifierType{}, std::function< AbstractTypePtr() >{})
    );
    */

    void add(ObjectTypeId)
    {
        idToObject_.add(std::move(ot));
    }

    auto get( TObjectIdentifierType )
    {
        for (size_t i = 0; i < storage_index; ++i)
        {
            if (id == std::get<0>(idToObject_[i]))
            {
                return (std::get<1>(idToObject_[i]))();
            }
        }
        return (TObjectType*)nullptr;
    }

    std::array< ObjectTypeId, StorageSize > FactoryStorage;
};
//
template
<
	typename TObjectType,
	typename TObjectIdentifierType,
    typename TFactoryStorage = DefaultFactoryStorage< TObjectType, TObjectIdentifierType >
>
class Factory
//	: public FactoryErrorPolicy
{
public:
    using ObjectTypeId = std::tuple< TObjectIdentifierType, TObjectType*(*)() >;

    //
	Factory()
	{
    }

	void Register( ObjectTypeId ot )
	{
		idToObject_.add( ot );
	}

	void Register( std::initializer_list< ObjectTypeId > ots )
	{
        for ( auto i: ots)
            Register( i );
    }
/*
	void UnRegister(ObjectIdentifierType const& id)
	{        
		for (size_t i = 0; i < storage_index; ++i)
		{
			if( id == std::get<0>( idToObject_[i] ) )
			{
                idToObject_[i] = std::make_tuple( ObjectIdentifierType{}, std::function< AbstractTypePtr() >{} );
				return;
			}
		}
	}
*/
	auto CreateObject( TObjectIdentifierType const& id) const
	{
        return (idToObject_.get())();
        /*
        for (size_t i = 0; i < storage_index; ++i)
		{
			if( id == std::get<0>( idToObject_[i] ) )
			{
				return (std::get<1>(idToObject_[i]))();
			}
		}
        return (TObjectType*)nullptr;
	//	return FactoryErrorPolicy::UnknownType( id );
    */
	}

private:
    TFactoryStorage idToObject_;
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
	fct.Register( 
        {
            make_object_type("Base",  []() { return new Base(); }),
            make_object_type("Base1", []() { return new Base(); })
        }
    );
	//
	try
	{
		Base* b = fct.CreateObject( std::string("Base") );
		Base* b1 = fct.CreateObject("Base1");
		Base* b2 = fct.CreateObject("Base2");
	}
	catch (std::exception& e)
	{
		std::cout << "ops:" << e.what() << '\n';
	}
	//
    return 0;
}

