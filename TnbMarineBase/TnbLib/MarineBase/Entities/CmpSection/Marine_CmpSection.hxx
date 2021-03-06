#pragma once
#ifndef _Marine_CmpSection_Header
#define _Marine_CmpSection_Header

#include <Entity2d_BoxFwd.hxx>
#include <Marine_CoordinatedEntity.hxx>
#include <Marine_SectionType.hxx>
#include <Marine_Module.hxx>
#include <OFstream.hxx>

#include <vector>
#include <list>

#include <Standard_Handle.hxx>

class TopoDS_Edge;
class TopoDS_Shape;
class Geom2d_Curve;
class gp_Trsf2d;

namespace tnbLib
{

	// Forward Declarations
	class MarineBase_Tools;
	class Marine_Section;
	class Marine_SectTools;
	class Pln_Curve;
	class Pln_Wire;

	class Marine_CmpSection
		: public Marine_CoordinatedEntity
	{

		friend class MarineBase_Tools;
		friend class Marine_SectTools;

		/*Private Data*/

		std::vector<std::shared_ptr<Marine_Section>> theSections_;


		/*private functions and operators*/

		DECLARE_SAVE_LOAD_HEADER(TnbMarine_EXPORT);

	protected:	

		void Insert
		(
			const std::shared_ptr<Marine_Section>& theSection
		);

	protected:

		Marine_CmpSection();

		Marine_CmpSection
		(
			const Standard_Integer theIndex
		);

		Marine_CmpSection
		(
			const Standard_Integer theIndex,
			const word& theName
		);

		Marine_CmpSection
		(
			const std::shared_ptr<Marine_Section>& theSection
		);

		Marine_CmpSection
		(
			const Standard_Integer theIndex, 
			const std::shared_ptr<Marine_Section>& theSection
		);

		Marine_CmpSection
		(
			const Standard_Integer theIndex, 
			const word& theName, 
			const std::shared_ptr<Marine_Section>& theSection
		);

		static Standard_Boolean 
			LessDiameterSize
			(
				const std::shared_ptr<Pln_Wire>& theW0, 
				const std::shared_ptr<Pln_Wire>& theW1
			);

		static void 
			SortWires
			(
				std::vector<std::shared_ptr<Pln_Wire>>& theWires
			);

		static void 
			RetrieveInnerOuterWires
			(
				std::list<std::shared_ptr<Pln_Wire>>& theWires,
				std::shared_ptr<Pln_Wire>& theOuter, 
				std::vector<std::shared_ptr<Pln_Wire>>& theInners
			);

	public:

		static const std::shared_ptr<Marine_CmpSection> null;

		Entity2d_Box BoundingBox() const;

		auto NbSections() const
		{
			return (Standard_Integer)theSections_.size();
		}

		const auto& Sections() const
		{
			return theSections_;
		}

		auto& ChangeSections()
		{
			return theSections_;
		}

		Marine_SectionType Type() const;

		virtual Standard_Boolean IsXsection() const
		{
			return Standard_False;
		}

		virtual Standard_Boolean IsZsection() const
		{
			return Standard_False;
		}

		virtual Standard_Real X() const = 0; 

		virtual std::shared_ptr<Marine_CmpSection> Copy() const = 0;

		void Transform(const gp_Trsf2d& t);

		void ExportToPlt(OFstream& File) const;

		static std::vector<Handle(Geom2d_Curve)>
			RetrieveParaCurves
			(
				const std::vector<TopoDS_Edge>& theEdges,
				const gp_Ax2& theSystem
			);

		static std::vector<TopoDS_Edge>
			RetrieveEdges
			(
				const TopoDS_Shape& theEdges
			);

	};
}

#endif // !_Marine_CmpSection_Header
