#include <mapDistribute.hxx>

#include <commSchedule.hxx>
#include <HashSet.hxx>
#include <ListOps.hxx>
#include <IPstream.hxx>
#include <OPstream.hxx>
#include <UIndirectList.hxx>

tnbLib::List<tnbLib::labelPair> tnbLib::mapDistribute::schedule
(
	const labelListList& subMap,
	const labelListList& constructMap
)
{
	// Communications: send and receive processor
	List<labelPair> allComms;

	{
		HashSet<labelPair, labelPair::Hash<> > commsSet(Pstream::nProcs());

		// Find what communication is required
		forAll(subMap, procI)
		{
			if (procI != Pstream::myProcNo())
			{
				if (subMap[procI].size())
				{
					// I need to send to procI
					commsSet.insert(labelPair(Pstream::myProcNo(), procI));
				}
				if (constructMap[procI].size())
				{
					// I need to receive from procI
					commsSet.insert(labelPair(procI, Pstream::myProcNo()));
				}
			}
		}
		allComms = commsSet.toc();
	}


	// Reduce
	if (Pstream::master())
	{
		// Receive and merge
		for
			(
				int slave = Pstream::firstSlave();
				slave <= Pstream::lastSlave();
				slave++
				)
		{
			IPstream fromSlave(Pstream::scheduled, slave);
			List<labelPair> nbrData(fromSlave);

			forAll(nbrData, i)
			{
				if (findIndex(allComms, nbrData[i]) == -1)
				{
					label sz = allComms.size();
					allComms.setSize(sz + 1);
					allComms[sz] = nbrData[i];
				}
			}
		}
		// Send back
		for
			(
				int slave = Pstream::firstSlave();
				slave <= Pstream::lastSlave();
				slave++
				)
		{
			OPstream toSlave(Pstream::scheduled, slave);
			toSlave << allComms;
		}
	}
	else
	{
		{
			OPstream toMaster(Pstream::scheduled, Pstream::masterNo());
			toMaster << allComms;
		}
		{
			IPstream fromMaster(Pstream::scheduled, Pstream::masterNo());
			fromMaster >> allComms;
		}
	}


	// Determine my schedule.
	labelList mySchedule
	(
		commSchedule
		(
			Pstream::nProcs(),
			allComms
		).procSchedule()[Pstream::myProcNo()]
	);

	// Processors involved in my schedule
	return List<labelPair>(UIndirectList<labelPair>(allComms, mySchedule));

	//     if (debug)
	//     {
	//         Pout<< "I need to:" << endl;
	//         const List<labelPair>& comms = schedule();
	//         forAll (comms, i)
	//         {
	//             const labelPair& twoProcs = comms[i];
	//             label sendProc = twoProcs[0];
	//             label recvProc = twoProcs[1];

	//             if (recvProc == Pstream::myProcNo())
	//             {
	//                 Pout<< "    receive from " << sendProc << endl;
	//             }
	//             else
	//             {
	//                 Pout<< "    send to " << recvProc << endl;
	//             }
	//         }
	//     }
}


const tnbLib::List<tnbLib::labelPair>& tnbLib::mapDistribute::schedule() const
{
	if (schedulePtr_.empty())
	{
		schedulePtr_.reset
		(
			new List<labelPair>
			(
				schedule(subMap_, constructMap_)
				)
		);
	}
	return schedulePtr_();
}


void tnbLib::mapDistribute::checkReceivedSize
(
	const label procI,
	const label expectedSize,
	const label receivedSize
)
{
	if (receivedSize != expectedSize)
	{
		FatalErrorIn
		(
			"template<class T>\n"
			"void mapDistribute::checkReceivedSize\n"
			"(\n"
			"    const label procI,\n"
			"    const label expectedSize,\n"
			"    const label receivedSize\n"
			")\n"
		) << "Expected from processor " << procI
			<< " " << expectedSize << " but received "
			<< receivedSize << " elements."
			<< abort(FatalError);
	}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

//- Construct from components
tnbLib::mapDistribute::mapDistribute
(
	const label constructSize,
	const labelListList& subMap,
	const labelListList& constructMap
)
	:
	constructSize_(constructSize),
	subMap_(subMap),
	constructMap_(constructMap),
	schedulePtr_()
{}


//- (optionally destructively) construct from components
tnbLib::mapDistribute::mapDistribute
(
	const label constructSize,
	labelListList& subMap,
	labelListList& constructMap,
	const bool reUse                // clone or reuse
)
	:
	constructSize_(constructSize),
	subMap_(subMap, reUse),
	constructMap_(constructMap, reUse),
	schedulePtr_()
{}


tnbLib::mapDistribute::mapDistribute
(
	const labelList& sendProcs,
	const labelList& recvProcs
)
	:
	constructSize_(0),
	schedulePtr_()
{
	if (sendProcs.size() != recvProcs.size())
	{
		FatalErrorIn
		(
			"mapDistribute::mapDistribute(const labelList&, const labelList&)"
		) << "The send and receive data is not the same length. sendProcs:"
			<< sendProcs.size() << " recvProcs:" << recvProcs.size()
			<< abort(FatalError);
	}

	// Per processor the number of samples we have to send/receive.
	labelList nSend(Pstream::nProcs(), 0);
	labelList nRecv(Pstream::nProcs(), 0);

	forAll(sendProcs, sampleI)
	{
		label sendProc = sendProcs[sampleI];
		label recvProc = recvProcs[sampleI];

		// Note that also need to include local communication (both
		// RecvProc and sendProc on local processor)

		if (Pstream::myProcNo() == sendProc)
		{
			// I am the sender. Count destination processor.
			nSend[recvProc]++;
		}
		if (Pstream::myProcNo() == recvProc)
		{
			// I am the receiver.
			nRecv[sendProc]++;
		}
	}

	subMap_.setSize(Pstream::nProcs());
	constructMap_.setSize(Pstream::nProcs());

	forAll(nSend, procI)
	{
		subMap_[procI].setSize(nSend[procI]);
		constructMap_[procI].setSize(nRecv[procI]);
	}
	nSend = 0;
	nRecv = 0;

	forAll(sendProcs, sampleI)
	{
		label sendProc = sendProcs[sampleI];
		label recvProc = recvProcs[sampleI];

		if (Pstream::myProcNo() == sendProc)
		{
			// I am the sender. Store index I need to send.
			subMap_[recvProc][nSend[recvProc]++] = sampleI;
		}

		if (Pstream::myProcNo() == recvProc)
		{
			// I am the receiver.
			constructMap_[sendProc][nRecv[sendProc]++] = sampleI;

			// Largest entry inside constructMap
			constructSize_ = sampleI + 1;
		}
	}
}


tnbLib::mapDistribute::mapDistribute(const mapDistribute& map)
	:
	constructSize_(map.constructSize_),
	subMap_(map.subMap_),
	constructMap_(map.constructMap_),
	schedulePtr_()
{}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void tnbLib::mapDistribute::compact(const boolList& elemIsUsed)
{
	// 1. send back to sender. Have him delete the corresponding element
	//    from the submap and do the same to the constructMap locally
	//    (and in same order).

	// Send elemIsUsed field to neighbour. Use nonblocking code from
	// mapDistribute but in reverse order.
	{
		List<boolList> sendFields(Pstream::nProcs());

		for (label domain = 0; domain < Pstream::nProcs(); domain++)
		{
			const labelList& map = constructMap_[domain];

			if (domain != Pstream::myProcNo() && map.size())
			{
				boolList& subField = sendFields[domain];
				subField.setSize(map.size());
				forAll(map, i)
				{
					subField[i] = elemIsUsed[map[i]];
				}

				OPstream::write
				(
					Pstream::nonBlocking,
					domain,
					reinterpret_cast<const char*>(subField.begin()),
					subField.size() * sizeof(bool)
				);
			}
		}

		// Set up receives from neighbours

		List<boolList> recvFields(Pstream::nProcs());

		for (label domain = 0; domain < Pstream::nProcs(); domain++)
		{
			const labelList& map = subMap_[domain];

			if (domain != Pstream::myProcNo() && map.size())
			{
				recvFields[domain].setSize(map.size());
				IPstream::read
				(
					Pstream::nonBlocking,
					domain,
					reinterpret_cast<char*>(recvFields[domain].begin()),
					recvFields[domain].size() * sizeof(bool)
				);
			}
		}


		// Set up 'send' to myself - write directly into recvFields

		{
			const labelList& map = constructMap_[Pstream::myProcNo()];

			recvFields[Pstream::myProcNo()].setSize(map.size());
			forAll(map, i)
			{
				recvFields[Pstream::myProcNo()][i] = elemIsUsed[map[i]];
			}
		}


		// Wait for all to finish

		OPstream::waitRequests();
		IPstream::waitRequests();


		// Compact out all submap entries that are referring to unused elements
		for (label domain = 0; domain < Pstream::nProcs(); domain++)
		{
			const labelList& map = subMap_[domain];

			labelList newMap(map.size());
			label newI = 0;

			forAll(map, i)
			{
				if (recvFields[domain][i])
				{
					// So element is used on destination side
					newMap[newI++] = map[i];
				}
			}
			if (newI < map.size())
			{
				newMap.setSize(newI);
				subMap_[domain].transfer(newMap);
			}
		}
	}


	// 2. remove from construct map - since end-result (element in elemIsUsed)
	//    not used.

	label maxConstructIndex = -1;

	for (label domain = 0; domain < Pstream::nProcs(); domain++)
	{
		const labelList& map = constructMap_[domain];

		labelList newMap(map.size());
		label newI = 0;

		forAll(map, i)
		{
			label destinationI = map[i];

			// Is element is used on destination side
			if (elemIsUsed[destinationI])
			{
				maxConstructIndex = max(maxConstructIndex, destinationI);

				newMap[newI++] = destinationI;
			}
		}
		if (newI < map.size())
		{
			newMap.setSize(newI);
			constructMap_[domain].transfer(newMap);
		}
	}

	constructSize_ = maxConstructIndex + 1;

	// Clear the schedule (note:not necessary if nothing changed)
	schedulePtr_.clear();
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

void tnbLib::mapDistribute::operator=(const mapDistribute& rhs)
{
	// Check for assignment to self
	if (this == &rhs)
	{
		FatalErrorIn
		(
			"tnbLib::mapDistribute::operator=(const tnbLib::mapDistribute&)"
		) << "Attempted assignment to self"
			<< abort(FatalError);
	}
	constructSize_ = rhs.constructSize_;
	subMap_ = rhs.subMap_;
	constructMap_ = rhs.constructMap_;
	schedulePtr_.clear();
}


// ************************************************************************* //