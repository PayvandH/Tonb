#include <fixedGradientFvPatchVectorNFields.hxx>

#include <fvPatchVectorNFieldsFwd.hxx>
#include <addToRunTimeSelectionTable.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

#define doMakePatchTypeField(type, Type, args)                             \
                                                                              \
makeTemplatePatchTypeField                                                    \
(                                                                             \
    fvPatch##Type##Field,                                                     \
    fixedGradientFvPatch##Type##Field                                         \
);

	forAllVectorNTypes(doMakePatchTypeField)

		forAllTensorNTypes(doMakePatchTypeField)

		forAllDiagTensorNTypes(doMakePatchTypeField)

		forAllSphericalTensorNTypes(doMakePatchTypeField)


#undef doMakePatchTypeField

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //