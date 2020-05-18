#include <writeMeshFPMA.hxx>

#include <objectRegistry.hxx>
#include <tnbTime.hxx>
#include <polyMeshGen.hxx>
#include <meshSurfaceEngine.hxx>
#include <OFstream.hxx>
#include <IOmanip.hxx>
#include <fileName.hxx>

#include <fpmaMesh.hxx>
#include <helperFunctions.hxx>

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	void writeMeshFPMA(const polyMeshGen& mesh, const word& fName)
	{
		const Time& time = mesh.returnTime();

		const word postProcDir = "FPMA";

		fileName postProcPath = time.path() / postProcDir;

		if (!tnbLib::isDir(postProcPath))
		{
			mkDir(postProcPath);
		}

		// Open the Case file
		const fileName fpmaFileName = fName + ".fpma";

		Info << "Writting mesh into " << fpmaFileName << endl;

		/*    OFstream fpmaGeometryFile
			(
				postProcPath/fpmaFileName,
				IOstream::ASCII,
				IOstream::currentVersion,
				IOstream::UNCOMPRESSED
			);
		*/

		OFstream fpmaGeometryFile(postProcPath / fpmaFileName);

		// Construct the FIRE mesh
		fpmaMesh Mesh(mesh);
		Mesh.write(fpmaGeometryFile);
	}

	void createFIRESelections(polyMeshGen& mesh)
	{
		if (!Pstream::parRun())
			return;

		const faceListPMG& faces = mesh.faces();
		const PtrList<processorBoundaryPatch>& procBoundaries =
			mesh.procBoundaries();

		//- create face selections from proc patches
		forAll(procBoundaries, patchI)
		{
			word sName = "InterFacesToProc";
			sName += help::scalarToText(procBoundaries[patchI].neiProcNo());
			const label sID = mesh.addFaceSubset(sName);

			label faceI = procBoundaries[patchI].patchStart();
			const label end = faceI + procBoundaries[patchI].patchSize();
			for (; faceI < end; ++faceI)
				mesh.addFaceToSubset(sID, faceI);
		}

		//- create cell selections
		DynList<label> subsets;
		mesh.faceSubsetIndices(subsets);
		forAll(subsets, subsetI)
		{
			const word sName = mesh.faceSubsetName(subsets[subsetI]);

			if (sName.substr(0, 10) == "processor_")
			{
				const word newName = "Proc" + sName.substr(10, sName.size() - 10);

				labelLongList cellsInSubset;
				mesh.cellsInSubset(subsets[subsetI], cellsInSubset);
				const label subsetID = mesh.addCellSubset(newName);
				forAll(cellsInSubset, i)
					mesh.addCellToSubset(subsetID, cellsInSubset[i]);
			}
		}

		//- creating node selections
		boolList bndVertex(mesh.points().size(), false);
		forAll(mesh.boundaries(), patchI)
		{
			label faceI = mesh.boundaries()[patchI].patchStart();
			const label end = faceI + mesh.boundaries()[patchI].patchSize();
			for (; faceI < end; ++faceI)
			{
				const face& f = mesh.faces()[faceI];

				forAll(f, pI)
					bndVertex[f[pI]] = true;
			}
		}

		forAll(procBoundaries, patchI)
		{
			word sName = "InterSurfaceEdgesToProc";
			sName += help::scalarToText(procBoundaries[patchI].neiProcNo());
			const label subsetID = mesh.addPointSubset(sName);

			label faceI = procBoundaries[patchI].patchStart();
			const label end = faceI + procBoundaries[patchI].patchSize();
			for (; faceI < end; ++faceI)
			{
				const face& f = faces[faceI];

				forAll(f, pI)
				{
					if (bndVertex[f[pI]])
						mesh.addPointToSubset(subsetID, f[pI]);
				}
			}
		}
	}

}

// ************************************************************************* //