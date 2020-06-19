#pragma once
#ifndef _mutSpalartAllmsStdRoughWallFuncFvPatchScalarField_Header
#define _mutSpalartAllmsStdRoughWallFuncFvPatchScalarField_Header

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
	tnbLib::compressible::RASModels::
		mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField

Description
	Wall function boundary condition for rough walls

SourceFiles
	mutSpalartAllamarasStandardWallFunctionFvPatchScalarField.C

\*---------------------------------------------------------------------------*/

#include <mutWallFunctionFvPatchScalarField.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace compressible
	{
		namespace RASModels
		{

			/*---------------------------------------------------------------------------*\
			Class mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField Declaration
			\*---------------------------------------------------------------------------*/

			class mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField
				:
				public mutWallFunctionFvPatchScalarField
			{
			protected:

				// Protected data

					// Roughness parameters

						//- Height
				scalar roughnessHeight_;

				//- Constant
				scalar roughnessConstant_;

				//- Scale factor
				scalar roughnessFudgeFactor_;


				// Protected member functions

					//- Calculate yPLus
				virtual tmp<scalarField> calcYPlus(const scalarField& magUp) const;

				//- Calculate the turbulence viscosity
				virtual tmp<scalarField> calcMut() const;


			public:

				//- Runtime type information
				TypeName("mutSpalartAllmarasStandardRoughWallFunction");


				// Constructors

					//- Construct from patch and internal field
				mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField
				(
					const fvPatch&,
					const DimensionedField<scalar, volMesh>&
				);

				//- Construct from patch, internal field and dictionary
				mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField
				(
					const fvPatch&,
					const DimensionedField<scalar, volMesh>&,
					const dictionary&
				);

				//- Construct by mapping given
				//  mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField
				//  onto a new patch
				mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField
				(
					const mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField&,
					const fvPatch&,
					const DimensionedField<scalar, volMesh>&,
					const fvPatchFieldMapper&
				);

				//- Construct as copy
				mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField
				(
					const mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField&
				);

				//- Construct and return a clone
				virtual tmp<fvPatchScalarField> clone() const
				{
					return tmp<fvPatchScalarField>
						(
							new mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField
							(
								*this
							)
							);
				}

				//- Construct as copy setting internal field reference
				mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField
				(
					const mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField&,
					const DimensionedField<scalar, volMesh>&
				);

				//- Construct and return a clone setting internal field reference
				virtual tmp<fvPatchScalarField> clone
				(
					const DimensionedField<scalar, volMesh>& iF
				) const
				{
					return tmp<fvPatchScalarField>
						(
							new mutSpalartAllmarasStandardRoughWallFunctionFvPatchScalarField
							(
								*this,
								iF
							)
							);
				}


				// Member functions

					// Access

						//- Return the roughness height scale
				const scalar& roughnessHeight() const
				{
					return roughnessHeight_;
				}

				//- Return reference to the roughness height to allow adjustment
				scalar& roughnessHeight()
				{
					return roughnessHeight_;
				}

				//- Return the roughness constant scale
				const scalar& roughnessConstant() const
				{
					return roughnessConstant_;
				}

				//- Return reference to the roughness constant to allow adjustment
				scalar& roughnessConstant()
				{
					return roughnessConstant_;
				}

				//- Return the roughness scale factor
				const scalar& roughnessFudgeFactor() const
				{
					return roughnessFudgeFactor_;
				}

				//- Return reference to the roughness scale factor to allow
				//  adjustment
				scalar& roughnessFudgeFactor()
				{
					return roughnessFudgeFactor_;
				}


				// Evaluation functions

					//- Calculate and return the yPlus at the boundary
				virtual tmp<scalarField> yPlus() const;


				// I-O

					//- Write
				virtual void write(Ostream& os) const;
			};


			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		} // End namespace RASModels
	} // End namespace compressible
} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_mutSpalartAllmsStdRoughWallFuncFvPatchScalarField_Header