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
	tnbLib::dictionaryEntry

Description
	A keyword and a list of tokens is a 'dictionaryEntry'.

	A dictionaryEntry can be read, written and printed, and the types and
	values of its tokens analysed. A dictionaryEntry is a high-level building
	block for data description. It is a front-end for the token parser.
	A list of entries can be used as a set of keyword syntax elements,
	for example.

SourceFiles
	dictionaryEntry.C
	dictionaryEntryIO.C

\*---------------------------------------------------------------------------*/
#ifndef _dictionaryEntry_Header
#define _dictionaryEntry_Header

#include <entry.hxx>
#include <dictionary.hxx>
#include <InfoProxy.hxx>

namespace tnbLib
{

	/*---------------------------------------------------------------------------*\
						   Class dictionaryEntry Declaration
\*---------------------------------------------------------------------------*/

	class dictionaryEntry
		: public entry
		, public dictionary
	{
		// Private member functions

		void readData(Istream&);

		//- Dissallow bitwise copy
		dictionaryEntry(const dictionary&);


	public:

		// Constructors

			//- Construct from the parent dictionary and Istream
		dictionaryEntry(const dictionary& parentDict, Istream&);

		//- Construct from the keyword, parent dictionary and a Istream
		dictionaryEntry
		(
			const keyType&,
			const dictionary& parentDict,
			Istream&
		);

		//- Construct from the keyword, parent dictionary and a dictionary
		dictionaryEntry
		(
			const keyType&,
			const dictionary& parentDict,
			const dictionary& dict
		);

		//- Construct as copy for the given parentDict
		dictionaryEntry
		(
			const dictionary& parentDict,
			const dictionaryEntry&
		);

		autoPtr<entry> clone(const dictionary& parentDict) const
		{
			return autoPtr<entry>(new dictionaryEntry(parentDict, *this));
		}


		// Member functions

			//- Return the dictionary name
		const fileName& name() const
		{
			return dictionary::name();
		}

		//- Return the dictionary name
		fileName& name()
		{
			return dictionary::name();
		}

		//- Return line number of first token in dictionary
		label startLineNumber() const;

		//- Return line number of last token in dictionary
		label endLineNumber() const;

		//- This entry is not a primitive,
		//  calling this function generates a FatalError
		ITstream& stream() const;

		//- Return true because this entry is a dictionary
		bool isDict() const
		{
			return true;
		}

		//- Return dictionary
		const dictionary& dict() const;

		//- Return non-const access to dictionary
		dictionary& dict();

		// Write
		void write(Ostream&) const;

		//- Return info proxy.
		//  Used to print token information to a stream
		InfoProxy<dictionaryEntry> info() const
		{
			return *this;
		}


		// Ostream operator

		friend Ostream& operator<<(Ostream&, const dictionaryEntry&);
	};


	template<>
	Ostream& operator<<(Ostream&, const InfoProxy<dictionaryEntry>&);


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
}

#endif // !_dictionaryEntry_Header
