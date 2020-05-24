#pragma once
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

	// Construct from components
	inline specie::specie
	(
		const word& name,
		const scalar nMoles,
		const scalar molWeight
	)
		:
		name_(name),
		nMoles_(nMoles),
		molWeight_(molWeight)
	{}


	// Construct from components without name
	inline specie::specie
	(
		const scalar nMoles,
		const scalar molWeight
	)
		:
		//name_(),
		nMoles_(nMoles),
		molWeight_(molWeight)
	{}


	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	// Construct as copy
	inline specie::specie(const specie& st)
		:
		name_(st.name_),
		nMoles_(st.nMoles_),
		molWeight_(st.molWeight_)
	{}


	// Construct as named copy
	inline specie::specie(const word& name, const specie& st)
		:
		name_(name),
		nMoles_(st.nMoles_),
		molWeight_(st.molWeight_)
	{}


	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	//- Molecular weight [kg/kmol]
	inline scalar specie::W() const
	{
		return molWeight_;
	}

	//- No of moles of this species in mixture
	inline scalar specie::nMoles() const
	{
		return nMoles_;
	}

	//- Gas constant [J/(kg K)]
	inline scalar specie::R() const
	{
		return RR() / molWeight_;
	}


	// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

	inline void specie::operator=(const specie& st)
	{
		//name_ = st.name_;
		nMoles_ = st.nMoles_;
		molWeight_ = st.molWeight_;
	}


	inline void specie::operator+=(const specie& st)
	{
		scalar sumNmoles_ = max(nMoles_ + st.nMoles_, SMALL);

		molWeight_ =
			nMoles_ / sumNmoles_ * molWeight_
			+ st.nMoles_ / sumNmoles_ * st.molWeight_;

		nMoles_ = sumNmoles_;
	}


	inline void specie::operator-=(const specie& st)
	{
		scalar diffnMoles_ = nMoles_ - st.nMoles_;
		if (mag(diffnMoles_) < SMALL)
		{
			diffnMoles_ = SMALL;
		}

		molWeight_ =
			nMoles_ / diffnMoles_ * molWeight_
			- st.nMoles_ / diffnMoles_ * st.molWeight_;

		nMoles_ = diffnMoles_;
	}


	inline void specie::operator*=(const scalar s)
	{
		nMoles_ *= s;
	}


	// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

	inline specie operator+(const specie& st1, const specie& st2)
	{
		scalar sumNmoles_ = max(st1.nMoles_ + st2.nMoles_, SMALL);

		return specie
		(
			sumNmoles_,
			st1.nMoles_ / sumNmoles_ * st1.molWeight_
			+ st2.nMoles_ / sumNmoles_ * st2.molWeight_
		);
	}


	inline specie operator-(const specie& st1, const specie& st2)
	{
		scalar diffNmoles_ = st1.nMoles_ - st2.nMoles_;
		if (mag(diffNmoles_) < SMALL)
		{
			diffNmoles_ = SMALL;
		}

		return specie
		(
			diffNmoles_,
			st1.nMoles_ / diffNmoles_ * st1.molWeight_
			- st2.nMoles_ / diffNmoles_ * st2.molWeight_
		);
	}


	inline specie operator*(const scalar s, const specie& st)
	{
		return specie
		(
			s*st.nMoles_,
			st.molWeight_
		);
	}


	inline specie operator==(const specie& st1, const specie& st2)
	{
		return st2 - st1;
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //