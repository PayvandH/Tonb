#include <fvPatchMapper.hxx>

#include <fvPatch.hxx>
#include <fvBoundaryMesh.hxx>
#include <fvMesh.hxx>
#include <mapPolyMesh.hxx>
#include <faceMapper.hxx>
#include <demandDrivenData.hxx>

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void tnbLib::fvPatchMapper::calcAddressing() const
{
	if
		(
			directAddrPtr_
			|| interpolationAddrPtr_
			|| weightsPtr_
			)
	{
		FatalErrorIn
		(
			"void fvPatchMapper::calcAddressing() const)"
		) << "Addressing already calculated"
			<< abort(FatalError);
	}

	// Mapping
	const label oldPatchStart =
		faceMap_.oldPatchStarts()[patch_.index()];

	const label oldPatchEnd =
		oldPatchStart + faceMap_.oldPatchSizes()[patch_.index()];

	// Assemble the maps: slice to patch
	if (direct())
	{
		// Direct mapping - slice to size
		directAddrPtr_ = new labelList
		(
			patch_.patchSlice
			(
				static_cast<const labelList&>(faceMap_.directAddressing())
			)
		);
		labelList& addr = *directAddrPtr_;

		// Adjust mapping to manage hits into other patches and into
		// internal
		forAll(addr, faceI)
		{
			if
				(
					addr[faceI] >= oldPatchStart
					&& addr[faceI] < oldPatchEnd
					)
			{
				addr[faceI] -= oldPatchStart;
			}
			else
			{
				addr[faceI] = 0;
			}
		}

		if (fvMesh::debug)
		{
			if (addr.size() > 0 && min(addr) < 0)
			{
				FatalErrorIn
				(
					"void fvPatchMapper::calcAddressing() const"
				) << "Error in patch mapping for patch "
					<< patch_.index() << " named " << patch_.name()
					<< abort(FatalError);
			}
		}
	}
	else
	{
		// Interpolative mapping
		interpolationAddrPtr_ =
			new labelListList
			(
				patch_.patchSlice(faceMap_.addressing())
			);
		labelListList& addr = *interpolationAddrPtr_;

		weightsPtr_ =
			new scalarListList
			(
				patch_.patchSlice(faceMap_.weights())
			);
		scalarListList& w = *weightsPtr_;

		// Adjust mapping to manage hits into other patches and into
		// internal
		forAll(addr, faceI)
		{
			labelList& curAddr = addr[faceI];
			scalarList& curW = w[faceI];

			if
				(
					min(curAddr) >= oldPatchStart
					&& max(curAddr) < oldPatchEnd
					)
			{
				// No adjustment of weights, just subtract patch start
				forAll(curAddr, i)
				{
					curAddr[i] -= oldPatchStart;
				}
			}
			else
			{
				// Need to recalculate weights to exclude hits into internal
				labelList newAddr(curAddr.size(), false);
				scalarField newWeights(curAddr.size());
				label nActive = 0;

				forAll(curAddr, lfI)
				{
					if
						(
							curAddr[lfI] >= oldPatchStart
							&& curAddr[lfI] < oldPatchEnd
							)
					{
						newAddr[nActive] = curAddr[lfI] - oldPatchStart;
						newWeights[nActive] = curW[lfI];
						nActive++;
					}
				}

				// Cater for bad mapping
				if (nActive == 0)
				{
					newAddr[nActive] = 0;
					newWeights[nActive] = 1;
					nActive++;
				}

				newAddr.setSize(nActive);
				newWeights.setSize(nActive);

				// Re-scale the weights
				newWeights /= sum(newWeights);

				// Reset addressing and weights
				curAddr = newAddr;
				curW = newWeights;
			}
		}

		if (fvMesh::debug)
		{
			forAll(addr, faceI)
			{
				if (min(addr[faceI]) < 0)
				{
					FatalErrorIn
					(
						"void fvPatchMapper::calcAddressing() const"
					) << "Error in patch mapping for patch "
						<< patch_.index() << " named " << patch_.name()
						<< " face " << faceI
						<< abort(FatalError);
				}
			}
		}
	}
}


void tnbLib::fvPatchMapper::clearOut()
{
	deleteDemandDrivenData(directAddrPtr_);
	deleteDemandDrivenData(interpolationAddrPtr_);
	deleteDemandDrivenData(weightsPtr_);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from components
tnbLib::fvPatchMapper::fvPatchMapper
(
	const fvPatch& patch,
	const faceMapper& faceMap
)
	:
	patch_(patch),
	faceMap_(faceMap),
	sizeBeforeMapping_(faceMap.oldPatchSizes()[patch_.index()]),
	directPtr_(NULL),
	directAddrPtr_(NULL),
	interpolationAddrPtr_(NULL),
	weightsPtr_(NULL)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

tnbLib::fvPatchMapper::~fvPatchMapper()
{
	clearOut();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const tnbLib::unallocLabelList& tnbLib::fvPatchMapper::directAddressing() const
{
	if (!direct())
	{
		FatalErrorIn
		(
			"const unallocLabelList& fvPatchMapper::directAddressing() const"
		) << "Requested direct addressing for an interpolative mapper."
			<< abort(FatalError);
	}

	if (!directAddrPtr_)
	{
		calcAddressing();
	}

	return *directAddrPtr_;
}


const tnbLib::labelListList& tnbLib::fvPatchMapper::addressing() const
{
	if (direct())
	{
		FatalErrorIn
		(
			"const labelListList& fvPatchMapper::addressing() const"
		) << "Requested interpolative addressing for a direct mapper."
			<< abort(FatalError);
	}

	if (!interpolationAddrPtr_)
	{
		calcAddressing();
	}

	return *interpolationAddrPtr_;
}


const tnbLib::scalarListList& tnbLib::fvPatchMapper::weights() const
{
	if (direct())
	{
		FatalErrorIn
		(
			"const scalarListList& fvPatchMapper::weights() const"
		) << "Requested interpolative weights for a direct mapper."
			<< abort(FatalError);
	}

	if (!weightsPtr_)
	{
		calcAddressing();
	}

	return *weightsPtr_;
}


// ************************************************************************* //