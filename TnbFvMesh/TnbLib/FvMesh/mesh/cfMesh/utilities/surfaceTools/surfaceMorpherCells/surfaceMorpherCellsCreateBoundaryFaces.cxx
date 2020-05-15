#include <surfaceMorpherCells.hxx>

#include <demandDrivenData.hxx>
#include <helperFunctions.hxx>
#include <primitiveMesh.hxx>
#include <PstreamReduceOps.hxx>

//#define DEBUGMorph

# ifdef DEBUGMorph
#include "HashSet.H"
# endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	bool surfaceMorpherCells::removeCellsWithAllVerticesAtTheBoundary()
	{
		boolList removeCells(cellFlags_.size(), false);

		const faceListPMG& faces = mesh_.faces();
		const cellListPMG& cells = mesh_.cells();

		bool changed(false);

		label nRemoved(0);
		forAll(cellFlags_, cellI)
			if (cellFlags_[cellI] & BOUNDARY)
			{
				const cell& c = cells[cellI];
				//- remove cells which have all their vertices at the boundary
				bool allBoundary(true);

				const labelList labels = c.labels(faces);

				forAll(labels, lI)
					if (!boundaryVertex_[labels[lI]])
					{
						allBoundary = false;
						break;
					}

				if (allBoundary)
				{
					++nRemoved;
					changed = true;
					removeCells[cellI] = true;
				}

				//- remove cells which are not topologically closed
				DynList<edge> edges;
				DynList<direction> nAppearances;

				forAll(c, fI)
				{
					const face& f = faces[c[fI]];
					forAll(f, eI)
					{
						const label pos = edges.containsAtPosition(f.faceEdge(eI));

						if (pos == -1)
						{
							edges.append(f.faceEdge(eI));
							nAppearances.append(1);
						}
						else
						{
							++nAppearances[pos];
						}
					}
				}

				forAll(nAppearances, eI)
					if (nAppearances[eI] != 2)
					{
						++nRemoved;
						changed = true;
						removeCells[cellI] = true;
					}

			}

		if (Pstream::parRun())
			reduce(nRemoved, sumOp<label>());

		if (nRemoved != 0)
		{
			Info << "Removing " << nRemoved
				<< " cells which cannot be morphed" << endl;
			polyMeshGenModifier(mesh_).removeCells(removeCells);
		}

		if (Pstream::parRun())
		{
			reduce(changed, maxOp<bool>());
		}

		return changed;
	}

	bool surfaceMorpherCells::morphBoundaryFaces()
	{
		Info << "Morphing boundary faces" << endl;

		newBoundaryFaces_.setSize(0);
		newBoundaryOwners_.setSize(0);
		newBoundaryPatches_.setSize(0);

		const faceListPMG& faces = mesh_.faces();
		const cellListPMG& cells = mesh_.cells();

		bool changed(false);

		forAll(cells, cellI)
			if (cellFlags_[cellI] & BOUNDARY)
			{
				const cell& c = cells[cellI];

				DynList<label> bFaces;

				forAll(c, fI)
					if (mesh_.faceIsInPatch(c[fI]) != -1)
						bFaces.append(c[fI]);

# ifdef DEBUGMorph
				Info << "Boundary faces in cell " << cellI
					<< " are " << bFaces << endl;
				forAll(bFaces, bfI)
					Info << "Face " << bFaces[bfI] << " is "
					<< faces[bFaces[bfI]] << endl;
# endif

				boolList mergedFaces(bFaces.size(), false);

				face mf = faces[bFaces[0]];
				mergedFaces[0] = true;

				bool finished;
				do
				{
					finished = true;
					for (label i = 1; i < bFaces.size(); ++i)
					{
						if (mergedFaces[i]) continue;

						const face& bf = faces[bFaces[i]];
						const edgeList bEdges = bf.edges();
						const edgeList mEdges = mf.edges();

						direction nSharedEdges(0);
						forAll(bEdges, eI)
							forAll(mEdges, eJ)
							if (bEdges[eI] == mEdges[eJ])
								++nSharedEdges;

						direction nSharedPoints(0);
						forAll(bf, pI)
							forAll(mf, pJ)
							if (bf[pI] == mf[pJ])
								++nSharedPoints;

						if (
							nSharedEdges &&
							((nSharedEdges + 1) == nSharedPoints)
							)
						{
							mf = help::mergeTwoFaces(mf, bf);
							mergedFaces[i] = true;
							changed = true;
							finished = false;

							//- set CHANGED flag
							cellFlags_[cellI] |= CHANGED;
						}
					}
				} while (!finished);

				newBoundaryFaces_.appendList(mf);
				newBoundaryOwners_.append(cellI);
				newBoundaryPatches_.append(0);

# ifdef DEBUGMorph
				Info << "Adding merged face " << mf << endl;
# endif

				for (label i = 1; i < bFaces.size(); ++i)
					if (!mergedFaces[i])
					{
						newBoundaryFaces_.appendList(faces[bFaces[i]]);
						newBoundaryOwners_.append(cellI);
						newBoundaryPatches_.append(0);

# ifdef DEBUGMorph
						Info << "Adding untouched boundary face "
							<< faces[bFaces[i]] << endl;
# endif
					}
			}

		if (Pstream::parRun())
		{
			reduce(changed, maxOp<bool>());
		}

		if (changed)
		{
			replaceMeshBoundary();
		}

# ifdef DEBUGMorph
		labelHashSet zipCells;
		mesh_.addressingData().checkCellsZipUp(true, &zipCells);
		if (zipCells.size())
		{
			Info << "Cells " << zipCells << " are not zipped!!" << endl;
			::exit(EXIT_FAILURE);
		}
		mesh_.clearAddressingData();
# endif

		Info << "Finished morphing boundary faces" << endl;

		return changed;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //