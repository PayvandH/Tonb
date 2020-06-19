#include <pressureDirectedInletVelocityFvPatchVectorField.hxx>

#include <addToRunTimeSelectionTable.hxx>
#include <fvPatchFieldMapper.hxx>
#include <volFields.hxx>
#include <surfaceFields.hxx>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	pressureDirectedInletVelocityFvPatchVectorField::
		pressureDirectedInletVelocityFvPatchVectorField
		(
			const fvPatch& p,
			const DimensionedField<vector, volMesh>& iF
		)
		:
		fixedValueFvPatchVectorField(p, iF),
		phiName_("phi"),
		rhoName_("rho"),
		inletDir_(p.size())
	{}


	pressureDirectedInletVelocityFvPatchVectorField::
		pressureDirectedInletVelocityFvPatchVectorField
		(
			const pressureDirectedInletVelocityFvPatchVectorField& ptf,
			const fvPatch& p,
			const DimensionedField<vector, volMesh>& iF,
			const fvPatchFieldMapper& mapper
		)
		:
		fixedValueFvPatchVectorField(ptf, p, iF, mapper),
		phiName_(ptf.phiName_),
		rhoName_(ptf.rhoName_),
		inletDir_(ptf.inletDir_, mapper)
	{}


	pressureDirectedInletVelocityFvPatchVectorField::
		pressureDirectedInletVelocityFvPatchVectorField
		(
			const fvPatch& p,
			const DimensionedField<vector, volMesh>& iF,
			const dictionary& dict
		)
		:
		fixedValueFvPatchVectorField(p, iF),
		phiName_(dict.lookupOrDefault<word>("phi", "phi")),
		rhoName_(dict.lookupOrDefault<word>("rho", "rho")),
		inletDir_("inletDirection", dict, p.size())
	{
		fvPatchVectorField::operator=(vectorField("value", dict, p.size()));
	}


	pressureDirectedInletVelocityFvPatchVectorField::
		pressureDirectedInletVelocityFvPatchVectorField
		(
			const pressureDirectedInletVelocityFvPatchVectorField& pivpvf
		)
		:
		fixedValueFvPatchVectorField(pivpvf),
		phiName_(pivpvf.phiName_),
		rhoName_(pivpvf.rhoName_),
		inletDir_(pivpvf.inletDir_)
	{}


	pressureDirectedInletVelocityFvPatchVectorField::
		pressureDirectedInletVelocityFvPatchVectorField
		(
			const pressureDirectedInletVelocityFvPatchVectorField& pivpvf,
			const DimensionedField<vector, volMesh>& iF
		)
		:
		fixedValueFvPatchVectorField(pivpvf, iF),
		phiName_(pivpvf.phiName_),
		rhoName_(pivpvf.rhoName_),
		inletDir_(pivpvf.inletDir_)
	{}


	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	void pressureDirectedInletVelocityFvPatchVectorField::autoMap
	(
		const fvPatchFieldMapper& m
	)
	{
		fixedValueFvPatchVectorField::autoMap(m);
		inletDir_.autoMap(m);
	}


	void pressureDirectedInletVelocityFvPatchVectorField::rmap
	(
		const fvPatchVectorField& ptf,
		const labelList& addr
	)
	{
		fixedValueFvPatchVectorField::rmap(ptf, addr);

		const pressureDirectedInletVelocityFvPatchVectorField& tiptf =
			refCast<const pressureDirectedInletVelocityFvPatchVectorField>(ptf);

		inletDir_.rmap(tiptf.inletDir_, addr);
	}


	void pressureDirectedInletVelocityFvPatchVectorField::updateCoeffs()
	{
		if (updated())
		{
			return;
		}

		if (!this->db().objectRegistry::found(phiName_))
		{
			// Flux not available, do not update
			InfoIn
			(
				"void pressureDirectedInletVelocityFvPatchVectorField::"
				"updateCoeffs()"
			) << "Flux field " << phiName_ << " not found.  "
				<< "Performing fixed value update" << endl;

			fixedValueFvPatchVectorField::updateCoeffs();

			return;
		}

		const surfaceScalarField& phi =
			db().lookupObject<surfaceScalarField>(phiName_);

		const fvsPatchField<scalar>& phip =
			patch().patchField<surfaceScalarField, scalar>(phi);

		vectorField n = patch().nf();
		scalarField ndmagS = (n & inletDir_)*patch().magSf();

		if (phi.dimensions() == dimVelocity * dimArea)
		{
			operator==(inletDir_*phip / ndmagS);
		}
		else if (phi.dimensions() == dimDensity * dimVelocity*dimArea)
		{
			if (!this->db().objectRegistry::found(rhoName_))
			{
				// Rho not available, do not update
				fixedValueFvPatchVectorField::updateCoeffs();

				return;
			}

			const fvPatchField<scalar>& rhop =
				lookupPatchField<volScalarField, scalar>(rhoName_);

			operator==(inletDir_*phip / (rhop*ndmagS));
		}
		else
		{
			FatalErrorIn
			(
				"pressureDirectedInletVelocityFvPatchVectorField::updateCoeffs()"
			) << "dimensions of phi are not correct"
				<< "\n    on patch " << this->patch().name()
				<< " of field " << this->dimensionedInternalField().name()
				<< " in file " << this->dimensionedInternalField().objectPath()
				<< exit(FatalError);
		}

		fixedValueFvPatchVectorField::updateCoeffs();
	}


	void pressureDirectedInletVelocityFvPatchVectorField::write(Ostream& os) const
	{
		fvPatchVectorField::write(os);
		if (phiName_ != "phi")
		{
			os.writeKeyword("phi") << phiName_ << token::END_STATEMENT << nl;
		}
		if (rhoName_ != "rho")
		{
			os.writeKeyword("rho") << rhoName_ << token::END_STATEMENT << nl;
		}
		inletDir_.writeEntry("inletDirection", os);
		writeEntry("value", os);
	}


	// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

	void pressureDirectedInletVelocityFvPatchVectorField::operator=
		(
			const fvPatchField<vector>& pvf
			)
	{
		fvPatchField<vector>::operator=(inletDir_*(inletDir_ & pvf));
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	makePatchTypeField
	(
		fvPatchVectorField,
		pressureDirectedInletVelocityFvPatchVectorField
	);


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //