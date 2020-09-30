#pragma once
#ifndef _SectPx_SlavePnt_Header
#define _SectPx_SlavePnt_Header

#include <SectPx_TPnt.hxx>

namespace tnbLib
{

	class SectPx_SlavePnt
		: public SectPx_TPnt
	{

		/*Private Data*/

	protected:

		template<class... _Types>
		SectPx_SlavePnt(_Types&&... _Args)
			: SectPx_TPnt(_Args...)
		{}

	public:

		virtual Standard_Boolean IsEmpty() const;

		Standard_Boolean IsSlave() const override;

	};
}

#endif // !_SectPx_SlavePnt_Header