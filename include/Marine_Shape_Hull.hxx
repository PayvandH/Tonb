#pragma once
#ifndef _Marine_Shape_Hull_Header
#define _Marine_Shape_Hull_Header

#include <Marine_Shape.hxx>

namespace tnbLib
{

	namespace marineLib
	{

		class Shape_Hull
			: public Marine_Shape
		{

			/*Private Data*/


			/*private functions and operators*/

			friend class boost::serialization::access;

			template<class Archive>
			void serialize(Archive &ar, const unsigned int file_version)
			{
				ar & boost::serialization::base_object<Marine_Shape>(*this);
			}

		public:

			template<class... _Types>
			Shape_Hull(_Types&&... _Args)
				: Marine_Shape(_Args...)
			{}


			Marine_ShapeType Type() const override
			{
				return Marine_ShapeType::hull;
			}

			Standard_Boolean IsHull() const override
			{
				return Standard_True;
			}
		};
	}
}

#endif // !_Marine_Shape_Hull_Header
