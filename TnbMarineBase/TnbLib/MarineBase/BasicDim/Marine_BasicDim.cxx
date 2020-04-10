#include <Marine_BasicDim.hxx>

#include <Marine_Body.hxx>
#include <Marine_CmptLib.hxx>

tnbLib::Marine_BasicDim::Marine_BasicDim()
	: theLoa_(0)
	, theB_(0)
	, theD_(0)
{
}

tnbLib::Marine_BasicDim::Marine_BasicDim
(
	const std::shared_ptr<Marine_Body>& theBody
)
	: theLoa_(0)
	, theB_(0)
	, theD_(0)
	, theBody_(theBody)
{
}

void tnbLib::Marine_BasicDim::Perform()
{
	const auto[loa, b, d] = Marine_CmptLib::CalcBasicDim(*Body());

	ChangeLOA() = std::move(loa);
	ChangeB() = std::move(b);
	ChangeD() = std::move(d);

	Change_IsDone() = Standard_True;
}