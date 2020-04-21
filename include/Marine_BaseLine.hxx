#pragma once
#ifndef _Marine_BaseLine_Header
#define _Marine_BaseLine_Header

#include <Marine_Entity.hxx>

#include <gp_Ax1.hxx>

namespace tnbLib
{

	class Marine_BaseLine
	{

		/*Private Data*/

		gp_Ax1 theBaseLib_;

	public:

		static const gp_Ax1 null;

		Marine_BaseLine();

		Marine_BaseLine(const gp_Ax1& theAx)
			: theBaseLib_(theAx)
		{}

		Marine_BaseLine(const gp_Ax2& theSystem);

		Standard_Boolean IsNull() const;

		const auto& BaseLine() const
		{
			return theBaseLib_;
		}

		auto& BaseLine()
		{
			return theBaseLib_;
		}
	};
}

#endif // !_Marine_BaseLine_Header