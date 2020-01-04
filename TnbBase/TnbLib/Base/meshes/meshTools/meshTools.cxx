#include <meshTools.hxx>

#include <polyMesh.hxx>
#include <hexMatcher.hxx>
#include <UIndirectList.hxx>
#include <ListOps.hxx>

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

// Check if n is in same direction as normals of all faceLabels
bool tnbLib::meshTools::visNormal
(
	const vector& n,
	const vectorField& faceNormals,
	const labelList& faceLabels
)
{
	forAll(faceLabels, i)
	{
		if ((faceNormals[faceLabels[i]] & n) < SMALL)
		{
			// Found normal in different direction from n.
			return false;
		}
	}

	return true;
}


tnbLib::vectorField tnbLib::meshTools::calcBoxPointNormals(const primitivePatch& pp)
{
	vectorField octantNormal(8);
	octantNormal[mXmYmZ] = vector(-1, -1, -1);
	octantNormal[pXmYmZ] = vector(1, -1, -1);
	octantNormal[mXpYmZ] = vector(-1, 1, -1);
	octantNormal[pXpYmZ] = vector(1, 1, -1);

	octantNormal[mXmYpZ] = vector(-1, -1, 1);
	octantNormal[pXmYpZ] = vector(1, -1, 1);
	octantNormal[mXpYpZ] = vector(-1, 1, 1);
	octantNormal[pXpYpZ] = vector(1, 1, 1);

	octantNormal /= mag(octantNormal);


	vectorField pn(pp.nPoints());

	const vectorField& faceNormals = pp.faceNormals();
	const vectorField& pointNormals = pp.pointNormals();
	const labelListList& pointFaces = pp.pointFaces();

	forAll(pointFaces, pointI)
	{
		const labelList& pFaces = pointFaces[pointI];

		if (visNormal(pointNormals[pointI], faceNormals, pFaces))
		{
			pn[pointI] = pointNormals[pointI];
		}
		else
		{
			WarningIn
			(
				"tnbLib::meshTools::calcBoxPointNormals(const primitivePatch& pp)"
			) << "Average point normal not visible for point:"
				<< pp.meshPoints()[pointI] << endl;

			label visOctant =
				mXmYmZMask
				| pXmYmZMask
				| mXpYmZMask
				| pXpYmZMask
				| mXmYpZMask
				| pXmYpZMask
				| mXpYpZMask
				| pXpYpZMask;

			forAll(pFaces, i)
			{
				const vector& n = faceNormals[pFaces[i]];

				if (n.x() > SMALL)
				{
					// All -x octants become invisible
					visOctant &= ~mXmYmZMask;
					visOctant &= ~mXmYpZMask;
					visOctant &= ~mXpYmZMask;
					visOctant &= ~mXpYpZMask;
				}
				else if (n.x() < -SMALL)
				{
					// All +x octants become invisible
					visOctant &= ~pXmYmZMask;
					visOctant &= ~pXmYpZMask;
					visOctant &= ~pXpYmZMask;
					visOctant &= ~pXpYpZMask;
				}

				if (n.y() > SMALL)
				{
					visOctant &= ~mXmYmZMask;
					visOctant &= ~mXmYpZMask;
					visOctant &= ~pXmYmZMask;
					visOctant &= ~pXmYpZMask;
				}
				else if (n.y() < -SMALL)
				{
					visOctant &= ~mXpYmZMask;
					visOctant &= ~mXpYpZMask;
					visOctant &= ~pXpYmZMask;
					visOctant &= ~pXpYpZMask;
				}

				if (n.z() > SMALL)
				{
					visOctant &= ~mXmYmZMask;
					visOctant &= ~mXpYmZMask;
					visOctant &= ~pXmYmZMask;
					visOctant &= ~pXpYmZMask;
				}
				else if (n.z() < -SMALL)
				{
					visOctant &= ~mXmYpZMask;
					visOctant &= ~mXpYpZMask;
					visOctant &= ~pXmYpZMask;
					visOctant &= ~pXpYpZMask;
				}
			}

			label visI = -1;

			label mask = 1;

			for (label octant = 0; octant < 8; octant++)
			{
				if (visOctant & mask)
				{
					// Take first visible octant found. Note: could use
					// combination of visible octants here.
					visI = octant;

					break;
				}
				mask <<= 1;
			}

			if (visI != -1)
			{
				// Take a vector in this octant.
				pn[pointI] = octantNormal[visI];
			}
			else
			{
				pn[pointI] = vector::zero;

				WarningIn
				(
					"tnbLib::meshTools::calcBoxPointNormals"
					"(const primitivePatch& pp)"
				) << "No visible octant for point:" << pp.meshPoints()[pointI]
					<< " cooord:" << pp.points()[pp.meshPoints()[pointI]] << nl
					<< "Normal set to " << pn[pointI] << endl;
			}
		}
	}

	return pn;
}


tnbLib::vector tnbLib::meshTools::normEdgeVec
(
	const primitiveMesh& mesh,
	const label edgeI
)
{
	vector eVec = mesh.edges()[edgeI].vec(mesh.points());

	eVec /= mag(eVec);

	return eVec;
}


void tnbLib::meshTools::writeOBJ
(
	Ostream& os,
	const point& pt
)
{
	os << "v " << pt.x() << ' ' << pt.y() << ' ' << pt.z() << endl;
}


void tnbLib::meshTools::writeOBJ
(
	Ostream& os,
	const faceList& faces,
	const pointField& points,
	const labelList& faceLabels
)
{
	Map<label> foamToObj(4 * faceLabels.size());

	label vertI = 0;

	forAll(faceLabels, i)
	{
		const face& f = faces[faceLabels[i]];

		forAll(f, fp)
		{
			if (foamToObj.insert(f[fp], vertI))
			{
				writeOBJ(os, points[f[fp]]);
				vertI++;
			}
		}

		os << 'l';
		forAll(f, fp)
		{
			os << ' ' << foamToObj[f[fp]] + 1;
		}
		os << ' ' << foamToObj[f[0]] + 1 << endl;
	}
}


void tnbLib::meshTools::writeOBJ
(
	Ostream& os,
	const faceList& faces,
	const pointField& points
)
{
	labelList allFaces(faces.size());
	forAll(allFaces, i)
	{
		allFaces[i] = i;
	}
	writeOBJ(os, faces, points, allFaces);
}


void tnbLib::meshTools::writeOBJ
(
	Ostream& os,
	const cellList& cells,
	const faceList& faces,
	const pointField& points,
	const labelList& cellLabels
)
{
	labelHashSet usedFaces(4 * cellLabels.size());

	forAll(cellLabels, i)
	{
		const cell& cFaces = cells[cellLabels[i]];

		forAll(cFaces, j)
		{
			usedFaces.insert(cFaces[j]);
		}
	}

	writeOBJ(os, faces, points, usedFaces.toc());
}


bool tnbLib::meshTools::edgeOnCell
(
	const primitiveMesh& mesh,
	const label cellI,
	const label edgeI
)
{
	return findIndex(mesh.edgeCells(edgeI), cellI) != -1;
}


bool tnbLib::meshTools::edgeOnFace
(
	const primitiveMesh& mesh,
	const label faceI,
	const label edgeI
)
{
	return findIndex(mesh.faceEdges(faceI), edgeI) != -1;
}


// Return true if faceI part of cellI
bool tnbLib::meshTools::faceOnCell
(
	const primitiveMesh& mesh,
	const label cellI,
	const label faceI
)
{
	if (mesh.isInternalFace(faceI))
	{
		if
			(
			(mesh.faceOwner()[faceI] == cellI)
				|| (mesh.faceNeighbour()[faceI] == cellI)
				)
		{
			return true;
		}
	}
	else
	{
		if (mesh.faceOwner()[faceI] == cellI)
		{
			return true;
		}
	}
	return false;
}


tnbLib::label tnbLib::meshTools::findEdge
(
	const edgeList& edges,
	const labelList& candidates,
	const label v0,
	const label v1
)
{
	forAll(candidates, i)
	{
		label edgeI = candidates[i];

		const edge& e = edges[edgeI];

		if ((e[0] == v0 && e[1] == v1) || (e[0] == v1 && e[1] == v0))
		{
			return edgeI;
		}
	}
	return -1;
}


tnbLib::label tnbLib::meshTools::findEdge
(
	const primitiveMesh& mesh,
	const label v0,
	const label v1
)
{
	const edgeList& edges = mesh.edges();

	const labelList& v0Edges = mesh.pointEdges()[v0];

	forAll(v0Edges, i)
	{
		label edgeI = v0Edges[i];

		const edge& e = edges[edgeI];

		if ((e.start() == v1) || (e.end() == v1))
		{
			return edgeI;
		}
	}
	return -1;
}


tnbLib::label tnbLib::meshTools::getSharedEdge
(
	const primitiveMesh& mesh,
	const label f0,
	const label f1
)
{
	const labelList& f0Edges = mesh.faceEdges()[f0];
	const labelList& f1Edges = mesh.faceEdges()[f1];

	forAll(f0Edges, f0EdgeI)
	{
		label edge0 = f0Edges[f0EdgeI];

		forAll(f1Edges, f1EdgeI)
		{
			label edge1 = f1Edges[f1EdgeI];

			if (edge0 == edge1)
			{
				return edge0;
			}
		}
	}
	FatalErrorIn
	(
		"meshTools::getSharedEdge(const primitiveMesh&, const label"
		", const label)"
	) << "Faces " << f0 << " and " << f1 << " do not share an edge"
		<< abort(FatalError);

	return -1;

}


tnbLib::label tnbLib::meshTools::getSharedFace
(
	const primitiveMesh& mesh,
	const label cell0I,
	const label cell1I
)
{
	const cell& cFaces = mesh.cells()[cell0I];

	forAll(cFaces, cFaceI)
	{
		label faceI = cFaces[cFaceI];

		if
			(
				mesh.isInternalFace(faceI)
				&& (
					mesh.faceOwner()[faceI] == cell1I
					|| mesh.faceNeighbour()[faceI] == cell1I
					)
				)
		{
			return faceI;
		}
	}


	FatalErrorIn
	(
		"meshTools::getSharedFace(const primitiveMesh&, const label"
		", const label)"
	) << "No common face for"
		<< "  cell0I:" << cell0I << "  faces:" << cFaces
		<< "  cell1I:" << cell1I << "  faces:"
		<< mesh.cells()[cell1I]
		<< abort(FatalError);

	return -1;
}


// Get the two faces on cellI using edgeI.
void tnbLib::meshTools::getEdgeFaces
(
	const primitiveMesh& mesh,
	const label cellI,
	const label edgeI,
	label& face0,
	label& face1
)
{
	const labelList& eFaces = mesh.edgeFaces(edgeI);

	face0 = -1;
	face1 = -1;

	forAll(eFaces, eFaceI)
	{
		label faceI = eFaces[eFaceI];

		if (faceOnCell(mesh, cellI, faceI))
		{
			if (face0 == -1)
			{
				face0 = faceI;
			}
			else
			{
				face1 = faceI;

				return;
			}
		}
	}

	if ((face0 == -1) || (face1 == -1))
	{
		FatalErrorIn
		(
			"meshTools::getEdgeFaces(const primitiveMesh&, const label"
			", const label, label&, label&"
		) << "Can not find faces using edge " << mesh.edges()[edgeI]
			<< " on cell " << cellI << abort(FatalError);
	}
}


// Return label of other edge connected to vertex
tnbLib::label tnbLib::meshTools::otherEdge
(
	const primitiveMesh& mesh,
	const labelList& edgeLabels,
	const label thisEdgeI,
	const label thisVertI
)
{
	forAll(edgeLabels, edgeLabelI)
	{
		label edgeI = edgeLabels[edgeLabelI];

		if (edgeI != thisEdgeI)
		{
			const edge& e = mesh.edges()[edgeI];

			if ((e.start() == thisVertI) || (e.end() == thisVertI))
			{
				return edgeI;
			}
		}
	}

	FatalErrorIn
	(
		"meshTools::otherEdge(const primitiveMesh&, const labelList&"
		", const label, const label)"
	) << "Can not find edge in "
		<< UIndirectList<edge>(mesh.edges(), edgeLabels)()
		<< " connected to edge "
		<< thisEdgeI << " with vertices " << mesh.edges()[thisEdgeI]
		<< " on side " << thisVertI << abort(FatalError);

	return -1;
}


// Return face on other side of edgeI
tnbLib::label tnbLib::meshTools::otherFace
(
	const primitiveMesh& mesh,
	const label cellI,
	const label faceI,
	const label edgeI
)
{
	label face0;
	label face1;

	getEdgeFaces(mesh, cellI, edgeI, face0, face1);

	if (face0 == faceI)
	{
		return face1;
	}
	else
	{
		return face0;
	}
}


// Return face on other side of edgeI
tnbLib::label tnbLib::meshTools::otherCell
(
	const primitiveMesh& mesh,
	const label otherCellI,
	const label faceI
)
{
	if (!mesh.isInternalFace(faceI))
	{
		FatalErrorIn
		(
			"meshTools::otherCell(const primitiveMesh&, const label"
			", const label)"
		) << "Face " << faceI << " is not internal"
			<< abort(FatalError);
	}

	label newCellI = mesh.faceOwner()[faceI];

	if (newCellI == otherCellI)
	{
		newCellI = mesh.faceNeighbour()[faceI];
	}
	return newCellI;
}


// Returns label of edge nEdges away from startEdge (in the direction of
// startVertI)
tnbLib::label tnbLib::meshTools::walkFace
(
	const primitiveMesh& mesh,
	const label faceI,
	const label startEdgeI,
	const label startVertI,
	const label nEdges
)
{
	const labelList& fEdges = mesh.faceEdges(faceI);

	label edgeI = startEdgeI;

	label vertI = startVertI;

	for (label iter = 0; iter < nEdges; iter++)
	{
		edgeI = otherEdge(mesh, fEdges, edgeI, vertI);

		vertI = mesh.edges()[edgeI].otherVertex(vertI);
	}

	return edgeI;
}


void tnbLib::meshTools::constrainToMeshCentre
(
	const polyMesh& mesh,
	point& pt
)
{
	const Vector<label>& dirs = mesh.geometricD();

	const point& min = mesh.bounds().min();
	const point& max = mesh.bounds().max();

	for (direction cmpt = 0; cmpt < vector::nComponents; cmpt++)
	{
		if (dirs[cmpt] == -1)
		{
			pt[cmpt] = 0.5*(min[cmpt] + max[cmpt]);
		}
	}
}


void tnbLib::meshTools::constrainToMeshCentre
(
	const polyMesh& mesh,
	pointField& pts
)
{
	const Vector<label>& dirs = mesh.geometricD();

	const point& min = mesh.bounds().min();
	const point& max = mesh.bounds().max();

	bool isConstrained = false;
	for (direction cmpt = 0; cmpt < vector::nComponents; cmpt++)
	{
		if (dirs[cmpt] == -1)
		{
			isConstrained = true;
			break;
		}
	}

	if (isConstrained)
	{
		forAll(pts, i)
		{
			for (direction cmpt = 0; cmpt < vector::nComponents; cmpt++)
			{
				if (dirs[cmpt] == -1)
				{
					pts[i][cmpt] = 0.5*(min[cmpt] + max[cmpt]);
				}
			}
		}
	}
}


//- Set the constrained components of directions/velocity to zero
void tnbLib::meshTools::constrainDirection
(
	const polyMesh& mesh,
	const Vector<label>& dirs,
	vector& d
)
{
	for (direction cmpt = 0; cmpt < vector::nComponents; cmpt++)
	{
		if (dirs[cmpt] == -1)
		{
			d[cmpt] = 0.0;
		}
	}
}


void tnbLib::meshTools::constrainDirection
(
	const polyMesh& mesh,
	const Vector<label>& dirs,
	vectorField& d
)
{
	bool isConstrained = false;
	for (direction cmpt = 0; cmpt < vector::nComponents; cmpt++)
	{
		if (dirs[cmpt] == -1)
		{
			isConstrained = true;
			break;
		}
	}

	if (isConstrained)
	{
		forAll(d, i)
		{
			for (direction cmpt = 0; cmpt < vector::nComponents; cmpt++)
			{
				if (dirs[cmpt] == -1)
				{
					d[i][cmpt] = 0.0;
				}
			}
		}
	}
}


void tnbLib::meshTools::getParallelEdges
(
	const primitiveMesh& mesh,
	const label cellI,
	const label e0,
	label& e1,
	label& e2,
	label& e3
)
{
	// Go to any face using e0
	label faceI = meshTools::otherFace(mesh, cellI, -1, e0);

	// Opposite edge on face
	e1 = meshTools::walkFace(mesh, faceI, e0, mesh.edges()[e0].end(), 2);

	faceI = meshTools::otherFace(mesh, cellI, faceI, e1);

	e2 = meshTools::walkFace(mesh, faceI, e1, mesh.edges()[e1].end(), 2);

	faceI = meshTools::otherFace(mesh, cellI, faceI, e2);

	e3 = meshTools::walkFace(mesh, faceI, e2, mesh.edges()[e2].end(), 2);
}


tnbLib::vector tnbLib::meshTools::edgeToCutDir
(
	const primitiveMesh& mesh,
	const label cellI,
	const label startEdgeI
)
{
	if (!hexMatcher().isA(mesh, cellI))
	{
		FatalErrorIn
		(
			"tnbLib::meshTools::getCutDir(const label, const label)"
		) << "Not a hex : cell:" << cellI << abort(FatalError);
	}


	vector avgVec(normEdgeVec(mesh, startEdgeI));

	label edgeI = startEdgeI;

	label faceI = -1;

	for (label i = 0; i < 3; i++)
	{
		// Step to next face, next edge
		faceI = meshTools::otherFace(mesh, cellI, faceI, edgeI);

		vector eVec(normEdgeVec(mesh, edgeI));

		if ((eVec & avgVec) > 0)
		{
			avgVec += eVec;
		}
		else
		{
			avgVec -= eVec;
		}

		label vertI = mesh.edges()[edgeI].end();

		edgeI = meshTools::walkFace(mesh, faceI, edgeI, vertI, 2);
	}

	avgVec /= mag(avgVec) + VSMALL;

	return avgVec;
}


// Find edges most aligned with cutDir
tnbLib::label tnbLib::meshTools::cutDirToEdge
(
	const primitiveMesh& mesh,
	const label cellI,
	const vector& cutDir
)
{
	if (!hexMatcher().isA(mesh, cellI))
	{
		FatalErrorIn
		(
			"tnbLib::meshTools::getCutDir(const label, const vector&)"
		) << "Not a hex : cell:" << cellI << abort(FatalError);
	}

	const labelList& cEdges = mesh.cellEdges()[cellI];

	labelHashSet doneEdges(2 * cEdges.size());

	scalar maxCos = -GREAT;
	label maxEdgeI = -1;

	for (label i = 0; i < 4; i++)
	{
		forAll(cEdges, cEdgeI)
		{
			label e0 = cEdges[cEdgeI];

			if (!doneEdges.found(e0))
			{
				vector avgDir(edgeToCutDir(mesh, cellI, e0));

				scalar cosAngle = mag(avgDir & cutDir);

				if (cosAngle > maxCos)
				{
					maxCos = cosAngle;
					maxEdgeI = e0;
				}

				// Mark off edges in cEdges.
				label e1, e2, e3;
				getParallelEdges(mesh, cellI, e0, e1, e2, e3);

				doneEdges.insert(e0);
				doneEdges.insert(e1);
				doneEdges.insert(e2);
				doneEdges.insert(e3);
			}
		}
	}

	forAll(cEdges, cEdgeI)
	{
		if (!doneEdges.found(cEdges[cEdgeI]))
		{
			FatalErrorIn
			(
				"meshTools::cutDirToEdge(const label, const vector&)"
			) << "Cell:" << cellI << " edges:" << cEdges << endl
				<< "Edge:" << cEdges[cEdgeI] << " not yet handled"
				<< abort(FatalError);
		}
	}

	if (maxEdgeI == -1)
	{
		FatalErrorIn
		(
			"meshTools::cutDirToEdge(const label, const vector&)"
		) << "Problem : did not find edge aligned with " << cutDir
			<< " on cell " << cellI << abort(FatalError);
	}

	return maxEdgeI;
}


// ************************************************************************* //