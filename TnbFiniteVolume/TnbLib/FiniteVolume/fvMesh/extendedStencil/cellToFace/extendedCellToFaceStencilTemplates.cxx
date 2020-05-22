#include <extendedCellToFaceStencil.hxx>

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void tnbLib::extendedCellToFaceStencil::collectData
(
	const mapDistribute& map,
	const labelListList& stencil,
	const GeometricField<Type, fvPatchField, volMesh>& fld,
	List<List<Type> >& stencilFld
)
{
	// 1. Construct cell data in compact addressing
	List<Type> compactFld(map.constructSize(), pTraits<Type>::zero);

	// Insert my internal values
	forAll(fld, cellI)
	{
		compactFld[cellI] = fld[cellI];
	}
	// Insert my boundary values
	label nCompact = fld.size();
	forAll(fld.boundaryField(), patchI)
	{
		const fvPatchField<Type>& pfld = fld.boundaryField()[patchI];

		forAll(pfld, i)
		{
			compactFld[nCompact++] = pfld[i];
		}
	}

	// Do all swapping
	map.distribute(compactFld);

	// 2. Pull to stencil
	stencilFld.setSize(stencil.size());

	forAll(stencil, faceI)
	{
		const labelList& compactCells = stencil[faceI];

		stencilFld[faceI].setSize(compactCells.size());

		forAll(compactCells, i)
		{
			stencilFld[faceI][i] = compactFld[compactCells[i]];
		}
	}
}


template<class Type>
tnbLib::tmp<tnbLib::GeometricField<Type, tnbLib::fvsPatchField, tnbLib::surfaceMesh> >
tnbLib::extendedCellToFaceStencil::weightedSum
(
	const mapDistribute& map,
	const labelListList& stencil,
	const GeometricField<Type, fvPatchField, volMesh>& fld,
	const List<List<scalar> >& stencilWeights
)
{
	const fvMesh& mesh = fld.mesh();

	// Collect internal and boundary values
	List<List<Type> > stencilFld;
	collectData(map, stencil, fld, stencilFld);

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
		const List<Type>& stField = stencilFld[faceI];
		const List<scalar>& stWeight = stencilWeights[faceI];

		forAll(stField, i)
		{
			sf[faceI] += stField[i] * stWeight[i];
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
				const List<Type>& stField = stencilFld[faceI];
				const List<scalar>& stWeight = stencilWeights[faceI];

				forAll(stField, j)
				{
					pSfCorr[i] += stField[j] * stWeight[j];
				}

				faceI++;
			}
		}
	}

	return tsfCorr;
}


// ************************************************************************* //