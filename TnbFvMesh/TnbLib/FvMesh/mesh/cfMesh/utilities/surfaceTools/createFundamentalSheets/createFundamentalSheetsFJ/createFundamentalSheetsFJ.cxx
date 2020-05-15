#include <createFundamentalSheetsFJ.hxx>

#include <demandDrivenData.hxx>
#include <meshSurfaceEngine.hxx>
#include <extrudeLayer.hxx>

#include <addToRunTimeSelectionTable.hxx>

# ifdef USE_OMP
#include <omp.h>
# endif

// #define DEBUGSearch

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	defineTypeNameAndDebug(createFundamentalSheetsFJ, 0);
	addToRunTimeSelectionTable
	(
		createFundamentalSheets,
		createFundamentalSheetsFJ,
		polyMeshGen
	);

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	void createFundamentalSheetsFJ::createInitialSheet()
	{
		if (!createWrapperSheet_)
			return;

		const PtrList<boundaryPatch>& boundaries = mesh_.boundaries();

		const label start = boundaries[0].patchStart();
		const label end
		(
			boundaries[boundaries.size() - 1].patchStart() +
			boundaries[boundaries.size() - 1].patchSize()
		);

		faceListPMG::subList bFaces(mesh_.faces(), end - start, start);

		const labelList& owner = mesh_.owner();

		LongList<labelPair> extrudeFaces(end - start);

# ifdef USE_OMP
# pragma omp parallel for
# endif
		for (label faceI = start; faceI < end; ++faceI)
			extrudeFaces[faceI - start] = labelPair(faceI, owner[faceI]);

		extrudeLayer(mesh_, extrudeFaces);
	}

	void createFundamentalSheetsFJ::createSheetsAtFeatureEdges()
	{
		const PtrList<boundaryPatch>& boundaries = mesh_.boundaries();

		forAll(boundaries, patchI)
		{
			const cellListPMG& cells = mesh_.cells();
			boolList patchCell(cells.size(), false);

			const labelList& owner = mesh_.owner();
			const labelList& neighbour = mesh_.neighbour();

			const label start = boundaries[patchI].patchStart();
			const label end = start + boundaries[patchI].patchSize();

			for (label faceI = start; faceI < end; ++faceI)
				patchCell[owner[faceI]] = true;

			LongList<labelPair> front;

			for (label faceI = start; faceI < end; ++faceI)
			{
				const cell& c = cells[owner[faceI]];

				forAll(c, fI)
				{
					if (neighbour[c[fI]] < 0)
						continue;

					label nei = owner[c[fI]];
					if (nei == owner[faceI])
						nei = neighbour[c[fI]];

					if (!patchCell[nei])
						front.append(labelPair(c[fI], nei));
				}
			}

			extrudeLayer(mesh_, front);
		}
		//     const cellListPMG& cells = mesh_.cells();
		//     const labelList& owner = mesh_.owner();
		//     const labelList& neighbour = mesh_.neighbour();
		//
		//     const label start = boundaries[0].patchStart();
		//     const label end
		//     (
		//         boundaries[boundaries.size()-1].patchStart() +
		//         boundaries[boundaries.size()-1].patchSize()
		//     );
		//
		//     faceListPMG::subList bFaces(mesh_.faces(), end-start, start);
		//
		//     labelList patchCell(mesh_.cells().size());
		//
		//     LongList<labelPair> front;
		//
		//     const label nThreads = 2 * omp_get_num_procs();
		//     # pragma omp parallel num_threads(nThreads)
		//     {
		//         # pragma omp for
		//         forAll(patchCell, cellI)
		//             patchCell[cellI] = -1;
		//
		//         # pragma omp barrier
		//
		//         # pragma omp for
		//         for(label faceI=start;faceI<end;++faceI)
		//             patchCell[owner[faceI]] = mesh_.faceIsInPatch(faceI);
		//
		//         //- create the front faces
		//         LongList<labelPair> localFront;
		//
		//         # pragma omp for
		//         for(label faceI=start;faceI<end;++faceI)
		//         {
		//             const cell& c = cells[owner[faceI]];
		//             const label patchI = mesh_.faceIsInPatch(faceI);
		//
		//             forAll(c, fI)
		//             {
		//                 if( neighbour[c[fI]] < 0 )
		//                     continue;
		//
		//                 label nei = owner[c[fI]];
		//                 if( nei == owner[faceI] )
		//                     nei = neighbour[c[fI]];
		//
		//                 if( patchCell[nei] != patchI )
		//                     localFront.append(labelPair(c[fI], nei));
		//             }
		//         }
		//
		//         label frontStart(-1);
		//         # pragma omp critical
		//         {
		//             frontStart = front.size();
		//             front.setSize(front.size()+localFront.size());
		//         }
		//
		//         # pragma omp barrier
		//
		//         //- copy the local front into the global front
		//         forAll(localFront, lfI)
		//             front[frontStart+lfI] = localFront[lfI];
		//     }
		//
		//     //- extrude the layer
		//     extrudeLayer(mesh_, front);
	}

	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	// Construct from mesh, octree, regions for boundary vertices
	createFundamentalSheetsFJ::createFundamentalSheetsFJ
	(
		polyMeshGen& mesh,
		const bool createWrapperSheet
	)
		:
		createFundamentalSheets(mesh, createWrapperSheet)
	{
		createInitialSheet();

		createSheetsAtFeatureEdges();
	}

	// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

	createFundamentalSheetsFJ::~createFundamentalSheetsFJ()
	{}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //