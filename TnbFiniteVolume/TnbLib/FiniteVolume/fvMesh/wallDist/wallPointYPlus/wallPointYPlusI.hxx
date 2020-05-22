#pragma once
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


	// Update this with w2 if w2 nearer to pt.
	inline bool wallPointYPlus::update
	(
		const point& pt,
		const wallPointYPlus& w2,
		const scalar tol
	)
	{
		scalar dist2 = magSqr(pt - w2.origin());

		scalar diff = distSqr() - dist2;

		if (diff < 0)
		{
			// already nearer to pt
			return false;
		}

		if ((diff < SMALL) || ((distSqr() > SMALL) && (diff / distSqr() < tol)))
		{
			// don't propagate small changes
			return false;
		}
		else
		{
			// only propagate if interesting (i.e. y+ < 100)
			scalar yPlus = tnbLib::sqrt(dist2) / w2.data();


			if (yPlus < yPlusCutOff)
			{
				// update with new values
				distSqr() = dist2;
				origin() = w2.origin();
				data() = w2.data();

				return true;
			}
			else
			{
				return false;
			}
		}
	}


	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	// Null constructor
	inline wallPointYPlus::wallPointYPlus()
		:
		wallPointData<scalar>()
	{
		// Important: value of yStar where meshWave does not come.
		data() = 1.0;
	}


	// Construct from components
	inline wallPointYPlus::wallPointYPlus
	(
		const point& origin,
		const scalar yStar,
		const scalar distSqr
	)
		:
		wallPointData<scalar>(origin, yStar, distSqr)
	{}


	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	// Update this with w2 if w2 nearer to pt.
	inline bool wallPointYPlus::updateCell
	(
		const polyMesh& mesh,
		const label thisCellI,
		const label neighbourFaceI,
		const wallPointYPlus& neighbourWallInfo,
		const scalar tol
	)
	{
		const vectorField& cellCentres = mesh.primitiveMesh::cellCentres();

		return update
		(
			cellCentres[thisCellI],
			neighbourWallInfo,
			tol
		);
	}


	// Update this with w2 if w2 nearer to pt.
	inline bool wallPointYPlus::updateFace
	(
		const polyMesh& mesh,
		const label thisFaceI,
		const label neighbourCellI,
		const wallPointYPlus& neighbourWallInfo,
		const scalar tol
	)
	{
		const vectorField& faceCentres = mesh.faceCentres();

		return update
		(
			faceCentres[thisFaceI],
			neighbourWallInfo,
			tol
		);
	}


	// Update this with w2 if w2 nearer to pt.
	inline bool wallPointYPlus::updateFace
	(
		const polyMesh& mesh,
		const label thisFaceI,
		const wallPointYPlus& neighbourWallInfo,
		const scalar tol
	)
	{
		const vectorField& faceCentres = mesh.faceCentres();

		return update
		(
			faceCentres[thisFaceI],
			neighbourWallInfo,
			tol
		);
	}


	// ************************************************************************* //

} // End namespace tnbLib

// ************************************************************************* //