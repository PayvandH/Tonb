#include <leastSquaresVectors.hxx>

#include <surfaceFields.hxx>
#include <volFields.hxx>
#include <mapPolyMesh.hxx>
#include <demandDrivenData.hxx>
#include <zeroGradientFvPatchFields.hxx>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
	defineTypeNameAndDebug(leastSquaresVectors, 0);
}


const tnbLib::debug::tolerancesSwitch
tnbLib::leastSquaresVectors::smallDotProdTol_
(
	"leastSquaresSmallDotProdTol",
	0.1
);


// * * * * * * * * * * * * * * * * Constructors * * * * * * * * * * * * * * //

tnbLib::leastSquaresVectors::leastSquaresVectors(const fvMesh& mesh)
	:
	MeshObject<fvMesh, leastSquaresVectors>(mesh),
	pVectorsPtr_(NULL),
	nVectorsPtr_(NULL)
{}


// * * * * * * * * * * * * * * * * Destructor * * * * * * * * * * * * * * * //

tnbLib::leastSquaresVectors::~leastSquaresVectors()
{
	deleteDemandDrivenData(pVectorsPtr_);
	deleteDemandDrivenData(nVectorsPtr_);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void tnbLib::leastSquaresVectors::makeLeastSquaresVectors() const
{
	if (debug)
	{
		Info << "leastSquaresVectors::makeLeastSquaresVectors() :"
			<< "Constructing least square gradient vectors"
			<< endl;
	}

	pVectorsPtr_ = new surfaceVectorField
	(
		IOobject
		(
			"LeastSquaresP",
			mesh().pointsInstance(),
			mesh(),
			IOobject::NO_READ,
			IOobject::NO_WRITE,
			false
		),
		mesh(),
		dimensionedVector("zero", dimless / dimLength, vector::zero)
	);
	surfaceVectorField& lsP = *pVectorsPtr_;

	nVectorsPtr_ = new surfaceVectorField
	(
		IOobject
		(
			"LeastSquaresN",
			mesh().pointsInstance(),
			mesh(),
			IOobject::NO_READ,
			IOobject::NO_WRITE,
			false
		),
		mesh(),
		dimensionedVector("zero", dimless / dimLength, vector::zero)
	);
	surfaceVectorField& lsN = *nVectorsPtr_;

	// Set local references to mesh data
	const unallocLabelList& owner = mesh().owner();
	const unallocLabelList& neighbour = mesh().neighbour();
	const volVectorField& C = mesh().C();

	// Set up temporary storage for the dd tensor (before inversion)
	symmTensorField dd(mesh().nCells(), symmTensor::zero);

	forAll(owner, faceI)
	{
		label own = owner[faceI];
		label nei = neighbour[faceI];

		vector d = C[nei] - C[own];
		symmTensor wdd = (1.0 / magSqr(d))*sqr(d);

		dd[own] += wdd;
		dd[nei] += wdd;
	}

	forAll(lsP.boundaryField(), patchI)
	{
		const fvPatch& p = mesh().boundary()[patchI];
		const unallocLabelList& fc = p.patch().faceCells();

		// Better version of d-vectors: Zeljko Tukovic, 25/Apr/2010
		const vectorField pd = p.delta();

		if (p.coupled())
		{
			forAll(pd, pFaceI)
			{
				const vector& d = pd[pFaceI];
				dd[fc[pFaceI]] += (1.0 / magSqr(d))*sqr(d);
			}
		}
		else
		{
			forAll(pd, pFaceI)
			{
				const vector& d = pd[pFaceI];
				dd[fc[pFaceI]] += (1.0 / magSqr(d))*sqr(d);
			}
		}
	}

	// For easy access of neighbour coupled patch field needed for
	// lsN vectors on implicitly coupled boundaries. VV, 18/June/2014
	volSymmTensorField volInvDd
	(
		IOobject
		(
			"volInvDd",
			mesh().pointsInstance(),
			mesh(),
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		mesh(),
		dimensionedSymmTensor("zero", dimless, symmTensor::zero),
		zeroGradientFvPatchScalarField::typeName
	);
	symmTensorField& invDd = volInvDd.internalField();

	// Invert least squares matrix using Householder transformations to avoid
	// badly posed cells
//    invDd = inv(dd);
	invDd = hinv(dd);

	// Evaluate coupled to exchange coupled neighbour field data
	// across coupled boundaries.  HJ, 18/Mar/2015
	volInvDd.boundaryField().evaluateCoupled();

	// Revisit all faces and calculate the lsP and lsN vectors
	vectorField& lsPIn = lsP.internalField();
	vectorField& lsNIn = lsN.internalField();

	// Least squares vectors on internal faces
	forAll(owner, faceI)
	{
		label own = owner[faceI];
		label nei = neighbour[faceI];

		vector d = C[nei] - C[own];
		scalar magSfByMagSqrd = 1.0 / magSqr(d);

		lsPIn[faceI] = magSfByMagSqrd * (invDd[own] & d);
		lsNIn[faceI] = -magSfByMagSqrd * (invDd[nei] & d);
	}

	// Least squares vectors on boundary faces
	forAll(lsP.boundaryField(), patchI)
	{
		fvsPatchVectorField& patchLsP = lsP.boundaryField()[patchI];
		fvsPatchVectorField& patchLsN = lsN.boundaryField()[patchI];
		const fvPatch& p = mesh().boundary()[patchI];
		const unallocLabelList& fc = p.faceCells();

		// Better version of d-vectors: Zeljko Tukovic, 25/Apr/2010
		const vectorField pd = p.delta();

		if (p.coupled())
		{
			const symmTensorField invDdNei =
				volInvDd.boundaryField()[patchI].patchNeighbourField();

			forAll(pd, pFaceI)
			{
				const vector& d = pd[pFaceI];

				patchLsP[pFaceI] = (1.0 / magSqr(d))*(invDd[fc[pFaceI]] & d);

				patchLsN[pFaceI] = -(1.0 / magSqr(d))*(invDdNei[pFaceI] & d);
			}
		}
		else
		{
			forAll(pd, pFaceI)
			{
				const vector& d = pd[pFaceI];

				patchLsP[pFaceI] = (1.0 / magSqr(d))*(invDd[fc[pFaceI]] & d);
			}
		}
	}

	// Coefficient sign correction on least squares vectors
	// The sign of the coefficient must be positive to ensure correct
	// behaviour in coupled systems.  This is checked by evaluating
	// dot-product of the P/N vectors with the face area vector.
	// If the dot-product is negative, cell is marked for use with the
	// Gauss gradient, which is unconditionally positive
	// HJ, 21/Apr/2015

	// First loop: detect cells with bad least squares vectors

	// Use Gauss Gradient field: set to 1 if Gauss is needed
	volScalarField useGaussGrad
	(
		IOobject
		(
			"useGaussGrad",
			mesh().pointsInstance(),
			mesh(),
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		mesh(),
		dimensionedScalar("zero", dimVolume, 0),
		zeroGradientFvPatchScalarField::typeName
	);

	const surfaceVectorField& Sf = mesh().Sf();
	const surfaceScalarField& w = mesh().weights();

	const vectorField& SfIn = Sf.internalField();
	const scalarField& wIn = w.internalField();
	scalarField& uggIn = useGaussGrad.internalField();

	// Check internal faces
	forAll(owner, faceI)
	{
		if
			(
			(lsPIn[faceI] & SfIn[faceI]) / (mag(lsPIn[faceI])*mag(SfIn[faceI]))
				< smallDotProdTol_()
				)
		{
			// Least square points in the wrong direction for owner
			// Use Gauss gradient
			uggIn[owner[faceI]] = 1;
		}

		if
			(
			(lsNIn[faceI] & SfIn[faceI]) / (mag(lsNIn[faceI])*mag(SfIn[faceI]))
				> -smallDotProdTol_()
				)
		{
			// Least square points in the wrong direction for owner.
			// Note that Sf points into neighbour cell
			// Use Gauss gradient
			uggIn[neighbour[faceI]] = 1;
		}
	}

	forAll(lsP.boundaryField(), patchI)
	{
		fvsPatchVectorField& patchLsP = lsP.boundaryField()[patchI];
		const vectorField& pSf = Sf.boundaryField()[patchI];
		const fvPatch& p = mesh().boundary()[patchI];
		const unallocLabelList& fc = p.faceCells();

		// Same check for coupled and uncoupled
		forAll(patchLsP, pFaceI)
		{
			if
				(
				(patchLsP[pFaceI] & pSf[pFaceI]) /
					(mag(patchLsP[pFaceI])*mag(pSf[pFaceI]))
					< smallDotProdTol_
					)
			{
				uggIn[fc[pFaceI]] = 1;
			}
		}
	}

	// Update boundary conditions for coupled boundaries.  This synchronises
	// the Gauss grad indication field
	useGaussGrad.boundaryField().evaluateCoupled();

	// Replace least square vectors with weighted Gauss gradient vectors
	// for marked cells

	// Prepare cell volumes with parallel communications
	volScalarField cellV
	(
		IOobject
		(
			"cellV",
			mesh().pointsInstance(),
			mesh(),
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		mesh(),
		dimensionedScalar("zero", dimVolume, 0),
		zeroGradientFvPatchScalarField::typeName
	);
	cellV.internalField() = mesh().V();

	// Evaluate coupled to exchange coupled neighbour field data
	// across coupled boundaries.  HJ, 18/Mar/2015
	cellV.boundaryField().evaluateCoupled();

	const scalarField& cellVIn = cellV.internalField();

	// Internal faces
	forAll(owner, faceI)
	{
		label own = owner[faceI];
		label nei = neighbour[faceI];

		if (uggIn[own] > SMALL)
		{
			// Gauss gradient for owner cell
			lsPIn[faceI] = (1 - wIn[faceI])*SfIn[faceI] / cellVIn[own];
		}

		if (uggIn[nei] > SMALL)
		{
			// Gauss gradient for neighbour cell
			lsNIn[faceI] = -wIn[faceI] * SfIn[faceI] / cellVIn[nei];
		}
	}

	// Boundary faces
	forAll(lsP.boundaryField(), patchI)
	{
		fvsPatchVectorField& patchLsP = lsP.boundaryField()[patchI];
		fvsPatchVectorField& patchLsN = lsN.boundaryField()[patchI];
		const fvPatch& p = mesh().boundary()[patchI];
		const unallocLabelList& fc = p.faceCells();

		const fvsPatchScalarField& pw = w.boundaryField()[patchI];
		const vectorField& pSf = Sf.boundaryField()[patchI];

		// Get indicator for local side
		const fvPatchScalarField& ugg = useGaussGrad.boundaryField()[patchI];
		const scalarField pUgg = ugg.patchInternalField();

		if (p.coupled())
		{
			const scalarField cellVInNei =
				cellV.boundaryField()[patchI].patchNeighbourField();

			// Get indicator for neighbour side
			const scalarField nUgg = ugg.patchNeighbourField();

			forAll(pUgg, pFaceI)
			{
				if (pUgg[pFaceI] > SMALL)
				{
					// Gauss grad for owner cell
					patchLsP[pFaceI] =
						(1 - pw[pFaceI])*pSf[pFaceI] / cellVIn[fc[pFaceI]];
				}

				if (nUgg[pFaceI] > SMALL)
				{
					// Gauss gradient for neighbour cell
					patchLsN[pFaceI] =
						-pw[pFaceI] * pSf[pFaceI] / cellVInNei[pFaceI];
				}
			}
		}
		else
		{
			forAll(pUgg, pFaceI)
			{
				if (pUgg[pFaceI] > SMALL)
				{
					// Gauss grad for owner cell
					patchLsP[pFaceI] =
						(1 - pw[pFaceI])*pSf[pFaceI] / cellVIn[fc[pFaceI]];
				}
			}
		}
	}

	// Manual check of least squares vectors

	vectorField sumLsq(mesh().nCells(), vector::zero);
	vectorField sumSf(mesh().nCells(), vector::zero);

	const vectorField& sfIn = mesh().Sf().internalField();

	// Least squares vectors on internal faces
	forAll(owner, faceI)
	{
		sumLsq[owner[faceI]] += lsPIn[faceI];
		sumLsq[neighbour[faceI]] += lsNIn[faceI];

		sumSf[owner[faceI]] += sfIn[faceI];
		sumSf[neighbour[faceI]] -= sfIn[faceI];
	}

	// Least squares vectors on boundary faces
	forAll(lsP.boundaryField(), patchI)
	{
		const vectorField& patchLsP = lsP.boundaryField()[patchI];
		const vectorField& patchSf = mesh().Sf().boundaryField()[patchI];

		const unallocLabelList& fc = mesh().boundary()[patchI].faceCells();

		forAll(fc, pFaceI)
		{
			//sumLsq[fc[pFaceI]] += 0.5*patchLsP[pFaceI]; // works!!!
			sumLsq[fc[pFaceI]] += patchLsP[pFaceI];

			sumSf[fc[pFaceI]] += patchSf[pFaceI];
		}
	}

	if (debug)
	{
		Info << "leastSquaresVectors::makeLeastSquaresVectors() :"
			<< "Finished constructing least square gradient vectors"
			<< endl;
	}
}


const tnbLib::surfaceVectorField& tnbLib::leastSquaresVectors::pVectors() const
{
	if (!pVectorsPtr_)
	{
		makeLeastSquaresVectors();
	}

	return *pVectorsPtr_;
}


const tnbLib::surfaceVectorField& tnbLib::leastSquaresVectors::nVectors() const
{
	if (!nVectorsPtr_)
	{
		makeLeastSquaresVectors();
	}

	return *nVectorsPtr_;
}


bool tnbLib::leastSquaresVectors::movePoints() const
{
	if (debug)
	{
		InfoIn("bool leastSquaresVectors::movePoints() const")
			<< "Clearing least square data" << endl;
	}

	deleteDemandDrivenData(pVectorsPtr_);
	deleteDemandDrivenData(nVectorsPtr_);

	return true;
}


bool tnbLib::leastSquaresVectors::updateMesh(const mapPolyMesh& mpm) const
{
	if (debug)
	{
		InfoIn
		(
			"bool leastSquaresVectors::updateMesh(const mapPolyMesh&) const"
		) << "Clearing least square data" << endl;
	}

	deleteDemandDrivenData(pVectorsPtr_);
	deleteDemandDrivenData(nVectorsPtr_);

	return true;
}

// ************************************************************************* //
