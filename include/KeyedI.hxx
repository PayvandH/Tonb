#pragma once
#include <IOstreams.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T>
tnbLib::Keyed<T>::Keyed()
	:
	key_(-1)
{}


template<class T>
tnbLib::Keyed<T>::Keyed(const T& item, const label key)
	:
	T(item),
	key_(key)
{}


template<class T>
tnbLib::Keyed<T>::Keyed(const Xfer<T>& item, const label key)
	:
	T(item),
	key_(key)
{}


template<class T>
tnbLib::Keyed<T>::Keyed(Istream& is)
{
	is >> *this;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
tnbLib::label tnbLib::Keyed<T>::key() const
{
	return key_;
}

template<class T>
tnbLib::label& tnbLib::Keyed<T>::key()
{
	return key_;
}


template<class T>
tnbLib::List<tnbLib::Keyed<T> >
tnbLib::Keyed<T>::createList(const List<T>& lst, const label key)
{
	List<Keyed<T> > newList(lst.size());

	forAll(lst, elemI)
	{
		newList[elemI] = Keyed(lst[elemI], key);
	}
	return newList;
}


template<class T>
tnbLib::List<tnbLib::Keyed<T> >
tnbLib::Keyed<T>::createList(const List<T>& lst, const List<label>& keys)
{
	if (lst.size() != keys.size())
	{
		FatalErrorIn
		(
			"tnbLib::Keyed<T>::createList(const List<T>&, const List<label>&)"
		)
			<< "size mismatch adding keys to a list:" << nl
			<< "List has size " << lst.size()
			<< " and keys has size " << keys.size() << nl
			<< abort(FatalError);
	}

	List<Keyed<T> > newList(lst.size());

	forAll(lst, elemI)
	{
		newList[elemI] = Keyed(lst[elemI], keys[elemI]);
	}
	return newList;
}


// * * * * * * * * * * * * * * * Ostream Operator  * * * * * * * * * * * * * //

template<class T>
tnbLib::Istream& tnbLib::operator>>(Istream& is, Keyed<T>& item)
{
	// Read beginning of Keyed item/key pair
	is.readBegin("Keyed<T>");

	is >> static_cast<T&>(item);
	is >> item.key_;

	// Read end of Keyed item/key pair
	is.readEnd("Keyed<T>");

	// Check state of Ostream
	is.check("Istream& operator>>(Istream&, Keyed&)");

	return is;
}


template<class T>
tnbLib::Ostream& tnbLib::operator<<(Ostream& os, const Keyed<T>& item)
{
	os << token::BEGIN_LIST
		<< static_cast<const T&>(item)
		<< token::SPACE << item.key_
		<< token::END_LIST;

	return os;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //