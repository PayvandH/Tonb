#include <PatchTools.hxx>

#include <dynamicLabelList.hxx>
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Finds area, starting at faceI, delimited by borderEdge.
// Marks all visited faces (from face-edge-face walk) with currentZone.
template
<
	class BoolListType,
	class Face,
	template<class> class FaceList,
	class PointField,
	class PointType
>

void
tnbLib::PatchTools::markZone
(
	const PrimitivePatch<Face, FaceList, PointField, PointType>& p,
	const BoolListType& borderEdge,
	const label faceI,
	const label currentZone,
	labelList&  faceZone
)
{
	const labelListList& faceEdges = p.faceEdges();
	const labelListList& edgeFaces = p.edgeFaces();

	// List of faces whose faceZone has been set.
	labelList changedFaces(1, faceI);

	while (true)
	{
		// Pick up neighbours of changedFaces
		dynamicLabelList newChangedFaces(2 * changedFaces.size());

		forAll(changedFaces, i)
		{
			label faceI = changedFaces[i];

			const labelList& fEdges = faceEdges[faceI];

			forAll(fEdges, fEdgeI)
			{
				label edgeI = fEdges[fEdgeI];

				if (!borderEdge[edgeI])
				{
					const labelList& eFaceLst = edgeFaces[edgeI];

					forAll(eFaceLst, j)
					{
						label nbrFaceI = eFaceLst[j];

						if (faceZone[nbrFaceI] == -1)
						{
							faceZone[nbrFaceI] = currentZone;
							newChangedFaces.append(nbrFaceI);
						}
						else if (faceZone[nbrFaceI] != currentZone)
						{
							FatalErrorIn
							(
								"PatchTools::markZone"
								"(const boolList&, const label, const label, labelList&)"
							)
								<< "Zones " << faceZone[nbrFaceI]
								<< " at face " << nbrFaceI
								<< " connects to zone " << currentZone
								<< " at face " << faceI
								<< abort(FatalError);
						}
					}
				}
			}
		}

		if (newChangedFaces.empty())
		{
			break;
		}

		// transfer from dynamic to normal list
		changedFaces.transfer(newChangedFaces);
	}
}


// Finds areas delimited by borderEdge (or 'real' edges).
// Fills faceZone accordingly
template
<
	class BoolListType,
	class Face,
	template<class> class FaceList,
	class PointField,
	class PointType
>

tnbLib::label
tnbLib::PatchTools::markZones
(
	const PrimitivePatch<Face, FaceList, PointField, PointType>& p,
	const BoolListType& borderEdge,
	labelList& faceZone
)
{
	faceZone.setSize(p.size());
	faceZone = -1;

	label zoneI = 0;
	for (label startFaceI = 0; startFaceI < faceZone.size();)
	{
		// Find next non-visited face
		for (; startFaceI < faceZone.size(); ++startFaceI)
		{
			if (faceZone[startFaceI] == -1)
			{
				faceZone[startFaceI] = zoneI;
				markZone(p, borderEdge, startFaceI, zoneI, faceZone);
				zoneI++;
				break;
			}
		}
	}

	return zoneI;
}



// Finds areas delimited by borderEdge (or 'real' edges).
// Fills faceZone accordingly
template
<
	class BoolListType,
	class Face,
	template<class> class FaceList,
	class PointField,
	class PointType
>

void
tnbLib::PatchTools::subsetMap
(
	const PrimitivePatch<Face, FaceList, PointField, PointType>& p,
	const BoolListType& includeFaces,
	labelList& pointMap,
	labelList& faceMap
)
{
	label faceI = 0;
	label pointI = 0;

	const List<Face>& localFaces = p.localFaces();

	faceMap.setSize(localFaces.size());
	pointMap.setSize(p.nPoints());

	boolList pointHad(pointMap.size(), false);

	forAll(p, oldFaceI)
	{
		if (includeFaces[oldFaceI])
		{
			// Store new faces compact
			faceMap[faceI++] = oldFaceI;

			// Renumber labels for face
			const Face& f = localFaces[oldFaceI];

			forAll(f, fp)
			{
				const label ptLabel = f[fp];
				if (!pointHad[ptLabel])
				{
					pointHad[ptLabel] = true;
					pointMap[pointI++] = ptLabel;
				}
			}
		}
	}

	// Trim
	faceMap.setSize(faceI);
	pointMap.setSize(pointI);
}


// ************************************************************************* //