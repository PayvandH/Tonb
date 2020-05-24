#include <IAPWSThermo.hxx>

#include <tnbTime.hxx>

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


void tnbLib::IAPWSThermo::calculate()
{
	scalarField& hCells = h_.internalField();
	scalarField& pCells = this->p_.internalField();
	scalarField& TCells = this->T_.internalField();
	scalarField& rhoCells = this->rho_.internalField();
	scalarField& psiCells = this->psi_.internalField();
	scalarField& drhodhCells = this->drhodh_.internalField();
	scalarField& muCells = this->mu_.internalField();
	scalarField& alphaCells = this->alpha_.internalField();

	//CL: Updating all cell properties
	//CL: loop through all cells
	forAll(TCells, celli)
	{
		//CL: see IAPWAS-IF97.H
		calculateProperties_ph
		(
			pCells[celli],
			hCells[celli],
			TCells[celli],
			rhoCells[celli],
			psiCells[celli],
			drhodhCells[celli],
			muCells[celli],
			alphaCells[celli]
		);
	}

	//CL: loop through all patches
	forAll(T_.boundaryField(), patchi)
	{
		fvPatchScalarField& pp = this->p_.boundaryField()[patchi];
		fvPatchScalarField& pT = this->T_.boundaryField()[patchi];
		fvPatchScalarField& ppsi = this->psi_.boundaryField()[patchi];
		fvPatchScalarField& pdrhodh = this->drhodh_.boundaryField()[patchi];
		fvPatchScalarField& prho = this->rho_.boundaryField()[patchi];
		fvPatchScalarField& ph = this->h_.boundaryField()[patchi];
		fvPatchScalarField& pmu = this->mu_.boundaryField()[patchi];
		fvPatchScalarField& palpha = this->alpha_.boundaryField()[patchi];

		//CL: Updating the patch properties for patches with fixed temperature BC's
		if (pT.fixesValue())
		{
			forAll(pT, facei)
			{
				//CL: see IAPWAS-IF97.H
				calculateProperties_pT
				(
					pp[facei],
					pT[facei],
					ph[facei],
					prho[facei],
					ppsi[facei],
					pdrhodh[facei],
					pmu[facei],
					palpha[facei]
				);
			}
		}
		//CL: Updating the patch properties for patches without fixed temperature BC's
		else
		{
			forAll(pT, facei)
			{
				//CL: see IAPWAS-IF97.H
				calculateProperties_ph
				(
					pp[facei],
					ph[facei],
					pT[facei],
					prho[facei],
					ppsi[facei],
					pdrhodh[facei],
					pmu[facei],
					palpha[facei]
				);
			}
		}
	}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


tnbLib::IAPWSThermo::IAPWSThermo
(
	const fvMesh& mesh,
	const objectRegistry& obj
)
	:
	basicPsiThermo(mesh, obj),

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

	drhodh_
	(
		IOobject
		(
			"drhodh",
			mesh.time().timeName(),
			obj,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		mesh,
		dimensionSet(1, -5, 2, 0, 0)
	)
{

	scalarField& hCells = h_.internalField();
	scalarField& TCells = this->T_.internalField();
	scalarField& pCells = this->p_.internalField();
	scalarField& rhoCells = this->rho_.internalField();

	forAll(hCells, celli)
	{
		hCells[celli] = h_pT(pCells[celli], TCells[celli]);
	}

	forAll(h_.boundaryField(), patchi)
	{
		h_.boundaryField()[patchi] ==
			h(this->T_.boundaryField()[patchi], patchi);
	}

	forAll(rhoCells, celli)
	{
		rhoCells[celli] = rho_pT(pCells[celli], TCells[celli]);
	}

	forAll(rho_.boundaryField(), patchi)
	{
		rho_.boundaryField()[patchi] ==
			rho(this->p_.boundaryField()[patchi], this->h_.boundaryField()[patchi], patchi);
	}

	hBoundaryCorrection(h_);

	calculate();

	// Switch on saving old time
	this->psi_.oldTime();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

tnbLib::IAPWSThermo::~IAPWSThermo()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void tnbLib::IAPWSThermo::correct()
{
	if (debug)
	{
		Info << "entering IAPWSThermo::correct()" << endl;
	}

	// force the saving of the old-time values
	this->psi_.oldTime();

	calculate();

	if (debug)
	{
		Info << "exiting IAPWSThermo::correct()" << endl;
	}
}


tnbLib::tmp<tnbLib::scalarField> tnbLib::IAPWSThermo::h
(
	const scalarField& T,
	const labelList& cells
) const
{
	//getting pressure field
	const scalarField& pCells = this->p_.internalField();

	tmp<scalarField> th(new scalarField(T.size()));
	scalarField& h = th();

	forAll(T, celli)
	{
		h[celli] = h_pT(pCells[cells[celli]], T[celli]);
	}

	return th;
}


tnbLib::tmp<tnbLib::scalarField> tnbLib::IAPWSThermo::h
(
	const scalarField& T,
	const label patchi
) const
{
	// getting pressure at the patch
	const fvPatchScalarField& pp = this->p_.boundaryField()[patchi];

	tmp<scalarField> th(new scalarField(T.size()));
	scalarField& h = th();

	forAll(T, facei)
	{
		h[facei] = h_pT(pp[facei], T[facei]);
	}

	return th;
}


//CL: Calculates rho at patch
tnbLib::tmp<tnbLib::scalarField> tnbLib::IAPWSThermo::rho
(
	const scalarField& p,
	const scalarField& h,
	const label patchi
) const
{
	tmp<scalarField> trho(new scalarField(h.size()));
	scalarField& rho = trho();

	forAll(h, facei)
	{
		rho[facei] = rho_ph(p[facei], h[facei]);
	}

	return trho;
}


tnbLib::tmp<tnbLib::scalarField> tnbLib::IAPWSThermo::Cp
(
	const scalarField& T,
	const label patchi
) const
{
	// getting pressure at the patch
	const fvPatchScalarField& pp = this->p_.boundaryField()[patchi];

	tmp<scalarField> tCp(new scalarField(T.size()));
	scalarField& cp = tCp();

	forAll(T, facei)
	{
		cp[facei] = cp_ph(pp[facei], h_pT(pp[facei], T[facei]));
	}

	return tCp;
}


tnbLib::tmp<tnbLib::volScalarField> tnbLib::IAPWSThermo::Cp() const
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
		cp[celli] = cp_ph(this->p_[celli], this->h_[celli]);
	}

	forAll(this->T_.boundaryField(), patchi)
	{
		const fvPatchScalarField& pp = this->p_.boundaryField()[patchi];
		const fvPatchScalarField& ph = this->h_.boundaryField()[patchi];
		fvPatchScalarField& pCp = cp.boundaryField()[patchi];

		forAll(ph, facei)
		{
			pCp[facei] = cp_ph(pp[facei], ph[facei]);
		}
	}

	return tCp;
}


//CL: Returns an updated field for rho
tnbLib::tmp<tnbLib::volScalarField> tnbLib::IAPWSThermo::rho() const
{
	const fvMesh& mesh = this->p_.mesh();

	tmp<volScalarField> prho
	(
		new volScalarField
		(
			IOobject
			(
				"rhoThermo2",
				mesh.time().timeName(),
				this->T_.db(),
				IOobject::NO_READ,
				IOobject::NO_WRITE
			),
			mesh,
			dimDensity
		)
	);

	volScalarField& rho = prho();

	forAll(this->p_, celli)
	{
		rho[celli] = rho_ph(this->p_[celli], this->h_[celli]);
	}

	forAll(this->T_.boundaryField(), patchi)
	{
		const fvPatchScalarField& pp = this->p_.boundaryField()[patchi];
		const fvPatchScalarField& ph = this->h_.boundaryField()[patchi];
		fvPatchScalarField& prho = rho.boundaryField()[patchi];

		forAll(ph, facei)
		{
			prho[facei] = rho_ph(pp[facei], ph[facei]);
		}
	}

	return prho;
}


tnbLib::tmp<tnbLib::scalarField> tnbLib::IAPWSThermo::Cv
(
	const scalarField& T,
	const label patchi
) const
{
	// getting pressure at the patch
	const fvPatchScalarField& pp = this->p_.boundaryField()[patchi];

	tmp<scalarField> tCv(new scalarField(T.size()));
	scalarField& cv = tCv();

	forAll(T, facei)
	{
		cv[facei] = cv_ph(pp[facei], h_pT(pp[facei], T[facei]));
	}

	return tCv;
}


tnbLib::tmp<tnbLib::volScalarField> tnbLib::IAPWSThermo::Cv() const
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

	forAll(this->h_, celli)
	{
		cv[celli] = cv_ph(this->p_[celli], this->h_[celli]);
	}

	forAll(this->T_.boundaryField(), patchi)
	{
		cv.boundaryField()[patchi] =
			Cv(this->T_.boundaryField()[patchi], patchi);
	}

	return tCv;
}


bool tnbLib::IAPWSThermo::read()
{
	basicPsiThermo::read();
	return true;
}


// ************************************************************************* //