#include <WedgePointPatchField.hxx>

#include <transformField.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		class WedgePointPatch,
		template<class> class MatrixType,
		class Type
		>
		WedgePointPatchField
		<PatchField, Mesh, PointPatch, WedgePointPatch, MatrixType, Type>::
		WedgePointPatchField
		(
			const PointPatch& p,
			const DimensionedField<Type, Mesh>& iF
		)
		:
		PatchField<Type>(p, iF)
	{}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		class WedgePointPatch,
		template<class> class MatrixType,
		class Type
		>
		WedgePointPatchField
		<PatchField, Mesh, PointPatch, WedgePointPatch, MatrixType, Type>::
		WedgePointPatchField
		(
			const PointPatch& p,
			const DimensionedField<Type, Mesh>& iF,
			const dictionary& dict
		)
		:
		PatchField<Type>(p, iF)
	{
		if (!isType<WedgePointPatch>(p))
		{
			FatalIOErrorIn
			(
				"WedgePointPatchField"
				"<PatchField, Mesh, PointPatch, WedgePointPatch, "
				"MatrixType, Type>::WedgePointPatchField\n"
				"(\n"
				"    const PointPatch& p,\n"
				"    const DimensionedField<Type, Mesh>& field,\n"
				"    const dictionary& dict\n"
				")\n",
				dict
			) << "patch " << this->patch().index() << " not wedge type. "
				<< "Patch type = " << p.type()
				<< exit(FatalIOError);
		}
	}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		class WedgePointPatch,
		template<class> class MatrixType,
		class Type
		>
		WedgePointPatchField
		<PatchField, Mesh, PointPatch, WedgePointPatch, MatrixType, Type>::
		WedgePointPatchField
		(
			const WedgePointPatchField
			<PatchField, Mesh, PointPatch, WedgePointPatch, MatrixType, Type>&,
			const PointPatch& p,
			const DimensionedField<Type, Mesh>& iF,
			const PointPatchFieldMapper&
		)
		:
		PatchField<Type>(p, iF)
	{
		if (!isType<WedgePointPatch>(this->patch()))
		{
			FatalErrorIn
			(
				"WedgePointPatchField"
				"<PatchField, Mesh, PointPatch, WedgePointPatch, "
				"MatrixType, Type>::WedgePointPatchField\n"
				"(\n"
				"    const WedgePointPatchField"
				"    <PatchField, Mesh, PointPatch, WedgePointPatch, "
				"MatrixType, Type>&,\n"
				"    const PointPatch& p,\n"
				"    const DimensionedField<Type, Mesh>& iF,\n"
				"    const PointPatchFieldMapper& mapper\n"
				")\n"
			) << "Field type does not correspond to patch type for patch "
				<< this->patch().index() << "." << endl
				<< "Field type: " << typeName << endl
				<< "Patch type: " << this->patch().type()
				<< exit(FatalError);
		}
	}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		class WedgePointPatch,
		template<class> class MatrixType,
		class Type
		>
		WedgePointPatchField
		<PatchField, Mesh, PointPatch, WedgePointPatch, MatrixType, Type>::
		WedgePointPatchField
		(
			const WedgePointPatchField
			<PatchField, Mesh, PointPatch, WedgePointPatch, MatrixType, Type>& ptf
		)
		:
		PatchField<Type>(ptf)
	{}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		class WedgePointPatch,
		template<class> class MatrixType,
		class Type
		>
		WedgePointPatchField
		<PatchField, Mesh, PointPatch, WedgePointPatch, MatrixType, Type>::
		WedgePointPatchField
		(
			const WedgePointPatchField
			<PatchField, Mesh, PointPatch, WedgePointPatch, MatrixType, Type>& ptf,
			const DimensionedField<Type, Mesh>& iF
		)
		:
		PatchField<Type>(ptf, iF)
	{}


	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	// Evaluate patch field
	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		class WedgePointPatch,
		template<class> class MatrixType,
		class Type
		>
		void
		WedgePointPatchField
		<PatchField, Mesh, PointPatch, WedgePointPatch, MatrixType, Type>::evaluate
		(
			const Pstream::commsTypes commsType
		)
	{
		// In order to ensure that the wedge patch is always flat, take the
		// normal vector from the first point
		const vector& nHat = this->patch().pointNormals()[0];

		tmp<Field<Type> > tvalues =
			transform(I - nHat * nHat, this->patchInternalField());
		const Field<Type>& values = tvalues();

		// Get internal field to insert values into
		Field<Type>& iF = const_cast<Field<Type>&>(this->internalField());

		// Get addressing
		const labelList& meshPoints = this->patch().meshPoints();

		forAll(meshPoints, pointI)
		{
			iF[meshPoints[pointI]] = values[pointI];
		}
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //