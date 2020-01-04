#include <primitiveMesh.hxx>

#include <ListOps.hxx>

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const tnbLib::labelListList& tnbLib::primitiveMesh::cellPoints() const
{
	if (!cpPtr_)
	{
		if (debug)
		{
			Pout << "primitiveMesh::cellPoints() : "
				<< "calculating cellPoints" << endl;

			if (debug == -1)
			{
				// For checking calls:abort so we can quickly hunt down
				// origin of call
				FatalErrorIn("primitiveMesh::cellPoints()")
					<< abort(FatalError);
			}
		}

		// Invert pointCells
		cpPtr_ = new labelListList(nCells());
		invertManyToMany(nCells(), pointCells(), *cpPtr_);
	}

	return *cpPtr_;
}


const tnbLib::labelList& tnbLib::primitiveMesh::cellPoints
(
	const label cellI,
	dynamicLabelList& storage
) const
{
	if (hasCellPoints())
	{
		return cellPoints()[cellI];
	}
	else
	{
		const faceList& fcs = faces();
		const labelList& cFaces = cells()[cellI];

		labelSet_.clear();

		forAll(cFaces, i)
		{
			const labelList& f = fcs[cFaces[i]];

			forAll(f, fp)
			{
				labelSet_.insert(f[fp]);
			}
		}

		storage.clear();
		if (labelSet_.size() > storage.capacity())
		{
			storage.setCapacity(labelSet_.size());
		}

		forAllConstIter(labelHashSet, labelSet_, iter)
		{
			storage.append(iter.key());
		}

		return storage;
	}
}


const tnbLib::labelList& tnbLib::primitiveMesh::cellPoints(const label cellI) const
{
	return cellPoints(cellI, labels_);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //