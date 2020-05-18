#include <topologicalCleaner.hxx>

#include <demandDrivenData.hxx>
#include <decomposeFaces.hxx>
#include <DynList.hxx>
#include <IPstream.hxx>
#include <OPstream.hxx>
#include <Pstream.hxx>
#include <PstreamReduceOps.hxx>

//#define DEBUGCleaner

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	void topologicalCleaner::checkNonMappableCells()
	{
		Info << "Checking for non-mappable cells" << endl;
		//- decompose cells with more than one boundary face
		const labelList& owner = mesh_.owner();

		List<direction> nBoundaryFaces(mesh_.cells().size(), direction(0));
		const PtrList<boundaryPatch>& boundaries = mesh_.boundaries();
		forAll(boundaries, patchI)
		{
			const label start = boundaries[patchI].patchStart();
			const label end = start + boundaries[patchI].patchSize();

			for (label faceI = start; faceI < end; ++faceI)
			{
				++nBoundaryFaces[owner[faceI]];
			}
		}

		label nBadCells(0);
		forAll(nBoundaryFaces, cI)
			if (nBoundaryFaces[cI] > 1)
			{
				++nBadCells;
				decomposeCell_[cI] = true;
			}

		if (Pstream::parRun())
			reduce(nBadCells, sumOp<label>());

		if (nBadCells != 0)
			changed_ = true;

		Info << "Found " << nBadCells << " non-mappable cells" << endl;
		Info << "Finished checking for non-mappable cells" << endl;
	}

	void topologicalCleaner::checkNonMappableFaces()
	{
		Info << "Checking for non-mappable faces" << endl;

		const faceListPMG& faces = mesh_.faces();
		const labelList& owner = mesh_.owner();
		const labelList& neighbour = mesh_.neighbour();

		//- find boundary vertices
		boolList boundaryVertex(mesh_.points().size(), false);
		const PtrList<boundaryPatch>& boundaries = mesh_.boundaries();
		forAll(boundaries, patchI)
		{
			const label start = boundaries[patchI].patchStart();
			const label end = start + boundaries[patchI].patchSize();

			for (label faceI = start; faceI < end; ++faceI)
			{
				const face& f = faces[faceI];
				forAll(f, pI)
					boundaryVertex[f[pI]] = true;
			}
		}

		boolList decomposeFace(faces.size(), false);

		//- internal faces which have more than two vertices at the boundary
		//- cannot always be mapped at the boundary and form a valid cell
		//- The second case of interest are faces which have two vertices at the
		//- boundary but are not connected over an edge
		const label nIntFaces = mesh_.nInternalFaces();

		//bool changed(false);

		label nBadFaces(0);
		for (label faceI = 0; faceI < nIntFaces; ++faceI)
		{
			const face& f = faces[faceI];

			DynList<label> bPos;
			forAll(f, pI)
				if (boundaryVertex[f[pI]])
					bPos.append(pI);

			if (
				(bPos.size() > 2) ||
				(
				(bPos.size() == 2) &&
					(
					(bPos[1] != (bPos[0] + 1)) &&
						!((bPos[0] == 0) && (bPos[1] == (f.size() - 1)))
						)
					)
				)
			{
				++nBadFaces;
				decomposeFace[faceI] = true;
				decomposeCell_[owner[faceI]] = true;
				decomposeCell_[neighbour[faceI]] = true;
			}
		}

		if (Pstream::parRun())
		{
			//- check processor faces
			const PtrList<processorBoundaryPatch>& procBoundaries =
				mesh_.procBoundaries();

			forAll(procBoundaries, patchI)
			{
				const label start = procBoundaries[patchI].patchStart();
				const label end = start + procBoundaries[patchI].patchSize();

				boolList decProcFace(procBoundaries[patchI].patchSize(), false);

				for (label faceI = start; faceI < end; ++faceI)
				{
					const face& f = faces[faceI];

					DynList<label> bPos;
					forAll(f, pI)
						if (boundaryVertex[f[pI]])
							bPos.append(pI);

					if (
						(bPos.size() > 2) ||
						(
						(bPos.size() == 2) &&
							(
							(bPos[1] != (bPos[0] + 1)) &&
								!((bPos[0] == 0) && (bPos[1] == (f.size() - 1)))
								)
							)
						)
					{
						++nBadFaces;
						decProcFace[faceI - start] = true;
						decomposeFace[faceI] = true;
						decomposeCell_[owner[faceI]] = true;
					}
				}

				//- send information about decomposed faces to other processor
				OPstream toOtherProc
				(
					Pstream::blocking,
					procBoundaries[patchI].neiProcNo(),
					decProcFace.byteSize()
				);
				toOtherProc << decProcFace;
			}

			forAll(procBoundaries, patchI)
			{
				boolList decOtherProc;
				IPstream fromOtherProc
				(
					Pstream::blocking,
					procBoundaries[patchI].neiProcNo()
				);
				fromOtherProc >> decOtherProc;

				const label start = procBoundaries[patchI].patchStart();
				forAll(decOtherProc, faceI)
					if (decOtherProc[faceI])
					{
						decomposeFace[start + faceI] = true;
						decomposeCell_[owner[start + faceI]] = true;
					}
			}

			reduce(nBadFaces, sumOp<label>());
		}

		Info << "Found " << nBadFaces << " non-mappable faces" << endl;

		if (nBadFaces != 0)
		{
			changed_ = true;
			decomposeFaces(mesh_).decomposeMeshFaces(decomposeFace);
		}

		Info << "Finished checking non-mappable faces" << endl;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //