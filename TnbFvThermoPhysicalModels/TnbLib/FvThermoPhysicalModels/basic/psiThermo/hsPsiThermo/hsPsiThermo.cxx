#include <hsPsiThermo.hxx>

#include <tnbTime.hxx>

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class MixtureType>
void tnbLib::hsPsiThermo<MixtureType>::calculate()
{
	const scalarField& hsCells = hs_.internalField();
	const scalarField& pCells = this->p_.internalField();

	scalarField& TCells = this->T_.internalField();
	scalarField& psiCells = this->psi_.internalField();
	scalarField& muCells = this->mu_.internalField();
	scalarField& alphaCells = this->alpha_.internalField();

	forAll(TCells, celli)
	{
		const typename MixtureType::thermoType& mixture_ =
			this->cellMixture(celli);

		TCells[celli] = mixture_.THs(hsCells[celli], TCells[celli]);
		psiCells[celli] = mixture_.psi(pCells[celli], TCells[celli]);

		muCells[celli] = mixture_.mu(TCells[celli]);
		alphaCells[celli] = mixture_.alpha(TCells[celli]);
	}

	forAll(T_.boundaryField(), patchi)
	{
		fvPatchScalarField& pp = this->p_.boundaryField()[patchi];
		fvPatchScalarField& pT = this->T_.boundaryField()[patchi];
		fvPatchScalarField& ppsi = this->psi_.boundaryField()[patchi];

		fvPatchScalarField& phs = hs_.boundaryField()[patchi];

		fvPatchScalarField& pmu = this->mu_.boundaryField()[patchi];
		fvPatchScalarField& palpha = this->alpha_.boundaryField()[patchi];

		if (pT.fixesValue())
		{
			forAll(pT, facei)
			{
				const typename MixtureType::thermoType& mixture_ =
					this->patchFaceMixture(patchi, facei);

				phs[facei] = mixture_.Hs(pT[facei]);

				ppsi[facei] = mixture_.psi(pp[facei], pT[facei]);
				pmu[facei] = mixture_.mu(pT[facei]);
				palpha[facei] = mixture_.alpha(pT[facei]);
			}
		}
		else
		{
			forAll(pT, facei)
			{
				const typename MixtureType::thermoType& mixture_ =
					this->patchFaceMixture(patchi, facei);

				pT[facei] = mixture_.THs(phs[facei], pT[facei]);

				ppsi[facei] = mixture_.psi(pp[facei], pT[facei]);
				pmu[facei] = mixture_.mu(pT[facei]);
				palpha[facei] = mixture_.alpha(pT[facei]);
			}
		}
	}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class MixtureType>
tnbLib::hsPsiThermo<MixtureType>::hsPsiThermo
(
	const fvMesh& mesh,
	const objectRegistry& obj
)
	:
	basicPsiThermo(mesh, obj),
	MixtureType(*this, mesh, obj),

	hs_
	(
		IOobject
		(
			"hs",
			mesh.time().timeName(),
			obj,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		mesh,
		dimEnergy / dimMass,
		this->hBoundaryTypes()
	)
{
	scalarField& hsCells = hs_.internalField();
	const scalarField& TCells = this->T_.internalField();

	forAll(hsCells, celli)
	{
		hsCells[celli] = this->cellMixture(celli).Hs(TCells[celli]);
	}

	forAll(hs_.boundaryField(), patchi)
	{
		hs_.boundaryField()[patchi] ==
			hs(this->T_.boundaryField()[patchi], patchi);
	}

	hBoundaryCorrection(hs_);

	calculate();

	// Switch on saving old time
	this->psi_.oldTime();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class MixtureType>
tnbLib::hsPsiThermo<MixtureType>::~hsPsiThermo()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class MixtureType>
void tnbLib::hsPsiThermo<MixtureType>::correct()
{
	if (debug)
	{
		Info << "entering hsPsiThermo<MixtureType>::correct()" << endl;
	}

	// force the saving of the old-time values
	this->psi_.oldTime();

	calculate();

	if (debug)
	{
		Info << "exiting hsPsiThermo<MixtureType>::correct()" << endl;
	}
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::hsPsiThermo<MixtureType>::hs
(
	const scalarField& T,
	const labelList& cells
) const
{
	tmp<scalarField> ths(new scalarField(T.size()));
	scalarField& hs = ths();

	forAll(T, celli)
	{
		hs[celli] = this->cellMixture(cells[celli]).Hs(T[celli]);
	}

	return ths;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::hsPsiThermo<MixtureType>::hs
(
	const scalarField& T,
	const label patchi
) const
{
	tmp<scalarField> ths(new scalarField(T.size()));
	scalarField& hs = ths();

	forAll(T, facei)
	{
		hs[facei] = this->patchFaceMixture(patchi, facei).Hs(T[facei]);
	}

	return ths;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::hsPsiThermo<MixtureType>::Cp
(
	const scalarField& T,
	const label patchi
) const
{
	tmp<scalarField> tCp(new scalarField(T.size()));
	scalarField& cp = tCp();

	forAll(T, facei)
	{
		cp[facei] = this->patchFaceMixture(patchi, facei).Cp(T[facei]);
	}

	return tCp;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::hsPsiThermo<MixtureType>::Cp
(
	const scalarField& T,
	const labelList& cells
) const
{
	tmp<scalarField> tCp(new scalarField(T.size()));
	scalarField& cp = tCp();

	forAll(T, celli)
	{
		cp[celli] = this->cellMixture(cells[celli]).Cp(T[celli]);
	}

	return tCp;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::volScalarField> tnbLib::hsPsiThermo<MixtureType>::Cp() const
{
	const fvMesh& mesh = this->T_.mesh();

	tmp<volScalarField> tCp
	(
		new volScalarField
		(
			IOobject
			(
				"Cp",
				mesh.time().timeName(),
				T_.db(),
				IOobject::NO_READ,
				IOobject::NO_WRITE
			),
			mesh,
			dimensionSet(0, 2, -2, -1, 0)
		)
	);

	volScalarField& cp = tCp();

	forAll(this->T_, celli)
	{
		cp[celli] = this->cellMixture(celli).Cp(this->T_[celli]);
	}

	forAll(this->T_.boundaryField(), patchi)
	{
		const fvPatchScalarField& pT = this->T_.boundaryField()[patchi];
		fvPatchScalarField& pCp = cp.boundaryField()[patchi];

		forAll(pT, facei)
		{
			pCp[facei] = this->patchFaceMixture(patchi, facei).Cp(pT[facei]);
		}
	}

	return tCp;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::hsPsiThermo<MixtureType>::Cv
(
	const scalarField& T,
	const label patchi
) const
{
	tmp<scalarField> tCv(new scalarField(T.size()));
	scalarField& cv = tCv();

	forAll(T, facei)
	{
		cv[facei] = this->patchFaceMixture(patchi, facei).Cv(T[facei]);
	}

	return tCv;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::volScalarField> tnbLib::hsPsiThermo<MixtureType>::Cv() const
{
	const fvMesh& mesh = this->T_.mesh();

	tmp<volScalarField> tCv
	(
		new volScalarField
		(
			IOobject
			(
				"Cv",
				mesh.time().timeName(),
				T_.db(),
				IOobject::NO_READ,
				IOobject::NO_WRITE
			),
			mesh,
			dimEnergy / dimMass / dimTemperature
		)
	);

	volScalarField& cv = tCv();

	forAll(this->T_, celli)
	{
		cv[celli] = this->cellMixture(celli).Cv(this->T_[celli]);
	}

	forAll(this->T_.boundaryField(), patchi)
	{
		cv.boundaryField()[patchi] =
			Cv(this->T_.boundaryField()[patchi], patchi);
	}

	return tCv;
}


template<class MixtureType>
bool tnbLib::hsPsiThermo<MixtureType>::read()
{
	if (basicPsiThermo::read())
	{
		MixtureType::read(*this);
		return true;
	}
	else
	{
		return false;
	}
}


// ************************************************************************* //