#pragma once
#ifndef _Pln_Wire_Header
#define _Pln_Wire_Header

#include <Pln_Entity.hxx>
#include <Pln_Orientation.hxx>
#include <Entity2d_BoxFwd.hxx>

#include <vector>

namespace tnbLib
{

	// Forward Declarations
	class Pln_Edge;
	class Pln_Curve;
	class Pln_Vertex;
	class Pln_CmpEdge;

	class Pln_Wire
		: public Pln_Entity
	{

		/*Private Data*/

		Pln_Orientation theOrientation_;

		std::shared_ptr<Pln_CmpEdge> theEdges_;

		std::shared_ptr<Entity2d_Box> theBoundingBox_;


		//- private functions

		void CalcBoundingBox(const std::vector<std::shared_ptr<Pln_Edge>>& theEdges);

		void CheckWire(const std::vector<std::shared_ptr<Pln_Edge>>& theEdges);

		void CreateWire(const std::shared_ptr<Pln_CmpEdge>& theEdge);

	public:

		Pln_Wire(const std::shared_ptr<Pln_CmpEdge>& theEdge);

		Pln_Wire
		(
			const Standard_Integer theIndex,
			const std::shared_ptr<Pln_CmpEdge>& theEdge
		);

		Pln_Wire
		(
			const Standard_Integer theIndex,
			const word& theName,
			const std::shared_ptr<Pln_CmpEdge>& theEdge
		);

		Standard_Integer NbEdges() const;

		const std::vector<std::shared_ptr<Pln_Edge>>& Edges() const;

		const std::shared_ptr<Pln_CmpEdge>& CmpEdge() const
		{
			return theEdges_;
		}

		const std::shared_ptr<Entity2d_Box>& BoundingBox() const
		{
			return theBoundingBox_;
		}

		Pln_Orientation Orientation() const
		{
			return theOrientation_;
		}

		void Reverse();

		std::vector<std::shared_ptr<Pln_Vertex>>
			RetrieveVertices() const;

		std::vector<std::shared_ptr<Pln_Curve>>
			RetrieveCurves() const;

		void RetrieveVerticesTo
		(
			std::vector<std::shared_ptr<Pln_Vertex>>& theVertices
		) const;

		void RetrieveCurvesTo
		(
			std::vector<std::shared_ptr<Pln_Curve>>& theCurves
		) const;

		//- Static functions and operators


		static std::vector<std::shared_ptr<Pln_Wire>>
			RetrieveWires
			(
				const std::vector<std::shared_ptr<Pln_Edge>>& theEdges
			);

		static Pln_Orientation RetrieveOrientation(const Pln_Wire& theWire);
	};
}

#include <Pln_WireI.hxx>

#endif // !_Pln_Wire_Header
