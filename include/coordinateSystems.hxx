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

Class
	tnbLib::coordinateSystems

Description
	Provides a centralized coordinateSystem collection.

Note
	Mixing normal constructors and the coordinateSystems::New constructor
	may yield unexpected results.

SourceFiles
	coordinateSystems.C

\*---------------------------------------------------------------------------*/
#ifndef _coordinateSystems_Header
#define _coordinateSystems_Header

#include <coordinateSystem.hxx>
#include <IOPtrList.hxx>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
						 Class coordinateSystems Declaration
	\*---------------------------------------------------------------------------*/

	class coordinateSystems
		:
		public IOPtrList<coordinateSystem>
	{

		// Private Member Functions

			//- Disallow default bitwise copy construct
		coordinateSystems(const coordinateSystems&);

		//- Disallow default bitwise assignment
		void operator=(const coordinateSystems&);

	public:

		//- Runtime type information
		TypeName("coordinateSystems");

		// Constructors

			//- Read construct from IOobject
		explicit coordinateSystems(const IOobject&);

		//- Construct from IOobject and a PtrList
		coordinateSystems
		(
			const IOobject&,
			const PtrList<coordinateSystem>&
		);

		//- Construct from IOobject and transferring the PtrList contents
		coordinateSystems
		(
			const IOobject&,
			const Xfer<PtrList<coordinateSystem> >&
		);

		// Selectors

			//- Return previously registered or read construct from "constant"
		static const coordinateSystems& New(const objectRegistry&);

		// Member Functions

			//- Find and return index for a given keyword, returns -1 if not found
		label find(const word& key) const;

		//- Search for given keyword
		bool found(const word& keyword) const;

		//- Return the table of contents (list of all keywords)
		wordList toc() const;

		//- write data
		bool writeData(Ostream&, bool subDict = true) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_coordinateSystems_Header