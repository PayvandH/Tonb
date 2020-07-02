#pragma once
#ifndef _Marine_Shape_Header
#define _Marine_Shape_Header

#include <Marine_Entity.hxx>
#include <Marine_ShapeType.hxx>

#include <TopoDS_Shape.hxx>

namespace tnbLib
{

	class Marine_Shape
		: public Marine_Entity
	{

		/*Private Data*/

		TopoDS_Shape theShape_;

	protected:

		Marine_Shape(const TopoDS_Shape& theShape);

		Marine_Shape
		(
			const Standard_Integer theIndex,
			const TopoDS_Shape& theShape
		);

		Marine_Shape
		(
			const Standard_Integer theIndex,
			const word& theName,
			const TopoDS_Shape& theShape
		);

	public:

		const auto& Shape() const
		{
			return theShape_;
		}

		virtual Marine_ShapeType Type() const = 0;

		virtual Standard_Boolean IsHull() const
		{
			return Standard_False;
		}

		virtual Standard_Boolean IsTank() const
		{
			return Standard_False;
		}

		virtual Standard_Boolean IsSail() const
		{
			return Standard_False;
		}
	};
}

#endif // !_Marine_Shape_Header