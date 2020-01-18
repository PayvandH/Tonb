#pragma once
#ifndef _DimensionedField_Header
#define _DimensionedField_Header

#include <regIOobject.hxx>
#include <Field.hxx>
#include <dimensionedType.hxx>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// Forward declaration of friend functions and operators

	template<class Type, class GeoMesh> class DimensionedField;

	template<class Type, class GeoMesh> Ostream& operator<<
		(
			Ostream&,
			const DimensionedField<Type, GeoMesh>&
			);

	template<class Type, class GeoMesh> Ostream& operator<<
		(
			Ostream&,
			const tmp<DimensionedField<Type, GeoMesh> >&
			);


	/*---------------------------------------------------------------------------*\
							   Class DimensionedField Declaration
	\*---------------------------------------------------------------------------*/

	template<class Type, class GeoMesh>
	class DimensionedField
		:
		public regIOobject,
		public Field<Type>
	{

	public:

		// Public typedefs

		typedef typename GeoMesh::Mesh Mesh;
		typedef typename Field<Type>::cmptType cmptType;


	private:

		// Private data

			//- Reference to mesh
		const Mesh& mesh_;

		//- Dimension set for this field
		dimensionSet dimensions_;


	public:

		//- Runtime type information
		TypeName("DimensionedField");


		// Static Member Functions

			//- Return a null DimensionedField
			//  Dangerous, but no null constructor.  Reconsider.  HJ, 10/Apr/2013
		inline static const DimensionedField<Type, GeoMesh>& null();


		// Constructors

			//- Construct from components
		DimensionedField
		(
			const IOobject&,
			const Mesh& mesh,
			const dimensionSet&,
			const Field<Type>&
		);

		//- Construct from components
		//  Used for temporary fields which are initialised after construction
		DimensionedField
		(
			const IOobject&,
			const Mesh& mesh,
			const dimensionSet&
		);

		//- Construct from components
		DimensionedField
		(
			const IOobject&,
			const Mesh& mesh,
			const dimensioned<Type>&
		);

		//- Construct from Istream
		DimensionedField
		(
			const IOobject&,
			const Mesh& mesh,
			const word& fieldDictEntry = "value"
		);

		void readField
		(
			const dictionary& fieldDict,
			const word& fieldDictEntry = "value"
		);

		//- Construct as copy
		DimensionedField
		(
			const DimensionedField<Type, GeoMesh>&
		);

		//- Construct as copy or re-use as specified.
		DimensionedField
		(
			DimensionedField<Type, GeoMesh>&,
			bool reUse
		);

		//- Construct by transferring the DimensionedField

#ifndef SWIG
		DimensionedField
		(
			const Xfer<DimensionedField<Type, GeoMesh> >&
		);
#endif

		//- Construct as copy of tmp<DimensionedField> deleting argument
#       ifdef ConstructFromTmp
		DimensionedField
		(
			const tmp<DimensionedField<Type, GeoMesh> >&
		);
#       endif

		//- Construct as copy resetting IO parameters
		DimensionedField
		(
			const IOobject&,
			const DimensionedField<Type, GeoMesh>&
		);

		//- Construct as copy resetting name
		DimensionedField
		(
			const word& newName,
			const DimensionedField<Type, GeoMesh>&
		);

		//- Construct as copy resetting name and re-use as specified.
		DimensionedField
		(
			const word& newName,
			DimensionedField<Type, GeoMesh>&,
			bool reUse
		);

		//- Construct by transferring the DimensionedField with a new name
#ifndef SWIG
		DimensionedField
		(
			const word& newName,
			const Xfer<DimensionedField<Type, GeoMesh> >&
		);
#endif

		//- Construct as copy resetting name
#       ifdef ConstructFromTmp
		DimensionedField
		(
			const word& newName,
			const tmp<DimensionedField<Type, GeoMesh> >&
		);
#       endif

		//- Clone
		tmp<DimensionedField<Type, GeoMesh> > clone() const;


		//- Destructor
		virtual ~DimensionedField();


		// Member Functions

			//- Return mesh
		inline const Mesh& mesh() const;

		//- Return dimensions
		inline const dimensionSet& dimensions() const;

		//- Return non-const access to dimensions
		inline dimensionSet& dimensions();

		inline const Field<Type>& field() const;

		inline Field<Type>& field();

		//- Return a component field of the field
		tmp<DimensionedField<cmptType, GeoMesh> > component
		(
			const direction
		) const;

		//- Replace a component field of the field
		void replace
		(
			const direction,
			const DimensionedField<cmptType, GeoMesh>&
		);

		//- Replace a component field of the field
		void replace
		(
			const direction,
			const tmp<DimensionedField<cmptType, GeoMesh> >&
		);

		//- Return the field transpose (only defined for second rank tensors)
		tmp<DimensionedField<Type, GeoMesh> > T() const;

		//- Calculate and return arithmetic average
		dimensioned<Type> average() const;

		//- Calculate and return weighted average
		dimensioned<Type> weightedAverage
		(
			const DimensionedField<scalar, GeoMesh>&
		) const;

		//- Calculate and return weighted average
		dimensioned<Type> weightedAverage
		(
			const tmp<DimensionedField<scalar, GeoMesh> >&
		) const;


		// Write

		bool writeData(Ostream&, const word& fieldDictEntry) const;

		bool writeData(Ostream&) const;


		// Member Operators

		void operator=(const DimensionedField<Type, GeoMesh>&);
		void operator=(const tmp<DimensionedField<Type, GeoMesh> >&);
		void operator=(const dimensioned<Type>&);

		void operator+=(const DimensionedField<Type, GeoMesh>&);
		void operator+=(const tmp<DimensionedField<Type, GeoMesh> >&);

		void operator-=(const DimensionedField<Type, GeoMesh>&);
		void operator-=(const tmp<DimensionedField<Type, GeoMesh> >&);

		void operator*=(const DimensionedField<scalar, GeoMesh>&);
		void operator*=(const tmp<DimensionedField<scalar, GeoMesh> >&);

		void operator/=(const DimensionedField<scalar, GeoMesh>&);
		void operator/=(const tmp<DimensionedField<scalar, GeoMesh> >&);

		void operator+=(const dimensioned<Type>&);
		void operator-=(const dimensioned<Type>&);

		void operator*=(const dimensioned<scalar>&);
		void operator/=(const dimensioned<scalar>&);


		// Ostream Operators

#ifndef SWIG
		friend Ostream& operator<< <Type, GeoMesh>
			(
				Ostream&,
				const DimensionedField<Type, GeoMesh>&
				);

		friend Ostream& operator<< <Type, GeoMesh>
			(
				Ostream&,
				const tmp<DimensionedField<Type, GeoMesh> >&
				);
#endif
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include <DimensionedFieldI.hxx>
#include <DimensionedFieldFunctions.hxx>

#ifdef NoRepository
#   include <DimensionedField.cxx>
#endif

#endif // !_DimensionedField_Header