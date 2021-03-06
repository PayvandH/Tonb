#pragma once
#ifndef _Marine_Section_Snail_Header
#define _Marine_Section_Snail_Header

#include <Marine_Section.hxx>

namespace tnbLib
{

	namespace marineLib
	{

		class Section_Sail
			: public Marine_Section
		{

			/*Private Data*/


			/*private functions and operators*/

			friend class boost::serialization::access;

			template<class Archive>
			void serialize(Archive &ar, const unsigned int file_version)
			{
				ar & boost::serialization::base_object<Marine_Section>(*this);
			}

		protected:

			template<class... _Types>
			Section_Sail(_Types&&... _Args)
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

			Standard_Boolean IsSail() const override
			{
				return Standard_True;
			}

			Marine_SectionType Type() const override
			{
				return Marine_SectionType::sail;
			}
		};
	}
}

#endif // !_Marine_Section_Snail_Header
