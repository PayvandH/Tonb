#include <SectPx_FieldFunTools.hxx>

#include <SectPx_FieldFuns.hxx>
#include <TnbError.hxx>
#include <OSstream.hxx>

Standard_Boolean 
tnbLib::SectPx_FieldFunTools::IsRemovable
(
	const std::shared_ptr<SectPx_FieldFun>& theField
)
{
	Debug_Null_Pointer(theField);
	return NOT theField->HasChildren();
}

Standard_Boolean 
tnbLib::SectPx_FieldFunTools::IsFieldFun
(
	const std::shared_ptr<SectPx_RegObj>& theObj
)
{
	Debug_Null_Pointer(theObj);
	return Standard_Boolean
	(std::dynamic_pointer_cast<SectPx_FieldFun>(theObj));
}

std::shared_ptr<tnbLib::SectPx_FieldFun> 
tnbLib::SectPx_FieldFunTools::FieldFun
(
	const std::shared_ptr<SectPx_RegObj>& theObj
)
{
	Debug_Null_Pointer(theObj);
	auto field = std::dynamic_pointer_cast<SectPx_FieldFun>(theObj);
	if (NOT field)
	{
		FatalErrorIn(FunctionSIG)
			<< " the object is not field" << endl
			<< " - object's type = " << theObj->RegObjTypeName() << endl
			<< abort(FatalError);
	}
	return std::move(field);
}