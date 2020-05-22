#include <linearUpwind.hxx>

#include <fvMesh.hxx>
#include <zeroGradientFvPatchField.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
tnbLib::tmp<tnbLib::GeometricField<Type, tnbLib::fvsPatchField, tnbLib::surfaceMesh> >
tnbLib::linearUpwind<Type>::correction
(
	const GeometricField<Type, fvPatchField, volMesh>& vf
) const
{
	const fvMesh& mesh = this->mesh();

	tmp<GeometricField<Type, fvsPatchField, surfaceMesh> > tsfCorr
	(
		new GeometricField<Type, fvsPatchField, surfaceMesh>
		(
			IOobject
			(
				"linearUpwindCorrection(" + vf.name() + ')',
				mesh.time().timeName(),
				mesh,
				IOobject::NO_READ,
				IOobject::NO_WRITE,
				false
			),
			mesh,
			dimensioned<Type>(vf.name(), vf.dimensions(), pTraits<Type>::zero)
			)
	);

	GeometricField<Type, fvsPatchField, surfaceMesh>& sfCorr = tsfCorr();

	const surfaceScalarField& faceFlux = this->faceFlux_;

	const labelList& owner = mesh.owner();
	const labelList& neighbour = mesh.neighbour();

	const volVectorField& C = mesh.C();
	const surfaceVectorField& Cf = mesh.Cf();

	// Due to gradient cacheing, must take a tmp field
	// HJ, 22/Apr/2016
	tmp
		<
		GeometricField
		<
		typename outerProduct<vector, Type>::type,
		fvPatchField,
		volMesh
		>
		> tgradVf = gradScheme_().grad(vf, gradSchemeName_);

	const GeometricField
		<
		typename outerProduct<vector, Type>::type,
		fvPatchField,
		volMesh
		>& gradVf = tgradVf();

	// Note: in order for the patchNeighbourField to be correct on coupled
	// boundaries, correctBoundaryConditions needs to be called.
	// The call shall be moved into the library fvc operators
	// Moved to cached gradScheme.  HJ, 22/Apr/2016
//     gradVf.correctBoundaryConditions();

	forAll(faceFlux, facei)
	{
		if (faceFlux[facei] > 0)
		{
			label own = owner[facei];
			sfCorr[facei] = (Cf[facei] - C[own]) & gradVf[own];
		}
		else
		{
			label nei = neighbour[facei];
			sfCorr[facei] = (Cf[facei] - C[nei]) & gradVf[nei];
		}
	}


	typename GeometricField<Type, fvsPatchField, surfaceMesh>::
		GeometricBoundaryField& bSfCorr = sfCorr.boundaryField();

	forAll(bSfCorr, patchi)
	{
		fvsPatchField<Type>& pSfCorr = bSfCorr[patchi];

		if (pSfCorr.coupled())
		{
			const fvPatch& p = mesh.boundary()[patchi];

			const unallocLabelList& pOwner = p.faceCells();

			const vectorField& pCf = Cf.boundaryField()[patchi];

			const scalarField& pFaceFlux = faceFlux.boundaryField()[patchi];

			Field<typename outerProduct<vector, Type>::type> pGradVfNei =
				gradVf.boundaryField()[patchi].patchNeighbourField();

			// Build the d-vectors
			// Better version of d-vectors: Zeljko Tukovic, 25/Apr/2010
			vectorField pd = p.delta();

			forAll(pOwner, facei)
			{
				label own = pOwner[facei];

				if (pFaceFlux[facei] > 0)
				{
					pSfCorr[facei] = (pCf[facei] - C[own]) & gradVf[own];
				}
				else
				{
					pSfCorr[facei] =
						(pCf[facei] - pd[facei] - C[own]) & pGradVfNei[facei];
				}
			}
		}
	}

	return tsfCorr;
}


namespace tnbLib
{
	//makelimitedSurfaceInterpolationScheme(linearUpwind)
	makelimitedSurfaceInterpolationTypeScheme(linearUpwind, scalar)
		makelimitedSurfaceInterpolationTypeScheme(linearUpwind, vector)
}

// ************************************************************************* //