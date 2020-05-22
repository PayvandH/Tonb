#include <LimitedScheme.hxx>

#include <surfaceFields.hxx>
#include <fvcGrad.hxx>
#include <coupledFvPatchFields.hxx>
#include <GeometricFields.hxx>
#include <surfaceMesh.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	template<class Type, class Limiter, template<class> class LimitFunc>
	tmp<surfaceScalarField> LimitedScheme<Type, Limiter, LimitFunc>::limiter
	(
		const GeometricField<Type, fvPatchField, volMesh>& phi
	) const
	{
		const fvMesh& mesh = this->mesh();

		tmp<surfaceScalarField> tLimiter
		(
			new surfaceScalarField
			(
				IOobject
				(
					type() + "Limiter(" + phi.name() + ')',
					mesh.time().timeName(),
					mesh
				),
				mesh,
				dimless
			)
		);
		surfaceScalarField& lim = tLimiter();

		tmp<GeometricField<typename Limiter::phiType, fvPatchField, volMesh> >
			tlPhi = LimitFunc<Type>()(phi);

		const GeometricField<typename Limiter::phiType, fvPatchField, volMesh>&
			lPhi = tlPhi();

		// Due to gradient cacheing, must take a tmp field
		// HJ, 22/Apr/2016
		tmp
			<
			GeometricField<typename Limiter::gradPhiType, fvPatchField, volMesh>
			> tgradc = fvc::grad(lPhi);

		const GeometricField
			<
			typename Limiter::gradPhiType, fvPatchField, volMesh
			>& gradc = tgradc();

		// Note: in order for the patchNeighbourField to be correct on coupled
		// boundaries, correctBoundaryConditions needs to be called.
		// The call shall be moved into the library fvc operators
		// Moved to cached gradScheme.  HJ, 22/Apr/2016
	//     gradc.correctBoundaryConditions();

		const surfaceScalarField& CDweights = mesh.surfaceInterpolation::weights();

		const unallocLabelList& owner = mesh.owner();
		const unallocLabelList& neighbour = mesh.neighbour();

		const vectorField& C = mesh.C();

		scalarField& pLim = lim.internalField();

		forAll(pLim, face)
		{
			label own = owner[face];
			label nei = neighbour[face];

			pLim[face] = Limiter::limiter
			(
				CDweights[face],
				this->faceFlux_[face],
				lPhi[own],
				lPhi[nei],
				gradc[own],
				gradc[nei],
				C[nei] - C[own]
			);
		}

		surfaceScalarField::GeometricBoundaryField& bLim = lim.boundaryField();

		forAll(bLim, patchi)
		{
			scalarField& pLim = bLim[patchi];

			if (bLim[patchi].coupled())
			{
				const scalarField& pCDweights = CDweights.boundaryField()[patchi];
				const scalarField& pFaceFlux =
					this->faceFlux_.boundaryField()[patchi];
				Field<typename Limiter::phiType> plPhiP =
					lPhi.boundaryField()[patchi].patchInternalField();
				Field<typename Limiter::phiType> plPhiN =
					lPhi.boundaryField()[patchi].patchNeighbourField();
				Field<typename Limiter::gradPhiType> pGradcP =
					gradc.boundaryField()[patchi].patchInternalField();
				Field<typename Limiter::gradPhiType> pGradcN =
					gradc.boundaryField()[patchi].patchNeighbourField();

				// Build the d-vectors
				// Better version of d-vectors: Zeljko Tukovic, 25/Apr/2010
				vectorField pd = bLim[patchi].patch().delta();

				forAll(pLim, face)
				{
					pLim[face] = Limiter::limiter
					(
						pCDweights[face],
						pFaceFlux[face],
						plPhiP[face],
						plPhiN[face],
						pGradcP[face],
						pGradcN[face],
						pd[face]
					);
				}
			}
			else
			{
				pLim = 1.0;
			}
		}

		return tLimiter;
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //