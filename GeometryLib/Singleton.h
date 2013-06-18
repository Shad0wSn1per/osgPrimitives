#include <assert.h>
#pragma once


template <class T> class Singleton
{
private:
	Singleton( const Singleton<T> & );
	Singleton & operator = ( const Singleton<T> & );

protected:
	static T *ms_singleton;

public:
	Singleton()
	{
		assert( !ms_singleton );
		ms_singleton = static_cast<T*>( this );
	}

	~Singleton()
	{
		assert( ms_singleton );
		ms_singleton = 0;
	}

	static T & Get()
	{
		//assert( ms_singleton );
		if( !ms_singleton )
			new T();

		return *ms_singleton;
	}

	static T * GetPtr()
	{
		return ms_singleton;
	}
};

template<class T> T* Singleton<T>::ms_singleton = 0;
