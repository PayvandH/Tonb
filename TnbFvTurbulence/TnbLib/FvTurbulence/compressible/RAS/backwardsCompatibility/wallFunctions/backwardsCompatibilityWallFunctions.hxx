#pragma once
#ifndef _backwardsCompatibilityWallFunctions_Header
#define _backwardsCompatibilityWallFunctions_Header

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

Class
	tnbLib::compressible

Description
	Auto creation of fields to provide backwards compatibility with
	runtime selectable wall functions

SourceFiles
	backwardsCompatibilityWallFunctions.C
	backwardsCompatibilityWallFunctionsTemplates.C

\*---------------------------------------------------------------------------*/

#include <fvMesh.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace compressible
	{
		//- mut
		tmp<volScalarField> autoCreateMut
		(
			const word& fieldName,
			const fvMesh& mesh,
			const objectRegistry& obj
		);

		tmp<volScalarField> autoCreateMut
		(
			const word& fieldName,
			const fvMesh& mesh
		);

		//- mut for Low-Reynolds number models
		tmp<volScalarField> autoCreateLowReMut
		(
			const word& fieldName,
			const fvMesh& mesh,
			const objectRegistry& obj
		);

		tmp<volScalarField> autoCreateLowReMut
		(
			const word& fieldName,
			const fvMesh& mesh
		);

		//- alphat
		tmp<volScalarField> autoCreateAlphat
		(
			const word& fieldName,
			const fvMesh& mesh,
			const objectRegistry& obj
		);

		tmp<volScalarField> autoCreateAlphat
		(
			const word& fieldName,
			const fvMesh& mesh
		);

		//- epsilon
		tmp<volScalarField> autoCreateEpsilon
		(
			const word& fieldName,
			const fvMesh& mesh,
			const objectRegistry& obj
		);

		tmp<volScalarField> autoCreateEpsilon
		(
			const word& fieldName,
			const fvMesh& mesh
		);

		//- omega
		tmp<volScalarField> autoCreateOmega
		(
			const word& fieldName,
			const fvMesh& mesh,
			const objectRegistry& obj
		);

		tmp<volScalarField> autoCreateOmega
		(
			const word& fieldName,
			const fvMesh& mesh
		);

		//- k
		tmp<volScalarField> autoCreateK
		(
			const word& fieldName,
			const fvMesh& mesh,
			const objectRegistry& obj
		);

		tmp<volScalarField> autoCreateK
		(
			const word& fieldName,
			const fvMesh& mesh
		);

		//- Q
		tmp<volScalarField> autoCreateQ
		(
			const word& fieldName,
			const fvMesh& mesh,
			const objectRegistry& obj
		);

		tmp<volScalarField> autoCreateQ
		(
			const word& fieldName,
			const fvMesh& mesh
		);

		//- R
		tmp<volSymmTensorField> autoCreateR
		(
			const word& fieldName,
			const fvMesh& mesh,
			const objectRegistry& obj
		);

		tmp<volSymmTensorField> autoCreateR
		(
			const word& fieldName,
			const fvMesh& mesh
		);

		//- Helper function to create the new field
		template<class Type, class PatchType>
		tmp<GeometricField<Type, fvPatchField, volMesh> >
			autoCreateWallFunctionField
			(
				const word& fieldName,
				const fvMesh& mesh,
				const objectRegistry& obj
			);


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace compressible
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#include <backwardsCompatibilityWallFunctionsTemplates.cxx>
#endif // NoRepository

#endif // !_backwardsCompatibilityWallFunctions_Header