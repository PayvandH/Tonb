#include <slipFvPatchField.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	template<class Type>
	slipFvPatchField<Type>::slipFvPatchField
	(
		const fvPatch& p,
		const DimensionedField<Type, volMesh>& iF
	)
		:
		basicSymmetryFvPatchField<Type>(p, iF)
	{}


	template<class Type>
	slipFvPatchField<Type>::slipFvPatchField
	(
		const slipFvPatchField<Type>& ptf,
		const fvPatch& p,
		const DimensionedField<Type, volMesh>& iF,
		const fvPatchFieldMapper& mapper
	)
		:
		basicSymmetryFvPatchField<Type>(ptf, p, iF, mapper)
	{}


	template<class Type>
	slipFvPatchField<Type>::slipFvPatchField
	(
		const fvPatch& p,
		const DimensionedField<Type, volMesh>& iF,
		const dictionary& dict
	)
		:
		basicSymmetryFvPatchField<Type>(p, iF, dict)
	{}


	template<class Type>
	slipFvPatchField<Type>::slipFvPatchField
	(
		const slipFvPatchField<Type>& ptf,
		const DimensionedField<Type, volMesh>& iF
	)
		:
		basicSymmetryFvPatchField<Type>(ptf, iF)
	{}


	template<class Type>
	slipFvPatchField<Type>::slipFvPatchField
	(
		const slipFvPatchField<Type>& ptf
	)
		:
		basicSymmetryFvPatchField<Type>(ptf)
	{}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //