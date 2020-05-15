#include <processorBoundaryPatch.hxx>

#include <addToRunTimeSelectionTable.hxx>
#include <dictionary.hxx>
#include <Ostream.hxx>
#include <Istream.hxx>
#include <token.hxx>

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	defineTypeNameAndDebug(processorBoundaryPatch, 0);
	addToRunTimeSelectionTable(boundaryPatchBase, processorBoundaryPatch, dictionary);

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	processorBoundaryPatch::processorBoundaryPatch
	(
		const word& name,
		const word& type,
		const label nFaces,
		const label startFace,
		const label myProcNo,
		const label neighbProcNo
	)
		:
		boundaryPatchBase(name, type, nFaces, startFace),
		myProcNo_(myProcNo),
		neighbProcNo_(neighbProcNo)
	{}

	processorBoundaryPatch::processorBoundaryPatch
	(
		const word& name,
		const dictionary& dict
	)
		:
		boundaryPatchBase(name, dict),
		myProcNo_(readLabel(dict.lookup("myProcNo"))),
		neighbProcNo_(readLabel(dict.lookup("neighbProcNo")))
	{
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	dictionary processorBoundaryPatch::dict() const
	{
		dictionary dict;

		dict.add("type", type_);

		dict.add("nFaces", nFaces_);
		dict.add("startFace", startFace_);
		dict.add("myProcNo", myProcNo_);
		dict.add("neighbProcNo", neighbProcNo_);

		return dict;
	}

	void processorBoundaryPatch::write(Ostream& os) const
	{
		this->operator<<(os);
	}

	void processorBoundaryPatch::writeDict(Ostream& os) const
	{

	}

	Ostream& processorBoundaryPatch::operator<<(Ostream& os) const
	{
		os << patchName() << nl << token::BEGIN_BLOCK << nl
			<< "    type         " << patchType() << token::END_STATEMENT << nl
			<< "    nFaces       " << patchSize() << token::END_STATEMENT << nl
			<< "    startFace    " << patchStart() << token::END_STATEMENT << nl
			<< "    myProcNo     " << myProcNo_ << token::END_STATEMENT << nl
			<< "    neighbProcNo " << neighbProcNo_
			<< token::END_STATEMENT << nl
			<< token::END_BLOCK << endl;

		return os;
	}

	Istream& processorBoundaryPatch::operator>>(Istream& is)
	{
		token t;
		is >> name_ >> t;
		is >> t >> type_ >> t;
		is >> t >> nFaces_ >> t;
		is >> t >> startFace_ >> t;
		is >> t >> myProcNo_ >> t;
		is >> t >> neighbProcNo_ >> t;
		is >> t;

		return is;
	}

	void processorBoundaryPatch::operator=(const processorBoundaryPatch& wp)
	{
		name_ = wp.name_;
		type_ = wp.type_;
		nFaces_ = wp.nFaces_;
		startFace_ = wp.startFace_;
		myProcNo_ = wp.myProcNo_;
		neighbProcNo_ = wp.neighbProcNo_;
	}

	bool processorBoundaryPatch::operator!=(const processorBoundaryPatch& wp) const
	{
		if (name_ != wp.name_)
		{
			return true;
		}
		else if (type_ != wp.name_)
		{
			return true;
		}
		else if ((nFaces_ != wp.nFaces_) || (startFace_ != wp.startFace_))
		{
			return true;
		}
		else if (
			(myProcNo_ != wp.myProcNo_) || (neighbProcNo_ != wp.neighbProcNo_)
			)
		{
			return true;
		}

		return false;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //