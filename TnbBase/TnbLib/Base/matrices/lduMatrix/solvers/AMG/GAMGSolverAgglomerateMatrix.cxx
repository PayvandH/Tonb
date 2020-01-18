#include <GAMGSolver.hxx>

#include <AMGInterfaceField.hxx>

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void tnbLib::GAMGSolver::agglomerateMatrix(const label fineLevelIndex)
{
	// Get fine matrix
	const lduMatrix& fineMatrix = matrixLevel(fineLevelIndex);

	// Set the coarse level matrix
	matrixLevels_.set
	(
		fineLevelIndex,
		new lduMatrix(agglomeration_.meshLevel(fineLevelIndex + 1))
	);
	lduMatrix& coarseMatrix = matrixLevels_[fineLevelIndex];

	// Get face restriction map for current level
	const labelList& faceRestrictAddr =
		agglomeration_.faceRestrictAddressing(fineLevelIndex);

	// Coarse matrix diagonal initialised by restricting the fine mesh diagonal
	scalarField& coarseDiag = coarseMatrix.diag();
	agglomeration_.restrictField
	(
		coarseDiag,
		fineMatrix.diag(),
		fineLevelIndex
	);

	// Get reference to fine-level interfaces
	const lduInterfaceFieldPtrsList& fineInterfaces =
		interfaceLevel(fineLevelIndex);

	// Get reference to fine-level boundary coefficients
	const FieldField<Field, scalar>& fineInterfaceBouCoeffs =
		coupleBouCoeffsLevel(fineLevelIndex);

	// Get reference to fine-level internal coefficients
	const FieldField<Field, scalar>& fineInterfaceIntCoeffs =
		coupleIntCoeffsLevel(fineLevelIndex);


	// Create coarse-level interfaces
	interfaceLevels_.set
	(
		fineLevelIndex,
		new lduInterfaceFieldPtrsList(fineInterfaces.size())
	);

	lduInterfaceFieldPtrsList& coarseInterfaceFields =
		interfaceLevels_[fineLevelIndex];

	// Set coarse-level boundary coefficients
	coupleLevelsBouCoeffs_.set
	(
		fineLevelIndex,
		new FieldField<Field, scalar>(fineInterfaces.size())
	);
	FieldField<Field, scalar>& coarseInterfaceBouCoeffs =
		coupleLevelsBouCoeffs_[fineLevelIndex];

	// Set coarse-level internal coefficients
	coupleLevelsIntCoeffs_.set
	(
		fineLevelIndex,
		new FieldField<Field, scalar>(fineInterfaces.size())
	);
	FieldField<Field, scalar>& coarseInterfaceIntCoeffs =
		coupleLevelsIntCoeffs_[fineLevelIndex];

	// Add the coarse level
	forAll(fineInterfaces, inti)
	{
		if (fineInterfaces.set(inti))
		{
			const AMGInterface& coarseInterface =
				refCast<const AMGInterface>
				(
					agglomeration_.interfaceLevel(fineLevelIndex + 1)[inti]
					);

			coarseInterfaceFields.set
			(
				inti,
				AMGInterfaceField::New
				(
					coarseInterface,
					fineInterfaces[inti]
				).ptr()
			);

			coarseInterfaceBouCoeffs.set
			(
				inti,
				coarseInterface.agglomerateCoeffs
				(
					fineInterfaceBouCoeffs[inti]
				)
			);

			coarseInterfaceIntCoeffs.set
			(
				inti,
				coarseInterface.agglomerateCoeffs
				(
					fineInterfaceIntCoeffs[inti]
				)
			);
		}
	}


	// Check if matrix is assymetric and if so agglomerate both upper and lower
	// coefficients ...
	if (fineMatrix.hasLower())
	{
		// Get off-diagonal matrix coefficients
		const scalarField& fineUpper = fineMatrix.upper();
		const scalarField& fineLower = fineMatrix.lower();

		// Coarse matrix upper coefficients
		scalarField& coarseUpper = coarseMatrix.upper();
		scalarField& coarseLower = coarseMatrix.lower();

		const labelList& restrictAddr =
			agglomeration_.restrictAddressing(fineLevelIndex);

		const unallocLabelList& l = fineMatrix.lduAddr().lowerAddr();
		const unallocLabelList& cl = coarseMatrix.lduAddr().lowerAddr();
		const unallocLabelList& cu = coarseMatrix.lduAddr().upperAddr();

		forAll(faceRestrictAddr, fineFacei)
		{
			label cFace = faceRestrictAddr[fineFacei];

			if (cFace >= 0)
			{
				// Check the orientation of the fine-face relative to the
				// coarse face it is being agglomerated into
				if (cl[cFace] == restrictAddr[l[fineFacei]])
				{
					coarseUpper[cFace] += fineUpper[fineFacei];
					coarseLower[cFace] += fineLower[fineFacei];
				}
				else if (cu[cFace] == restrictAddr[l[fineFacei]])
				{
					coarseUpper[cFace] += fineLower[fineFacei];
					coarseLower[cFace] += fineUpper[fineFacei];
				}
				else
				{
					FatalErrorIn
					(
						"GAMGSolver::agglomerateMatrix(const label)"
					) << "Inconsistent addressing between "
						"fine and coarse grids"
						<< exit(FatalError);
				}
			}
			else
			{
				// Add the fine face coefficients into the diagonal.
				coarseDiag[-1 - cFace] +=
					fineUpper[fineFacei] + fineLower[fineFacei];
			}
		}
	}
	else // ... Otherwise it is symmetric so agglomerate just the upper
	{
		// Get off-diagonal matrix coefficients
		const scalarField& fineUpper = fineMatrix.upper();

		// Coarse matrix upper coefficients
		scalarField& coarseUpper = coarseMatrix.upper();

		forAll(faceRestrictAddr, fineFacei)
		{
			label cFace = faceRestrictAddr[fineFacei];

			if (cFace >= 0)
			{
				coarseUpper[cFace] += fineUpper[fineFacei];
			}
			else
			{
				// Add the fine face coefficient into the diagonal.
				coarseDiag[-1 - cFace] += 2 * fineUpper[fineFacei];
			}
		}
	}
}


// ************************************************************************* //