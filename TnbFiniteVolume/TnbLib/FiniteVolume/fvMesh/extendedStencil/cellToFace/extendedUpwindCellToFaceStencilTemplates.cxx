#include <extendedUpwindCellToFaceStencil.hxx>

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
tnbLib::tmp<tnbLib::GeometricField<Type, tnbLib::fvsPatchField, tnbLib::surfaceMesh> >
tnbLib::extendedUpwindCellToFaceStencil::weightedSum
(
	const surfaceScalarField& phi,
	const GeometricField<Type, fvPatchField, volMesh>& fld,
	const List<List<scalar> >& ownWeights,
	const List<List<scalar> >& neiWeights
) const
{
	const fvMesh& mesh = fld.mesh();

	// Collect internal and boundary values
	List<List<Type> > ownFld;
	collectData(ownMap(), ownStencil(), fld, ownFld);
	List<List<Type> > neiFld;
	collectData(neiMap(), neiStencil(), fld, neiFld);

	tmp<GeometricField<Type, fvsPatchField, surfaceMesh> > tsfCorr
	(
		new GeometricField<Type, fvsPatchField, surfaceMesh>
		(
			IOobject
			(
				fld.name(),
				mesh.time().timeName(),
				mesh,
				IOobject::NO_READ,
				IOobject::NO_WRITE,
				false
			),
			mesh,
			dimensioned<Type>
			(
				fld.name(),
				fld.dimensions(),
				pTraits<Type>::zero
				)
			)
	);
	GeometricField<Type, fvsPatchField, surfaceMesh>& sf = tsfCorr();

	// Internal faces
	for (label faceI = 0; faceI < mesh.nInternalFaces(); faceI++)
	{
		if (phi[faceI] > 0)
		{
			// Flux out of owner. Use upwind (= owner side) stencil.
			const List<Type>& stField = ownFld[faceI];
			const List<scalar>& stWeight = ownWeights[faceI];

			forAll(stField, i)
			{
				sf[faceI] += stField[i] * stWeight[i];
			}
		}
		else
		{
			const List<Type>& stField = neiFld[faceI];
			const List<scalar>& stWeight = neiWeights[faceI];

			forAll(stField, i)
			{
				sf[faceI] += stField[i] * stWeight[i];
			}
		}
	}

	// Boundaries. Either constrained or calculated so assign value
	// directly (instead of nicely using operator==)
	typename GeometricField<Type, fvsPatchField, surfaceMesh>::
		GeometricBoundaryField& bSfCorr = sf.boundaryField();

	forAll(bSfCorr, patchi)
	{
		fvsPatchField<Type>& pSfCorr = bSfCorr[patchi];

		if (pSfCorr.coupled())
		{
			label faceI = pSfCorr.patch().patch().start();

			forAll(pSfCorr, i)
			{
				if (phi.boundaryField()[patchi][i] > 0)
				{
					// Flux out of owner. Use upwind (= owner side) stencil.
					const List<Type>& stField = ownFld[faceI];
					const List<scalar>& stWeight = ownWeights[faceI];

					forAll(stField, j)
					{
						pSfCorr[i] += stField[j] * stWeight[j];
					}
				}
				else
				{
					const List<Type>& stField = neiFld[faceI];
					const List<scalar>& stWeight = neiWeights[faceI];

					forAll(stField, j)
					{
						pSfCorr[i] += stField[j] * stWeight[j];
					}
				}
				faceI++;
			}
		}
	}

	return tsfCorr;
}


// ************************************************************************* //