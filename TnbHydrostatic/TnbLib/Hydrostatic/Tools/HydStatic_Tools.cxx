#include <HydStatic_Tools.hxx>

#include <Merge2d_Pnt.hxx>
#include <Cad2d_CmptLib.hxx>
#include <MarineBase_Tools.hxx>
#include <HydStatic_CmptLib.hxx>
#include <HydStatic_GzQ.hxx>
#include <HydStatic_GzQP.hxx>
#include <HydStatic_rAuCurve.hxx>
#include <HydStatic_rArmCurve.hxx>
#include <HydStatic_hArmCurve.hxx>
#include <HydStatic_hAuCurve.hxx>
#include <HydStatic_rArmCurve_Body.hxx>
#include <HydStatic_rArmCurve_FSLq.hxx>
#include <HydStatic_rArmCurve_LDT.hxx>
#include <HydStatic_rArmCurve_LDV.hxx>
#include <HydStatic_rArmCurve_Eff.hxx>
#include <HydStatic_ArmCurveCreator.hxx>
#include <NumAlg_AdaptiveInteg_Info.hxx>
#include <TnbError.hxx>
#include <OSstream.hxx>

#include <Geo_Tools.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <StdFail_NotDone.hxx>

std::shared_ptr<tnbLib::HydStatic_hArmCurve> 
tnbLib::HydStatic_Tools::HeelingCurve
(
	const std::vector<HydStatic_GzQP>& theQs
)
{
	const auto offsets = OffsetsFrom(theQs);
	
	auto curve = MarineBase_Tools::Curve(offsets);
	Debug_Null_Pointer(curve);

	auto heeling = std::make_shared<HydStatic_hArmCurve>(std::move(curve));
	Debug_Null_Pointer(heeling);

	heeling->ChangeQs() = theQs;

	HydStatic_CmptLib::CalcParameters(heeling);

	return std::move(heeling);
}

std::shared_ptr<tnbLib::HydStatic_hArmCurve> 
tnbLib::HydStatic_Tools::HeelingCurve
(
	const std::vector<HydStatic_GzQP>&& theQs
)
{
	auto Qs = std::move(theQs);
	const auto offsets = OffsetsFrom(Qs);

	auto curve = MarineBase_Tools::Curve(offsets);
	Debug_Null_Pointer(curve);

	auto heeling = std::make_shared<HydStatic_hArmCurve>(std::move(curve));
	Debug_Null_Pointer(heeling);

	heeling->SetQs(std::move(Qs));

	HydStatic_CmptLib::CalcParameters(heeling);

	return std::move(heeling);
}

std::vector<tnbLib::HydStatic_GzQ> 
tnbLib::HydStatic_Tools::MirroredByO
(
	const std::vector<HydStatic_GzQ>& theQs
)
{
	const auto O = Pnt2d(0, 0);

	std::vector<HydStatic_GzQ> Qs;
	Qs.reserve(theQs.size());
	for (const auto& x : theQs)
	{
		auto mirroed = x.Mirrored(O);
		Qs.push_back(*mirroed);
	}
	return std::move(Qs);
}

std::vector<tnbLib::HydStatic_GzQP>
tnbLib::HydStatic_Tools::MirroredByO
(
	const std::vector<HydStatic_GzQP>& theQs
)
{
	const auto O = Pnt2d(0, 0);

	std::vector<HydStatic_GzQP> Qs;
	Qs.reserve(theQs.size());
	for (const auto& x : theQs)
	{
		auto mirroed = std::dynamic_pointer_cast<HydStatic_GzQP>(x.Mirrored(O));
		Debug_Null_Pointer(mirroed);
		Qs.push_back(*mirroed);
	}
	return std::move(Qs);
}

std::vector<tnbLib::HydStatic_GzQ> 
tnbLib::HydStatic_Tools::MirroredByOy
(
	const std::vector<HydStatic_GzQ>& theQs
)
{
	const auto ax = gp_Ax2d(gp_Pnt2d(0, 0), gp_Dir2d(0, 1));

	std::vector<HydStatic_GzQ> Qs;
	Qs.reserve(theQs.size());
	for (const auto& x : theQs)
	{
		auto mirroed = x.Mirrored(ax);
		Qs.push_back(*mirroed);
	}
	return std::move(Qs);
}

std::vector<tnbLib::HydStatic_GzQP>
tnbLib::HydStatic_Tools::MirroredByOy
(
	const std::vector<HydStatic_GzQP>& theQs
)
{
	const auto ax = gp_Ax2d(gp_Pnt2d(0, 0), gp_Dir2d(0, 1));

	std::vector<HydStatic_GzQP> Qs;
	Qs.reserve(theQs.size());
	for (const auto& x : theQs)
	{
		auto mirroed = std::dynamic_pointer_cast<HydStatic_GzQP>(x.Mirrored(ax));
		Debug_Null_Pointer(mirroed);
		Qs.push_back(*mirroed);
	}
	return std::move(Qs);
}

std::vector<tnbLib::HydStatic_GzQ> 
tnbLib::HydStatic_Tools::Merged
(
	const std::vector<HydStatic_GzQ>& theQs
)
{
	std::vector<Pnt2d> pts;
	pts.reserve(theQs.size());
	for (const auto& x : theQs)
	{
		auto pt = Pnt2d(x.Heel(), x.LeverArm());
		pts.push_back(std::move(pt));
	}

	Merge2d_Pnt alg(pts, 1.0E-3, 1.0E-5);
	alg.Perform();

	Debug_If_Condition_Message(NOT alg.IsDone(), "the merging algorithm is not performed!");

	const auto merged = alg.CompactPoints();
	std::vector<HydStatic_GzQ> Qs;
	for (const auto& x : merged)
	{
		auto q = HydStatic_GzQ(x.X(), x.Y());
		Qs.push_back(std::move(q));
	}
	return std::move(Qs);
}

std::vector<tnbLib::HydStatic_GzQP>
tnbLib::HydStatic_Tools::Merged
(
	const std::vector<HydStatic_GzQP>& theQs
)
{
	std::vector<Pnt2d> pts;
	pts.reserve(theQs.size());
	for (const auto& x : theQs)
	{
		auto pt = Pnt2d(x.Heel(), x.LeverArm());
		pts.push_back(std::move(pt));
	}

	Merge2d_Pnt alg(pts, 1.0E-3, 1.0E-5);
	alg.Perform();

	Debug_If_Condition_Message(NOT alg.IsDone(), "the merging algorithm is not performed!");

	const auto merged = alg.CompactPoints();
	std::vector<HydStatic_GzQP> Qs;
	for (const auto& x : merged)
	{
		auto q = HydStatic_GzQP(x.X(), x.Y(), 0);
		Qs.push_back(std::move(q));
	}
	return std::move(Qs);
}

std::shared_ptr<tnbLib::hydStcLib::rArmCurve_Body>
tnbLib::HydStatic_Tools::ExpandToPort
(
	const std::shared_ptr<hydStcLib::rArmCurve_Body>& theGZ
)
{
	if (NOT hydStcLib::IsStbHeel(theGZ))
	{
		FatalErrorIn("std::shared_ptr<hydStcLib::rArmCurve_Body> ExpandToPort(const std::shared_ptr<hydStcLib::rArmCurve_Body>& theGZ)")
			<< "for expanding a righting arm to the port, it must be STB!" << endl
			<< abort(FatalError);
	}

	const auto& stQs = theGZ->Qs();
	const auto prtQs = MirroredByO(stQs);

	std::vector<HydStatic_GzQP> Qs;
	Qs.reserve(stQs.size() + prtQs.size());
	for (const auto& x : stQs)
		Qs.push_back(x);
	for (const auto& x : prtQs)
		Qs.push_back(x);

	const auto mQs = Merged(Qs);

	const auto offsets = OffsetsFrom(Qs);

	const auto curve = MarineBase_Tools::Curve(offsets);

	auto rArm = std::make_shared<hydStcLib::ArmCurveCreator_SymmHeel<hydStcLib::rArmCurve_Body>>(curve);
	Debug_Null_Pointer(rArm);

	HydStatic_CmptLib::CalcParameters(rArm);

	rArm->SetQs(std::move(mQs));
	return std::move(rArm);
}

std::shared_ptr<tnbLib::hydStcLib::rArmCurve_FSLq>
tnbLib::HydStatic_Tools::ExpandToPort
(
	const std::shared_ptr<hydStcLib::rArmCurve_FSLq>& theGZ
)
{
	if (NOT hydStcLib::IsStbHeel(theGZ))
	{
		FatalErrorIn("std::shared_ptr<hydStcLib::rArmCurve_FSLq> ExpandToPort(const std::shared_ptr<hydStcLib::rArmCurve_FSLq>& theGz)")
			<< "for expanding a righting arm to the port, it must be STB!" << endl
			<< abort(FatalError);
	}

	const auto& stQs = theGZ->Qs();
	const auto prtQs = MirroredByO(stQs);

	std::vector<HydStatic_GzQP> Qs;
	Qs.reserve(stQs.size() + prtQs.size());
	for (const auto& x : stQs)
		Qs.push_back(x);
	for (const auto& x : prtQs)
		Qs.push_back(x);

	const auto mQs = Merged(Qs);

	const auto offsets = OffsetsFrom(Qs);

	const auto curve = MarineBase_Tools::Curve(offsets);

	auto rArm = std::make_shared<hydStcLib::ArmCurveCreator_SymmHeel<hydStcLib::rArmCurve_FSLq>>(curve);
	Debug_Null_Pointer(rArm);

	HydStatic_CmptLib::CalcParameters(rArm);

	rArm->SetQs(std::move(mQs));
	return std::move(rArm);
}

//std::shared_ptr<tnbLib::hydStcLib::rArmCurve_Hlds>
//tnbLib::HydStatic_Tools::ExpandToPort
//(
//	const std::shared_ptr<hydStcLib::rArmCurve_Hlds>& theGZ
//)
//{
//	if (NOT hydStcLib::IsStbHeel(theGZ))
//	{
//		FatalErrorIn("std::shared_ptr<hydStcLib::rArmCurve_Hlds> ExpandToPort(const std::shared_ptr<hydStcLib::rArmCurve_Hlds>& theGz)")
//			<< "for expanding a righting arm to the port, it must be STB!" << endl
//			<< abort(FatalError);
//	}
//
//	const auto& stQs = theGZ->Qs();
//	const auto prtQs = MirroredByO(stQs);
//
//	std::vector<HydStatic_GzQP> Qs;
//	Qs.reserve(stQs.size() + prtQs.size());
//	for (const auto& x : stQs)
//		Qs.push_back(x);
//	for (const auto& x : prtQs)
//		Qs.push_back(x);
//
//	const auto mQs = Merged(Qs);
//
//	const auto offsets = OffsetsFrom(Qs);
//
//	const auto curve = MarineBase_Tools::Curve(offsets);
//
//	auto rArm = std::make_shared<hydStcLib::ArmCurveCreator_SymmHeel<hydStcLib::rArmCurve_Hlds>>(curve);
//	Debug_Null_Pointer(rArm);
//
//	HydStatic_CmptLib::CalcParameters(rArm);
//
//	rArm->SetQs(std::move(mQs));
//}

std::shared_ptr<tnbLib::hydStcLib::rArmCurve_LDT> 
tnbLib::HydStatic_Tools::ExpandToPort
(
	const std::shared_ptr<hydStcLib::rArmCurve_LDT>& theGZ
)
{
	if (NOT hydStcLib::IsStbHeel(theGZ))
	{
		FatalErrorIn("std::shared_ptr<hydStcLib::rArmCurve_LDT> ExpandToPort(const std::shared_ptr<hydStcLib::rArmCurve_LDT>& theGz)")
			<< "for expanding a righting arm to the port, it must be STB!" << endl
			<< abort(FatalError);
	}

	const auto& stQs = theGZ->Qs();
	const auto prtQs = MirroredByOy(stQs);

	std::vector<HydStatic_GzQP> Qs;
	Qs.reserve(stQs.size() + prtQs.size());
	for (const auto& x : stQs)
		Qs.push_back(x);
	for (const auto& x : prtQs)
		Qs.push_back(x);

	const auto mQs = Merged(Qs);

	const auto offsets = OffsetsFrom(Qs);

	const auto curve = MarineBase_Tools::Curve(offsets);

	auto rArm = std::make_shared<hydStcLib::ArmCurveCreator_SymmHeel<hydStcLib::rArmCurve_LDT>>(curve, theGZ->D());
	Debug_Null_Pointer(rArm);

	HydStatic_CmptLib::CalcParameters(rArm);

	rArm->SetQs(std::move(mQs));
	return std::move(rArm);
}

std::shared_ptr<tnbLib::hydStcLib::rArmCurve_LDV> 
tnbLib::HydStatic_Tools::ExpandToPort
(
	const std::shared_ptr<hydStcLib::rArmCurve_LDV>& theGZ
)
{
	if (NOT hydStcLib::IsStbHeel(theGZ))
	{
		FatalErrorIn("std::shared_ptr<hydStcLib::rArmCurve_LDV> ExpandToPort(const std::shared_ptr<hydStcLib::rArmCurve_LDV>& theGz)")
			<< "for expanding a righting arm to the port, it must be STB!" << endl
			<< abort(FatalError);
	}

	const auto& stQs = theGZ->Qs();
	const auto prtQs = MirroredByO(stQs);

	std::vector<HydStatic_GzQP> Qs;
	Qs.reserve(stQs.size() + prtQs.size());
	for (const auto& x : stQs)
		Qs.push_back(x);
	for (const auto& x : prtQs)
		Qs.push_back(x);

	const auto mQs = Merged(Qs);

	const auto offsets = OffsetsFrom(Qs);

	const auto curve = MarineBase_Tools::Curve(offsets);

	auto rArm = std::make_shared<hydStcLib::ArmCurveCreator_SymmHeel<hydStcLib::rArmCurve_LDV>>(curve, theGZ->D());
	Debug_Null_Pointer(rArm);

	HydStatic_CmptLib::CalcParameters(rArm);

	rArm->SetQs(std::move(mQs));
	return std::move(rArm);
}


//std::shared_ptr<tnbLib::HydStatic_rArmCurve> 
//tnbLib::HydStatic_Tools::ExpandRigthingArmToPort
//(
//	const HydStatic_rArmCurve & theCurve,
//	const marineLib::KG & theKG
//)
//{
//	Debug_Null_Pointer(theCurve.AuCurve());
//	const auto& AuC = *theCurve.AuCurve();
//
//	const auto& old_Qs = AuC.Qs();
//	Debug_If_Condition(old_Qs.empty());
//
//	if (old_Qs[0].Heel() < 0)
//	{
//		FatalErrorIn("std::shared_ptr<HydStatic_rArmCurve> HydStatic_Tools::ExpandRigthingArmToPort(Args...)")
//			<< "unable to expand the arm-curve: the curve must be starboard only" << endl
//			<< abort(FatalError);
//	}
//
//	const auto kg = theKG();
//
//	std::vector<HydStatic_GzQ> Qs;
//	Qs.reserve(old_Qs.size() * 2 - 1);
//	
//	auto iter = old_Qs.end();
//	iter--;
//	while (iter NOT_EQUAL old_Qs.begin())
//	{
//		const auto& x = *iter;
//
//		auto lk = x.LeverArm() + kg * sin(Geo_Tools::DegToRadian(x.Heel()));
//		HydStatic_GzQ q(-x.Heel())
//	}
//}

void tnbLib::HydStatic_Tools::AuCurve
(
	const std::shared_ptr<hydStcLib::rArmCurve_Eff>& theCurve,
	const Standard_Real y0,
	const std::shared_ptr<info>& theInfo
)
{
	Debug_Null_Pointer(theCurve);

	const auto Qs = HydStatic_CmptLib::CalcAuCurve(*theCurve, y0, theInfo);
	const auto rAu = HydStatic_rAuCurve::AuCurve(Qs);
	Debug_Null_Pointer(rAu);

	theCurve->SetAuCurve(std::move(rAu));
}

void tnbLib::HydStatic_Tools::AuCurve
(
	const std::shared_ptr<HydStatic_hArmCurve>& theCurve,
	const Standard_Real y0,
	const std::shared_ptr<info>& theInfo
)
{
	Debug_Null_Pointer(theCurve);

	const auto Qs = HydStatic_CmptLib::CalcAuCurve(*theCurve, y0, theInfo);
	const auto hAu = HydStatic_hAuCurve::AuCurve(Qs);
	Debug_Null_Pointer(hAu);

	theCurve->SetAuCurve(std::move(hAu));
}