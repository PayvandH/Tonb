#pragma once
#ifndef _NoPreconditionerTemplate_Header
#define _NoPreconditionerTemplate_Header

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
	tnbLib::NoPreconditioner

Description
	Null preconditioner for both symmetric and asymmetric matrices.

SourceFiles
	NoPreconditioner.C

\*---------------------------------------------------------------------------*/

#include <LduMatrixTemplate.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							   Class NoPreconditioner Declaration
	\*---------------------------------------------------------------------------*/

	template<class Type, class DType, class LUType>
	class NoPreconditioner
		:
		public LduMatrix<Type, DType, LUType>::preconditioner
	{

	public:

		//- Runtime type information
		TypeName("none");


		// Constructors

			//- Construct from matrix components and preconditioner data dictionary
		NoPreconditioner
		(
			const typename LduMatrix<Type, DType, LUType>::solver& sol,
			const dictionary& preconditionerDict
		);

		//- Disallow default bitwise copy construction
		NoPreconditioner(const NoPreconditioner&) = delete;


		// Destructor

		virtual ~NoPreconditioner()
		{}


		// Member Functions

			//- Read and reset the preconditioner parameters from the given
			//  dictionary
		virtual void read(const dictionary& preconditionerDict);

		//- Return wA the preconditioned form of residual rA
		virtual void precondition
		(
			Field<Type>& wA,
			const Field<Type>& rA
		) const;

		//- Return wT the transpose-matrix preconditioned form of
		//  residual rT.
		virtual void preconditionT
		(
			Field<Type>& wT,
			const Field<Type>& rT
		) const
		{
			return(precondition(wT, rT));
		}


		// Member Operators

			//- Disallow default bitwise assignment
		void operator=(const NoPreconditioner&) = delete;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <NoPreconditionerTemplateI.hxx>

//#ifdef NoRepository
//#include <NoPreconditionerTemplate.cxx>
//#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_NoPreconditionerTemplate_Header