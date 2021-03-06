#pragma once
#ifndef _Geo_ProfileFun_Geom_Header
#define _Geo_ProfileFun_Geom_Header

#include <Standard_Handle.hxx>
#include <Geo_ProfileFun.hxx>

class Geom2d_Curve;

namespace tnbLib
{

	namespace GeoLib
	{

		class ProfileFun_Geom
			: public Geo_ProfileFun
		{

			/*Private Data*/

			Handle(Geom2d_Curve) theCurve_;


			DECLARE_SAVE_LOAD_HEADER(TnbGeo_EXPORT);

		public:

			ProfileFun_Geom
			(
				const Handle(Geom2d_Curve)& theCurve,
				const Standard_Real theLower,
				const Standard_Real theUpper
			);

			ProfileFun_Geom
			(
				Handle(Geom2d_Curve)&& theCurve, 
				const Standard_Real theLower, 
				const Standard_Real theUpper
			);

			ProfileFun_Geom
			(
				const Standard_Integer theIndex, 
				const Handle(Geom2d_Curve)& theCurve, 
				const Standard_Real theLower, 
				const Standard_Real theUpper
			);

			ProfileFun_Geom
			(
				const Standard_Integer theIndex,
				Handle(Geom2d_Curve)&& theCurve,
				const Standard_Real theLower, 
				const Standard_Real theUpper
			);

			ProfileFun_Geom
			(
				const Standard_Integer theIndex, 
				const word& theName, 
				const Handle(Geom2d_Curve)& theCurve, 
				const Standard_Real theLower, 
				const Standard_Real theUpper
			);

			ProfileFun_Geom
			(
				const Standard_Integer theIndex, 
				const word& theName, 
				Handle(Geom2d_Curve)&& theCurve, 
				const Standard_Real theLower,
				const Standard_Real theUpper
			);


			const auto& Curve() const
			{
				return theCurve_;
			}

			Standard_Real Value(const Standard_Real x) const override;


		};
	}
}

#endif // !_Geo_ProfileFun_Geom_Header
