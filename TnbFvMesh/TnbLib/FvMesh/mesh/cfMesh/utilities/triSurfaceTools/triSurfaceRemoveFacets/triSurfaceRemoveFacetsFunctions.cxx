#include <triSurfaceRemoveFacets.hxx>

#include <triSurfModifier.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	void triSurfaceRemoveFacets::markFacetsForRemoval(boolList& removeFacet) const
	{
		removeFacet.setSize(surf_.size());
		removeFacet = false;

		const geometricSurfacePatchList& patches = surf_.patches();

		//- mark patches which will be removed
		boolList removePatch(patches.size(), false);

		forAll(patches, patchI)
		{
			if (selectedEntities_.contains(patches[patchI].name()))
				removePatch[patchI] = true;
		}

		//- select facets affected by the deletion of a patch
		forAll(surf_, triI)
		{
			if (removePatch[surf_[triI].region()])
				removeFacet[triI] = true;
		}

		//- mark facets contained in selected subsets
		DynList<label> facetSubsetsIDs;
		surf_.facetSubsetIndices(facetSubsetsIDs);

		forAll(facetSubsetsIDs, i)
		{
			const word fsName = surf_.facetSubsetName(facetSubsetsIDs[i]);

			if (selectedEntities_.contains(fsName))
			{
				labelLongList containedFacets;
				surf_.facetsInSubset(facetSubsetsIDs[i], containedFacets);

				forAll(containedFacets, cfI)
					removeFacet[containedFacets[cfI]] = true;
			}
		}
	}

	void triSurfaceRemoveFacets::removeFacets()
	{
		boolList removeFacet;
		markFacetsForRemoval(removeFacet);

		//- calculate new indices of vertices and facets
		const pointField& points = surf_.points();
		labelLongList newPointLabel(surf_.points().size(), -1);
		labelLongList newFacetLabel(surf_.size(), -1);

		label pointCounter(0), facetCounter(0);

		forAll(removeFacet, triI)
		{
			if (removeFacet[triI])
				continue;

			const labelledTri& tri = surf_[triI];

			forAll(tri, pI)
			{
				if (newPointLabel[tri[pI]] == -1)
					newPointLabel[tri[pI]] = pointCounter++;
			}

			newFacetLabel[triI] = facetCounter++;
		}

		//- remove vertices
		pointField newPts(pointCounter);
		forAll(newPointLabel, pI)
		{
			if (newPointLabel[pI] < 0)
				continue;

			newPts[newPointLabel[pI]] = points[pI];
		}

		triSurfModifier(surf_).pointsAccess().transfer(newPts);
		surf_.updatePointSubsets(newPointLabel);

		//- remove facets
		LongList<labelledTri> newFacets(facetCounter);

		forAll(newFacetLabel, triI)
		{
			if (newFacetLabel[triI] < 0)
				continue;

			const labelledTri& tri = surf_[triI];

			newFacets[newFacetLabel[triI]] =
				labelledTri
				(
					newPointLabel[tri[0]],
					newPointLabel[tri[1]],
					newPointLabel[tri[2]],
					tri.region()
				);
		}

		triSurfModifier(surf_).facetsAccess().transfer(newFacets);
		surf_.updateFacetsSubsets(newFacetLabel);

		//- update feature edges
		const edgeLongList& featureEdges = surf_.featureEdges();
		const VRWGraph& pointEdges = surf_.pointEdges();
		const edgeLongList& edges = surf_.edges();
		const VRWGraph& edgeFacets = surf_.edgeFacets();

		label edgeCounter(0);
		labelLongList newFeatureEdgeLabel(featureEdges.size(), -1);

		forAll(featureEdges, feI)
		{
			const edge& e = featureEdges[feI];

			if ((newPointLabel[e.start()] < 0) || (newPointLabel[e.end()] < 0))
				continue;

			//- find global edge label
			label eI(-1);
			forAllRow(pointEdges, e.start(), peI)
			{
				const label eJ = pointEdges(e.start(), peI);
				if (edges[eJ] == e)
				{
					eI = eJ;
					break;
				}
			}

			if (eI < 0)
				continue;

			//- check if the edge is attached to at least one triangle
			bool foundTriangle(false);
			forAllRow(edgeFacets, eI, efI)
			{
				if (newFacetLabel[edgeFacets(eI, efI)] >= 0)
				{
					foundTriangle = true;
					break;
				}
			}
			if (!foundTriangle)
				continue;

			newFeatureEdgeLabel[feI] = edgeCounter++;
		}

		edgeLongList newFeatureEdges(edgeCounter);
		forAll(newFeatureEdgeLabel, eI)
		{
			if (newFeatureEdgeLabel[eI] < 0)
				continue;

			const edge& e = featureEdges[eI];

			newFeatureEdges[newFeatureEdgeLabel[eI]] =
				edge
				(
					newPointLabel[e.start()],
					newPointLabel[e.end()]
				);
		}

		triSurfModifier(surf_).featureEdgesAccess().transfer(newFeatureEdges);
		surf_.updateEdgeSubsets(newFeatureEdgeLabel);

		selectedEntities_.clear();
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //