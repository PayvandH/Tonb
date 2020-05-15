#include <polyMeshGenModifier.hxx>

#include <demandDrivenData.hxx>
#include <DynList.hxx>
#include <Pstream.hxx>

// #define DEBUGSearch

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	void polyMeshGenModifier::replaceBoundary
	(
		const wordList& patchNames,
		const VRWGraph& boundaryFaces,
		const labelLongList& faceOwners,
		const labelLongList& facePatches
	)
	{
		const label nIntFaces = mesh_.nInternalFaces();

		faceListPMG& faces = this->facesAccess();
		cellListPMG& cells = this->cellsAccess();

		labelLongList newFaceLabel(faces.size(), -1);
		for (label faceI = 0; faceI < nIntFaces; ++faceI)
			newFaceLabel[faceI] = faceI;

		if (Pstream::parRun())
		{
			//- shift processor faces
			PtrList<processorBoundaryPatch>& procBoundaries =
				mesh_.procBoundaries_;

			label nProcFaces(0);
			forAll(procBoundaries, patchI)
				nProcFaces += procBoundaries[patchI].patchSize();

			const label procStart = nIntFaces + boundaryFaces.size();
			const label shift = procStart - procBoundaries[0].patchStart();

			if (shift > 0)
			{
				faces.setSize(procStart + nProcFaces);
				forAllReverse(procBoundaries, patchI)
				{
					const label start = procBoundaries[patchI].patchStart();
					const label end = procBoundaries[patchI].patchSize() + start;
					for (label faceI = end - 1; faceI >= start; --faceI)
					{
						faces[faceI + shift].transfer(faces[faceI]);
						newFaceLabel[faceI] = faceI + shift;
					}

					procBoundaries[patchI].patchStart() += shift;
				}
			}
			else if (shift < 0)
			{
				forAll(procBoundaries, patchI)
				{
					const label start = procBoundaries[patchI].patchStart();
					const label end = procBoundaries[patchI].patchSize() + start;
					for (label faceI = start; faceI < end; ++faceI)
					{
						faces[faceI + shift].transfer(faces[faceI]);
						newFaceLabel[faceI] = faceI + shift;
					}

					procBoundaries[patchI].patchStart() += shift;
				}

				faces.setSize(procStart + nProcFaces);
			}
			else
			{
				//- processor faces are not moved
				for (label fI = 0; fI < nProcFaces; ++fI)
					newFaceLabel[procStart + fI] = procStart + fI;
			}
		}
		else
		{
			faces.setSize(nIntFaces + boundaryFaces.size());
		}

		//- change cells according to the new face ordering
		List<direction> nFacesInCell(cells.size(), direction(0));
		forAll(cells, cellI)
		{
			const cell& c = cells[cellI];

			cell newC(c.size());
			forAll(c, fI)
				if (newFaceLabel[c[fI]] != -1)
					newC[nFacesInCell[cellI]++] = newFaceLabel[c[fI]];

			cells[cellI].transfer(newC);
		}

		mesh_.updateFaceSubsets(newFaceLabel);
		newFaceLabel.setSize(0);

		//- store boundary faces
		labelList newPatchStart(patchNames.size());
		labelList newPatchSize(patchNames.size(), 0);
		forAll(facePatches, bfI)
			++newPatchSize[facePatches[bfI]];

		newPatchStart[0] = nIntFaces;
		for (label i = 1; i < newPatchSize.size(); ++i)
			newPatchStart[i] = newPatchStart[i - 1] + newPatchSize[i - 1];

		//- store boundary faces
		newPatchSize = 0;
		forAll(boundaryFaces, faceI)
		{
			const label fPatch = facePatches[faceI];
			const label fOwn = faceOwners[faceI];

			const label fLabel = newPatchStart[fPatch] + newPatchSize[fPatch]++;

			cells[fOwn].newElmt(nFacesInCell[fOwn]++) = fLabel;
			faces[fLabel].setSize(boundaryFaces.sizeOfRow(faceI));
			forAllRow(boundaryFaces, faceI, pI)
				faces[fLabel][pI] = boundaryFaces(faceI, pI);
		}

		forAll(cells, cellI)
			cells[cellI].setSize(nFacesInCell[cellI]);

		PtrList<boundaryPatch>& boundaries = mesh_.boundaries_;
		if (boundaries.size() == patchNames.size())
		{
			forAll(boundaries, patchI)
			{
				boundaries[patchI].patchName() = patchNames[patchI];
				boundaries[patchI].patchStart() = newPatchStart[patchI];
				boundaries[patchI].patchSize() = newPatchSize[patchI];
			}
		}
		else
		{
			boundaries.clear();
			boundaries.setSize(patchNames.size());
			forAll(boundaries, patchI)
				boundaries.set
				(
					patchI,
					new boundaryPatch
					(
						patchNames[patchI],
						"patch",
						newPatchSize[patchI],
						newPatchStart[patchI]
					)
				);
		}

		mesh_.clearOut();
		this->clearOut();
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //