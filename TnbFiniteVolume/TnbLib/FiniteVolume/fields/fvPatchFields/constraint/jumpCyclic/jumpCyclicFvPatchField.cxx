#include <jumpCyclicFvPatchField.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	template<class Type>
	jumpCyclicFvPatchField<Type>::jumpCyclicFvPatchField
	(
		const fvPatch& p,
		const DimensionedField<Type, volMesh>& iF
	)
		:
		cyclicFvPatchField<Type>(p, iF)
	{}


	template<class Type>
	jumpCyclicFvPatchField<Type>::jumpCyclicFvPatchField
	(
		const jumpCyclicFvPatchField<Type>& ptf,
		const fvPatch& p,
		const DimensionedField<Type, volMesh>& iF,
		const fvPatchFieldMapper& mapper
	)
		:
		cyclicFvPatchField<Type>(ptf, p, iF, mapper)
	{}


	template<class Type>
	jumpCyclicFvPatchField<Type>::jumpCyclicFvPatchField
	(
		const fvPatch& p,
		const DimensionedField<Type, volMesh>& iF,
		const dictionary& dict
	)
		:
		cyclicFvPatchField<Type>(p, iF, dict)
	{
		// Call this evaluation in derived classes
		//this->evaluate(Pstream::blocking);
	}


	template<class Type>
	jumpCyclicFvPatchField<Type>::jumpCyclicFvPatchField
	(
		const jumpCyclicFvPatchField<Type>& ptf
	)
		:
		cyclicLduInterfaceField(),
		cyclicFvPatchField<Type>(ptf)
	{}


	template<class Type>
	jumpCyclicFvPatchField<Type>::jumpCyclicFvPatchField
	(
		const jumpCyclicFvPatchField<Type>& ptf,
		const DimensionedField<Type, volMesh>& iF
	)
		:
		cyclicFvPatchField<Type>(ptf, iF)
	{}


	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	template<class Type>
	tmp<Field<Type> > jumpCyclicFvPatchField<Type>::patchNeighbourField() const
	{
		const Field<Type>& iField = this->internalField();
		const unallocLabelList& faceCells = this->cyclicPatch().faceCells();

		tmp<Field<Type> > tpnf(new Field<Type>(this->size()));
		Field<Type>& pnf = tpnf();

		tmp<Field<Type> > tjf = jump();
		const Field<Type>& jf = tjf();

		label sizeby2 = this->size() / 2;

		if (this->doTransform())
		{
			for (label facei = 0; facei < sizeby2; facei++)
			{
				pnf[facei] = transform
				(
					this->forwardT()[0], iField[faceCells[facei + sizeby2]]
				) - jf[facei];

				pnf[facei + sizeby2] = transform
				(
					this->reverseT()[0], iField[faceCells[facei]] + jf[facei]
				);
			}
		}
		else
		{
			for (label facei = 0; facei < sizeby2; facei++)
			{
				pnf[facei] = iField[faceCells[facei + sizeby2]] - jf[facei];
				pnf[facei + sizeby2] = iField[faceCells[facei]] + jf[facei];
			}
		}

		return tpnf;
	}


	template<class Type>
	void jumpCyclicFvPatchField<Type>::updateInterfaceMatrix
	(
		const scalarField& psiInternal,
		scalarField& result,
		const lduMatrix&,
		const scalarField& coeffs,
		const direction cmpt,
		const Pstream::commsTypes,
		const bool switchToLhs
	) const
	{
		scalarField pnf(this->size());

		label sizeby2 = this->size() / 2;
		const unallocLabelList& faceCells = this->cyclicPatch().faceCells();

		// Add void pointer cast to keep compiler happy when instantiated
		// for vector/tensor fields.  HJ, 4/Jun/2013
		if
			(
				reinterpret_cast<const void*>(&psiInternal)
				== reinterpret_cast<const void*>(&this->internalField())
				)
		{
			// Get component of jump.  HJ, 11/Aug/2009
			const Field<scalar> jf = jump()().component(cmpt);

			for (label facei = 0; facei < sizeby2; facei++)
			{
				pnf[facei] = psiInternal[faceCells[facei + sizeby2]] - jf[facei];
				pnf[facei + sizeby2] = psiInternal[faceCells[facei]] + jf[facei];
			}
		}
		else
		{
			for (label facei = 0; facei < sizeby2; facei++)
			{
				pnf[facei] = psiInternal[faceCells[facei + sizeby2]];
				pnf[facei + sizeby2] = psiInternal[faceCells[facei]];
			}
		}

		// Transform according to the transformation tensors
		this->transformCoupleField(pnf, cmpt);

		// Multiply the field by coefficients and add into the result
		if (switchToLhs)
		{
			forAll(faceCells, elemI)
			{
				result[faceCells[elemI]] += coeffs[elemI] * pnf[elemI];
			}
		}
		else
		{
			forAll(faceCells, elemI)
			{
				result[faceCells[elemI]] -= coeffs[elemI] * pnf[elemI];
			}
		}
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //