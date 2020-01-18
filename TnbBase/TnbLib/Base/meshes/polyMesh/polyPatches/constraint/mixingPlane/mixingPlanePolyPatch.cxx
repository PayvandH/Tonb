#include <mixingPlanePolyPatch.hxx>

#include <polyBoundaryMesh.hxx>
#include <addToRunTimeSelectionTable.hxx>
#include <demandDrivenData.hxx>
#include <polyPatchID.hxx>
#include <polyBoundaryMesh.hxx>
#include <polyMesh.hxx>
#include <ZoneIDs.hxx>
#include <tnbTime.hxx>
#include <SubField.hxx>
#include <IPstream.hxx>
#include <OPstream.hxx>
#include <PstreamReduceOps.hxx>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace tnbLib
{
	defineTypeNameAndDebug(mixingPlanePolyPatch, 0);

	addToRunTimeSelectionTable(polyPatch, mixingPlanePolyPatch, word);
	addToRunTimeSelectionTable(polyPatch, mixingPlanePolyPatch, dictionary);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

bool tnbLib::mixingPlanePolyPatch::active() const
{
	polyPatchID shadow(shadowName_, boundaryMesh());
	faceZoneID zone(zoneName_, boundaryMesh().mesh().faceZones());

	return shadow.active() && zone.active();
}


void tnbLib::mixingPlanePolyPatch::calcZoneAddressing() const
{
	// Calculate patch-to-zone addressing
	if (zoneAddressingPtr_)
	{
		FatalErrorIn("void mixingPlanePolyPatch::calcZoneAddressing() const")
			<< "Patch to zone addressing already calculated"
			<< abort(FatalError);
	}

	// Calculate patch-to-zone addressing
	zoneAddressingPtr_ = new labelList(size());
	labelList& zAddr = *zoneAddressingPtr_;
	const faceZone& myZone = zone();

	for (label i = 0; i < size(); i++)
	{
		zAddr[i] = myZone.whichFace(start() + i);
	}

	// Check zone addressing
	if (zAddr.size() > 0 && min(zAddr) < 0)
	{
		Info << "myZone: " << myZone << nl
			<< "my start and size: " << start() << " and " << size() << nl
			<< "zAddr: " << zAddr << endl;

		FatalErrorIn("void mixingPlanePolyPatch::calcZoneAddressing() const")
			<< "Problem with patch-to-zone addressing: some patch faces "
			<< "not found in interpolation zone"
			<< abort(FatalError);
	}
}


void tnbLib::mixingPlanePolyPatch::calcPatchToPatch() const
{
	// Create patch-to-patch interpolation
	if (patchToPatchPtr_)
	{
		FatalErrorIn("void mixingPlanePolyPatch::calcPatchToPatch() const")
			<< "Patch to patch interpolation already calculated"
			<< abort(FatalError);
	}

	if (master())
	{
		// Create dummy interpolation profile
		pointField iProfile;

		if
			(
				discretisationType_ == mixingPlaneInterpolation::USER_DEFINED
				)
		{
			Info << "Reading interpolation profile from file: "
				<< userProfileFile_ << endl;

			iProfile = vectorIOField
			(
				IOobject
				(
					userProfileFile_,
					boundaryMesh().mesh().time().constant(),
					boundaryMesh().mesh().time(),
					IOobject::MUST_READ,
					IOobject::NO_WRITE
				)
			);

			if (iProfile.empty())
			{
				FatalErrorIn
				(
					"void mixingPlanePolyPatch::calcPatchToPatch() const"
				) << "Empty user-defined mixing plane profile for patch "
					<< name() << " read from file " << userProfileFile_
					<< abort(FatalError);
			}
		}

		if (debug)
		{
			Info << "Creating mixingPlaneInterpolation for patch "
				<< name() << " with shadow " << shadowName() << nl
				<< "discretisationType = "
				<< mixingPlaneInterpolation::discretisationNames_
				[discretisationType_]
			<< " " << discretisationType_
				<< " sweepAxisType = "
				<< mixingPlaneInterpolation::sweepAxisNames_[sweepAxisType_]
				<< " " << sweepAxisType_
				<< " stackAxisType = "
				<< mixingPlaneInterpolation::stackAxisNames_[stackAxisType_]
				<< " " << stackAxisType_
				<< endl;

		}

		patchToPatchPtr_ =
			new mixingPlaneZoneInterpolation
			(
				zone()(),
				shadow().zone()(),
				csPtr_(),
				discretisationType_,
				sweepAxisType_,
				stackAxisType_,
				iProfile
			);
	}
	else
	{
		FatalErrorIn("void mixingPlanePolyPatch::calcPatchToPatch() const")
			<< "Attempting to create MixingPlaneInterpolation on a shadow"
			<< abort(FatalError);
	}

	if (debug > 1 && master())
	{
		Info << "Writing transformed mixing plane patches as VTK." << nl
			<< "Master: " << name()
			<< " Slave: " << shadowName()
			<< endl;

		const polyMesh& mesh = boundaryMesh().mesh();

		fileName fvPath(mesh.time().path() / "VTK");
		mkDir(fvPath);

		patchToPatchPtr_->mixingPlanePatch().writeVTK
		(
			fvPath / fileName
			(
				"mixingPlaneRibbon_" + name() + "_" + shadow().name()
			)
		);

		patchToPatchPtr_->transformedMasterPatch().writeVTK
		(
			fvPath / fileName
			(
				"mixingPlaneMaster_" + name() + "_" + shadow().name()
			)
		);

		patchToPatchPtr_->transformedShadowPatch().writeVTK
		(
			fvPath / fileName
			(
				"mixingPlaneShadow_" + name() + "_" + shadow().name()
			)
		);
	}
}


void tnbLib::mixingPlanePolyPatch::calcReconFaceCellCentres() const
{
	if (reconFaceCellCentresPtr_)
	{
		FatalErrorIn
		(
			"void mixingPlanePolyPatch::calcReconFaceCellCentres() const"
		) << "Reconstructed cell centres already calculated"
			<< abort(FatalError);
	}

	// Create neighbouring face centres using interpolation
	if (master())
	{
		const label shadowID = shadowIndex();

		reconFaceCellCentresPtr_ =
			new vectorField
			(
				interpolate
				(
					boundaryMesh()[shadowID].faceCellCentres()
					- boundaryMesh()[shadowID].faceCentres()
				)
				+ faceCentres()
			);
	}
	else
	{
		FatalErrorIn
		(
			"void mixingPlanePolyPatch::calcReconFaceCellCentres() const"
		) << "Attempting to create reconFaceCellCentres on a shadow"
			<< abort(FatalError);
	}
}


void tnbLib::mixingPlanePolyPatch::calcLocalParallel() const
{
	// Calculate patch-to-zone addressing
	if (localParallelPtr_)
	{
		FatalErrorIn("void mixingPlanePolyPatch::calcLocalParallel() const")
			<< "Local parallel switch already calculated"
			<< abort(FatalError);
	}

	localParallelPtr_ = new bool(false);
	bool& emptyOrComplete = *localParallelPtr_;

	// If running in serial, all mixingPlanes are expanded to zone size.
	// This happens on decomposition and reconstruction where
	// size and shadow size may be zero, but zone size may not
	// HJ, 1/Jun/2011
	if (!Pstream::parRun())
	{
		emptyOrComplete = false;
	}
	else
	{
		// Check that patch size is greater than the zone size.
		// This is an indication of the error where the face zone is not global
		// in a parallel run.  HJ, 9/Nov/2014
		if (size() > zone().size())
		{
			FatalErrorIn
			(
				"void mixingPlanePolyPatch::calcLocalParallel() const"
			) << "Patch size is greater than zone size for GGI patch "
				<< name() << ".  This is not allowerd: "
				<< "the face zone must contain all patch faces and be "
				<< "global in parallel runs"
				<< abort(FatalError);
		}

		// Calculate localisation on master and shadow
		emptyOrComplete =
			(
				zone().size() == size()
				&& shadow().zone().size() == shadow().size()
				)
			|| (size() == 0 && shadow().size() == 0);

		reduce(emptyOrComplete, andOp<bool>());
	}

	if (debug && Pstream::parRun())
	{
		Info << "mixingPlane patch Master: " << name()
			<< " Slave: " << shadowName() << " is ";

		if (emptyOrComplete)
		{
			Info << "local parallel" << endl;
		}
		else
		{
			Info << "split between multiple processors" << endl;
		}
	}
}


void tnbLib::mixingPlanePolyPatch::calcReceive() const
{
	// Note: all processors will execute calcReceive but only master will
	// hold the information.  Therefore, pointers on slave processors
	// will remain meaningless, but for purposes of consistency
	// (of the calc-call) they will be set to zero-sized array
	// HJ, 4/Jun/2011

	if (receiveAddrPtr_)
	{
		FatalErrorIn("void mixingPlanePolyPatch::calcReceive() const")
			<< "Receive addressing already calculated"
			<< abort(FatalError);
	}

	if (debug)
	{
		Pout << "mixingPlanePolyPatch::calcReceive() const for patch "
			<< index() << endl;
	}

	if (!Pstream::parRun())
	{
		FatalErrorIn("void mixingPlanePolyPatch::calcReceive() const")
			<< "Requested calculation of send-receive addressing for a "
			<< "serial run.  This is not allowed"
			<< abort(FatalError);
	}

	// Master will receive and store the maps
	if (Pstream::master())
	{
		receiveAddrPtr_ = new labelListList(Pstream::nProcs());
		labelListList& rAddr = *receiveAddrPtr_;

		// Insert master
		rAddr[0] = zoneAddressing();

		for (label procI = 1; procI < Pstream::nProcs(); procI++)
		{
			// Note: must use normal comms because the size of the
			// communicated lists is unknown on the receiving side
			// HJ, 4/Jun/2011

			// Opt: reconsider mode of communication
			IPstream ip(Pstream::scheduled, procI);

			rAddr[procI] = labelList(ip);
		}
	}
	else
	{
		// Create dummy pointers: only master processor stores maps
		receiveAddrPtr_ = new labelListList();

		// Send information to master
		const labelList& za = zoneAddressing();

		// Note: must use normal comms because the size of the
		// communicated lists is unknown on the receiving side
		// HJ, 4/Jun/2011

		// Opt: reconsider mode of communication
		OPstream op(Pstream::scheduled, Pstream::masterNo());

		// Send local and remote addressing to master
		op << za;
	}
}


const tnbLib::labelListList& tnbLib::mixingPlanePolyPatch::receiveAddr() const
{
	if (!receiveAddrPtr_)
	{
		calcReceive();
	}

	return *receiveAddrPtr_;
}


void tnbLib::mixingPlanePolyPatch::clearGeom() const
{
	deleteDemandDrivenData(reconFaceCellCentresPtr_);
}


void tnbLib::mixingPlanePolyPatch::clearOut() const
{
	clearGeom();

	shadowIndex_ = -1;
	zoneIndex_ = -1;

	// Receive maps do not depend on the local position
	deleteDemandDrivenData(receiveAddrPtr_);

	deleteDemandDrivenData(zoneAddressingPtr_);
	deleteDemandDrivenData(patchToPatchPtr_);
	deleteDemandDrivenData(localParallelPtr_);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

tnbLib::mixingPlanePolyPatch::mixingPlanePolyPatch
(
	const word& name,
	const label size,
	const label start,
	const label index,
	const polyBoundaryMesh& bm
)
	:
	coupledPolyPatch(name, size, start, index, bm),
	shadowName_(fileName::null),
	zoneName_("initializeMe"),
	csPtr_
	(
		new coordinateSystem
		(
			"mixingCS",
			vector::zero,
			vector(0, 0, 1),
			vector(1, 0, 0)
		)
	),
	discretisationType_(mixingPlaneInterpolation::USER_DEFINED),
	sweepAxisType_(mixingPlaneInterpolation::SWEEP_UNKNOWN),
	stackAxisType_(mixingPlaneInterpolation::STACK_UNKNOWN),
	userProfileFile_(fileName::null),
	shadowIndex_(-1),
	zoneIndex_(-1),
	patchToPatchPtr_(NULL),
	zoneAddressingPtr_(NULL),
	reconFaceCellCentresPtr_(NULL),
	localParallelPtr_(NULL),
	receiveAddrPtr_(NULL)
{}


tnbLib::mixingPlanePolyPatch::mixingPlanePolyPatch
(
	const word& name,
	const label size,
	const label start,
	const label index,
	const polyBoundaryMesh& bm,
	const word& shadowName,
	const word& zoneName,
	const coordinateSystem& cs,
	const mixingPlaneInterpolation::discretisation discretisationType,
	const mixingPlaneInterpolation::sweepAxis sweepAxisType,
	const mixingPlaneInterpolation::stackAxis stackAxisType,
	const fileName& userProfileFile
)
	:
	coupledPolyPatch(name, size, start, index, bm),
	shadowName_(shadowName),
	zoneName_(zoneName),
	csPtr_(cs.clone()),
	discretisationType_(discretisationType),
	sweepAxisType_(sweepAxisType),
	stackAxisType_(stackAxisType),
	userProfileFile_(fileName::null),
	shadowIndex_(-1),
	patchToPatchPtr_(NULL),
	zoneAddressingPtr_(NULL),
	reconFaceCellCentresPtr_(NULL),
	localParallelPtr_(NULL),
	receiveAddrPtr_(NULL)
{}


tnbLib::mixingPlanePolyPatch::mixingPlanePolyPatch
(
	const word& name,
	const dictionary& dict,
	const label index,
	const polyBoundaryMesh& bm
)
	:
	coupledPolyPatch(name, dict, index, bm),
	shadowName_(dict.lookup("shadowPatch")),
	zoneName_(dict.lookup("zone")),
	csPtr_
	(
		new coordinateSystem
		(
			"mixingCS",
			vector::zero,
			vector(0, 0, 1),
			vector(1, 0, 0)
		)
	),
	discretisationType_(mixingPlaneInterpolation::USER_DEFINED),
	sweepAxisType_(mixingPlaneInterpolation::SWEEP_UNKNOWN),
	stackAxisType_(mixingPlaneInterpolation::STACK_UNKNOWN),
	userProfileFile_(fileName::null),
	shadowIndex_(-1),
	patchToPatchPtr_(NULL),
	zoneAddressingPtr_(NULL),
	reconFaceCellCentresPtr_(NULL),
	localParallelPtr_(NULL),
	receiveAddrPtr_(NULL)
{
	// When construting from dictionary, only master side information will be
	// read and used.  This requires special check, because polyBoundaryMesh
	// is being filled at this point.  See fix in findPatchID.
	// HJ, and MB, 28/Jan/2011

	// Check if shadow exists.  If so, we are on the slave side
	polyPatchID shadow(shadowName_, boundaryMesh());

	if (!shadow.active())
	{
		// Master side, read additional data

		csPtr_ =
			coordinateSystem::New
			(
				"mixingCS",
				dict.subDict("coordinateSystem")
			);

		const dictionary& ribbonPatchSubDict = dict.subDict("ribbonPatch");

		discretisationType_ =
			mixingPlaneInterpolation::discretisationNames_.read
			(
				ribbonPatchSubDict.lookup("discretisation")
			);

		sweepAxisType_ =
			mixingPlaneInterpolation::sweepAxisNames_.read
			(
				ribbonPatchSubDict.lookup("sweepAxis")
			);

		stackAxisType_ =
			mixingPlaneInterpolation::stackAxisNames_.read
			(
				ribbonPatchSubDict.lookup("stackAxis")
			);

		if (discretisationType_ == mixingPlaneInterpolation::USER_DEFINED)
		{
			if (dict.found("userProfileFile"))
			{
				dict.lookup("userProfileFile") >> userProfileFile_;
			}
			else
			{
				FatalIOErrorIn
				(
					"mixingPlanePolyPatch::mixingPlanePolyPatch\n"
					"(\n"
					"    const word& name,\n"
					"    const dictionary& dict,\n"
					"    const label index,\n"
					"    const polyBoundaryMesh& bm\n"
					")",
					dict
				) << "Patch: " << name << " : Missing profile entry for "
					<< "userDefined profile"
					<< abort(FatalIOError);
			}
		}
	}
}


tnbLib::mixingPlanePolyPatch::mixingPlanePolyPatch
(
	const mixingPlanePolyPatch& pp,
	const polyBoundaryMesh& bm
)
	:
	coupledPolyPatch(pp, bm),
	shadowName_(pp.shadowName_),
	zoneName_(pp.zoneName_),
	csPtr_(pp.csPtr_->clone()),
	discretisationType_(pp.discretisationType_),
	sweepAxisType_(pp.sweepAxisType_),
	stackAxisType_(pp.stackAxisType_),
	userProfileFile_(pp.userProfileFile_),
	shadowIndex_(-1),
	patchToPatchPtr_(NULL),
	zoneAddressingPtr_(NULL),
	reconFaceCellCentresPtr_(NULL),
	localParallelPtr_(NULL),
	receiveAddrPtr_(NULL)
{}


//- Construct as copy, resetting the face list and boundary mesh data
tnbLib::mixingPlanePolyPatch::mixingPlanePolyPatch
(
	const mixingPlanePolyPatch& pp,
	const polyBoundaryMesh& bm,
	const label index,
	const label newSize,
	const label newStart
)
	:
	coupledPolyPatch(pp, bm, index, newSize, newStart),
	shadowName_(pp.shadowName_),
	zoneName_(pp.zoneName_),
	csPtr_(pp.csPtr_->clone()),
	discretisationType_(pp.discretisationType_),
	sweepAxisType_(pp.sweepAxisType_),
	stackAxisType_(pp.stackAxisType_),
	userProfileFile_(pp.userProfileFile_),
	shadowIndex_(-1),
	patchToPatchPtr_(NULL),
	zoneAddressingPtr_(NULL),
	reconFaceCellCentresPtr_(NULL),
	localParallelPtr_(NULL),
	receiveAddrPtr_(NULL)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

tnbLib::mixingPlanePolyPatch::~mixingPlanePolyPatch()
{
	clearOut();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

tnbLib::label tnbLib::mixingPlanePolyPatch::shadowIndex() const
{
	if (shadowIndex_ == -1 && shadowName_ != tnbLib::word::null)
	{
		// Grab shadow patch index
		polyPatchID shadow(shadowName_, boundaryMesh());

		if (!shadow.active())
		{
			FatalErrorIn("label mixingPlanePolyPatch::shadowIndex() const")
				<< "Shadow patch name " << shadowName_
				<< " not found.  Please check your MixingPlane definition.  "
				<< "This may be fine at mesh generation stage."
				<< endl;
		}

		shadowIndex_ = shadow.index();

		// Check the other side is a mixingPlane
		if (!isA<mixingPlanePolyPatch>(boundaryMesh()[shadowIndex_]))
		{
			FatalErrorIn("label mixingPlanePolyPatch::shadowIndex() const")
				<< "Shadow of mixingPlane patch " << name()
				<< " named " << shadowName() << " is not a mixingPlane.  "
				<< "Type: " << boundaryMesh()[shadowIndex_].type() << nl
				<< "This is not allowed.  Please check your mesh definition."
				<< abort(FatalError);
		}

		// Check for mixingPlane onto self
		if (index() == shadowIndex_)
		{
			FatalErrorIn("label mixingPlanePolyPatch::shadowIndex() const")
				<< "mixingPlane patch " << name()
				<< " created as its own shadow"
				<< abort(FatalError);
		}
	}

	return shadowIndex_;
}


const tnbLib::coordinateSystem& tnbLib::mixingPlanePolyPatch::cs() const
{
	if (master())
	{
		return csPtr_();
	}
	else
	{
		return shadow().cs();
	}
}


tnbLib::label tnbLib::mixingPlanePolyPatch::zoneIndex() const
{
	if (zoneIndex_ == -1 && zoneName_ != tnbLib::word::null)
	{
		// Grab zone patch index
		faceZoneID zone(zoneName_, boundaryMesh().mesh().faceZones());

		if (!zone.active())
		{
			FatalErrorIn("label mixingPlanePolyPatch::zoneIndex() const")
				<< "Face zone name " << zoneName_
				<< " for mixingPlane patch " << name()
				<< " not found.  "
				<< "Please check mixingPlane interface definition."
				<< abort(FatalError);
		}

		zoneIndex_ = zone.index();
	}

	return zoneIndex_;
}


const tnbLib::mixingPlanePolyPatch& tnbLib::mixingPlanePolyPatch::shadow() const
{
	return refCast<const mixingPlanePolyPatch>(boundaryMesh()[shadowIndex()]);
}


const tnbLib::faceZone& tnbLib::mixingPlanePolyPatch::zone() const
{
	return boundaryMesh().mesh().faceZones()[zoneIndex()];
}


const tnbLib::labelList& tnbLib::mixingPlanePolyPatch::zoneAddressing() const
{
	if (!zoneAddressingPtr_)
	{
		calcZoneAddressing();
	}

	return *zoneAddressingPtr_;
}


bool tnbLib::mixingPlanePolyPatch::localParallel() const
{
	// Calculate patch-to-zone addressing
	if (!localParallelPtr_)
	{
		calcLocalParallel();
	}

	return *localParallelPtr_;
}


const tnbLib::mixingPlaneZoneInterpolation&
tnbLib::mixingPlanePolyPatch::patchToPatch() const
{
	if (master())
	{
		if (!patchToPatchPtr_)
		{
			Info << "Initializing the mixingPlane interpolator between "
				<< "master/shadow patches: "
				<< name() << "/" << shadowName()
				<< endl;


			calcPatchToPatch();
		}

		return *patchToPatchPtr_;
	}
	else
	{
		return shadow().patchToPatch();
	}
}


const tnbLib::vectorField&
tnbLib::mixingPlanePolyPatch::reconFaceCellCentres() const
{
	if (!reconFaceCellCentresPtr_)
	{
		calcReconFaceCellCentres();
	}

	return *reconFaceCellCentresPtr_;
}


void tnbLib::mixingPlanePolyPatch::initAddressing()
{
	if (active())
	{
		// Calculate transforms for correct mixingPlane cut
		calcTransforms();

		// Force zone addressing and remote zone addressing
		// (uses mixingPlane interpolator)
		zoneAddressing();

		// Force local parallel
		if (Pstream::parRun() && !localParallel())
		{
			// Calculate send addressing
			receiveAddr();
		}
	}

	polyPatch::initAddressing();
}


void tnbLib::mixingPlanePolyPatch::calcAddressing()
{
	polyPatch::calcAddressing();
}


void tnbLib::mixingPlanePolyPatch::initGeometry()
{
	// Communication is allowed either before or after processor
	// patch comms.  HJ, 11/Jul/2011
	if (active())
	{
		// Note: Only master calculates recon; slave uses master interpolation
		if (master())
		{
			reconFaceCellCentres();
		}
	}

	polyPatch::initGeometry();
}


void tnbLib::mixingPlanePolyPatch::calcGeometry()
{
	polyPatch::calcGeometry();

	// Note: Calculation of transforms must be forced before the
	// reconFaceCellCentres in order to correctly set the transformation
	// in the interpolation routines.
	// HJ, 3/Jul/2009
}


void tnbLib::mixingPlanePolyPatch::initMovePoints(const pointField& p)
{
	clearGeom();

	// Calculate transforms on mesh motion?
	calcTransforms();

	if (master())
	{
		shadow().clearGeom();
		shadow().calcTransforms();
	}

	// Update interpolation for new relative position of mixingPlane interfaces
	if (patchToPatchPtr_)
	{
		patchToPatchPtr_->movePoints();
	}

	// Recalculate send and receive maps
	if (active())
	{
		// Force zone addressing first
		zoneAddressing();

		if (Pstream::parRun() && !localParallel())
		{
			receiveAddr();
		}
	}

	if (active() && master())
	{
		reconFaceCellCentres();
	}

	polyPatch::initMovePoints(p);
}


void tnbLib::mixingPlanePolyPatch::movePoints(const pointField& p)
{
	polyPatch::movePoints(p);
}


void tnbLib::mixingPlanePolyPatch::initUpdateMesh()
{
	polyPatch::initUpdateMesh();
}


void tnbLib::mixingPlanePolyPatch::updateMesh()
{
	polyPatch::updateMesh();
	clearOut();
}


void tnbLib::mixingPlanePolyPatch::calcTransforms() const
{
	forwardT_.setSize(0);
	reverseT_.setSize(0);
	separation_.setSize(0);
}


void tnbLib::mixingPlanePolyPatch::initOrder(const primitivePatch& pp) const
{}


bool tnbLib::mixingPlanePolyPatch::order
(
	const primitivePatch& pp,
	labelList& faceMap,
	labelList& rotation
) const
{
	faceMap.setSize(pp.size(), -1);
	rotation.setSize(pp.size(), 0);

	// Nothing changes
	return false;
}


void tnbLib::mixingPlanePolyPatch::syncOrder() const
{}


void tnbLib::mixingPlanePolyPatch::write(Ostream& os) const
{
	polyPatch::write(os);
	os.writeKeyword("shadowPatch") << shadowName_
		<< token::END_STATEMENT << nl;
	os.writeKeyword("zone") << zoneName_
		<< token::END_STATEMENT << nl;

	// Note: only master writes the data
	if (master() || shadowIndex_ == -1)
	{
		// Write coordinate system dictionary.  Check by hand.  HJ, 26/Jan/2011
		os.writeKeyword("coordinateSystem");
		csPtr_().writeDict(os, true);

		dictionary ribbonPatchDict("ribbonPatch");

		ribbonPatchDict.add
		(
			"sweepAxis",
			mixingPlaneInterpolation::sweepAxisNames_[sweepAxisType_]
		);

		ribbonPatchDict.add
		(
			"stackAxis",
			mixingPlaneInterpolation::stackAxisNames_[stackAxisType_]
		);

		ribbonPatchDict.add
		(
			"discretisation",
			mixingPlaneInterpolation::discretisationNames_[discretisationType_]
		);

		os.writeKeyword("ribbonPatch")
			<< ribbonPatchDict << nl;

		if (userProfileFile_ != fileName::null)
		{
			os.writeKeyword("userProfileFile") << userProfileFile_
				<< token::END_STATEMENT << nl;
		}
	}
}


// ************************************************************************* //