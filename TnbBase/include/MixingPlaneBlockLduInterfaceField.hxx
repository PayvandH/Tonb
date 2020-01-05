#pragma once
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

ClassMixing
	tnbLib::MixingPlaneBlockLduInterfaceField

Description
	Abstract base class for mixing plane coupled interfaces.

Author
	Hrvoje Jasak, Wikki Ltd

SourceFiles
	MixingPlaneBlockLduInterfaceField.C

\*---------------------------------------------------------------------------*/
#ifndef _MixingPlaneBlockLduInterfaceField_Header
#define _MixingPlaneBlockLduInterfaceField_Header

#include <primitiveFieldsFwd.hxx>
#include <typeInfo.hxx>
#include <Field.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
					   Class MixingPlaneBlockLduInterfaceField Declaration
	\*---------------------------------------------------------------------------*/

	template<class Type>
	class MixingPlaneBlockLduInterfaceField
	{
	public:

		//- Runtime type information
		TypeName("MixingPlaneBlockLduInterfaceField");


		// Constructors

			//- Construct given coupled patch
		MixingPlaneBlockLduInterfaceField()
		{}


		//- Destructor
		virtual ~MixingPlaneBlockLduInterfaceField();


		// Member Functions

			// Access

				//- Is the transform required
		virtual bool doTransform() const = 0;

		//- Return face transformation tensor
		virtual const tensorField& forwardT() const = 0;

		//- Return rank of component for transform
		virtual int rank() const = 0;


		//- Transform given patch component field
		void transformCoupleField
		(
			scalarField& f,
			const direction cmpt
		) const;

		//- Transform given patch field
		void transformCoupleField
		(
			Field<Type>& f
		) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include <MixingPlaneBlockLduInterfaceField.cxx>
#endif

// ************************************************************************* //

#endif // !_MixingPlaneBlockLduInterfaceField_Header
