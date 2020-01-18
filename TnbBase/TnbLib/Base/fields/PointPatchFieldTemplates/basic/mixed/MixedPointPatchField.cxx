#include <MixedPointPatchField.hxx>

#include <Map.hxx>
#include <constraints.hxx>
#include <PointPatchFieldMapper.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		void
		MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::
		checkFieldSize() const
	{
		if
			(
				this->size() != this->patch().size()
				|| refValue_.size() != this->patch().size()
				|| valueFraction_.size() != this->patch().size()
				)
		{
			FatalErrorIn
			(
				"void MixedPointPatchField"
				"<PatchField, Mesh, PointPatch, MatrixType, "
				"Type>::checkField() const"
			) << "field does not correspond to patch. " << endl
				<< "Field size: " << this->size() << " value size: "
				<< refValue_.size()
				<< " valueFraction size: " << valueFraction_.size()
				<< " patch size: " << this->patch().size()
				<< abort(FatalError);
		}
	}


	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::
		MixedPointPatchField
		(
			const PointPatch& p,
			const DimensionedField<Type, Mesh>& iF
		)
		:
		ValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>(p, iF),
		refValue_(p.size()),
		valueFraction_(p.size())
	{
		this->checkPointField();
	}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::
		MixedPointPatchField
		(
			const PointPatch& p,
			const DimensionedField<Type, Mesh>& iF,
			const dictionary& dict
		)
		:
		ValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>(p, iF),
		refValue_("refValue", dict, p.size()),
		valueFraction_("valueFraction", dict, p.size())
	{
		updateBoundaryField();
	}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::
		MixedPointPatchField
		(
			const MixedPointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type>& ptf,
			const PointPatch& p,
			const DimensionedField<Type, Mesh>& iF,
			const PointPatchFieldMapper& mapper
		)
		:
		ValuePointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>
		(
			ptf,
			p,
			iF,
			mapper
			),
		refValue_(ptf.refValue_, mapper),
		valueFraction_(ptf.valueFraction_, mapper)

	{}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::
		MixedPointPatchField
		(
			const MixedPointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type>& ptf
		)
		:
		ValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>(ptf),
		refValue_(ptf.refValue_),
		valueFraction_(ptf.valueFraction_)
	{}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::
		MixedPointPatchField
		(
			const MixedPointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type>& ptf,
			const DimensionedField<Type, Mesh>& iF
		)
		:
		ValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>(ptf, iF),
		refValue_(ptf.refValue_),
		valueFraction_(ptf.valueFraction_)
	{}


	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	// Map and resize from self given a mapper
	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		void
		MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::autoMap
		(
			const PointPatchFieldMapper& m
		)
	{
		Field<Type>::autoMap(m);
		refValue_.autoMap(m);
		valueFraction_.autoMap(m);
	}


	// Grab the values using rmap
	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		void MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::rmap
		(
			const PointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>& ptf,
			const labelList& addr
		)
	{
		const MixedPointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type>& mptf =
			refCast<const MixedPointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type> >(ptf);

		Field<Type>::rmap(mptf, addr);
		refValue_.rmap(mptf.refValue_, addr);
		valueFraction_.rmap(mptf.valueFraction_, addr);
	}


	// Evaluate patch field
	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		void MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::
		updateBoundaryField()
	{
		Field<Type>& values = *this;

		tmp<Field<Type> > internalValues = this->patchInternalField();

		values = valueFraction_ * refValue_ + (1.0 - valueFraction_)*internalValues;
	}


	// Set boundary condition to matrix
	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		void MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::
		setBoundaryCondition
		(
			Map<typename MatrixType<Type>::ConstraintType>& fix
		) const
	{
		const Field<Type>& values = *this;

		// get addressing
		const labelList& meshPoints = this->patch().meshPoints();

		forAll(meshPoints, pointI)
		{
			const label curPoint = meshPoints[pointI];

			// create a constraint
			typename MatrixType<Type>::ConstraintType bc
			(
				curPoint,
				values[pointI],
				pTraits<Type>::one*valueFraction_[pointI]
			);

			// If not set add it, otherwise combine with
			// already existing value
			if (!fix.found(curPoint))
			{
				fix.insert(curPoint, bc);
			}
			else
			{
				fix[curPoint].combine(bc);
			}
		}
	}


	// Write
	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		void
		MixedPointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>::
		write
		(
			Ostream& os
		) const
	{
		PatchField<Type>::write(os);
		refValue_.writeEntry("refValue", os);
		valueFraction_.writeEntry("valueFraction", os);
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //