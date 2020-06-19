#include <hPsiThermo.hxx>

#include <tnbTime.hxx>

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class MixtureType>
void tnbLib::hPsiThermo<MixtureType>::calculate()
{
	const scalarField& hCells = h_.internalField();
	const scalarField& pCells = this->p_.internalField();

	scalarField& TCells = this->T_.internalField();
	scalarField& psiCells = this->psi_.internalField();
	scalarField& muCells = this->mu_.internalField();
	scalarField& alphaCells = this->alpha_.internalField();

	forAll(TCells, celli)
	{
		const typename MixtureType::thermoType& mixture_ =
			this->cellMixture(celli);

		TCells[celli] = mixture_.TH(hCells[celli], TCells[celli]);
		psiCells[celli] = mixture_.psi(pCells[celli], TCells[celli]);

		muCells[celli] = mixture_.mu(TCells[celli]);
		alphaCells[celli] = mixture_.alpha(TCells[celli]);
	}

	forAll(T_.boundaryField(), patchi)
	{
		fvPatchScalarField& pp = this->p_.boundaryField()[patchi];
		fvPatchScalarField& pT = this->T_.boundaryField()[patchi];
		fvPatchScalarField& ppsi = this->psi_.boundaryField()[patchi];

		fvPatchScalarField& ph = h_.boundaryField()[patchi];

		fvPatchScalarField& pmu = this->mu_.boundaryField()[patchi];
		fvPatchScalarField& palpha = this->alpha_.boundaryField()[patchi];

		if (pT.fixesValue())
		{
			forAll(pT, facei)
			{
				const typename MixtureType::thermoType& mixture_ =
					this->patchFaceMixture(patchi, facei);

				ph[facei] = mixture_.H(pT[facei]);

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

				pT[facei] = mixture_.TH(ph[facei], pT[facei]);

				ppsi[facei] = mixture_.psi(pp[facei], pT[facei]);
				pmu[facei] = mixture_.mu(pT[facei]);
				palpha[facei] = mixture_.alpha(pT[facei]);
			}
		}
	}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class MixtureType>
tnbLib::hPsiThermo<MixtureType>::hPsiThermo
(
	const fvMesh& mesh,
	const objectRegistry& obj
)
	:
	basicPsiThermo(mesh, obj),
	MixtureType(*this, mesh, obj),

	h_
	(
		IOobject
		(
			"h",
			mesh.time().timeName(),
			obj,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		mesh,
		dimensionSet(0, 2, -2, 0, 0),
		this->hBoundaryTypes()
	)
{
	scalarField& hCells = h_.internalField();
	const scalarField& TCells = this->T_.internalField();

	forAll(hCells, celli)
	{
		hCells[celli] = this->cellMixture(celli).H(TCells[celli]);
	}

	forAll(h_.boundaryField(), patchi)
	{
		h_.boundaryField()[patchi] ==
			h(this->T_.boundaryField()[patchi], patchi);
	}

	hBoundaryCorrection(h_);

	calculate();

	// Switch on saving old time
	this->psi_.oldTime();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class MixtureType>
tnbLib::hPsiThermo<MixtureType>::~hPsiThermo()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class MixtureType>
void tnbLib::hPsiThermo<MixtureType>::correct()
{
	if (debug)
	{
		Info << "entering hPsiThermo<MixtureType>::correct()" << endl;
	}

	// force the saving of the old-time values
	this->psi_.oldTime();

	calculate();

	if (debug)
	{
		Info << "exiting hPsiThermo<MixtureType>::correct()" << endl;
	}
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::hPsiThermo<MixtureType>::h
(
	const scalarField& T,
	const labelList& cells
) const
{
	tmp<scalarField> th(new scalarField(T.size()));
	scalarField& h = th();

	forAll(T, celli)
	{
		h[celli] = this->cellMixture(cells[celli]).H(T[celli]);
	}

	return th;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::hPsiThermo<MixtureType>::h
(
	const scalarField& T,
	const label patchi
) const
{
	tmp<scalarField> th(new scalarField(T.size()));
	scalarField& h = th();

	forAll(T, facei)
	{
		h[facei] = this->patchFaceMixture(patchi, facei).H(T[facei]);
	}

	return th;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::hPsiThermo<MixtureType>::Cp
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
tnbLib::tmp<tnbLib::scalarField> tnbLib::hPsiThermo<MixtureType>::Cp
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
tnbLib::tmp<tnbLib::volScalarField> tnbLib::hPsiThermo<MixtureType>::Cp() const
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
				this->T_.db(),
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
tnbLib::tmp<tnbLib::scalarField> tnbLib::hPsiThermo<MixtureType>::Cv
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
tnbLib::tmp<tnbLib::volScalarField> tnbLib::hPsiThermo<MixtureType>::Cv() const
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
				this->T_.db(),
				IOobject::NO_READ,
				IOobject::NO_WRITE
			),
			mesh,
			dimensionSet(0, 2, -2, -1, 0)
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
bool tnbLib::hPsiThermo<MixtureType>::read()
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