#include <realGasEThermo.hxx>

#include <tnbTime.hxx>

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class MixtureType>
void tnbLib::realGasEThermo<MixtureType>::calculate()
{
	const scalarField& eCells = e_.internalField();
	const scalarField& pCells = this->p_.internalField();

	scalarField& TCells = this->T_.internalField();
	scalarField& rhoCells = this->rho_.internalField();
	scalarField& psiCells = this->psi_.internalField();
	scalarField& drhodeCells = this->drhode_.internalField();
	scalarField& muCells = this->mu_.internalField();
	scalarField& alphaCells = this->alpha_.internalField();

	forAll(TCells, celli)
	{
		const typename MixtureType::thermoType& mixture_ =
			this->cellMixture(celli);

		mixture_.TE(eCells[celli], TCells[celli], pCells[celli], rhoCells[celli]);
		psiCells[celli] = mixture_.psiE(rhoCells[celli], TCells[celli]);
		drhodeCells[celli] = mixture_.drhodE(rhoCells[celli], TCells[celli]);
		muCells[celli] = mixture_.mu(TCells[celli]);
		alphaCells[celli] = mixture_.alpha(rhoCells[celli], TCells[celli]);
	}


	forAll(T_.boundaryField(), patchi)
	{
		fvPatchScalarField& pp = this->p_.boundaryField()[patchi];
		fvPatchScalarField& pT = this->T_.boundaryField()[patchi];
		fvPatchScalarField& ppsi = this->psi_.boundaryField()[patchi];
		fvPatchScalarField& pdrhode = this->drhode_.boundaryField()[patchi];
		fvPatchScalarField& prho = this->rho_.boundaryField()[patchi];
		fvPatchScalarField& pe = e_.boundaryField()[patchi];
		fvPatchScalarField& pmu = this->mu_.boundaryField()[patchi];
		fvPatchScalarField& palpha = this->alpha_.boundaryField()[patchi];

		if (pT.fixesValue())
		{
			forAll(pT, facei)
			{
				const typename MixtureType::thermoType& mixture_ =
					this->patchFaceMixture(patchi, facei);

				prho[facei] = mixture_.rho(pp[facei], pT[facei], prho[facei]);
				ppsi[facei] = mixture_.psiE(prho[facei], pT[facei]);
				pdrhode[facei] = mixture_.drhodE(prho[facei], pT[facei]);
				pe[facei] = mixture_.E(prho[facei], pT[facei]);
				pmu[facei] = mixture_.mu(pT[facei]);
				palpha[facei] = mixture_.alpha(prho[facei], pT[facei]);
			}
		}
		else
		{
			forAll(pT, facei)
			{
				const typename MixtureType::thermoType& mixture_ =
					this->patchFaceMixture(patchi, facei);
				mixture_.TE(pe[facei], pT[facei], pp[facei], prho[facei]);
				pmu[facei] = mixture_.mu(pT[facei]);
				ppsi[facei] = mixture_.psiE(prho[facei], pT[facei]);
				pdrhode[facei] = mixture_.drhodE(prho[facei], pT[facei]);
				palpha[facei] = mixture_.alpha(prho[facei], pT[facei]);
			}
		}
	}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class MixtureType>
tnbLib::realGasEThermo<MixtureType>::realGasEThermo
(
	const fvMesh& mesh,
	const objectRegistry& obj
)
	:
	basicPsiThermo(mesh, obj),
	MixtureType(*this, mesh, obj),

	e_
	(
		IOobject
		(
			"e",
			mesh.time().timeName(),
			obj,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		mesh,
		dimensionSet(0, 2, -2, 0, 0),
		this->eBoundaryTypes()
	),
	rho_
	(
		IOobject
		(
			"rhoThermo",
			mesh.time().timeName(),
			obj,
			IOobject::READ_IF_PRESENT,
			IOobject::NO_WRITE
		),
		mesh,
		dimDensity
	),
	drhode_
	(
		IOobject
		(
			"drhode",
			mesh.time().timeName(),
			obj,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		mesh,
		dimensionSet(1, -5, 2, 0, 0)
	)
{
	scalarField& eCells = e_.internalField();
	const scalarField& TCells = this->T_.internalField();
	const scalarField& pCells = this->p_.internalField();
	scalarField& rhoCells = this->rho_.internalField();


	forAll(rhoCells, celli)
	{
		rhoCells[celli] = this->cellMixture(celli).rho(pCells[celli], TCells[celli]);
	}

	forAll(rho_.boundaryField(), patchi)
	{
		rho_.boundaryField()[patchi] ==
			rho(this->T_.boundaryField()[patchi], patchi);
	}

	forAll(eCells, celli)
	{
		eCells[celli] = this->cellMixture(celli).E(rhoCells[celli], TCells[celli]);
	}

	forAll(e_.boundaryField(), patchi)
	{
		e_.boundaryField()[patchi] ==
			e(this->T_.boundaryField()[patchi], patchi);
	}

	this->eBoundaryCorrection(e_);

	calculate();

	// Switch on saving old time
	this->psi_.oldTime();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class MixtureType>
tnbLib::realGasEThermo<MixtureType>::~realGasEThermo()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class MixtureType>
void tnbLib::realGasEThermo<MixtureType>::correct()
{
	if (debug)
	{
		Info << "entering realGasEThermo<MixtureType>::correct()" << endl;
	}

	// force the saving of the old-time values
	this->psi_.oldTime();

	calculate();

	if (debug)
	{
		Info << "exiting realGasEThermo<MixtureType>::correct()" << endl;
	}
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::realGasEThermo<MixtureType>::e
(
	const scalarField& T,
	const labelList& cells
) const
{
	//CL: need the pressure of the internal field to calculate the realGas internal energy
	//CL: this is done this way to assure compatibility to old OF Thermo-Versions
	const scalarField& pCells = this->p_.internalField();

	tmp<scalarField> te(new scalarField(T.size()));
	scalarField& e = te();

	forAll(T, celli)
	{
		e[celli] = this->cellMixture(cells[celli]).E(this->cellMixture(cells[celli]).rho(pCells[cells[celli]], T[celli]), T[celli]);
	}

	return te;
}

template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::realGasEThermo<MixtureType>::e
(
	const scalarField& T,
	const label patchi
) const
{
	//CL: need the pressure at the patch to calculate the realGas internal energy
	//CL: this is done this way to assure compatibility to old OF Thermo-Versions
	const fvPatchScalarField& pp = this->p_.boundaryField()[patchi];

	tmp<scalarField> te(new scalarField(T.size()));
	scalarField& e = te();

	forAll(T, facei)
	{
		e[facei] = this->patchFaceMixture(patchi, facei).E(this->patchFaceMixture(patchi, facei).rho(pp[facei], T[facei]), T[facei]);
	}

	return te;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::realGasEThermo<MixtureType>::rho
(
	const scalarField& T,
	const label patchi
) const
{
	//CL: need the pressure at the patch to calculate the realGas density
	//CL: this is done this way to assure compatibility to old OF Thermo-Versions
	const fvPatchScalarField& pp = this->p_.boundaryField()[patchi];

	tmp<scalarField> trho(new scalarField(T.size()));
	scalarField& rho = trho();

	forAll(T, facei)
	{
		rho[facei] = this->patchFaceMixture(patchi, facei).rho(pp[facei], T[facei]);
	}

	return trho;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::realGasEThermo<MixtureType>::Cp
(
	const scalarField& T,
	const label patchi
) const
{
	//CL: need the pressure at the patch to calculate the realGas cp
	//CL: this is done this way to assure compatibility to old OF Thermo-Versions
	const fvPatchScalarField& pp = this->p_.boundaryField()[patchi];

	tmp<scalarField> tCp(new scalarField(T.size()));
	scalarField& cp = tCp();

	forAll(T, facei)
	{
		cp[facei] = this->patchFaceMixture(patchi, facei).Cp(this->patchFaceMixture(patchi, facei).rho(pp[facei], T[facei]), T[facei]);
	}

	return tCp;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::volScalarField> tnbLib::realGasEThermo<MixtureType>::Cp() const
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
		cp[celli] = this->cellMixture(celli).Cp(this->rho_[celli], this->T_[celli]);
	}

	forAll(this->T_.boundaryField(), patchi)
	{
		const fvPatchScalarField& pT = this->T_.boundaryField()[patchi];
		const fvPatchScalarField& prho = this->rho_.boundaryField()[patchi];
		fvPatchScalarField& pCp = cp.boundaryField()[patchi];

		forAll(pT, facei)
		{
			pCp[facei] = this->patchFaceMixture(patchi, facei).Cp(prho[facei], pT[facei]);
		}
	}

	return tCp;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::scalarField> tnbLib::realGasEThermo<MixtureType>::Cv
(
	const scalarField& T,
	const label patchi
) const
{
	//CL: need the pressure at the patch to calculate the realGas internal energy
	//CL: this is done this way to assure compatibility to old OF Thermo-Versions
	const fvPatchScalarField& pp = this->p_.boundaryField()[patchi];

	tmp<scalarField> tCv(new scalarField(T.size()));
	scalarField& cv = tCv();

	forAll(T, facei)
	{
		cv[facei] = this->patchFaceMixture(patchi, facei).Cv(this->patchFaceMixture(patchi, facei).rho(pp[facei], T[facei]), T[facei]);
	}

	return tCv;
}


// CL: Maybe this function should be changed so that it is not "const" function anymore
template<class MixtureType>
tnbLib::tmp<tnbLib::volScalarField> tnbLib::realGasEThermo<MixtureType>::rho()  const
{

	const fvMesh& mesh = this->T_.mesh();

	//CL: create an rho Field, which will be return
	//CL: the problem is that this function is "const",
	//CL: so a new variabel is needed
	tmp<volScalarField> trho
	(
		new volScalarField
		(
			IOobject
			(
				"rhoFunctionThermo",
				mesh.time().timeName(),
				T_.db(),
				IOobject::NO_READ,
				IOobject::NO_WRITE
			),
			mesh,
			dimDensity
		)
	);

	//CL: copy "old" rho value onto the new rho field as start point
	//CL: for the newton solver used in this->TE( ... )
	trho() = rho_;

	volScalarField& rho = trho();

	const scalarField& eCells = e_.internalField();
	const scalarField& pCells = this->p_.internalField();
	scalarField TCells = this->T_.internalField();

	forAll(pCells, celli)
	{
		const typename MixtureType::thermoType& mixture_ =
			this->cellMixture(celli);

		// getting the new rho Field
		mixture_.TE(eCells[celli], TCells[celli], pCells[celli], rho[celli]);
	}

	forAll(p_.boundaryField(), patchi)
	{
		fvPatchScalarField pp = this->p_.boundaryField()[patchi];
		fvPatchScalarField pe = e_.boundaryField()[patchi];
		fvPatchScalarField pT = this->T_.boundaryField()[patchi];

		fvPatchScalarField& prho_ = rho.boundaryField()[patchi];

		forAll(pp, facei)
		{
			const typename MixtureType::thermoType& mixture_ =
				this->patchFaceMixture(patchi, facei);

			// getting the new rho patch Field
			mixture_.TE(pe[facei], pT[facei], pp[facei], prho_[facei]);
		}
	}
	return trho;
}


template<class MixtureType>
tnbLib::tmp<tnbLib::volScalarField> tnbLib::realGasEThermo<MixtureType>::Cv() const
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
			dimensionSet(0, 2, -2, -1, 0)
		)
	);

	volScalarField& cv = tCv();

	forAll(this->T_, celli)
	{
		cv[celli] = this->cellMixture(celli).Cv(this->rho_[celli], this->T_[celli]);
	}

	forAll(this->T_.boundaryField(), patchi)
	{
		cv.boundaryField()[patchi] =
			Cv(this->T_.boundaryField()[patchi], patchi);
	}

	return tCv;
}


template<class MixtureType>
bool tnbLib::realGasEThermo<MixtureType>::read()
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