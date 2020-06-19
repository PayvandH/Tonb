#pragma once
#ifndef _realGasHThermo_Header
#define _realGasHThermo_Header

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
	tnbLib::realGasHThermo

Description
	Enthalpy for a real gas fluid libary


SourceFiles
	realGasHThermo.C

Author
Christian Lucas
Institut f�r Thermodynamik
Technische Universit�t Braunschweig
Germany


\*---------------------------------------------------------------------------*/

#include <basicPsiThermo.hxx>
#include <basicMixture.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							 Class realGasHThermo Declaration
	\*---------------------------------------------------------------------------*/

	template<class MixtureType>
	class realGasHThermo
		:
		public basicPsiThermo,
		public MixtureType
	{
		// Private data

			//- Enthalpy field
		volScalarField h_;

		//- DensityField
		volScalarField rho_;

		//- drhodh_Field
		volScalarField drhodh_;

		// Private member functions

			//- Calculate the thermo variables
		void calculate();

		//- Construct as copy (not implemented)
		realGasHThermo(const realGasHThermo<MixtureType>&);


	public:

		//- Runtime type information
		TypeName("realGasHThermo");


		// Constructors

			//- Construct from mesh
		realGasHThermo(const fvMesh&, const objectRegistry& obj);

		//- Destructor
		virtual ~realGasHThermo();


		// Member functions

			//- Return the compostion of the mixture
		virtual basicMixture& composition()
		{
			return *this;
		}

		//- Return the compostion of the mixture
		virtual const basicMixture& composition() const
		{
			return *this;
		}

		//- Update properties
		virtual void correct();


		// Access to thermodynamic state variables

			//- Enthalpy [J/kg]
			//  Non-const access allowed for transport equations
		virtual volScalarField& h()
		{
			return h_;
		}

		//- Enthalpy [J/kg]
		virtual const volScalarField& h() const
		{
			return h_;
		}

		//CL: drhodh needed for pressure equation of the real gas solver
		virtual const volScalarField& drhodh() const
		{
			return drhodh_;
		}

		// Fields derived from thermodynamic state variables

			//- Enthalpy for cell-set [J/kg]
		virtual tmp<scalarField> h
		(
			const scalarField& T,
			const labelList& cells
		) const;

		//- Enthalpy for patch [J/kg]
		virtual tmp<scalarField> h
		(
			const scalarField& T,
			const label patchi
		) const;

		//- Density for patch [J/kg]
		virtual tmp<scalarField> rho
		(
			const scalarField& T,
			const label patchi
		) const;

		//- Heat capacity at constant pressure for patch [J/kg/K]
		virtual tmp<scalarField> Cp
		(
			const scalarField& T,
			const label patchi
		) const;

		//- Heat capacity at constant pressure [J/kg/K]
		virtual tmp<volScalarField> Cp() const;

		//- Heat capacity at constant volume for patch [J/kg/K]
		virtual tmp<scalarField> Cv
		(
			const scalarField& T,
			const label patchi
		) const;

		//- Heat capacity at constant volume [J/kg/K]
		virtual tmp<volScalarField> Cv() const;

		//- Density [kg/m^3] - uses current value of pressure
		virtual tmp<volScalarField> rho() const;


		//- Read thermophysicalProperties dictionary
		virtual bool read();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifdef NoRepository
#   include <realGasHThermo.cxx>
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_realGasHThermo_Header