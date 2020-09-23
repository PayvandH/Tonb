#pragma once
#ifndef _wedgePointPatch_Header
#define _wedgePointPatch_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
	\\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
	 \\/     M anipulation  |
-------------------------------------------------------------------------------
License
	This file is part of OpenFOAM.

	OpenFOAM is free software: you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
	for more details.

	You should have received a copy of the GNU General Public License
	along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
	tnbLib::wedgePointPatch

Description
	Wedge front and back plane patch.

SourceFiles
	wedgePointPatch.C

\*---------------------------------------------------------------------------*/

#include <facePointPatch.hxx>
#include <wedgePolyPatch.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
						   Class wedgePointPatch Declaration
	\*---------------------------------------------------------------------------*/

	class wedgePointPatch
		:
		public facePointPatch
	{
		// Private Data

			//- Local reference cast into the symmetryPlane patch
		const wedgePolyPatch& wedgePolyPatch_;


	public:

		//- Runtime type information
		TypeName(wedgePolyPatch::typeName_());


		// Constructors

			//- Construct from polyPatch
		wedgePointPatch
		(
			const polyPatch& patch,
			const pointBoundaryMesh& bm
		);


		// Member Functions

			//- Return the constraint type this pointPatch implements.
		virtual const word& constraintType() const
		{
			return type();
		}

		//- Accumulate the effect of constraint direction of this patch
		virtual void applyConstraint
		(
			const label pointi,
			pointConstraint&
		) const;

		//- Return symmetry plane normal
		const vector& n() const
		{
			return wedgePolyPatch_.n();
		}
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_wedgePointPatch_Header
