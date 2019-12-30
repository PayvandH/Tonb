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
	tnbLib::wordRe

Description
	A wordRe is a word, but can also have a regular expression for matching
	words.

	By default the constructors will generally preserve the argument as
	string literal and the assignment operators will use the wordRe::DETECT
	compOption to scan the string for regular expression meta characters
	and/or invalid word characters and react accordingly.

	The exceptions are when constructing/assigning from another
	tnbLib::wordRe (preserve the same type) or from a tnbLib::word (always
	literal).

Note
	If the string contents are changed - eg, by the operator+=() or by
	string::replace(), etc - it will be necessary to use compile() or
	recompile() to synchronize the regular expression.

SourceFiles
	wordRe.C
	wordReIO.C

\*---------------------------------------------------------------------------*/
#ifndef _wordRe_Header
#define _wordRe_Header

#include <word.hxx>
#include <regExp.hxx>

namespace tnbLib
{

	// Forward declaration of friend functions and operators
	class wordRe;

	class Istream;
	class Ostream;

	Istream& operator>>(Istream&, wordRe&);
	Ostream& operator<<(Ostream&, const wordRe&);


	/*---------------------------------------------------------------------------*\
							   Class wordRe Declaration
	\*---------------------------------------------------------------------------*/

	class wordRe
		:
		public word
	{
		// Private member data

			//- The regular expression
		mutable regExp re_;

	public:

		// Public data types

			//- Enumeration with compile options
			//  Note that 'REGEXP' is implicit if 'NOCASE' is specified alone.
		enum compOption
		{
			LITERAL = 0, /*!< treat as a string literal */
			DETECT = 1, /*!< treat as regular expression */
			REGEXP = 2, /*!< detect if the string contains meta-characters */
			NOCASE = 4, /*!< ignore case in regular expression */
			DETECT_NOCASE = DETECT | NOCASE,
			REGEXP_NOCASE = REGEXP | NOCASE
		};


		//- Is this a meta character?
		static bool meta(char);

		//- Test string for regular expression meta characters
		static bool isPattern(const string&);

		// Constructors

			//- Construct null
		wordRe();

		//- Construct as copy
		wordRe(const wordRe&);

		//- Construct as copy of word
		wordRe(const word&);

		//- Construct as copy of character array
		//  Optionally specify how it should be treated.
		wordRe(const char*, const compOption = LITERAL);

		//- Construct as copy of string.
		//  Optionally specify how it should be treated.
		wordRe(const string&, const compOption = LITERAL);

		//- Construct as copy of std::string
		//  Optionally specify how it should be treated.
		wordRe(const std::string&, const compOption = LITERAL);

		//- Construct from Istream
		//  Words are treated as literals, strings with an auto-test
		wordRe(Istream&);

		// Member functions

		//- Access

			//- Should be treated as a match rather than a literal string?
		bool isPattern() const;

		//- Infrastructure

			//- Compile the regular expression
		bool compile() const;

		//- Possibly compile the regular expression, with greater control
		bool compile(const compOption) const;

		//- Recompile an existing regular expression
		bool recompile() const;

		//- Frees precompiled regular expression, making wordRe a literal.
		//  Optionally strips invalid word characters
		void uncompile(const bool doStripInvalid = false) const;

		//- Editing

			//- Copy string, auto-test for regular expression or other options
		void set(const std::string&, const compOption = DETECT);

		//- Copy string, auto-test for regular expression or other options
		void set(const char*, const compOption = DETECT);

		//- Clear string and precompiled regular expression
		void clear();

		//- Searching

			//- Smart match as regular expression or as a string
			//  Optionally specify a literal match only
		bool match(const string&, bool literalMatch = false) const;

		//- Miscellaneous

			//- Return a string with quoted meta-characters
		string quotemeta() const;

		//- Output some basic info
		Ostream& info(Ostream&) const;


		// Member operators

			// Assignment

				//- Assign copy
				//  Always case sensitive
		const wordRe& operator=(const wordRe&);

		//- Copy word, never a regular expression
		const wordRe& operator=(const word&);

		//- Copy string, auto-test for regular expression
		//  Always case sensitive
		const wordRe& operator=(const string&);

		//- Copy string, auto-test for regular expression
		//  Always case sensitive
		const wordRe& operator=(const std::string&);

		//- Copy string, auto-test for regular expression
		//  Always case sensitive
		const wordRe& operator=(const char*);


		// IOstream operators

		friend Istream& operator>>(Istream&, wordRe&);
		friend Ostream& operator<<(Ostream&, const wordRe&);
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
}

#include <wordReI.hxx>

#endif // !_wordRe_Header
