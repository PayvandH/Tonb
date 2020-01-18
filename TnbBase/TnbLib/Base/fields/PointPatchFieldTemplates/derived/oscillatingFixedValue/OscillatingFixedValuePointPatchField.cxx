#include <OscillatingFixedValuePointPatchField.hxx>

#include <transformField.hxx>
#include <error.hxx>

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
		scalar
		OscillatingFixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>::
		currentScale() const
	{
		return tnbLib::cos
		(
			2 * mathematicalConstant::pi*frequency_*
			this->db().time().value()
		);
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
		OscillatingFixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>::
		OscillatingFixedValuePointPatchField
		(
			const PointPatch& p,
			const DimensionedField<Type, Mesh>& iF
		)
		:
		FixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>(p, iF),
		refValue_(p.size()),
		amplitude_(p.size()),
		frequency_(0.0),
		curTimeIndex_(-1)
	{}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		OscillatingFixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>::
		OscillatingFixedValuePointPatchField
		(
			const PointPatch& p,
			const DimensionedField<Type, Mesh>& iF,
			const dictionary& dict
		)
		:
		FixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>(p, iF),
		refValue_("refValue", dict, p.size()),
		amplitude_("amplitude", dict, p.size()),
		frequency_(readScalar(dict.lookup("frequency"))),
		curTimeIndex_(-1)
	{
		if (dict.found("value"))
		{
			FixedValuePointPatchField
				<PatchField, Mesh, PointPatch, MatrixType, Type>::operator==
				(
					Field<Type>("value", dict, p.size())
					);
		}
		else
		{
			FixedValuePointPatchField
				<PatchField, Mesh, PointPatch, MatrixType, Type>::operator==
				(refValue_ + amplitude_ * currentScale());
		}
	}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		OscillatingFixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>::
		OscillatingFixedValuePointPatchField
		(
			const
			OscillatingFixedValuePointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type>& ptf,
			const PointPatch& p,
			const DimensionedField<Type, Mesh>& iF,
			const PointPatchFieldMapper& mapper
		)
		:
		FixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>(p, iF),
		refValue_(ptf.refValue_, mapper),
		amplitude_(ptf.amplitude_, mapper),
		frequency_(ptf.frequency_),
		curTimeIndex_(-1)
	{}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		OscillatingFixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>::
		OscillatingFixedValuePointPatchField
		(
			const OscillatingFixedValuePointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type>& ptf
		)
		:
		FixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>(ptf),
		refValue_(ptf.refValue_),
		amplitude_(ptf.amplitude_),
		frequency_(ptf.frequency_),
		curTimeIndex_(-1)
	{}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		OscillatingFixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>::
		OscillatingFixedValuePointPatchField
		(
			const OscillatingFixedValuePointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type>& ptf,
			const DimensionedField<Type, Mesh>& iF
		)
		:
		FixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>(ptf, iF),
		refValue_(ptf.refValue_),
		amplitude_(ptf.amplitude_),
		frequency_(ptf.frequency_),
		curTimeIndex_(-1)
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
		OscillatingFixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>::autoMap
		(
			const PointPatchFieldMapper& m
		)
	{
		Field<Type>::autoMap(m);
		refValue_.autoMap(m);
		amplitude_.autoMap(m);
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
		void
		OscillatingFixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>::rmap
		(
			const PointPatchField<PatchField, Mesh, PointPatch, MatrixType, Type>& ptf,
			const labelList& addr
		)
	{
		FixedValuePointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type>::rmap(ptf, addr);

		const OscillatingFixedValuePointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type>& tiptf =
			refCast
			<const OscillatingFixedValuePointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type> >(ptf);

		refValue_.rmap(tiptf.refValue_, addr);
		amplitude_.rmap(tiptf.amplitude_, addr);
	}


	template
		<
		template<class> class PatchField,
		class Mesh,
		class PointPatch,
		template<class> class MatrixType,
		class Type
		>
		void
		OscillatingFixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>::
		initEvaluate
		(
			const Pstream::commsTypes commsType
		)
	{
		if (curTimeIndex_ != this->db().time().timeIndex())
		{
			Field<Type>& patchField = *this;

			patchField = refValue_ + amplitude_ * currentScale();

			curTimeIndex_ = this->db().time().timeIndex();
		}

		FixedValuePointPatchField
			<PatchField, Mesh, PointPatch, MatrixType, Type>::
			initEvaluate(commsType);
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
		OscillatingFixedValuePointPatchField
		<PatchField, Mesh, PointPatch, MatrixType, Type>::write(Ostream& os) const
	{
		PatchField<Type>::write(os);
		this->writeEntry("value", os);
		refValue_.writeEntry("refValue", os);
		amplitude_.writeEntry("amplitude", os);
		os.writeKeyword("frequency")
			<< frequency_ << token::END_STATEMENT << nl;
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //