#include <surfaceInterpolationScheme.hxx>

#include <volFields.hxx>
#include <surfaceFields.hxx>
#include <coupledFvPatchField.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * //

	// Return weighting factors for scheme given by name in dictionary
	template<class Type>
	tmp<surfaceInterpolationScheme<Type> > surfaceInterpolationScheme<Type>::New
	(
		const fvMesh& mesh,
		Istream& schemeData
	)
	{
		if (schemeData.eof())
		{
			FatalIOErrorIn
			(
				"surfaceInterpolationScheme<Type>::New(const fvMesh&, Istream&)",
				schemeData
			) << "Discretisation scheme not specified"
				<< endl << endl
				<< "Valid schemes are :" << endl
				<< MeshConstructorTablePtr_->sortedToc()
				<< exit(FatalIOError);
		}

		const word schemeName(schemeData);

		if (surfaceInterpolation::debug || surfaceInterpolationScheme<Type>::debug)
		{
			Info << "surfaceInterpolationScheme<Type>::New"
				"(const fvMesh&, Istream&)"
				" : discretisation scheme = "
				<< schemeName
				<< endl;
		}

		typename MeshConstructorTable::iterator constructorIter =
			MeshConstructorTablePtr_->find(schemeName);

		if (constructorIter == MeshConstructorTablePtr_->end())
		{
			FatalIOErrorIn
			(
				"surfaceInterpolationScheme<Type>::New(const fvMesh&, Istream&)",
				schemeData
			) << "Unknown discretisation scheme "
				<< schemeName << nl << nl
				<< "Valid schemes are :" << endl
				<< MeshConstructorTablePtr_->sortedToc()
				<< exit(FatalIOError);
		}

		return constructorIter()(mesh, schemeData);
	}


	// Return weighting factors for scheme given by name in dictionary
	template<class Type>
	tmp<surfaceInterpolationScheme<Type> > surfaceInterpolationScheme<Type>::New
	(
		const fvMesh& mesh,
		const surfaceScalarField& faceFlux,
		Istream& schemeData
	)
	{
		if (schemeData.eof())
		{
			FatalIOErrorIn
			(
				"surfaceInterpolationScheme<Type>::New"
				"(const fvMesh&, const surfaceScalarField&, Istream&)",
				schemeData
			) << "Discretisation scheme not specified"
				<< endl << endl
				<< "Valid schemes are :" << endl
				<< MeshConstructorTablePtr_->sortedToc()
				<< exit(FatalIOError);
		}

		const word schemeName(schemeData);

		if (surfaceInterpolation::debug || surfaceInterpolationScheme<Type>::debug)
		{
			Info << "surfaceInterpolationScheme<Type>::New"
				"(const fvMesh&, const surfaceScalarField&, Istream&)"
				" : discretisation scheme = "
				<< schemeName
				<< endl;
		}

		typename MeshFluxConstructorTable::iterator constructorIter =
			MeshFluxConstructorTablePtr_->find(schemeName);

		if (constructorIter == MeshFluxConstructorTablePtr_->end())
		{
			FatalIOErrorIn
			(
				"surfaceInterpolationScheme<Type>::New"
				"(const fvMesh&, const surfaceScalarField&, Istream&)",
				schemeData
			) << "Unknown discretisation scheme "
				<< schemeName << nl << nl
				<< "Valid schemes are :" << endl
				<< MeshFluxConstructorTablePtr_->sortedToc()
				<< exit(FatalIOError);
		}

		return constructorIter()(mesh, faceFlux, schemeData);
	}


	// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

	template<class Type>
	surfaceInterpolationScheme<Type>::~surfaceInterpolationScheme()
	{}


	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	//- Return the face-interpolate of the given cell field
	//  with the given owner and neighbour weighting factors
	template<class Type>
	tmp<GeometricField<Type, fvsPatchField, surfaceMesh> >
		surfaceInterpolationScheme<Type>::interpolate
		(
			const GeometricField<Type, fvPatchField, volMesh>& vf,
			const tmp<surfaceScalarField>& tlambdas,
			const tmp<surfaceScalarField>& tys
		)
	{
		if (surfaceInterpolation::debug)
		{
			Info << "surfaceInterpolationScheme<Type>::uncorrectedInterpolate"
				"(const GeometricField<Type, fvPatchField, volMesh>&, "
				"const tmp<surfaceScalarField>&, "
				"const tmp<surfaceScalarField>&) : "
				"interpolating "
				<< vf.type() << " "
				<< vf.name()
				<< " from cells to faces "
				"without explicit correction"
				<< endl;
		}

		const surfaceScalarField& lambdas = tlambdas();
		const surfaceScalarField& ys = tys();

		const Field<Type>& vfi = vf.internalField();
		const scalarField& lambda = lambdas.internalField();
		const scalarField& y = ys.internalField();

		const fvMesh& mesh = vf.mesh();
		const unallocLabelList& P = mesh.owner();
		const unallocLabelList& N = mesh.neighbour();

		tmp<GeometricField<Type, fvsPatchField, surfaceMesh> > tsf
		(
			new GeometricField<Type, fvsPatchField, surfaceMesh>
			(
				IOobject
				(
					"interpolate(" + vf.name() + ')',
					vf.instance(),
					vf.db()
				),
				mesh,
				vf.dimensions()
				)
		);
		GeometricField<Type, fvsPatchField, surfaceMesh>& sf = tsf();

		Field<Type>& sfi = sf.internalField();

		for (register label fi = 0; fi < P.size(); fi++)
		{
			sfi[fi] = lambda[fi] * vfi[P[fi]] + y[fi] * vfi[N[fi]];
		}


		// Interpolate across coupled patches using given lambdas and ys

		forAll(vf.boundaryField(), patchI)
		{
			vf.boundaryField()[patchI].patchInterpolate
			(
				sf,
				lambdas.boundaryField()[patchI],
				ys.boundaryField()[patchI]
			);
		}

		tlambdas.clear();
		tys.clear();

		return tsf;
	}


	//- Return the face-interpolate of the given cell field
	//  with the given weighting factors
	template<class Type>
	tmp<GeometricField<Type, fvsPatchField, surfaceMesh> >
		surfaceInterpolationScheme<Type>::interpolate
		(
			const GeometricField<Type, fvPatchField, volMesh>& vf,
			const tmp<surfaceScalarField>& tlambdas
		)
	{
		if (surfaceInterpolation::debug)
		{
			Info << "surfaceInterpolationScheme<Type>::interpolate"
				"(const GeometricField<Type, fvPatchField, volMesh>&, "
				"const tmp<surfaceScalarField>&) : "
				"interpolating "
				<< vf.type() << " "
				<< vf.name()
				<< " from cells to faces "
				"without explicit correction"
				<< endl;
		}

		const surfaceScalarField& lambdas = tlambdas();

		const Field<Type>& vfi = vf.internalField();
		const scalarField& lambda = lambdas.internalField();

		const fvMesh& mesh = vf.mesh();
		const unallocLabelList& P = mesh.owner();
		const unallocLabelList& N = mesh.neighbour();

		tmp<GeometricField<Type, fvsPatchField, surfaceMesh> > tsf
		(
			new GeometricField<Type, fvsPatchField, surfaceMesh>
			(
				IOobject
				(
					"interpolate(" + vf.name() + ')',
					vf.instance(),
					vf.db()
				),
				mesh,
				vf.dimensions()
				)
		);
		GeometricField<Type, fvsPatchField, surfaceMesh>& sf = tsf();

		Field<Type>& sfi = sf.internalField();

		for (register label fi = 0; fi < P.size(); fi++)
		{
			sfi[fi] = lambda[fi] * (vfi[P[fi]] - vfi[N[fi]]) + vfi[N[fi]];
		}

		// Interpolate across coupled patches using given lambdas
		// Code moved under virtual functions into fvPatchField
		// HJ, 13/Jun/2013
		forAll(vf.boundaryField(), patchI)
		{
			vf.boundaryField()[patchI].patchInterpolate
			(
				sf,
				lambdas.boundaryField()[patchI]
			);
		}

		tlambdas.clear();

		return tsf;
	}


	//- Return the face-interpolate of the given cell field
	//  with explicit correction
	template<class Type>
	tmp<GeometricField<Type, fvsPatchField, surfaceMesh> >
		surfaceInterpolationScheme<Type>::interpolate
		(
			const GeometricField<Type, fvPatchField, volMesh>& vf
		) const
	{
		if (surfaceInterpolation::debug)
		{
			Info << "surfaceInterpolationScheme<Type>::interpolate"
				"(const GeometricField<Type, fvPatchField, volMesh>&) : "
				"interpolating "
				<< vf.type() << " "
				<< vf.name()
				<< " from cells to faces"
				<< endl;
		}

		tmp<GeometricField<Type, fvsPatchField, surfaceMesh> > tsf
			= interpolate(vf, weights(vf));

		if (corrected())
		{
			tsf() += correction(vf);
		}

		return tsf;
	}


	//- Return the face-interpolate of the given cell field
	//  with explicit correction
	template<class Type>
	tmp<GeometricField<Type, fvsPatchField, surfaceMesh> >
		surfaceInterpolationScheme<Type>::interpolate
		(
			const tmp<GeometricField<Type, fvPatchField, volMesh> >& tvf
		) const
	{
		tmp<GeometricField<Type, fvsPatchField, surfaceMesh> > tinterpVf
			= interpolate(tvf());
		tvf.clear();
		return tinterpVf;
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //