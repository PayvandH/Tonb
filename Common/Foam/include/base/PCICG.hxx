#pragma once
#ifndef _PCICG_Header
#define _PCICG_Header

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
	tnbLib::PCICG

Description
	Preconditioned conjugate gradient solver for symmetric lduMatrices
	using a run-time selectable preconditioner.

SourceFiles
	PCICG.C

\*---------------------------------------------------------------------------*/

#include <LduMatrixTemplate.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							   Class PCICG Declaration
	\*---------------------------------------------------------------------------*/

	template<class Type, class DType, class LUType>
	class PCICG
		:
		public LduMatrix<Type, DType, LUType>::solver
	{

	public:

		//- Runtime type information
		TypeName("PCICG");


		// Constructors

			//- Construct from matrix components and solver data dictionary
		PCICG
		(
			const word& fieldName,
			const LduMatrix<Type, DType, LUType>& matrix,
			const dictionary& solverDict
		);

		//- Disallow default bitwise copy construction
		PCICG(const PCICG&) = delete;


		// Destructor

		virtual ~PCICG()
		{}


		// Member Functions

			//- Solve the matrix with this solver
		virtual SolverPerformance<Type> solve(Field<Type>& psi) const;


		// Member Operators

			//- Disallow default bitwise assignment
		void operator=(const PCICG&) = delete;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <PCICGI.hxx>

//#ifdef NoRepository
//#include <PCICG.cxx>
//#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_PCICG_Header
