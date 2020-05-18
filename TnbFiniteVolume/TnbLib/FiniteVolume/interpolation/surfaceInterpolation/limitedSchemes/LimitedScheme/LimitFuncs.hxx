#pragma once
#ifndef _LimitFuncs_Header
#define _LimitFuncs_Header

/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.0
	\\  /    A nd           | Web:         http://www.foam-extend.org
	 \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
	This file is part of foam-extend.

	foam-extend is free software: you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation, either version 3 of the License, or (at your
	option) any later version.

	foam-extend is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Namespace
	tnbLib::limitFuncs

Description
	Namespace for limiting functions


Class
	tnbLib::limitFuncs::LimitFuncs

Description
	Class to create NVD/TVD limited weighting-factors.

	The particular differencing scheme class is supplied as a template
	argument, the weight function of which is called by the weight function
	of this class for the internal faces as well as faces of coupled
	patches (e.g. processor-processor patches). The weight function is
	supplied the central-differencing weighting factor, the face-flux, the
	cell and face gradients (from which the normalised variable
	distribution may be created) and the cell centre distance.

	This code organisation is both neat and efficient, allowing for
	convenient implementation of new schemes to run on parallelised cases.

SourceFiles
	LimitFuncs.C

\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <tmp.hxx>
#include <volFieldsFwd.hxx>
//#include <GeometricField.hxx>

namespace tnbLib
{

	template<class Type, template<class> class PatchField, class GeoMesh>
	class GeometricField;

	namespace limitFuncs
	{

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		template<class Type>
		class null
		{
		public:

			null()
			{}

			inline tmp<GeometricField<Type, fvPatchField, volMesh> > operator()
				(
					const GeometricField<Type, fvPatchField, volMesh>& phi
					) const
			{
				return phi;
			}
		};


		template<class Type>
		class magSqr
		{
		public:

			magSqr()
			{}

			inline tmp<volScalarField> operator()
				(
					const GeometricField<Type, fvPatchField, volMesh>&
					) const;
		};


		template<class Type>
		class rhoMagSqr
		{
		public:

			rhoMagSqr()
			{}

			inline tmp<volScalarField> operator()
				(
					const GeometricField<Type, fvPatchField, volMesh>&
					) const;
		};


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace limitFuncs
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include <LimitFuncs.cxx>
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_LimitFuncs_Header