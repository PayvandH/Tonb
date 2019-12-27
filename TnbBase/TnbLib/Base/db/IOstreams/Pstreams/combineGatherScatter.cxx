#include <OPstream.hxx>
#include <IPstream.hxx>
#include <IOstreams.hxx>
#include <contiguous.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	template <class T, class CombineOp>
	void Pstream::combineGather
	(
		const List<Pstream::commsStruct>& comms,
		T& Value,
		const CombineOp& cop
	)
	{
		if (Pstream::parRun())
		{
			// Get my communication order
			const commsStruct& myComm = comms[Pstream::myProcNo()];

			// Receive from my downstairs neighbours
			forAll(myComm.below(), belowI)
			{
				label belowID = myComm.below()[belowI];

				if (contiguous<T>())
				{
					T value;
					IPstream::read
					(
						Pstream::scheduled,
						belowID,
						reinterpret_cast<char*>(&value),
						sizeof(T)
					);

					if (debug > 1)
					{
						Pout << " received from "
							<< belowID << " data:" << value << endl;
					}

					cop(Value, value);
				}
				else
				{
					IPstream fromBelow(Pstream::scheduled, belowID);
					T value(fromBelow);

					if (debug > 1)
					{
						Pout << " received from "
							<< belowID << " data:" << value << endl;
					}

					cop(Value, value);
				}
			}

			// Send up Value
			if (myComm.above() != -1)
			{
				if (debug > 1)
				{
					Pout << " sending to " << myComm.above()
						<< " data:" << Value << endl;
				}

				if (contiguous<T>())
				{
					OPstream::write
					(
						Pstream::scheduled,
						myComm.above(),
						reinterpret_cast<const char*>(&Value),
						sizeof(T)
					);
				}
				else
				{
					OPstream toAbove(Pstream::scheduled, myComm.above());
					toAbove << Value;
				}
			}
		}
	}


	template <class T, class CombineOp>
	void Pstream::combineGather(T& Value, const CombineOp& cop)
	{
		if (Pstream::nProcs() < Pstream::nProcsSimpleSum())
		{
			combineGather(Pstream::linearCommunication(), Value, cop);
		}
		else
		{
			combineGather(Pstream::treeCommunication(), Value, cop);
		}
	}


	template <class T>
	void Pstream::combineScatter(const List<Pstream::commsStruct>& comms, T& Value)
	{
		if (Pstream::parRun())
		{
			// Get my communication order
			const Pstream::commsStruct& myComm = comms[Pstream::myProcNo()];

			// Receive from up
			if (myComm.above() != -1)
			{
				if (contiguous<T>())
				{
					IPstream::read
					(
						Pstream::scheduled,
						myComm.above(),
						reinterpret_cast<char*>(&Value),
						sizeof(T)
					);
				}
				else
				{
					IPstream fromAbove(Pstream::scheduled, myComm.above());
					Value = T(fromAbove);
				}

				if (debug > 1)
				{
					Pout << " received from "
						<< myComm.above() << " data:" << Value << endl;
				}
			}

			// Send to my downstairs neighbours
			forAll(myComm.below(), belowI)
			{
				label belowID = myComm.below()[belowI];

				if (debug > 1)
				{
					Pout << " sending to " << belowID << " data:" << Value << endl;
				}

				if (contiguous<T>())
				{
					OPstream::write
					(
						Pstream::scheduled,
						belowID,
						reinterpret_cast<const char*>(&Value),
						sizeof(T)
					);
				}
				else
				{
					OPstream toBelow(Pstream::scheduled, belowID);
					toBelow << Value;
				}
			}
		}
	}


	template <class T>
	void Pstream::combineScatter(T& Value)
	{
		if (Pstream::nProcs() < Pstream::nProcsSimpleSum())
		{
			combineScatter(Pstream::linearCommunication(), Value);
		}
		else
		{
			combineScatter(Pstream::treeCommunication(), Value);
		}
	}


	// Same thing but for whole list at a time
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	template <class T, class CombineOp>
	void Pstream::listCombineGather
	(
		const List<Pstream::commsStruct>& comms,
		List<T>& Values,
		const CombineOp& cop
	)
	{
		if (Pstream::parRun())
		{
			// Get my communication order
			const commsStruct& myComm = comms[Pstream::myProcNo()];

			// Receive from my downstairs neighbours
			forAll(myComm.below(), belowI)
			{
				label belowID = myComm.below()[belowI];

				if (contiguous<T>())
				{
					List<T> receivedValues(Values.size());

					IPstream::read
					(
						Pstream::scheduled,
						belowID,
						reinterpret_cast<char*>(receivedValues.begin()),
						receivedValues.byteSize()
					);

					if (debug > 1)
					{
						Pout << " received from "
							<< belowID << " data:" << receivedValues << endl;
					}

					forAll(Values, i)
					{
						cop(Values[i], receivedValues[i]);
					}
				}
				else
				{
					IPstream fromBelow(Pstream::scheduled, belowID);
					List<T> receivedValues(fromBelow);

					if (debug > 1)
					{
						Pout << " received from "
							<< belowID << " data:" << receivedValues << endl;
					}

					forAll(Values, i)
					{
						cop(Values[i], receivedValues[i]);
					}
				}
			}

			// Send up Value
			if (myComm.above() != -1)
			{
				if (debug > 1)
				{
					Pout << " sending to " << myComm.above()
						<< " data:" << Values << endl;
				}

				if (contiguous<T>())
				{
					OPstream::write
					(
						Pstream::scheduled,
						myComm.above(),
						reinterpret_cast<const char*>(Values.begin()),
						Values.byteSize()
					);
				}
				else
				{
					OPstream toAbove(Pstream::scheduled, myComm.above());
					toAbove << Values;
				}
			}
		}
	}


	template <class T, class CombineOp>
	void Pstream::listCombineGather(List<T>& Values, const CombineOp& cop)
	{
		if (Pstream::nProcs() < Pstream::nProcsSimpleSum())
		{
			listCombineGather(Pstream::linearCommunication(), Values, cop);
		}
		else
		{
			listCombineGather(Pstream::treeCommunication(), Values, cop);
		}
	}


	template <class T>
	void Pstream::listCombineScatter
	(
		const List<Pstream::commsStruct>& comms,
		List<T>& Values
	)
	{
		if (Pstream::parRun())
		{
			// Get my communication order
			const Pstream::commsStruct& myComm = comms[Pstream::myProcNo()];

			// Receive from up
			if (myComm.above() != -1)
			{
				if (contiguous<T>())
				{
					IPstream::read
					(
						Pstream::scheduled,
						myComm.above(),
						reinterpret_cast<char*>(Values.begin()),
						Values.byteSize()
					);
				}
				else
				{
					IPstream fromAbove(Pstream::scheduled, myComm.above());
					fromAbove >> Values;
				}

				if (debug > 1)
				{
					Pout << " received from "
						<< myComm.above() << " data:" << Values << endl;
				}
			}

			// Send to my downstairs neighbours
			forAll(myComm.below(), belowI)
			{
				label belowID = myComm.below()[belowI];

				if (debug > 1)
				{
					Pout << " sending to " << belowID << " data:" << Values << endl;
				}

				if (contiguous<T>())
				{
					OPstream::write
					(
						Pstream::scheduled,
						belowID,
						reinterpret_cast<const char*>(Values.begin()),
						Values.byteSize()
					);
				}
				else
				{
					OPstream toBelow(Pstream::scheduled, belowID);
					toBelow << Values;
				}
			}
		}
	}


	template <class T>
	void Pstream::listCombineScatter(List<T>& Values)
	{
		if (Pstream::nProcs() < Pstream::nProcsSimpleSum())
		{
			listCombineScatter(Pstream::linearCommunication(), Values);
		}
		else
		{
			listCombineScatter(Pstream::treeCommunication(), Values);
		}
	}




	// Same thing but for sparse list (map)
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	template <class Container, class CombineOp>
	void Pstream::mapCombineGather
	(
		const List<Pstream::commsStruct>& comms,
		Container& Values,
		const CombineOp& cop
	)
	{
		if (Pstream::parRun())
		{
			// Get my communication order
			const commsStruct& myComm = comms[Pstream::myProcNo()];

			// Receive from my downstairs neighbours
			forAll(myComm.below(), belowI)
			{
				label belowID = myComm.below()[belowI];

				IPstream fromBelow(Pstream::scheduled, belowID);
				Container receivedValues(fromBelow);

				if (debug > 1)
				{
					Pout << " received from "
						<< belowID << " data:" << receivedValues << endl;
				}

				for
					(
						typename Container::const_iterator slaveIter =
						receivedValues.begin();
						slaveIter != receivedValues.end();
						++slaveIter
						)
				{
					typename Container::iterator
						masterIter = Values.find(slaveIter.key());

					if (masterIter != Values.end())
					{
						cop(masterIter(), slaveIter());
					}
					else
					{
						Values.insert(slaveIter.key(), slaveIter());
					}
				}
			}

			// Send up Value
			if (myComm.above() != -1)
			{
				if (debug > 1)
				{
					Pout << " sending to " << myComm.above()
						<< " data:" << Values << endl;
				}

				OPstream toAbove(Pstream::scheduled, myComm.above());
				toAbove << Values;
			}
		}
	}


	template <class Container, class CombineOp>
	void Pstream::mapCombineGather(Container& Values, const CombineOp& cop)
	{
		if (Pstream::nProcs() < Pstream::nProcsSimpleSum())
		{
			mapCombineGather(Pstream::linearCommunication(), Values, cop);
		}
		else
		{
			mapCombineGather(Pstream::treeCommunication(), Values, cop);
		}
	}


	template <class Container>
	void Pstream::mapCombineScatter
	(
		const List<Pstream::commsStruct>& comms,
		Container& Values
	)
	{
		if (Pstream::parRun())
		{
			// Get my communication order
			const Pstream::commsStruct& myComm = comms[Pstream::myProcNo()];

			// Receive from up
			if (myComm.above() != -1)
			{
				IPstream fromAbove(Pstream::scheduled, myComm.above());
				fromAbove >> Values;

				if (debug > 1)
				{
					Pout << " received from "
						<< myComm.above() << " data:" << Values << endl;
				}
			}

			// Send to my downstairs neighbours
			forAll(myComm.below(), belowI)
			{
				label belowID = myComm.below()[belowI];

				if (debug > 1)
				{
					Pout << " sending to " << belowID << " data:" << Values << endl;
				}

				OPstream toBelow(Pstream::scheduled, belowID);
				toBelow << Values;
			}
		}
	}


	template <class Container>
	void Pstream::mapCombineScatter(Container& Values)
	{
		if (Pstream::nProcs() < Pstream::nProcsSimpleSum())
		{
			mapCombineScatter(Pstream::linearCommunication(), Values);
		}
		else
		{
			mapCombineScatter(Pstream::treeCommunication(), Values);
		}
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //