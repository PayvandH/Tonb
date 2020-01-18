#include <faceZone.hxx>

#include <faceZoneMesh.hxx>
#include <polyMesh.hxx>
#include <primitiveMesh.hxx>
#include <demandDrivenData.hxx>
#include <cyclicPolyPatch.hxx>
#include <mapPolyMesh.hxx>
#include <syncTools.hxx>
#include <PstreamReduceOps.hxx>
#include <addToRunTimeSelectionTable.hxx>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
	defineTypeNameAndDebug(faceZone, 0);
	defineRunTimeSelectionTable(faceZone, dictionary);
	addToRunTimeSelectionTable(faceZone, faceZone, dictionary);
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void tnbLib::faceZone::calcFaceZonePatch() const
{
	if (debug)
	{
		Info << "void faceZone::calcFaceZonePatch() const : "
			<< "Calculating primitive patch"
			<< endl;
	}

	if (patchPtr_)
	{
		FatalErrorIn
		(
			"void faceZone::calcFaceZonePatch() const"
		) << "primitive face zone patch already calculated"
			<< abort(FatalError);
	}

	patchPtr_ =
		new primitiveFacePatch
		(
			faceList(size()),
			zoneMesh().mesh().allPoints()
		);

	primitiveFacePatch& patch = *patchPtr_;

	const faceList& f = zoneMesh().mesh().allFaces();

	const labelList& addr = *this;
	const boolList& flip = flipMap();

	forAll(addr, faceI)
	{
		if (flip[faceI])
		{
			patch[faceI] = f[addr[faceI]].reverseFace();
		}
		else
		{
			patch[faceI] = f[addr[faceI]];
		}
	}

	if (debug)
	{
		Info << "void faceZone::calcFaceZonePatch() const : "
			<< "Finished calculating primitive patch"
			<< endl;
	}
}


const tnbLib::Map<tnbLib::label>& tnbLib::faceZone::faceLookupMap() const
{
	if (!faceLookupMapPtr_)
	{
		calcFaceLookupMap();
	}

	return *faceLookupMapPtr_;
}


void tnbLib::faceZone::calcFaceLookupMap() const
{
	if (debug)
	{
		Info << "void faceZone::calcFaceLookupMap() const : "
			<< "Calculating face lookup map"
			<< endl;
	}

	if (faceLookupMapPtr_)
	{
		FatalErrorIn
		(
			"void faceZone::calcFaceLookupMap() const"
		) << "face lookup map already calculated"
			<< abort(FatalError);
	}

	const labelList& addr = *this;

	faceLookupMapPtr_ = new Map<label>(2 * addr.size());
	Map<label>& flm = *faceLookupMapPtr_;

	forAll(addr, faceI)
	{
		flm.insert(addr[faceI], faceI);
	}

	if (debug)
	{
		Info << "void faceZone::calcFaceLookupMap() const : "
			<< "Finished calculating face lookup map"
			<< endl;
	}
}


void tnbLib::faceZone::calcCellLayers() const
{
	if (debug)
	{
		Info << "void tnbLib::faceZone::calcCellLayers() const : "
			<< "calculating master cells"
			<< endl;
	}

	// It is an error to attempt to recalculate edgeCells
	// if the pointer is already set
	if (masterCellsPtr_ || slaveCellsPtr_)
	{
		FatalErrorIn("void faceZone::calcCellLayers() const")
			<< "cell layers already calculated"
			<< abort(FatalError);
	}
	else
	{
		// Go through all the faces in the master zone.  Choose the
		// master or slave cell based on the face flip
		const polyMesh& mesh = zoneMesh().mesh();

		const labelList& own = mesh.faceOwner();
		const labelList& nei = mesh.faceNeighbour();

		const labelList& mf = *this;

		const boolList& faceFlip = flipMap();

		masterCellsPtr_ = new labelList(mf.size());
		labelList& mc = *masterCellsPtr_;

		slaveCellsPtr_ = new labelList(mf.size());
		labelList& sc = *slaveCellsPtr_;

		forAll(mf, faceI)
		{
			label curMc = -1;
			label curSc = -1;

			if (!faceFlip[faceI])
			{
				// Face is oriented correctly, no flip needed
				// Master is the neighbour, with normal pointing into it
				// Bug fix, HJ, 7/Mar/2011
				if (mesh.isInternalFace(mf[faceI]))
				{
					curMc = nei[mf[faceI]];
					curSc = own[mf[faceI]];
				}
				else if (mf[faceI] < mesh.nFaces())
				{
					curMc = -1;
					curSc = own[mf[faceI]];
				}
			}
			else
			{
				// Face flip
				// Master is the owner, with normal pointing into it
				// Bug fix, HJ, 7/Mar/2011
				if (mesh.isInternalFace(mf[faceI]))
				{
					curMc = own[mf[faceI]];
					curSc = nei[mf[faceI]];
				}
				else if (mf[faceI] < mesh.nFaces())
				{
					curMc = own[mf[faceI]];
					curSc = -1;
				}

			}

			mc[faceI] = curMc;
			sc[faceI] = curSc;
		}
	}
}


void tnbLib::faceZone::checkAddressing() const
{
	if (size() != flipMap_.size())
	{
		FatalErrorIn("void tnbLib::faceZone::checkAddressing() const")
			<< "Different sizes of the addressing and flipMap arrays.  "
			<< "Size of addressing: " << size()
			<< " size of flip map: " << flipMap_.size()
			<< abort(FatalError);
	}
}


void tnbLib::faceZone::clearAddressing()
{
	deleteDemandDrivenData(patchPtr_);

	deleteDemandDrivenData(masterCellsPtr_);
	deleteDemandDrivenData(slaveCellsPtr_);

	deleteDemandDrivenData(mePtr_);
	deleteDemandDrivenData(faceLookupMapPtr_);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from components
tnbLib::faceZone::faceZone
(
	const word& name,
	const labelList& addr,
	const boolList& fm,
	const label index,
	const faceZoneMesh& zm
)
	:
	labelList(addr),
	name_(name),
	flipMap_(fm),
	index_(index),
	zoneMesh_(zm),
	patchPtr_(NULL),
	masterCellsPtr_(NULL),
	slaveCellsPtr_(NULL),
	mePtr_(NULL),
	faceLookupMapPtr_(NULL)
{
	checkAddressing();
}


tnbLib::faceZone::faceZone
(
	const word& name,
	const Xfer<labelList>& addr,
	const Xfer<boolList>& fm,
	const label index,
	const faceZoneMesh& zm
)
	:
	labelList(addr),
	name_(name),
	flipMap_(fm),
	index_(index),
	zoneMesh_(zm),
	patchPtr_(NULL),
	masterCellsPtr_(NULL),
	slaveCellsPtr_(NULL),
	mePtr_(NULL),
	faceLookupMapPtr_(NULL)
{
	checkAddressing();
}


// Construct from dictionary
tnbLib::faceZone::faceZone
(
	const word& name,
	const dictionary& dict,
	const label index,
	const faceZoneMesh& zm
)
	:
	labelList(dict.lookup("faceLabels")),
	name_(name),
	flipMap_(dict.lookup("flipMap")),
	index_(index),
	zoneMesh_(zm),
	patchPtr_(NULL),
	masterCellsPtr_(NULL),
	slaveCellsPtr_(NULL),
	mePtr_(NULL),
	faceLookupMapPtr_(NULL)
{
	checkAddressing();
}


// Construct given the original zone and resetting the
// face list and zone mesh information
tnbLib::faceZone::faceZone
(
	const faceZone& fz,
	const labelList& addr,
	const boolList& fm,
	const label index,
	const faceZoneMesh& zm
)
	:
	labelList(addr),
	name_(fz.name()),
	flipMap_(fm),
	index_(index),
	zoneMesh_(zm),
	patchPtr_(NULL),
	masterCellsPtr_(NULL),
	slaveCellsPtr_(NULL),
	mePtr_(NULL),
	faceLookupMapPtr_(NULL)
{
	checkAddressing();
}


tnbLib::faceZone::faceZone
(
	const faceZone& fz,
	const Xfer<labelList>& addr,
	const Xfer<boolList>& fm,
	const label index,
	const faceZoneMesh& zm
)
	:
	labelList(addr),
	name_(fz.name()),
	flipMap_(fm),
	index_(index),
	zoneMesh_(zm),
	patchPtr_(NULL),
	masterCellsPtr_(NULL),
	slaveCellsPtr_(NULL),
	mePtr_(NULL),
	faceLookupMapPtr_(NULL)
{
	checkAddressing();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

tnbLib::faceZone::~faceZone()
{
	clearAddressing();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

tnbLib::label tnbLib::faceZone::whichFace(const label globalFaceID) const
{
	const Map<label>& flm = faceLookupMap();

	Map<label>::const_iterator flmIter = flm.find(globalFaceID);

	if (flmIter == flm.end())
	{
		return -1;
	}
	else
	{
		return flmIter();
	}
}


const tnbLib::faceZoneMesh& tnbLib::faceZone::zoneMesh() const
{
	return zoneMesh_;
}


const tnbLib::primitiveFacePatch& tnbLib::faceZone::operator()() const
{
	if (!patchPtr_)
	{
		calcFaceZonePatch();
	}

	return *patchPtr_;
}


const tnbLib::labelList& tnbLib::faceZone::masterCells() const
{
	if (!masterCellsPtr_)
	{
		calcCellLayers();
	}

	return *masterCellsPtr_;
}


const tnbLib::labelList& tnbLib::faceZone::slaveCells() const
{
	if (!slaveCellsPtr_)
	{
		calcCellLayers();
	}

	return *slaveCellsPtr_;
}


const tnbLib::labelList& tnbLib::faceZone::meshEdges() const
{
	if (!mePtr_)
	{
		// Old form.  Under testing: merge, HJ, 17/Aug/2010
		//labelList faceCells(size());
		//
		//const labelList& own = zoneMesh().mesh().faceOwner();
		//
		//const labelList& faceLabels = *this;
		//
		//forAll (faceCells, faceI)
		//{
		//    faceCells[faceI] = own[faceLabels[faceI]];
		//}
		//
		//mePtr_ =
		//    new labelList
		//    (
		//        operator()().meshEdges
		//        (
		//            zoneMesh().mesh().edges(),
		//            zoneMesh().mesh().cellEdges(),
		//            faceCells
		//        )
		//    );

		mePtr_ =
			new labelList
			(
				operator()().meshEdges
				(
					zoneMesh().mesh().edges(),
					zoneMesh().mesh().pointEdges()
				)
			);
	}

	return *mePtr_;
}


void tnbLib::faceZone::resetAddressing
(
	const labelList& addr,
	const boolList& flipMap
)
{
	clearAddressing();
	labelList::operator=(addr);
	flipMap_ = flipMap;
}


void tnbLib::faceZone::updateMesh()
{
	clearAddressing();
}


bool tnbLib::faceZone::checkDefinition(const bool report) const
{
	const labelList& addr = *this;

	bool boundaryError = false;

	forAll(addr, i)
	{
		if (addr[i] < 0 || addr[i] >= zoneMesh().mesh().allFaces().size())
		{
			boundaryError = true;

			if (report)
			{
				SeriousErrorIn
				(
					"bool faceZone::checkDefinition("
					"const bool report) const"
				) << "Zone " << name()
					<< " contains invalid face label " << addr[i] << nl
					<< "Valid face labels are 0.."
					<< zoneMesh().mesh().allFaces().size() - 1 << endl;
			}
		}
	}
	return boundaryError;
}


bool tnbLib::faceZone::checkParallelSync(const bool report) const
{
	const polyMesh& mesh = zoneMesh().mesh();
	const polyBoundaryMesh& bm = mesh.boundaryMesh();

	bool boundaryError = false;


	// Check that zone faces are synced
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	{
		boolList neiZoneFace(mesh.nFaces() - mesh.nInternalFaces(), false);
		boolList neiZoneFlip(mesh.nFaces() - mesh.nInternalFaces(), false);

		forAll(*this, i)
		{
			label faceI = operator[](i);

			// Only check live faces
			if (faceI < mesh.nFaces())
			{
				if (!mesh.isInternalFace(faceI))
				{
					neiZoneFace[faceI - mesh.nInternalFaces()] = true;
					neiZoneFlip[faceI - mesh.nInternalFaces()] = flipMap()[i];
				}
			}
		}

		boolList myZoneFace(neiZoneFace);
		syncTools::swapBoundaryFaceList(mesh, neiZoneFace, false);
		boolList myZoneFlip(neiZoneFlip);
		syncTools::swapBoundaryFaceList(mesh, neiZoneFlip, false);

		forAll(*this, i)
		{
			label faceI = operator[](i);

			// Only check live faces
			if (faceI < mesh.nFaces())
			{
				label patchI = bm.whichPatch(faceI);

				if (patchI != -1 && isA<cyclicPolyPatch>(bm[patchI]))
				{
					label bFaceI = faceI - mesh.nInternalFaces();

					// Check face in zone on both sides
					if (myZoneFace[bFaceI] != neiZoneFace[bFaceI])
					{
						boundaryError = true;

						if (report)
						{
							Pout << " ***Problem with faceZone " << index()
								<< " named " << name()
								<< ". Face " << faceI
								<< " on coupled patch "
								<< bm[patchI].name()
								<< " is not consistent with its "
								<< "coupled neighbour."
								<< endl;
						}
					}

					// Flip state should be opposite.
					if (myZoneFlip[bFaceI] == neiZoneFlip[bFaceI])
					{
						boundaryError = true;

						if (report)
						{
							Pout << " ***Problem with faceZone " << index()
								<< " named " << name()
								<< ". Face " << faceI
								<< " on coupled patch "
								<< bm[patchI].name()
								<< " does not have consistent flipMap"
								<< " across coupled faces."
								<< endl;
						}
					}
				}
			}
		}
	}

	return returnReduce(boundaryError, orOp<bool>());
}


void tnbLib::faceZone::movePoints(const pointField& p)
{
	if (patchPtr_)
	{
		patchPtr_->movePoints(p);
	}
}

void tnbLib::faceZone::write(Ostream& os) const
{
	os << nl << name()
		<< nl << static_cast<const labelList&>(*this)
		<< nl << flipMap();
}


void tnbLib::faceZone::writeDict(Ostream& os) const
{
	os << nl << name() << nl << token::BEGIN_BLOCK << incrIndent << nl
		<< indent << "type " << type() << token::END_STATEMENT << nl;

	writeEntry("faceLabels", os);
	flipMap().writeEntry("flipMap", os);

	os << decrIndent << token::END_BLOCK << endl;
}


// * * * * * * * * * * * * * * * Ostream Operator  * * * * * * * * * * * * * //

tnbLib::Ostream& tnbLib::operator<<(Ostream& os, const faceZone& p)
{
	p.write(os);
	os.check("Ostream& operator<<(Ostream& f, const faceZone& p");
	return os;
}


// ************************************************************************* //