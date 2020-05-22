#include <extendedLeastSquaresVectors.hxx>

#include <surfaceFields.hxx>
#include <volFields.hxx>
#include <mapPolyMesh.hxx>
#include <demandDrivenData.hxx>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
	defineTypeNameAndDebug(extendedLeastSquaresVectors, 0);
}


// * * * * * * * * * * * * * * * * Constructors * * * * * * * * * * * * * * //

tnbLib::extendedLeastSquaresVectors::extendedLeastSquaresVectors
(
	const fvMesh& mesh,
	const scalar minDet
)
	:
	MeshObject<fvMesh, extendedLeastSquaresVectors>(mesh),
	minDet_(minDet),
	pVectorsPtr_(NULL),
	nVectorsPtr_(NULL),
	additionalCellsPtr_(NULL),
	additionalVectorsPtr_(NULL)
{}


// * * * * * * * * * * * * * * * * Destructor * * * * * * * * * * * * * * * //

tnbLib::extendedLeastSquaresVectors::~extendedLeastSquaresVectors()
{
	deleteDemandDrivenData(pVectorsPtr_);
	deleteDemandDrivenData(nVectorsPtr_);

	deleteDemandDrivenData(additionalCellsPtr_);
	deleteDemandDrivenData(additionalVectorsPtr_);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void tnbLib::extendedLeastSquaresVectors::makeLeastSquaresVectors() const
{
	if (debug)
	{
		Info << "extendedLeastSquaresVectors::makeLeastSquaresVectors() :"
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
		// Build the d-vectors
		label own = owner[faceI];
		label nei = neighbour[faceI];

		vector d = C[nei] - C[own];

		symmTensor wdd = (1.0 / magSqr(d))*sqr(d);

		dd[owner[faceI]] += wdd;
		dd[neighbour[faceI]] += wdd;
	}

	// Visit the boundaries. Coupled boundaries are taken into account
	// in the construction of d vectors.
	forAll(lsP.boundaryField(), patchI)
	{
		const fvPatch& p = mesh().boundary()[patchI];
		const unallocLabelList& faceCells = p.faceCells();

		// Better version of d-vectors: Zeljko Tukovic, 25/Apr/2010
		vectorField pd = p.delta();

		forAll(pd, patchFaceI)
		{
			dd[faceCells[patchFaceI]] +=
				(1.0 / magSqr(pd[patchFaceI]))*sqr(pd[patchFaceI]);
		}
	}

	scalarField detdd = det(dd);

	label nAddCells = 0;
	label maxNaddCells = 4 * detdd.size();
	additionalCellsPtr_ = new List<labelPair>(maxNaddCells);
	List<labelPair>& additionalCells_ = *additionalCellsPtr_;

	forAll(detdd, i)
	{
		label count = 0;

		while (++count < 100 && detdd[i] < minDet_)
		{
			if (nAddCells == maxNaddCells)
			{
				FatalErrorIn
				(
					"extendedLeastSquaresVectors::"
					"makeLeastSquaresVectors() const"
				) << "nAddCells exceeds maxNaddCells"
					<< exit(FatalError);
			}

			labelList pointLabels = mesh().cells()[i].labels(mesh().faces());

			scalar maxDetddij = 0.0;

			label addCell = -1;

			forAll(pointLabels, j)
			{
				forAll(mesh().pointCells()[pointLabels[j]], k)
				{
					label cellj = mesh().pointCells()[pointLabels[j]][k];

					if (cellj != i)
					{
						if (cellj != -1)
						{
							vector dCij = (C[cellj] - C[i]);

							symmTensor ddij =
								dd[i] + (1.0 / magSqr(dCij))*sqr(dCij);

							scalar detddij = det(ddij);

							if (detddij > maxDetddij)
							{
								addCell = cellj;
								maxDetddij = detddij;
							}
						}
					}
				}
			}

			if (addCell != -1)
			{
				additionalCells_[nAddCells][0] = i;
				additionalCells_[nAddCells++][1] = addCell;
				vector dCij = C[addCell] - C[i];
				dd[i] += (1.0 / magSqr(dCij))*sqr(dCij);
				detdd[i] = det(dd[i]);
			}
		}
	}

	additionalCells_.setSize(nAddCells);

	// Invert the dd tensor
	symmTensorField invDd = inv(dd);


	// Revisit all faces and calculate the lsP and lsN vectors
	forAll(owner, faceI)
	{
		// Build the d-vectors
		label own = owner[faceI];
		label nei = neighbour[faceI];

		vector d = C[nei] - C[own];

		lsP[faceI] = (1.0 / magSqr(d))*(invDd[owner[faceI]] & d);

		lsN[faceI] = ((-1.0) / magSqr(d))*(invDd[neighbour[faceI]] & d);
	}

	forAll(lsP.boundaryField(), patchI)
	{
		vectorField pd = mesh().boundary()[patchI].delta();

		fvsPatchVectorField& patchLsP = lsP.boundaryField()[patchI];

		const fvPatch& p = patchLsP.patch();
		const unallocLabelList& faceCells = p.faceCells();

		forAll(p, patchFaceI)
		{
			patchLsP[patchFaceI] =
				(1.0 / magSqr(pd[patchFaceI]))
				*(invDd[faceCells[patchFaceI]] & pd[patchFaceI]);
		}
	}


	additionalVectorsPtr_ = new vectorField(additionalCells_.size());
	vectorField& additionalVectors_ = *additionalVectorsPtr_;

	forAll(additionalCells_, i)
	{
		vector dCij = C[additionalCells_[i][1]]
			- C[additionalCells_[i][0]];

		additionalVectors_[i] =
			(1.0 / magSqr(dCij))*(invDd[additionalCells_[i][0]] & dCij);
	}

	if (debug)
	{
		Info << "extendedLeastSquaresVectors::makeLeastSquaresVectors() :"
			<< "Finished constructing least square gradient vectors"
			<< endl;
	}
}


const tnbLib::surfaceVectorField&
tnbLib::extendedLeastSquaresVectors::pVectors() const
{
	if (!pVectorsPtr_)
	{
		makeLeastSquaresVectors();
	}

	return *pVectorsPtr_;
}


const tnbLib::surfaceVectorField&
tnbLib::extendedLeastSquaresVectors::nVectors() const
{
	if (!nVectorsPtr_)
	{
		makeLeastSquaresVectors();
	}

	return *nVectorsPtr_;
}


const tnbLib::List<tnbLib::labelPair>&
tnbLib::extendedLeastSquaresVectors::additionalCells() const
{
	if (!additionalCellsPtr_)
	{
		makeLeastSquaresVectors();
	}

	return *additionalCellsPtr_;
}


const tnbLib::vectorField&
tnbLib::extendedLeastSquaresVectors::additionalVectors() const
{
	if (!additionalVectorsPtr_)
	{
		makeLeastSquaresVectors();
	}

	return *additionalVectorsPtr_;
}


bool tnbLib::extendedLeastSquaresVectors::movePoints() const
{
	deleteDemandDrivenData(pVectorsPtr_);
	deleteDemandDrivenData(nVectorsPtr_);

	deleteDemandDrivenData(additionalCellsPtr_);
	deleteDemandDrivenData(additionalVectorsPtr_);

	return true;
}

bool tnbLib::extendedLeastSquaresVectors::updateMesh(const mapPolyMesh&) const
{
	deleteDemandDrivenData(pVectorsPtr_);
	deleteDemandDrivenData(nVectorsPtr_);

	deleteDemandDrivenData(additionalCellsPtr_);
	deleteDemandDrivenData(additionalVectorsPtr_);

	return true;
}

// ************************************************************************* //