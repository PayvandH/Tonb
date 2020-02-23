#include <MarineBase_Tools.hxx>

#include <Pnt2d.hxx>
#include <Pnt3d.hxx>
#include <Cad2d_CmptLib.hxx>
#include <Marine_Section.hxx>
#include <Marine_CmpSection.hxx>
#include <Marine_CrossSection.hxx>
#include <NumAlg_AdaptiveInteg_Info.hxx>
#include <UnitSystem.hxx>
#include <error.hxx>
#include <OSstream.hxx>

#include <gp_Trsf2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Ax2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <StdFail_NotDone.hxx>

Handle(Geom2d_Curve)
tnbLib::MarineBase_Tools::Curve
(
	const std::vector<marineLib::xSectionParam>& theQ
)
{
	if (theQ.size() < 2)
	{
		FatalErrorIn("Standard_Real MarineBase_Tools::CalcArea(const std::vector<marineLib::xSectionParam>& theQ)")
			<< "Not enough points"
			<< abort(FatalError);
	}

	Handle(TColgp_HArray1OfPnt2d) PtsPtr = new TColgp_HArray1OfPnt2d(1, (Standard_Integer)theQ.size());
	auto& Pts = *PtsPtr;

	Standard_Integer K = 0;
	for (const auto& x : theQ)
	{
		Pts.SetValue(++K, gp_Pnt2d(x.x, x.value));
	}

	Geom2dAPI_Interpolate Interpolation(PtsPtr, Standard_False, 1.0e-6);
	Interpolation.Perform();

	if (!Interpolation.IsDone())
	{
		FatalErrorIn("Standard_Real MarineBase_Tools::CalcArea(const std::vector<marineLib::xSectionParam>& theQ)")
			<< "Failed to interpolation!"
			<< abort(FatalError);
	}

	try
	{
		return Interpolation.Curve();
	}
	catch (StdFail_NotDone&)
	{
		return nullptr;
	}
}

Standard_Real 
tnbLib::MarineBase_Tools::CalcArea
(
	const std::vector<marineLib::xSectionParam>& theQ,
	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
)
{
	if (theQ.size() < 2)
	{
		FatalErrorIn("Standard_Real MarineBase_Tools::CalcArea(const std::vector<marineLib::xSectionParam>& theQ)")
			<< "Not enough points"
			<< abort(FatalError);
	}

	Handle(TColgp_HArray1OfPnt2d) PtsPtr = new TColgp_HArray1OfPnt2d(1, (Standard_Integer)theQ.size());
	auto& Pts = *PtsPtr;

	Standard_Integer K = 0;
	for (const auto& x : theQ)
	{
		Pts.SetValue(++K, gp_Pnt2d(x.x, x.value));
	}

	Geom2dAPI_Interpolate Interpolation(PtsPtr, Standard_False, 1.0e-6);
	Interpolation.Perform();

	if (!Interpolation.IsDone())
	{
		FatalErrorIn("Standard_Real MarineBase_Tools::CalcArea(const std::vector<marineLib::xSectionParam>& theQ)")
			<< "Failed to interpolation!"
			<< abort(FatalError);
	}

	try
	{
		return Cad2d_CmptLib::AreaUnderCurve(Interpolation.Curve(), 0, theInfo);
	}
	catch (StdFail_NotDone&)
	{
		FatalErrorIn("Standard_Real MarineBase_Tools::CalcArea(const std::vector<marineLib::xSectionParam>& theQ)")
			<< "Failed to interpolation!"
			<< abort(FatalError);

		return 0;
	}
}

Standard_Real
tnbLib::MarineBase_Tools::CalcArea
(
	const Marine_Section & theSection, 
	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
)
{
	if (NOT theSection.Wire())
	{
		FatalErrorIn("Standard_Real CalcArea(const Marine_Section& theSection, const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo)")
			<< "invalid section: null wire" << endl
			<< abort(FatalError);
	}

	return Cad2d_CmptLib::Area(*theSection.Wire(), theInfo);
}

Standard_Real 
tnbLib::MarineBase_Tools::CalcArea
(
	const Marine_CmpSection & theSection,
	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
)
{
	Standard_Real sumArea = 0;
	for (const auto& x : theSection.Sections())
	{
		Debug_Null_Pointer(x);
		sumArea += CalcArea(*x, theInfo);
	}
	return sumArea;
}

Standard_Real 
tnbLib::MarineBase_Tools::CalcWetArea
(
	const Marine_CmpSection & theSection, 
	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
)
{
	Standard_Real sumArea = 0;
	for (const auto& x : theSection.Sections())
	{
		Debug_Null_Pointer(x);
		if(x->IsDry())
			continue;

		sumArea += CalcArea(*x, theInfo);
	}
	return sumArea;
}

tnbLib::Pnt2d 
tnbLib::MarineBase_Tools::CalcCentre
(
	const Marine_Section & theSection, 
	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
)
{
	if (NOT theSection.Wire())
	{
		FatalErrorIn("Pnt2d CalcCentre(const Marine_Section& theSection, const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo)")
			<< "invalid section: null wire" << endl
			<< abort(FatalError);
	}

	auto c = Cad2d_CmptLib::Centre(*theSection.Wire(), theInfo);
	return std::move(c);
}

tnbLib::Pnt2d 
tnbLib::MarineBase_Tools::CalcCentre
(
	const Marine_CmpSection & theSection, 
	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
)
{
	std::vector<Pnt2d> cs;
	cs.reserve(theSection.NbSections());

	std::vector<Standard_Real> h;
	h.reserve(theSection.NbSections());

	for (const auto& x : theSection.Sections())
	{
		Debug_Null_Pointer(x);

		auto area = CalcArea(*x, theInfo);
		auto c = CalcCentre(*x, theInfo);

		cs.push_back(std::move(c));
		h.push_back(area);
	}

	if (cs.size() IS_EQUAL 1)
		return cs[0];

	Pnt2d sum;
	Standard_Integer K = 0;
	for (const auto& x : cs)
	{
		sum += h[K] * x;
	}

	Standard_Real sumA = 0;
	for (const auto x : h)
	{
		sumA += x;
	}

	return sum / sumA;
}

tnbLib::Pnt2d 
tnbLib::MarineBase_Tools::CalcWetCentre
(
	const Marine_CmpSection & theSection, 
	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
)
{
	std::vector<Pnt2d> cs;
	std::vector<Standard_Real> h;

	for (const auto& x : theSection.Sections())
	{
		Debug_Null_Pointer(x);

		if(x->IsDry())
			continue;

		auto area = CalcArea(*x, theInfo);
		auto c = CalcCentre(*x, theInfo);

		cs.push_back(std::move(c));
		h.push_back(area);
	}

	if (cs.size() IS_EQUAL 1)
		return cs[0];

	Pnt2d sum;
	Standard_Integer K = 0;
	for (const auto& x : cs)
	{
		sum += h[K] * x;
	}

	Standard_Real sumA = 0;
	for (const auto x : h)
	{
		sumA += x;
	}

	return sum / sumA;
}

//tnbLib::Pnt2d 
//tnbLib::MarineBase_Tools::CalcWetCentre
//(
//	const Marine_CmpSection & theSection, 
//	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
//)
//{
//	
//}

Standard_Real 
tnbLib::MarineBase_Tools::CalcVolume
(
	const std::vector<std::shared_ptr<Marine_CmpSection>>& theSections,
	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
)
{
	std::vector<marineLib::xSectionParam> params;
	for (const auto& x : theSections)
	{
		Debug_Null_Pointer(x);

		marineLib::xSectionParam xParam;

		xParam.x = x->X();
		xParam.value = CalcArea(*x, theInfo);

		params.push_back(std::move(xParam));
	}

	return CalcArea(params, theInfo);
}

tnbLib::Pnt3d 
tnbLib::MarineBase_Tools::CalcCentreProductVolume
(
	const std::vector<std::shared_ptr<Marine_CmpSection>>& theSections, 
	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
)
{
	std::vector<Pnt2d> Bx;
	Bx.reserve(theSections.size());

	std::vector<Standard_Real> Ax;
	Ax.reserve(theSections.size());

	for (const auto& x : theSections)
	{
		Debug_Null_Pointer(x);
		
		auto bx = CalcCentre(*x, theInfo);

		Bx.push_back(std::move(bx));
		Ax.push_back(CalcArea(*x, theInfo));
	}

	std::vector<marineLib::xSectionParam> xBarQ;
	xBarQ.reserve(theSections.size());

	Standard_Integer K = 0;
	for (const auto& x : theSections)
	{
		Debug_Null_Pointer(x);
		
		marineLib::xSectionParam xSect;
		xSect.x = x->X();
		xSect.value = Ax[K++] * x->X();

		xBarQ.push_back(std::move(xSect));
	}

	const auto xBarC = Curve(xBarQ);
	if (NOT xBarC)
	{
		FatalErrorIn("Pnt3d MarineBase_Tools::CalcCentre(Args...)")
			<< "Unable to interpolate the offset points: xBarC" << endl
			<< abort(FatalError);
	}

	std::vector<marineLib::xSectionParam> yBarQ;
	yBarQ.reserve(theSections.size());

	K = 0;
	for (const auto& x : theSections)
	{
		Debug_Null_Pointer(x);

		marineLib::xSectionParam xSect;
		xSect.x = x->X();
		xSect.value = Bx[K].X() * Ax[K];

		K++;

		yBarQ.push_back(std::move(xSect));
	}

	const auto yBarC = Curve(yBarQ);
	if (NOT yBarC)
	{
		FatalErrorIn("Pnt3d MarineBase_Tools::CalcCentre(Args...)")
			<< "Unable to interpolate the offset points: yBarC" << endl
			<< abort(FatalError);
	}

	std::vector<marineLib::xSectionParam> zBarQ;
	zBarQ.reserve(theSections.size());

	K = 0;
	for (const auto& x : theSections)
	{
		Debug_Null_Pointer(x);

		marineLib::xSectionParam xSect;
		xSect.x = x->X();
		xSect.value = Bx[K++].Y() * Ax[K];

		K++;

		zBarQ.push_back(std::move(xSect));
	}

	const auto zBarC = Curve(zBarQ);
	if (NOT zBarC)
	{
		FatalErrorIn("Pnt3d MarineBase_Tools::CalcCentre(Args...)")
			<< "Unable to interpolate the offset points: zBarC" << endl
			<< abort(FatalError);
	}

	//const auto vol = Cad2d_CmptLib::AreaUnderCurve(xBarC, 0, theInfo);

	const auto xBar = Cad2d_CmptLib::AreaUnderCurve(xBarC, 0, theInfo);
	const auto yBar = Cad2d_CmptLib::AreaUnderCurve(yBarC, 0, theInfo);
	const auto zBar = Cad2d_CmptLib::AreaUnderCurve(zBarC, 0, theInfo);

	return Pnt3d(xBar, yBar, zBar);
}

void tnbLib::MarineBase_Tools::Heel
(
	const std::shared_ptr<Marine_CmpSection>& theSection,
	const gp_Ax2d & theAx
)
{
	const auto& sys = theSection->CoordinateSystem();
	const auto& loc = sys.Location();

	gp_Ax2d O(Pnt2d(loc.X(), loc.Y()), gp_Dir2d(1, 0));

	gp_Trsf2d t;
	t.SetTransformation(O, theAx);

	theSection->Transform(t.Inverted());

	gp_Trsf2d t1;
	t1.SetTranslation(theAx.Location(), O.Location());

	theSection->Transform(t1);
}

//void tnbLib::MarineBase_Tools::Heel
//(
//	const std::shared_ptr<Marine_Section>& theSection,
//	const gp_Ax2d & theAx
//)
//{
//	const auto& sys = theSection->CoordinateSystem();
//	const auto& loc = sys.Location();
//
//	gp_Ax2d O(Pnt2d(loc.X(), loc.Y()), gp_Dir2d(1, 0));
//
//	gp_Trsf2d t;
//	t.SetTransformation(O, theAx);
//
//	theSection->Transform(t.Inverted());
//
//	
//}

void tnbLib::MarineBase_Tools::Heel
(
	const std::shared_ptr<Marine_CrossSection>& theModel, 
	const gp_Ax1 & theAx, 
	const Standard_Real theAngle
)
{
	const auto& loc = theAx.Location();
	const auto angle = ConvertUnit(UnitSystem_Angle_deg, UnitSystem_Angle_rad)*theAngle;

	Pnt2d O(loc.Y(), loc.Z());
	gp_Dir2d dir(cos(angle), sin(angle));

	gp_Ax2d ax(O, dir);

	for (const auto& x : theModel->Sections())
	{
		Debug_Null_Pointer(x);
		Heel(x, ax);
	}
}

#include <Cad2d_Plane.hxx>
#include <Cad2d_Boolean.hxx>
#include <Cad_Tools.hxx>
#include <Marine_WetSection.hxx>
#include <Marine_Wave.hxx>
#include <Marine_WetSection.hxx>
#include <Marine_DrySection.hxx>

#include <Standard_Handle.hxx>
#include <gp_Pln.hxx>
#include <Geom_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Plane.hxx>
#include <GeomProjLib.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Edge.hxx>

namespace tnbLib
{

	namespace marineLib
	{

		static Handle(Geom_Curve)
			CreateLine
			(
				const Pnt3d& theP0,
				const Pnt3d& theP1
			)
		{
			gp_Vec v(theP0, theP1);
			Handle(Geom_Curve) c = new Geom_Line(theP0, gp_Dir(v));

			GeomAPI_ProjectPointOnCurve proj;
			proj.Init(theP0, c);

			const auto u0 = proj.LowerDistanceParameter();

			proj.Init(theP1, c);

			const auto u1 = proj.LowerDistanceParameter();

			auto trimmed = Cad_Tools::ConvertToTrimmed(c, u0, u1);
			return std::move(trimmed);
		}
	}
}

std::shared_ptr<tnbLib::Pln_Wire> 
tnbLib::MarineBase_Tools::WaterSection
(
	const Handle(Geom_Curve)& theCurve,
	const gp_Ax2 & theSystem,
	const Standard_Real theZmin, 
	const Standard_Real theMinTol,
	const Standard_Real theMaxTol
)
{
	Pnt3d leftPt;
	Pnt3d rightPt;

	auto p0 = theCurve->Value(theCurve->FirstParameter());
	auto p1 = theCurve->Value(theCurve->LastParameter());

	if (p0.Y() < p1.Y())
	{
		leftPt = p0;
		rightPt = p1;
	}
	else
	{
		leftPt = p1;
		rightPt = p0;
	}

	auto p2 = leftPt + (theZmin - leftPt.Z()) * Pnt3d(theSystem.XDirection().XYZ());
	auto p3 = rightPt + (theZmin - rightPt.Z()) * Pnt3d(theSystem.XDirection().XYZ());

	auto c0 = marineLib::CreateLine(leftPt, p2);
	auto c1 = marineLib::CreateLine(p2, p3);
	auto c2 = marineLib::CreateLine(p3, rightPt);

	gp_Pln plane(theSystem);
	Handle(Geom_Plane) g = new Geom_Plane(plane);

	auto cprj0 = GeomProjLib::ProjectOnPlane(c0, g, theSystem.Direction(), Standard_True);
	auto cprj1 = GeomProjLib::ProjectOnPlane(c1, g, theSystem.Direction(), Standard_True);
	auto cprj2 = GeomProjLib::ProjectOnPlane(c2, g, theSystem.Direction(), Standard_True);
	auto cprj3 = GeomProjLib::ProjectOnPlane(theCurve, g, theSystem.Direction(), Standard_True);

	auto c2d0 = GeomProjLib::Curve2d(cprj0, g);
	if (NOT c2d0)
	{
		FatalErrorIn(FunctionSIG)
			<< "unable to project the curve on the plane!" << endl
			<< abort(FatalError);
	}

	auto c2d1 = GeomProjLib::Curve2d(cprj1, g);
	if (NOT c2d1)
	{
		FatalErrorIn(FunctionSIG)
			<< "unable to project the curve on the plane!" << endl
			<< abort(FatalError);
	}

	auto c2d2 = GeomProjLib::Curve2d(cprj2, g);
	if (NOT c2d2)
	{
		FatalErrorIn(FunctionSIG)
			<< "unable to project the curve on the plane!" << endl
			<< abort(FatalError);
	}

	auto c2d3 = GeomProjLib::Curve2d(cprj3, g);
	if (NOT c2d3)
	{
		FatalErrorIn(FunctionSIG)
			<< "unable to project the curve on the plane!" << endl
			<< abort(FatalError);
	}

	std::vector<Handle(Geom2d_Curve)> curves;
	curves.push_back(c2d0);
	curves.push_back(c2d1);
	curves.push_back(c2d2);
	curves.push_back(c2d3);

	auto cmpSection =
		Marine_CmpSection::CreateCmpSection(curves, theSystem, theMinTol, theMaxTol);
	Debug_Null_Pointer(cmpSection);

	if (cmpSection->NbSections() NOT_EQUAL 1)
	{
		FatalErrorIn(FunctionSIG)
			<< "contradictory data" << endl
			<< abort(FatalError);
	}

	auto wire = cmpSection->Sections()[0]->Wire();
	Debug_Null_Pointer(wire);

	return std::move(wire);
}

namespace tnbLib
{

	namespace marineLib
	{

		static std::shared_ptr<Marine_Section>
			CreateWaterSection
			(
				const std::shared_ptr<Pln_Wire>& theSection,
				const std::shared_ptr<Pln_Wire>& theWater
			)
		{
			auto section = Cad2d_Plane::MakePlane(theSection, nullptr);
			Debug_Null_Pointer(section);

			auto water = Cad2d_Plane::MakePlane(theWater, nullptr);
			Debug_Null_Pointer(water);

			auto intsct = Cad2d_Boolean::Intersection(section, water);

			if (intsct)
			{
				if (intsct IS_EQUAL section)
				{
					Debug_Null_Pointer(intsct->OuterWire());

					auto wet = std::make_shared<Marine_WetSection>(intsct->OuterWire());
					Debug_Null_Pointer(wet);

					wet->SetDeep(Standard_True);
					return std::move(wet);
				}
				else
				{
					Debug_Null_Pointer(intsct->OuterWire());

					auto wet = std::make_shared<Marine_WetSection>(intsct->OuterWire());
					Debug_Null_Pointer(wet);

					wet->SetDeep(Standard_False);
					return std::move(wet);
				}
			}
			return nullptr;
		}

		static Handle(Geom_Curve)
			WaterLine
			(
				const Handle(Geom_Surface)& theSurface,
				const gp_Ax2& theSystem
			)
		{
			gp_Pln plane(theSystem);

			BRepAlgoAPI_Section alg;
			alg.SetRunParallel(Standard_True);
			alg.Init1(plane);
			alg.Init2(theSurface);

			alg.Build();

			if (alg.Shape().IsNull())
			{
				FatalErrorIn(FunctionSIG)
					<< "Failed to calculate intersection" << endl
					<< abort(FatalError);
			}

			auto edges = Marine_CmpSection::RetrieveEdges(alg.Shape());
			if (edges.size() NOT_EQUAL 1)
			{
				FatalErrorIn(FunctionSIG)
					<< "Contradictory data" << endl
					<< abort(FatalError);
			}

			Standard_Real f, l;
			auto curve = BRep_Tool::Curve(edges[0], f, l);
			Debug_Null_Pointer(curve);

			if (NOT Handle(Geom_TrimmedCurve)::DownCast(curve))
			{
				curve = new Geom_TrimmedCurve(curve, f, l);
			}

			return std::move(curve);
		}
	}
}

std::vector<std::shared_ptr<tnbLib::Marine_Section>> 
tnbLib::MarineBase_Tools::WaterSections
(
	const Marine_CrossSection& theModel,
	const Marine_Wave& theWave,
	const Entity3d_Box& theDomain,
	const Standard_Real theMinTol,
	const Standard_Real theMaxTol
)
{
	const auto zmin = theDomain.P0().Z();
	const auto& waterSurface = theWave.SurfaceGeometry();
	if (NOT waterSurface)
	{
		FatalErrorIn("std::vector<std::shared_ptr<Marine_Section>> MarineBase_Tools::WaterSections(Args...)")
			<< "no geometry is found for water surface" << endl
			<< abort(FatalError);
	}

	std::vector<std::shared_ptr<Marine_Section>> waters;
	Standard_Integer K = 0;
	for (const auto& x : theModel.Sections())
	{
		Debug_Null_Pointer(x);

		auto wl = marineLib::WaterLine(waterSurface, x->CoordinateSystem());
		Debug_Null_Pointer(wl);

		auto wa = WaterSection(wl, x->CoordinateSystem(), zmin, theMinTol, theMaxTol);
		Debug_Null_Pointer(wa);

		auto section = std::make_shared<Marine_Section>(++K, wa);
		Debug_Null_Pointer(section);

		section->SetCoordinateSystem(x->CoordinateSystem());
		waters.push_back(std::move(section));
	}
	return std::move(waters);
}

std::shared_ptr<tnbLib::Marine_Section> 
tnbLib::MarineBase_Tools::WettedSection
(
	const std::shared_ptr<Pln_Wire>& theSection,
	const std::shared_ptr<Pln_Wire>& theWater
)
{
	auto section = Cad2d_Plane::MakePlane(theSection, nullptr);
	Debug_Null_Pointer(section);

	auto water = Cad2d_Plane::MakePlane(theWater, nullptr);
	Debug_Null_Pointer(water);
	cout << "injaaaaaaaaa" << std::endl;
	auto intsct = Cad2d_Boolean::Intersection(section, water);
	PAUSE;
	if (intsct)
	{
		if (intsct IS_EQUAL section)
		{
			Debug_Null_Pointer(intsct->OuterWire());

			auto wet = std::make_shared<Marine_WetSection>(intsct->OuterWire());
			Debug_Null_Pointer(wet);

			wet->SetDeep(Standard_True);
			return std::move(wet);
		}
		else
		{
			Debug_Null_Pointer(intsct->OuterWire());

			auto wet = std::make_shared<Marine_WetSection>(intsct->OuterWire());
			Debug_Null_Pointer(wet);

			wet->SetDeep(Standard_False);
			return std::move(wet);
		}
	}
	return nullptr;
}

std::shared_ptr<tnbLib::Marine_CmpSection>
tnbLib::MarineBase_Tools::WettedSection
(
	const std::shared_ptr<Marine_CmpSection>& theSection,
	const std::shared_ptr<Marine_Section>& theWater
)
{
	auto cmpSection = std::make_shared<Marine_CmpSection>(theSection->Index(), theSection->Name());
	Debug_Null_Pointer(cmpSection);

	auto& sections = cmpSection->ChangeSections();
	for (const auto& x : theSection->Sections())
	{
		Debug_Null_Pointer(x);

		auto wet = WettedSection(x->Wire(), theWater->Wire());
		if (wet)
		{
			sections.push_back(std::move(wet));
		}
	}
	if (sections.size())
	{
		return std::move(cmpSection);
	}
	return nullptr;
}

std::shared_ptr<tnbLib::Marine_Section> 
tnbLib::MarineBase_Tools::DrySection
(
	const std::shared_ptr<Pln_Wire>& theSection,
	const std::shared_ptr<Pln_Wire>& theWater
)
{
	auto section = Cad2d_Plane::MakePlane(theSection, nullptr);
	Debug_Null_Pointer(section);

	auto water = Cad2d_Plane::MakePlane(theWater, nullptr);
	Debug_Null_Pointer(water);

	auto subt = Cad2d_Boolean::Subtract(section, water);
	if (subt)
	{
		Debug_Null_Pointer(subt->OuterWire());

		auto dry = std::make_shared<Marine_DrySection>(subt->OuterWire());
		Debug_Null_Pointer(dry);
		return std::move(dry);
	}
	return nullptr;
}

std::shared_ptr<tnbLib::Marine_CmpSection>
tnbLib::MarineBase_Tools::DrySection
(
	const std::shared_ptr<Marine_CmpSection>& theSection,
	const std::shared_ptr<Marine_Section>& theWater
)
{
	auto cmpSection = std::make_shared<Marine_CmpSection>(theSection->Index(), theSection->Name());
	Debug_Null_Pointer(cmpSection);

	auto& sections = cmpSection->ChangeSections();
	for (const auto& x : theSection->Sections())
	{
		Debug_Null_Pointer(x);

		auto dry = DrySection(x->Wire(), theWater->Wire());
		if (dry)
		{
			sections.push_back(std::move(dry));
		}
	}
	if (sections.size())
	{
		return std::move(cmpSection);
	}
	return nullptr;
}

std::vector<std::shared_ptr<tnbLib::Marine_CmpSection>> 
tnbLib::MarineBase_Tools::WettedSections
(
	const std::shared_ptr<Marine_CrossSection>& theModel,
	const std::vector<std::shared_ptr<Marine_Section>>& theWaterSections
)
{
	std::vector<std::shared_ptr<Marine_CmpSection>> wetted;

	Standard_Integer K = 0;
	const auto& sections = theModel->Sections();
	for (const auto& x : sections)
	{
		Debug_Null_Pointer(x);

		const auto& wa = theWaterSections[K++];
		Debug_Null_Pointer(wa);

		auto wet = WettedSection(x, wa);
		if (wet)
		{
			wetted.push_back(std::move(wet));
		}
	}
	return std::move(wetted);
}

std::vector<std::shared_ptr<tnbLib::Marine_CmpSection>>
tnbLib::MarineBase_Tools::DrySections
(
	const std::shared_ptr<Marine_CrossSection>& theModel,
	const std::vector<std::shared_ptr<Marine_Section>>& theWaterSections
)
{
	std::vector<std::shared_ptr<Marine_CmpSection>> drySections;

	Standard_Integer K = 0;
	const auto& sections = theModel->Sections();
	for (const auto& x : sections)
	{
		Debug_Null_Pointer(x);

		const auto& wa = theWaterSections[K++];
		Debug_Null_Pointer(wa);

		auto dry = DrySection(x, wa);
		if (dry)
		{
			drySections.push_back(std::move(dry));
		}
	}
	return std::move(drySections);
}

//Standard_Real 
//tnbLib::MarineBase_Tools::CalcWetVolume
//(
//	const std::vector<std::shared_ptr<Marine_CmpSection>>& theSections, 
//	const std::shared_ptr<NumAlg_AdaptiveInteg_Info>& theInfo
//)
//{
//	std::vector<marineLib::xSectionParam> params;
//	for (const auto& x : theSections)
//	{
//		Debug_Null_Pointer(x);
//
//		marineLib::xSectionParam xParam;
//
//		xParam.x = x->X();
//		xParam.value = CalcWetArea(*x, theInfo);
//
//		params.push_back(std::move(xParam));
//	}
//
//	return CalcArea(params, theInfo);
//}

//#include <TModel_Surface.hxx>
//#include <Cad3d_TModel.hxx>
//#include <Marine_CmpSection.hxx>
//
//#include <BRepAlgoAPI_Section.hxx>
//#include <BRepBuilderAPI_MakeFace.hxx>
//#include <TopTools_ListOfShape.hxx>
//
//namespace tnbLib
//{
//
//	
//
//	
//
//
//}
//
//std::shared_ptr<tnbLib::Marine_CmpSection> 
//tnbLib::MarineBase_Tools::CreateSection
//(
//	const Cad3d_TModel & theModel,
//	const gp_Pln & thePlane,
//	const Standard_Boolean ParallelFalg
//)
//{
//	const auto& surfaces = theModel.RetrieveFaces();
//
//	std::map<Standard_Integer, std::shared_ptr<TModel_Surface>>
//		allSurfaces, meetSurface;
//	for (const auto& x : surfaces)
//	{
//		Debug_Null_Pointer(x);
//
//		auto insert = allSurfaces.insert(std::make_pair(x->Index(), x));
//		if (NOT insert.second)
//		{
//			FatalErrorIn("std::shared_ptr<tnbLib::Marine_CmpSection> MarineBase_Tools::CreateSection()")
//				<< "duplicate data: cannot insert the face into the map: " << x->Index() << endl
//				<< abort(FatalError);
//		}
//	}
//
//	auto builder = BRepBuilderAPI_MakeFace(thePlane);
//	auto plane = builder.Face();
//
//	for (const auto& x : surfaces)
//	{
//		Debug_Null_Pointer(x);
//
//		auto iter = meetSurface.find(x->Index());
//		if (iter IS_EQUAL meetSurface.end())
//		{
//			auto insert = meetSurface.insert(std::make_pair(x->Index(), x));
//			if (NOT insert.second)
//			{
//				FatalErrorIn("std::shared_ptr<tnbLib::Marine_CmpSection> MarineBase_Tools::CreateSection()")
//					<< "duplicate data: cannot insert the face into the map: " << x->Index() << endl
//					<< abort(FatalError);
//			}
//
//			auto intShape =
//				CrossSection(plane, x->Face(), ParallelFalg);
//
//			if (NOT intShape.IsNull())
//			{
//
//			}
//		}
//	}
//
//	
//}

