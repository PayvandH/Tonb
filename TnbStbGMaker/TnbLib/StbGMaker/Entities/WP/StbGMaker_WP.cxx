#include <StbGMaker_WP.hxx>

#include <Entity2d_Polygon.hxx>
#include <Entity3d_Chain.hxx>
#include <Geo_Tools.hxx>
#include <Cad2d_Modeler.hxx>
#include <Cad2d_Plane.hxx>
#include <Pln_Ring.hxx>
#include <StbGMaker_Edge.hxx>
#include <TnbError.hxx>
#include <OSstream.hxx>

void tnbLib::StbGMaker_WP::MakeModeler()
{
	theModeler_ = std::make_shared<Cad2d_Modeler>();
}

tnbLib::StbGMaker_WP::StbGMaker_WP()
{
	MakeModeler();
}

tnbLib::StbGMaker_WP::StbGMaker_WP(const Standard_Real theX)
	: theX_(theX)
{
	MakeModeler();
}

tnbLib::StbGMaker_WP::StbGMaker_WP
(
	const Standard_Integer theIndex,
	const Standard_Real theX
)
	: StbGMaker_Entity(theIndex)
{
	MakeModeler();
}

tnbLib::StbGMaker_WP::StbGMaker_WP
(
	const Standard_Integer theIndex, 
	const word & theName, 
	const Standard_Real theX
)
	: StbGMaker_Entity(theIndex, theName)
{
	MakeModeler();
}

Standard_Integer 
tnbLib::StbGMaker_WP::NbPlanes() const
{
	Debug_Null_Pointer(Modeler());
	return Modeler()->NbPlanes();
}

std::shared_ptr<tnbLib::Entity3d_Chain> 
tnbLib::StbGMaker_WP::MakeMesh() const
{
	if (NOT Modeler())
	{
		return nullptr;
	}

	auto chain = std::make_shared<Entity3d_Chain>();
	Debug_Null_Pointer(chain);

	const auto& modeler = *Modeler();
	for (const auto& pedge : modeler.Edges())
	{
		const auto& edge = pedge.second;
		Debug_Null_Pointer(edge);

		if (edge->IsRing())
		{
			auto edge3 = std::dynamic_pointer_cast<StbGMaker_Edge<Pln_Ring>>(edge);
			Debug_Null_Pointer(edge3);

			if (edge3->Mesh3d())
			{
				chain->Add(*Geo_Tools::RetrieveChain(*edge3->Mesh3d()));
			}
		}
		else
		{
			auto edge3 = std::dynamic_pointer_cast<StbGMaker_Edge<Pln_Edge>>(edge);
			Debug_Null_Pointer(edge3);

			if (edge3->Mesh3d())
			{
				chain->Add(*Geo_Tools::RetrieveChain(*edge3->Mesh3d()));
			}
		}
	}
	return std::move(chain);
}

std::vector<std::shared_ptr<tnbLib::Cad2d_Plane>> 
tnbLib::StbGMaker_WP::RetrievePlanes() const
{
	std::vector<std::shared_ptr<Cad2d_Plane>> planes;
	RetrievePlanesTo(planes);
	return std::move(planes);
}

#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>

gp_Pln 
tnbLib::StbGMaker_WP::gpPln
(
	const gp_Ax2 & ax2
) const
{
	gp_Ax2 syst0(ax2.Location(), ax2.XDirection(), ax2.YDirection());
	const auto x0 = syst0.Location().Z();
	const auto ax = syst0.Translated(gp_Vec(X() - x0, 0, 0));

	gp_Pln plane(ax);
	return std::move(plane);
}

void tnbLib::StbGMaker_WP::Approx
(
	const std::shared_ptr<Pln_Edge>& theEdge
)
{
	if (theEdge->IsRing())
	{
		auto edge3 = std::dynamic_pointer_cast<StbGMaker_Edge<Pln_Ring>>(theEdge);
		if (NOT edge3)
		{
			FatalErrorIn(FunctionSIG)
				<< "the edge is not a GMaker" << endl
				<< abort(FatalError);
		}

		if (theEdge->Mesh())
		{
			auto mesh3 = std::make_shared<Entity3d_Polygon>();
			Debug_Null_Pointer(mesh3);

			auto& pts3 = mesh3->Points();
			pts3.reserve(theEdge->Mesh()->NbPoints());
			for (const auto& p : theEdge->Mesh()->Points())
			{
				Pnt3d pt3(X(), p.X(), p.Y());
				pts3.push_back(std::move(pt3));
			}

			edge3->SetMesh3d(std::move(mesh3));
		}
	}
	else
	{
		auto edge3 = std::dynamic_pointer_cast<StbGMaker_Edge<Pln_Edge>>(theEdge);
		if (NOT edge3)
		{
			FatalErrorIn(FunctionSIG)
				<< "the edge is not a GMaker" << endl
				<< abort(FatalError);
		}

		if (theEdge->Mesh())
		{
			auto mesh3 = std::make_shared<Entity3d_Polygon>();
			Debug_Null_Pointer(mesh3);

			auto& pts3 = mesh3->Points();
			pts3.reserve(theEdge->Mesh()->NbPoints());
			for (const auto& p : theEdge->Mesh()->Points())
			{
				Pnt3d pt3(X(), p.X(), p.Y());
				pts3.push_back(std::move(pt3));
			}

			edge3->SetMesh3d(std::move(mesh3));
		}
	}
}

void tnbLib::StbGMaker_WP::RetrievePlanesTo
(
	std::vector<std::shared_ptr<Cad2d_Plane>>& thePlanes
) const
{
	Debug_Null_Pointer(Modeler());
	if (NOT Modeler()->NbPlanes())
	{
		FatalErrorIn(FunctionSIG)
			<< "there is no plane to extract it:" << endl
			<< " - make sure you have created one!"
			<< abort(FatalError);
	}
	Modeler()->RetrievePlanesTo(thePlanes);
}