#pragma once
#ifndef _SectPx_Edge_Header
#define _SectPx_Edge_Header

#include <SectPx_RegObj.hxx>

namespace tnbLib
{

	// Forward Declarations
	class SectPx_Pnt;
	class SectPx_PntTools;

	class SectPx_Edge
		: public SectPx_RegObj
	{

		friend class SectPx_PntTools;

		/*Private Data*/

		std::shared_ptr<SectPx_Pnt> theP0_;
		std::shared_ptr<SectPx_Pnt> theP1_;


		DECLARE_SAVE_LOAD_HEADER(TnbSectPx_EXPORT);

		void SetP0
		(
			const std::shared_ptr<SectPx_Pnt>& theP0
		)
		{
			theP0_ = theP0;
		}

		void SetP1
		(
			const std::shared_ptr<SectPx_Pnt>& theP1
		)
		{
			theP1_ = theP1;
		}

	protected:

		SectPx_Edge()
		{}

	public:

		static const char* typeName_;

		SectPx_Edge
		(
			const std::shared_ptr<SectPx_Pnt>& theP0,
			const std::shared_ptr<SectPx_Pnt>& theP1
		);

		SectPx_Edge
		(
			const Standard_Integer theIndex,
			const std::shared_ptr<SectPx_Pnt>& theP0,
			const std::shared_ptr<SectPx_Pnt>& theP1
		);

		SectPx_Edge
		(
			const Standard_Integer theIndex,
			const word& theName, 
			const std::shared_ptr<SectPx_Pnt>& theP0, 
			const std::shared_ptr<SectPx_Pnt>& theP1
		);

		const auto& P0() const
		{
			return theP0_;
		}

		const auto& P1() const
		{
			return theP1_;
		}

		Standard_Integer 
			IndexOf
			(
				const std::shared_ptr<SectPx_Pnt>& thePnt
			) const;

		std::shared_ptr<SectPx_Pnt> 
			Point
			(
				const Standard_Integer theIndex
			) const;

		std::shared_ptr<SectPx_Pnt> 
			Other
			(
				const std::shared_ptr<SectPx_Pnt>& thePnt
			) const;

		word RegObjTypeName() const override;

		sectPxLib::regObjType RegObjType() const override;

		Standard_Boolean IsEdge() const override;

	};
}

BOOST_CLASS_EXPORT_KEY(tnbLib::SectPx_Edge);

#endif // !_SectPx_Edge_Header
