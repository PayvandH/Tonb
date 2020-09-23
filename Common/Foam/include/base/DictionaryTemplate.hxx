#pragma once
#ifndef _DictionaryTemplate_Header
#define _DictionaryTemplate_Header

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
	tnbLib::Dictionary

Description
	Gerneral purpose template dictionary class which manages the storage
	associated with it.

	It is derived from DictionaryBase instantiated on a memory managed form
	of intrusive doubly-linked list of \<T\>.

SourceFiles
	Dictionary.C

\*---------------------------------------------------------------------------*/

#include <DictionaryBase.hxx>
#include <IDLList.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
							 Class Dictionary Declaration
	\*---------------------------------------------------------------------------*/

	template<class T>
	class Dictionary
		:
		public DictionaryBase<IDLList<T>, T>
	{

	public:

		// Constructors

			//- Construct given initial table size
		Dictionary(const label size = 128);

		//- Copy constructor
		Dictionary(const Dictionary&);

		//- Move constructor
		Dictionary(Dictionary&&);


		// Member Functions

			//- Remove an entry specified by keyword and delete the pointer.
			//  Returns true if the keyword was found
		bool erase(const word& keyword);
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <DictionaryTemplateI.hxx>

//#ifdef NoRepository
//#include <DictionaryTemplate.cxx>
//#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // !_DictionaryTemplate_Header