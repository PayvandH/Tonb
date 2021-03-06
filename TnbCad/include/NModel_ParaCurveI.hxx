#pragma once
#include <Pnt2d.hxx>
namespace tnbLib
{

	inline
		Pnt2d tnbLib::NModel_ParaCurve::FirstCoord() const
	{
		auto p = Value(FirstParameter());
		return std::move(p);
	}

	inline
		Pnt2d tnbLib::NModel_ParaCurve::LastCoord() const
	{
		auto p = Value(LastParameter());
		return std::move(p);
	}
}