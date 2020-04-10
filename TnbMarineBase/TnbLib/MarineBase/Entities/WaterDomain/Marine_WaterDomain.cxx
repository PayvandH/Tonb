#include <Marine_WaterDomain.hxx>

#include <Marine_Section.hxx>

tnbLib::Marine_WaterDomain::Marine_WaterDomain
(
	const std::shared_ptr<Marine_Domain>& theDomain
)
	: theDomain_(theDomain)
{
}

tnbLib::Marine_WaterDomain::Marine_WaterDomain
(
	const Standard_Integer theIndex,
	const std::shared_ptr<Marine_Domain>& theDomain
)
	: Marine_Entity(theIndex)
	, theDomain_(theDomain)
{
}

tnbLib::Marine_WaterDomain::Marine_WaterDomain
(
	const Standard_Integer theIndex,
	const word & theName, 
	const std::shared_ptr<Marine_Domain>& theDomain
)
	: Marine_Entity(theIndex, theName)
	, theDomain_(theDomain)
{
}

void tnbLib::Marine_WaterDomain::ExportToPlt(OFstream & File) const
{
	for (const auto& x : Waters())
	{
		Debug_Null_Pointer(x);
		x->ExportToPlt(File);
	}
}