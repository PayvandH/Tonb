#pragma once
#ifndef _face_Header
#define _face_Header

#include <pointField.hxx>
#include <labelList.hxx>
#include <edgeList.hxx>
#include <vectorField.hxx>
#include <faceListFwd.hxx>
#include <intersection.hxx>
#include <pointHit.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// Forward declaration of friend functions and operators

	class face;
	class triFace;

	template<class T, unsigned SizeInc, unsigned SizeMult, unsigned SizeDiv>
	class DynamicList;

	inline bool operator==(const face& a, const face& b);
	inline bool operator!=(const face& a, const face& b);
	inline Istream& operator>>(Istream&, face&);

	/*---------------------------------------------------------------------------*\
							   Class face Declaration
	\*---------------------------------------------------------------------------*/

	class face
		: public labelList
	{
		// Private Member Functions

			//- Edge to the right of face vertex i
		label right(const label i) const;

		//- Edge to the left of face vertex i
		label left(const label i) const;

		//- Construct list of edge vectors for face
		tmp<vectorField> calcEdges
		(
			const pointField& points
		) const;

		//- Cos between neighbouring edges
		scalar edgeCos
		(
			const vectorField& edges,
			const label index
		) const;

		//- Find index of largest internal angle on face
		label mostConcaveAngle
		(
			const pointField& points,
			const vectorField& edges,
			scalar& edgeCos
		) const;

		//- Enumeration listing the modes for split()
		enum splitMode
		{
			COUNTTRIANGLE,  // count if split into triangles
			COUNTQUAD,      // count if split into triangles&quads
			SPLITTRIANGLE,  // split into triangles
			SPLITQUAD       // split into triangles&quads
		};

		//- Split face into triangles or triangles&quads.
		//  Stores results quadFaces[quadI], triFaces[triI]
		//  Returns number of new faces created
		label split
		(
			const splitMode mode,
			const pointField& points,
			label& triI,
			label& quadI,
			faceList& triFaces,
			faceList& quadFaces
		) const;


	public:

		// Static data members

		static const char* const typeName;


		// Constructors

		//- Construct null
		face();

		//- Construct given size
		explicit face(label);

		//- Construct from list of labels
		explicit face(const UList<label>&);

		//- Construct from list of labels
		explicit face(const labelList&);

		//- Construct by transferring the parameter contents
		explicit face(const Xfer<labelList>&);

		//- Copy construct from triFace
		face(const triFace&);

		//- Construct from Istream
		face(Istream&);


		// Member Functions

			//- Collapse face by removing duplicate point labels
			//  return the collapsed size
		label collapse();

		//- Return the points corresponding to this face
		pointField points(const pointField& meshPoints) const;

		//- Centre point of face
		point centre(const pointField&) const;

		//- Calculate average value at centroid of face
		template<class Type>
		Type average(const pointField&, const Field<Type>&) const;

		//- Scalar magnitude
		scalar mag(const pointField&) const;

		//- Vector normal; magnitude is equal to area of face
		vector normal(const pointField&) const;

		//- Return face with reverse direction
		face reverseFace() const;

		//- Navigation through face vertices

			//- Which vertex on face (face index given a global index)
		label which(const label globalIndex) const;

		//- Next vertex on face
		label nextLabel(const label i) const;

		//- Previous vertex on face
		label prevLabel(const label i) const;


		//- Return the volume swept out by the face when its points move
		scalar sweptVol
		(
			const pointField& oldPoints,
			const pointField& newPoints
		) const;

		//- Return potential intersection with face with a ray starting
		//  at p, direction n (does not need to be normalized)
		//  Does face-center decomposition and returns triangle intersection
		//  point closest to p. Face-center is calculated from point average.
		//  For a hit, the distance is signed.  Positive number
		//  represents the point in front of triangle
		//  In case of miss the point is the nearest point on the face
		//  and the distance is the distance between the intersection point
		//  and the original point.
		//  The half-ray or full-ray intersection and the contact
		//  sphere adjustment of the projection vector is set by the
		//  intersection parameters
		pointHit ray
		(
			const point& p,
			const vector& n,
			const pointField& meshPoints,
			const intersection::algorithm alg = intersection::FULL_RAY,
			const intersection::direction dir = intersection::VECTOR
		) const;

		//- Fast intersection with a ray.
		//  For a hit, the pointHit.distance() is the line parameter t :
		//  intersection=p+t*q. Only defined for FULL_RAY or
		//  HALF_RAY.
		pointHit fastIntersection
		(
			const point& p,
			const vector& q,
			const point& ctr,
			const pointField& meshPoints,
			const intersection::algorithm alg,
			const scalar tol = 0.0
		) const;

		//- Return nearest point to face
		pointHit nearestPoint
		(
			const point& p,
			const pointField& meshPoints
		) const;

		//- Return contact sphere diameter
		scalar contactSphereDiameter
		(
			const point& p,
			const vector& n,
			const pointField& meshPoints
		) const;

		//- Return area in contact, given the displacement in vertices
		scalar areaInContact
		(
			const pointField& points,
			const scalarField& v
		) const;

		//- Return number of edges
		label nEdges() const;

		//- Return edges in face point ordering, i.e. edges()[0] is edge
		//  between [0] and [1]
		edgeList edges() const;

		//- Return n-th face edge
		edge faceEdge(const label n) const;

		//- Return the edge direction on the face
		//  -  0: edge not found on the face
		//  - +1: forward (counter-clockwise) on the face
		//  - -1: reverse (clockwise) on the face
		int edgeDirection(const edge&) const;

		// Face splitting utilities

			//- Number of triangles after splitting
		label nTriangles() const;

		//- Number of triangles after splitting
		label nTriangles(const pointField& points) const;

		//- Split into triangles using existing points.
		//  Result in triFaces[triI..triI+nTri].
		//  Splits intelligently to maximize triangle quality.
		//  Returns number of faces created.
		label triangles
		(
			const pointField& points,
			label& triI,
			faceList& triFaces
		) const;

		//- Split into triangles using existing points.
		//  Append to DynamicList.
		//  Returns number of faces created.
		template<unsigned SizeInc, unsigned SizeMult, unsigned SizeDiv>
		label triangles
		(
			const pointField& points,
			DynamicList<face, SizeInc, SizeMult, SizeDiv>& triFaces
		) const;

		//- Number of triangles and quads after splitting
		//  Returns the sum of both
		label nTrianglesQuads
		(
			const pointField& points,
			label& nTris,
			label& nQuads
		) const;

		//- Split into triangles and quads.
		//  Results in triFaces (starting at triI) and quadFaces
		//  (starting at quadI).
		//  Returns number of new faces created.
		label trianglesQuads
		(
			const pointField& points,
			label& triI,
			label& quadI,
			faceList& triFaces,
			faceList& quadFaces
		) const;

		//- compare faces
		//   0: different
		//  +1: identical
		//  -1: same face, but different orientation
		static int compare(const face&, const face&);


		// Friend Operators

		friend bool operator==(const face& a, const face& b);
		friend bool operator!=(const face& a, const face& b);


		// Istream Operator

		friend Istream& operator>>(Istream&, face&);
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <faceI.hxx>

#ifdef NoRepository
#   include <faceTemplates.cxx>
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_face_Header