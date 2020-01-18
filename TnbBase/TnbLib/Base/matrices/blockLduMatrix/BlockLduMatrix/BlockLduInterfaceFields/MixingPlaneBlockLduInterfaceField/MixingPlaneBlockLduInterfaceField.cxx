#include <MixingPlaneBlockLduInterfaceField.hxx>

#include <diagTensorField.hxx>
#include <transformField.hxx>

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class Type>
tnbLib::MixingPlaneBlockLduInterfaceField<Type>::
~MixingPlaneBlockLduInterfaceField()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void tnbLib::MixingPlaneBlockLduInterfaceField<Type>::transformCoupleField
(
	scalarField& f,
	const direction cmpt
) const
{
	// KRJ: 2013-02-08: Transform not tested
	if (doTransform())
	{
		if (forwardT().size() == 1)
		{
			f *= pow(diag(forwardT()[0]).component(cmpt), rank());
		}
		else
		{
			f *= pow(diag(forwardT())().component(cmpt), rank());
		}
	}
}


template<class Type>
void tnbLib::MixingPlaneBlockLduInterfaceField<Type>::transformCoupleField
(
	Field<Type>& f
) const
{
	// KRJ: 2013-02-08: Transform not tested
	if (doTransform())
	{
		if (forwardT().size() == 1)
		{
			transform(f, forwardT()[0], f);
		}
		else
		{
			transform(f, forwardT(), f);
		}
	}
}


// ************************************************************************* //