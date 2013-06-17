#pragma once


template <class T> class Singleton
{
public:
	static T& Get()
	{
		if( !m_pInstance )
			m_pInstance = new T();
		return *m_pInstance;
	}

	static T* GetPtr()
	{
		if( !m_pInstance )
			m_pInstance = new T();
		return m_pInstance;
	}
	~Singleton()
	{
		if( m_pInstance )
			delete m_pInstance;
	}
private:
	Singleton()
	{
		m_pInstance = static_cast<T*>( this );
	}
	static T* m_pInstance;
};
template<class T> T* Singleton<T>::m_pInstance = 0;