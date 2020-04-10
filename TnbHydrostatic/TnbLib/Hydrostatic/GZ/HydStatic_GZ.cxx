#include <HydStatic_GZ.hxx>

#include <MarineBase_Tools.hxx>
#include <HydStatic_LeverArmPair.hxx>
#include <HydStatic_rArmCurve.hxx>
#include <HydStatic_CrossCurves.hxx>
#include <HydStatic_CmptLib.hxx>
#include <error.hxx>
#include <OSstream.hxx>

tnbLib::HydStatic_GZ::HydStatic_GZ()
	: theKg_(marineLib::KG::null)
	, theDispv_(marineLib::DISPV::null)
{
}

tnbLib::HydStatic_GZ::HydStatic_GZ
(
	const std::shared_ptr<HydStatic_CrossCurves>& theCrossCurves,
	const Standard_Real theKG,
	const Standard_Real theDispv
)
	: theCrossCurves_(theCrossCurves)
	, theKg_(theKG)
	, theDispv_(theDispv)
{
}

void tnbLib::HydStatic_GZ::Perform()
{
	if (NOT CrossCurves())
	{
		FatalErrorIn("void tnbLib::HydStatic_GZ::Perform()")
			<< "no crossCurves is loaded!" << endl
			<< abort(FatalError);
	}

	if (KG().IsNull())
	{
		FatalErrorIn("void tnbLib::HydStatic_GZ::Perform()")
			<< "no KG is specified!" << endl
			<< abort(FatalError);
	}

	if (DISPV().IsNull())
	{
		FatalErrorIn("void tnbLib::HydStatic_GZ::Perform()")
			<< "no DISPV is specified!" << endl
			<< abort(FatalError);
	}

	const auto& crossCurves = *CrossCurves();

	const auto leverArms = HydStatic_CmptLib::LeverArms(crossCurves.CrossCurves(), DISPV()());
	const auto gzQ = HydStatic_CmptLib::GZ(leverArms, KG()());

	const auto curve = MarineBase_Tools::Curve(gzQ);

	const auto rArm = std::make_shared<HydStatic_rArmCurve>(std::move(curve));

	ChangeRightingArm() = std::move(rArm);

	Change_IsDone() = Standard_True;
}

void tnbLib::HydStatic_GZ::SetKG
(
	const Standard_Real theKG
)
{
	ChangeKG()() = theKG;
}