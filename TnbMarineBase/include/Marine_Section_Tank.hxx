#pragma once
#ifndef _Marine_Section_Tank_Header
#define _Marine_Section_Tank_Header

#include <Marine_Section.hxx>

namespace tnbLib
{

	namespace marineLib
	{

		class Section_Tank
			: public Marine_Section
		{

			/*Private Data*/

		protected:

			template<class... _Types>
			Section_Tank(_Types&&... _Args)
				: Marine_Section(_Args...)
			{}

		public:

			virtual Standard_Boolean IsInner() const
			{
				return Standard_False;
			}

			virtual Standard_Boolean IsOuter() const
			{
				return Standard_False;
			}

			Standard_Boolean IsTank() const override
			{
				return Standard_True;
			}

			Marine_SectionType Type() const override
			{
				return Marine_SectionType::tank;
			}
		};
	}
}

#endif // !_Marine_Section_Tank_Header