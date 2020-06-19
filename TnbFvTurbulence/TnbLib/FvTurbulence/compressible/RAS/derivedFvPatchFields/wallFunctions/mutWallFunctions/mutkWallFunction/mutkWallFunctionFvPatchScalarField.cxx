#include <mutkWallFunctionFvPatchScalarField.hxx>

#include <compRASModel.hxx>
#include <fvPatchFieldMapper.hxx>
#include <volFields.hxx>
#include <addToRunTimeSelectionTable.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{
	namespace compressible
	{
		namespace RASModels
		{

			// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

			void mutkWallFunctionFvPatchScalarField::checkType()
			{
				if (!patch().isWall())
				{
					FatalErrorIn("mutkWallFunctionFvPatchScalarField::checkType()")
						<< "Invalid wall function specification" << nl
						<< "    Patch type for patch " << patch().name()
						<< " must be wall" << nl
						<< "    Current patch type is " << patch().type() << nl << endl
						<< abort(FatalError);
				}
			}


			scalar mutkWallFunctionFvPatchScalarField::calcYPlusLam
			(
				const scalar kappa,
				const scalar E
			) const
			{
				scalar ypl = 11.0;

				for (int i = 0; i < 10; i++)
				{
					ypl = log(E*ypl) / kappa;
				}

				return ypl;
			}


			tmp<scalarField> mutkWallFunctionFvPatchScalarField::calcMut() const
			{
				const label patchI = patch().index();
				const RASModel& rasModel = db().lookupObject<RASModel>("RASProperties");
				const scalarField& y = rasModel.y()[patchI];
				const scalarField& rhow = rasModel.rho().boundaryField()[patchI];
				const tmp<volScalarField> tk = rasModel.k();
				const volScalarField& k = tk();
				const scalarField& muw = rasModel.mu().boundaryField()[patchI];

				const scalar Cmu25 = pow(Cmu_, 0.25);

				tmp<scalarField> tmutw(new scalarField(patch().size(), 0.0));
				scalarField& mutw = tmutw();

				forAll(mutw, faceI)
				{
					label faceCellI = patch().faceCells()[faceI];

					scalar yPlus =
						Cmu25 * y[faceI] * sqrt(k[faceCellI]) / (muw[faceI] / rhow[faceI]);

					if (yPlus > yPlusLam_)
					{
						mutw[faceI] = muw[faceI] * (yPlus*kappa_ / log(E_*yPlus) - 1);
					}
				}

				return tmutw;
			}


			void mutkWallFunctionFvPatchScalarField::writeLocalEntries(Ostream& os) const
			{
				os.writeKeyword("Cmu") << Cmu_ << token::END_STATEMENT << nl;
				os.writeKeyword("kappa") << kappa_ << token::END_STATEMENT << nl;
				os.writeKeyword("E") << E_ << token::END_STATEMENT << nl;
			}


			// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

			mutkWallFunctionFvPatchScalarField::mutkWallFunctionFvPatchScalarField
			(
				const fvPatch& p,
				const DimensionedField<scalar, volMesh>& iF
			)
				:
				fixedValueFvPatchScalarField(p, iF),
				Cmu_(0.09),
				kappa_(0.41),
				E_(9.8),
				yPlusLam_(calcYPlusLam(kappa_, E_))
			{}


			mutkWallFunctionFvPatchScalarField::mutkWallFunctionFvPatchScalarField
			(
				const mutkWallFunctionFvPatchScalarField& ptf,
				const fvPatch& p,
				const DimensionedField<scalar, volMesh>& iF,
				const fvPatchFieldMapper& mapper
			)
				:
				fixedValueFvPatchScalarField(ptf, p, iF, mapper),
				Cmu_(ptf.Cmu_),
				kappa_(ptf.kappa_),
				E_(ptf.E_),
				yPlusLam_(ptf.yPlusLam_)
			{}


			mutkWallFunctionFvPatchScalarField::mutkWallFunctionFvPatchScalarField
			(
				const fvPatch& p,
				const DimensionedField<scalar, volMesh>& iF,
				const dictionary& dict
			)
				:
				fixedValueFvPatchScalarField(p, iF, dict),
				Cmu_(dict.lookupOrDefault<scalar>("Cmu", 0.09)),
				kappa_(dict.lookupOrDefault<scalar>("kappa", 0.41)),
				E_(dict.lookupOrDefault<scalar>("E", 9.8)),
				yPlusLam_(calcYPlusLam(kappa_, E_))
			{}


			mutkWallFunctionFvPatchScalarField::mutkWallFunctionFvPatchScalarField
			(
				const mutkWallFunctionFvPatchScalarField& wfpsf
			)
				:
				fixedValueFvPatchScalarField(wfpsf),
				Cmu_(wfpsf.Cmu_),
				kappa_(wfpsf.kappa_),
				E_(wfpsf.E_),
				yPlusLam_(wfpsf.yPlusLam_)
			{}


			mutkWallFunctionFvPatchScalarField::mutkWallFunctionFvPatchScalarField
			(
				const mutkWallFunctionFvPatchScalarField& wfpsf,
				const DimensionedField<scalar, volMesh>& iF
			)
				:
				fixedValueFvPatchScalarField(wfpsf, iF),
				Cmu_(wfpsf.Cmu_),
				kappa_(wfpsf.kappa_),
				E_(wfpsf.E_),
				yPlusLam_(wfpsf.yPlusLam_)
			{}


			// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

			void mutkWallFunctionFvPatchScalarField::updateCoeffs()
			{
				if (updated())
				{
					return;
				}

				operator==(calcMut());

				fixedValueFvPatchScalarField::updateCoeffs();
			}


			tmp<scalarField> mutkWallFunctionFvPatchScalarField::yPlus() const
			{
				const label patchI = patch().index();

				const RASModel& rasModel = db().lookupObject<RASModel>("RASProperties");
				const scalarField& y = rasModel.y()[patchI];

				const tmp<volScalarField> tk = rasModel.k();
				const volScalarField& k = tk();
				const scalarField kwc = k.boundaryField()[patchI].patchInternalField();
				const scalarField& muw = rasModel.mu().boundaryField()[patchI];
				const scalarField& rhow = rasModel.rho().boundaryField()[patchI];

				return pow(Cmu_, 0.25)*y*sqrt(kwc) / (muw / rhow);
			}


			void mutkWallFunctionFvPatchScalarField::write(Ostream& os) const
			{
				fvPatchField<scalar>::write(os);
				writeLocalEntries(os);
				writeEntry("value", os);
			}


			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

			makePatchTypeField(fvPatchScalarField, mutkWallFunctionFvPatchScalarField);

			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		} // End namespace RASModels
	} // End namespace compressible
} // End namespace tnbLib

// ************************************************************************* //