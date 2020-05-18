#pragma once
#ifndef _gaussConvectionScheme_Header
#define _gaussConvectionScheme_Header

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
	tnbLib::fv::gaussConvectionScheme

Description
	Basic second-order convection using face-gradients and Gauss' theorem.

SourceFiles
	gaussConvectionScheme.C

\*---------------------------------------------------------------------------*/

#include <convectionScheme.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	namespace fv
	{

		/*---------------------------------------------------------------------------*\
							   Class gaussConvectionScheme Declaration
		\*---------------------------------------------------------------------------*/

		template<class Type>
		class gaussConvectionScheme
			:
			public fv::convectionScheme<Type>
		{
			// Private data

			tmp<surfaceInterpolationScheme<Type> > tinterpScheme_;


			// Private Member Functions

				//- Disallow default bitwise copy construct
			gaussConvectionScheme(const gaussConvectionScheme&);

			//- Disallow default bitwise assignment
			void operator=(const gaussConvectionScheme&);


		public:

			//- Runtime type information
			TypeName("Gauss");


			// Constructors

				//- Construct from flux and interpolation scheme
			gaussConvectionScheme
			(
				const fvMesh& mesh,
				const surfaceScalarField& faceFlux,
				const tmp<surfaceInterpolationScheme<Type> >& scheme
			)
				:
				convectionScheme<Type>(mesh, faceFlux),
				tinterpScheme_(scheme)
			{}

			//- Construct from flux and Istream
			gaussConvectionScheme
			(
				const fvMesh& mesh,
				const surfaceScalarField& faceFlux,
				Istream& is
			)
				:
				convectionScheme<Type>(mesh, faceFlux),
				tinterpScheme_
				(
					surfaceInterpolationScheme<Type>::New(mesh, faceFlux, is)
				)
			{}


			// Member Functions

			tmp<GeometricField<Type, fvsPatchField, surfaceMesh> > interpolate
			(
				const surfaceScalarField&,
				const GeometricField<Type, fvPatchField, volMesh>&
			) const;

			tmp<GeometricField<Type, fvsPatchField, surfaceMesh> > flux
			(
				const surfaceScalarField&,
				const GeometricField<Type, fvPatchField, volMesh>&
			) const;

			tmp<fvMatrix<Type> > fvmDiv
			(
				const surfaceScalarField&,
				const GeometricField<Type, fvPatchField, volMesh>&
			) const;

			tmp<GeometricField<Type, fvPatchField, volMesh> > fvcDiv
			(
				const surfaceScalarField&,
				const GeometricField<Type, fvPatchField, volMesh>&
			) const;
		};


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace fv

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include <gaussConvectionScheme.cxx>
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_gaussConvectionScheme_Header