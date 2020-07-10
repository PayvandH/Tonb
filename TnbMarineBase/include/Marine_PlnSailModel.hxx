#pragma once
#ifndef _Marine_PlnSailModel_Header
#define _Marine_PlnSailModel_Header

#include <Marine_GeoSailModel.hxx>

namespace tnbLib
{

	class Marine_PlnSailModel
		: public Marine_GeoSailModel
	{

		/*Private Data*/

	protected:

		template<class... _Types>
		Marine_PlnSailModel(_Types&&... _Args)
			: Marine_GeoSailModel(_Args...)
		{}

	public:

		Standard_Boolean IsPlanar() const override
		{
			return Standard_True;
		}

		virtual Standard_Boolean IsTwoDimApprox() const
		{
			return Standard_False;
		}
	};
}

#endif // !_Marine_PlnSailModel_Header