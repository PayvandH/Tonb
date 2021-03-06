#pragma once
#ifndef _SectPx_RegObj_Header
#define _SectPx_RegObj_Header

#include <SectPx_Entity.hxx>
#include <SectPx_RegObjType.hxx>

namespace tnbLib
{

	class SectPx_RegObj
		: public SectPx_Entity
	{

		/*Private Data*/


		/*private functions and operators*/

		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<SectPx_Entity>(*this);
		}

	protected:

		template<class... _Types>
		SectPx_RegObj(_Types&&... _Args)
			: SectPx_Entity(_Args...)
		{}

	public:

		virtual word RegObjTypeName() const = 0;

		virtual sectPxLib::regObjType RegObjType() const = 0;

		virtual Standard_Boolean IsParent() const;

		virtual Standard_Boolean IsChild() const;

		virtual Standard_Boolean IsEdge() const;

		virtual Standard_Boolean IsSegment() const;

		virtual Standard_Boolean IsPole() const;

		virtual Standard_Boolean IsInterface() const;

		virtual Standard_Boolean IsNode() const;

		virtual Standard_Boolean IsProfile() const;

		virtual Standard_Boolean IsCmptProfile() const;

		virtual Standard_Boolean IsFrame() const;

		virtual Standard_Boolean IsOverrider() const;

		virtual Standard_Boolean IsOffsetProfile() const;
	};
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(tnbLib::SectPx_RegObj);

#endif // !_SectPx_RegObj_Header
