// GeometryLib.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "IProcedural.h"
#include "Factory.h"

extern "C"
	{
		__declspec( dllexport ) bool __stdcall GetInterface( Utility::GeometryFactory::IFactory **pInterface )
		{
			*pInterface = 0;
			*pInterface = new Utility::GeometryFactory::Factory();
			return *pInterface != 0;
		}


		__declspec( dllexport ) void __stdcall FreeInterface( Utility::GeometryFactory::IFactory *pInterface )
		{
			if ( pInterface )
				delete (Utility::GeometryFactory::Factory*)pInterface;
		}
	}
