#include <boundedBackwardDdtScheme.hxx>

#include <surfaceInterpolate.hxx>
#include <volFields.hxx>
#include <fvcDiv.hxx>
#include <fvMesh.hxx>
#include <tnbTime.hxx>
#include <FieldFieldFunctions.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	namespace fv
	{

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		scalar boundedBackwardDdtScheme::deltaT_() const
		{
			return mesh().time().deltaT().value();
		}


		scalar boundedBackwardDdtScheme::deltaT0_() const
		{
			return mesh().time().deltaT0().value();
		}


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

		tmp<volScalarField>
			boundedBackwardDdtScheme::fvcDdt
			(
				const dimensionedScalar& dt
			)
		{
			// No change compared to backward

			dimensionedScalar rDeltaT = 1.0 / mesh().time().deltaT();

			IOobject ddtIOobject
			(
				"ddt(" + dt.name() + ')',
				mesh().time().timeName(),
				mesh()
			);

			scalar deltaT = deltaT_();
			scalar deltaT0 = deltaT0_();

			scalar coefft = 1 + deltaT / (deltaT + deltaT0);
			scalar coefft00 = deltaT * deltaT / (deltaT0*(deltaT + deltaT0));
			scalar coefft0 = coefft + coefft00;

			if (mesh().moving())
			{
				tmp<volScalarField> tdtdt
				(
					new volScalarField
					(
						ddtIOobject,
						mesh(),
						dimensionedScalar
						(
							"0",
							dt.dimensions() / dimTime,
							0.0
						)
					)
				);

				tdtdt().internalField() = rDeltaT.value()*dt.value()*
					(
						coefft - (coefft0*mesh().V0() - coefft00 * mesh().V00()) / mesh().V()
						);

				return tdtdt;
			}
			else
			{
				return tmp<volScalarField>
					(
						new volScalarField
						(
							ddtIOobject,
							mesh(),
							dimensionedScalar
							(
								"0",
								dt.dimensions() / dimTime,
								0.0
							),
							calculatedFvPatchScalarField::typeName
						)
						);
			}
		}


		tmp<volScalarField>
			boundedBackwardDdtScheme::fvcDdt
			(
				const volScalarField& vf
			)
		{
			dimensionedScalar rDeltaT = 1.0 / mesh().time().deltaT();

			IOobject ddtIOobject
			(
				"ddt(" + vf.name() + ')',
				mesh().time().timeName(),
				mesh()
			);

			scalar deltaT = deltaT_();
			scalar deltaT0 = deltaT0_(vf);

			// Calculate unboundedness indicator
			// Note: all times moved by one because access to internal field
			// copies current field into the old-time level.
			volScalarField phict =
				mag
				(
					vf.oldTime().oldTime()
					- vf.oldTime().oldTime().oldTime()
				) /
				(
					mag
					(
						vf.oldTime()
						- vf.oldTime().oldTime()
					)
					+ dimensionedScalar("small", vf.dimensions(), VSMALL)
					);

			volScalarField limiter(pos(phict) - pos(phict - scalar(1)));

			volScalarField coefft = scalar(1) + limiter * deltaT / (deltaT + deltaT0);
			volScalarField coefft00 = limiter * sqr(deltaT) / (deltaT0*(deltaT + deltaT0));
			volScalarField coefft0 = coefft + coefft00;

			if (mesh().moving())
			{
				return tmp<volScalarField>
					(
						new volScalarField
						(
							ddtIOobject,
							mesh(),
							rDeltaT.dimensions()*vf.dimensions(),
							rDeltaT.value()*
							(
								coefft*vf.internalField() -
								(
									coefft0.internalField()
									*vf.oldTime().internalField()*mesh().V0()
									- coefft00.internalField()
									*vf.oldTime().oldTime().internalField()
									*mesh().V00()
									) / mesh().V()
								),
							rDeltaT.value()*
							(
								coefft.boundaryField()*vf.boundaryField() -
								(
									coefft0.boundaryField()*
									vf.oldTime().boundaryField()
									- coefft00.boundaryField()*
									vf.oldTime().oldTime().boundaryField()
									)
								)
						)
						);
			}
			else
			{
				return tmp<volScalarField>
					(
						new volScalarField
						(
							ddtIOobject,
							rDeltaT*
							(
								coefft*vf
								- coefft0 * vf.oldTime()
								+ coefft00 * vf.oldTime().oldTime()
								)
						)
						);
			}
		}


		tmp<volScalarField>
			boundedBackwardDdtScheme::fvcDdt
			(
				const dimensionedScalar& rho,
				const volScalarField& vf
			)
		{
			dimensionedScalar rDeltaT = 1.0 / mesh().time().deltaT();

			IOobject ddtIOobject
			(
				"ddt(" + rho.name() + ',' + vf.name() + ')',
				mesh().time().timeName(),
				mesh()
			);

			scalar deltaT = deltaT_();
			scalar deltaT0 = deltaT0_(vf);

			// Calculate unboundedness indicator
			// Note: all times moved by one because access to internal field
			// copies current field into the old-time level.
			volScalarField phict =
				mag
				(
					vf.oldTime().oldTime()
					- vf.oldTime().oldTime().oldTime()
				) /
				(
					mag
					(
						vf.oldTime()
						- vf.oldTime().oldTime()
					)
					+ dimensionedScalar("small", vf.dimensions(), VSMALL)
					);

			volScalarField limiter(pos(phict) - pos(phict - scalar(1)));

			volScalarField coefft = scalar(1) + limiter * deltaT / (deltaT + deltaT0);
			volScalarField coefft00 = limiter * sqr(deltaT) / (deltaT0*(deltaT + deltaT0));
			volScalarField coefft0 = coefft + coefft00;

			if (mesh().moving())
			{
				return tmp<volScalarField>
					(
						new volScalarField
						(
							ddtIOobject,
							mesh(),
							rDeltaT.dimensions()*rho.dimensions()*vf.dimensions(),
							rDeltaT.value()*rho.value()*
							(
								coefft*vf.internalField() -
								(
									coefft0.internalField()*
									vf.oldTime().internalField()*mesh().V0()
									- coefft00.internalField()*
									vf.oldTime().oldTime().internalField()
									*mesh().V00()
									) / mesh().V()
								),
							rDeltaT.value()*rho.value()*
							(
								coefft.boundaryField()*vf.boundaryField() -
								(
									coefft0.boundaryField()*
									vf.oldTime().boundaryField()
									- coefft00.boundaryField()*
									vf.oldTime().oldTime().boundaryField()
									)
								)
						)
						);
			}
			else
			{
				return tmp<volScalarField>
					(
						new volScalarField
						(
							ddtIOobject,
							rDeltaT*rho*
							(
								coefft*vf
								- coefft0 * vf.oldTime()
								+ coefft00 * vf.oldTime().oldTime()
								)
						)
						);
			}
		}


		tmp<volScalarField>
			boundedBackwardDdtScheme::fvcDdt
			(
				const volScalarField& rho,
				const volScalarField& vf
			)
		{
			dimensionedScalar rDeltaT = 1.0 / mesh().time().deltaT();

			IOobject ddtIOobject
			(
				"ddt(" + rho.name() + ',' + vf.name() + ')',
				mesh().time().timeName(),
				mesh()
			);

			scalar deltaT = deltaT_();
			scalar deltaT0 = deltaT0_(vf);

			// Calculate unboundedness indicator
			// Note: all times moved by one because access to internal field
			// copies current field into the old-time level.
			volScalarField phict =
				mag
				(
					rho.oldTime().oldTime()*vf.oldTime().oldTime()
					- rho.oldTime().oldTime().oldTime()*vf.oldTime().oldTime().oldTime()
				) /
				(
					mag
					(
						rho.oldTime()*vf.oldTime()
						- rho.oldTime().oldTime()*vf.oldTime().oldTime()
					)
					+ dimensionedScalar("small", rho.dimensions()*vf.dimensions(), VSMALL)
					);

			volScalarField limiter(pos(phict) - pos(phict - scalar(1)));

			volScalarField coefft = scalar(1) + limiter * deltaT / (deltaT + deltaT0);
			volScalarField coefft00 = limiter * sqr(deltaT) / (deltaT0*(deltaT + deltaT0));
			volScalarField coefft0 = coefft + coefft00;

			if (mesh().moving())
			{
				return tmp<volScalarField>
					(
						new volScalarField
						(
							ddtIOobject,
							mesh(),
							rDeltaT.dimensions()*rho.dimensions()*vf.dimensions(),
							rDeltaT.value()*
							(
								coefft*rho.internalField()*vf.internalField() -
								(
									coefft0.internalField()*
									rho.oldTime().internalField()*
									vf.oldTime().internalField()*mesh().V0()
									- coefft00.internalField()*
									rho.oldTime().oldTime().internalField()
									*vf.oldTime().oldTime().internalField()*mesh().V00()
									) / mesh().V()
								),
							rDeltaT.value()*
							(
								coefft.boundaryField()*vf.boundaryField() -
								(
									coefft0.boundaryField()*
									rho.oldTime().boundaryField()*
									vf.oldTime().boundaryField()
									- coefft00.boundaryField()*
									rho.oldTime().oldTime().boundaryField()*
									vf.oldTime().oldTime().boundaryField()
									)
								)
						)
						);
			}
			else
			{
				return tmp<volScalarField>
					(
						new volScalarField
						(
							ddtIOobject,
							rDeltaT*
							(
								coefft*rho*vf
								- coefft0 * rho.oldTime()*vf.oldTime()
								+ coefft00 * rho.oldTime().oldTime()*vf.oldTime().oldTime()
								)
						)
						);
			}
		}


		tmp<fvScalarMatrix>
			boundedBackwardDdtScheme::fvmDdt
			(
				const volScalarField& vf
			)
		{
			tmp<fvScalarMatrix> tfvm
			(
				new fvScalarMatrix
				(
					vf,
					vf.dimensions()*dimVol / dimTime
				)
			);

			fvScalarMatrix& fvm = tfvm();

			scalar rDeltaT = 1.0 / deltaT_();

			scalar deltaT = deltaT_();
			scalar deltaT0 = deltaT0_(vf);

			// Calculate unboundedness indicator
			// Note: all times moved by one because access to internal field
			// copies current field into the old-time level.
			scalarField phict =
				mag
				(
					vf.oldTime().oldTime().internalField()
					- vf.oldTime().oldTime().oldTime().internalField()
				) /
				(
					mag
					(
						vf.oldTime().internalField()
						- vf.oldTime().oldTime().internalField()
					)
					+ VSMALL
					);

			scalarField limiter(pos(phict) - pos(phict - 1.0));

			scalarField coefft = 1.0 + limiter * deltaT / (deltaT + deltaT0);
			scalarField coefft00 = limiter * deltaT*deltaT / (deltaT0*(deltaT + deltaT0));
			scalarField coefft0 = coefft + coefft00;

			fvm.diag() = (coefft*rDeltaT)*mesh().V();

			if (mesh().moving())
			{
				fvm.source() = rDeltaT *
					(
						coefft0*vf.oldTime().internalField()*mesh().V0()
						- coefft00 * vf.oldTime().oldTime().internalField()
						*mesh().V00()
						);
			}
			else
			{
				fvm.source() = rDeltaT * mesh().V()*
					(
						coefft0*vf.oldTime().internalField()
						- coefft00 * vf.oldTime().oldTime().internalField()
						);
			}

			return tfvm;
		}


		tmp<fvScalarMatrix>
			boundedBackwardDdtScheme::fvmDdt
			(
				const dimensionedScalar& rho,
				const volScalarField& vf
			)
		{
			tmp<fvScalarMatrix> tfvm
			(
				new fvScalarMatrix
				(
					vf,
					rho.dimensions()*vf.dimensions()*dimVol / dimTime
				)
			);
			fvScalarMatrix& fvm = tfvm();

			scalar rDeltaT = 1.0 / deltaT_();

			scalar deltaT = deltaT_();
			scalar deltaT0 = deltaT0_(vf);

			// Calculate unboundedness indicator
			// Note: all times moved by one because access to internal field
			// copies current field into the old-time level.
			scalarField phict =
				mag
				(
					vf.oldTime().oldTime().internalField()
					- vf.oldTime().oldTime().oldTime().internalField()
				) /
				(
					mag
					(
						vf.oldTime().internalField()
						- vf.oldTime().oldTime().internalField()
					)
					+ VSMALL
					);

			scalarField limiter(pos(phict) - pos(phict - 1.0));

			scalarField coefft = 1.0 + limiter * deltaT / (deltaT + deltaT0);
			scalarField coefft00 = limiter * deltaT*deltaT / (deltaT0*(deltaT + deltaT0));
			scalarField coefft0 = coefft + coefft00;

			fvm.diag() = (coefft*rDeltaT*rho.value())*mesh().V();

			if (mesh().moving())
			{
				fvm.source() = rDeltaT * rho.value()*
					(
						coefft0*vf.oldTime().internalField()*mesh().V0()
						- coefft00 * vf.oldTime().oldTime().internalField()
						*mesh().V00()
						);
			}
			else
			{
				fvm.source() = rDeltaT * mesh().V()*rho.value()*
					(
						coefft0*vf.oldTime().internalField()
						- coefft00 * vf.oldTime().oldTime().internalField()
						);
			}

			return tfvm;
		}


		tmp<fvScalarMatrix>
			boundedBackwardDdtScheme::fvmDdt
			(
				const volScalarField& rho,
				const volScalarField& vf
			)
		{
			tmp<fvScalarMatrix> tfvm
			(
				new fvScalarMatrix
				(
					vf,
					rho.dimensions()*vf.dimensions()*dimVol / dimTime
				)
			);
			fvScalarMatrix& fvm = tfvm();

			scalar rDeltaT = 1.0 / deltaT_();

			scalar deltaT = deltaT_();
			scalar deltaT0 = deltaT0_(vf);

			// Calculate unboundedness indicator
			// Note: all times moved by one because access to internal field
			// copies current field into the old-time level.
			scalarField phict =
				mag
				(
					rho.oldTime().oldTime().internalField()*
					vf.oldTime().oldTime().internalField()
					- rho.oldTime().oldTime().oldTime().internalField()*
					vf.oldTime().oldTime().oldTime().internalField()
				) /
				(
					mag
					(
						rho.oldTime().internalField()*
						vf.oldTime().internalField()
						- rho.oldTime().oldTime().internalField()*
						vf.oldTime().oldTime().internalField()
					)
					+ VSMALL
					);

			scalarField limiter(pos(phict) - pos(phict - 1.0));

			scalarField coefft = 1.0 + limiter * deltaT / (deltaT + deltaT0);
			scalarField coefft00 = limiter * deltaT*deltaT / (deltaT0*(deltaT + deltaT0));
			scalarField coefft0 = coefft + coefft00;

			fvm.diag() = (coefft*rDeltaT)*rho.internalField()*mesh().V();

			if (mesh().moving())
			{
				fvm.source() = rDeltaT *
					(
						coefft0*rho.oldTime().internalField()
						*vf.oldTime().internalField()*mesh().V0()
						- coefft00 * rho.oldTime().oldTime().internalField()
						*vf.oldTime().oldTime().internalField()*mesh().V00()
						);
			}
			else
			{
				fvm.source() = rDeltaT * mesh().V()*
					(
						coefft0*rho.oldTime().internalField()
						*vf.oldTime().internalField()
						- coefft00 * rho.oldTime().oldTime().internalField()
						*vf.oldTime().oldTime().internalField()
						);
			}

			return tfvm;
		}


		tmp<surfaceScalarField> boundedBackwardDdtScheme::fvcDdtPhiCorr
		(
			const volScalarField& rA,
			const volScalarField& U,
			const surfaceScalarField& phi
		)
		{
			notImplemented
			(
				"boundedBackwardDdtScheme::fvcDdtPhiCorr"
			);

			return surfaceScalarField::null();
		}


		tmp<surfaceScalarField> boundedBackwardDdtScheme::fvcDdtPhiCorr
		(
			const volScalarField& rA,
			const volScalarField& rho,
			const volScalarField& U,
			const surfaceScalarField& phi
		)
		{
			notImplemented
			(
				"boundedBackwardDdtScheme::fvcDdtPhiCorr"
			);

			return surfaceScalarField::null();
		}


		tmp<surfaceScalarField> boundedBackwardDdtScheme::meshPhi
		(
			const volScalarField& vf
		)
		{
			scalar deltaT = deltaT_();
			scalar deltaT0 = deltaT0_(vf);

			scalar coefft = 1 + deltaT / (deltaT + deltaT0);
			scalar coefft00 = deltaT / (deltaT + deltaT0);

			return coefft * mesh().phi() - coefft00 * mesh().phi().oldTime();
		}


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	} // End namespace fv

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //