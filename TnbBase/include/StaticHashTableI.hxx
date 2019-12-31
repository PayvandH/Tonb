#pragma once
#include <error.hxx>
#include <IOstreams.hxx>

// * * * * * * * * * * * * * Private Member Classes * * * * * * * * * * * * //

// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

template<class T, class Key, class Hash>
tnbLib::label
tnbLib::StaticHashTable<T, Key, Hash>::hashKeyIndex(const Key& key) const
{
	// size is power of two - this is the modulus
	return Hash()(key) & (keys_.size() - 1);
}


// * * * * * * * * * * * * * * * Member Functions * * * * * * * * * * * * * //

template<class T, class Key, class Hash>
tnbLib::label tnbLib::StaticHashTable<T, Key, Hash>::size() const
{
	return nElmts_;
}


template<class T, class Key, class Hash>
bool tnbLib::StaticHashTable<T, Key, Hash>::empty() const
{
	return !nElmts_;
}


template<class T, class Key, class Hash>
bool tnbLib::StaticHashTable<T, Key, Hash>::insert
(
	const Key& key,
	const T& newEntry
)
{
	return set(key, newEntry, true);
}


template<class T, class Key, class Hash>
bool tnbLib::StaticHashTable<T, Key, Hash>::set
(
	const Key& key,
	const T& newEntry
)
{
	return set(key, newEntry, false);
}


template<class T, class Key, class Hash>
tnbLib::Xfer<tnbLib::StaticHashTable<T, Key, Hash> >
tnbLib::StaticHashTable<T, Key, Hash>::xfer()
{
	return xferMove(*this);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class T, class Key, class Hash>
T& tnbLib::StaticHashTable<T, Key, Hash>::operator[](const Key& key)
{
	iterator iter = find(key);

	if (iter == end())
	{
		FatalErrorIn("StaticHashTable<T, Key, Hash>::operator[](const Key&)")
			<< key << " not found in table.  Valid entries: "
			<< toc()
			<< exit(FatalError);
	}

	return *iter;
}


template<class T, class Key, class Hash>
const T& tnbLib::StaticHashTable<T, Key, Hash>::operator[]
(
	const Key& key
	) const
{
	const_iterator iter = find(key);

	if (iter == cend())
	{
		FatalErrorIn
		(
			"StaticHashTable<T, Key, Hash>::operator[](const Key&) const"
		) << key << " not found in table.  Valid entries: "
			<< toc()
			<< exit(FatalError);
	}

	return *iter;
}


template<class T, class Key, class Hash>
T& tnbLib::StaticHashTable<T, Key, Hash>::operator()(const Key& key)
{
	iterator iter = find(key);

	if (iter == end())
	{
		insert(key, T());
		return *find(key);
	}
	else
	{
		return *iter;
	}
}


// * * * * * * * * * * * * * * * * STL iterator  * * * * * * * * * * * * * * //

template<class T, class Key, class Hash>
template<class TRef, class TableRef>
tnbLib::StaticHashTable<T, Key, Hash>::Iterator<TRef, TableRef>::Iterator
(
	TableRef hashTbl,
	label hashIndex,
	label elemIndex
)
	:
	hashTable_(hashTbl),
	hashIndex_(hashIndex),
	elemIndex_(elemIndex)
{}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
tnbLib::StaticHashTable<T, Key, Hash>::Iterator<TRef, TableRef>::Iterator
(
	const iterator& iter
)
	:
	hashTable_(iter.hashTable_),
	hashIndex_(iter.hashIndex_),
	elemIndex_(iter.elemIndex_)
{}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
void tnbLib::StaticHashTable<T, Key, Hash>::Iterator<TRef, TableRef>::operator=
(
	const iterator& iter
	)
{
	this->hashIndex_ = iter.hashIndex_;
	this->elemIndex_ = iter.elemIndex_;
}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
bool tnbLib::StaticHashTable<T, Key, Hash>::Iterator<TRef, TableRef>::operator==
(
	const iterator& iter
	) const
{
	return hashIndex_ == iter.hashIndex_ && elemIndex_ == iter.elemIndex_;
}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
bool tnbLib::StaticHashTable<T, Key, Hash>::Iterator<TRef, TableRef>::operator==
(
	const const_iterator& iter
	) const
{
	return hashIndex_ == iter.hashIndex_ && elemIndex_ == iter.elemIndex_;
}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
bool tnbLib::StaticHashTable<T, Key, Hash>::Iterator<TRef, TableRef>::operator!=
(
	const iterator& iter
	) const
{
	return !operator==(iter);
}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
bool tnbLib::StaticHashTable<T, Key, Hash>::Iterator<TRef, TableRef>::operator!=
(
	const const_iterator& iter
	) const
{
	return !operator==(iter);
}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
TRef tnbLib::StaticHashTable<T, Key, Hash>::Iterator<TRef, TableRef>::operator*()
{
	return hashTable_.objects_[hashIndex_][elemIndex_];
}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
TRef tnbLib::StaticHashTable<T, Key, Hash>::Iterator<TRef, TableRef>::operator()()
{
	return operator*();
}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
typename tnbLib::StaticHashTable<T, Key, Hash>::template Iterator
<
	TRef,
	TableRef
>&
tnbLib::StaticHashTable<T, Key, Hash>::Iterator
<
	TRef,
	TableRef
>::operator++()
{
	// A negative index is a special value from erase
	// (see notes in HashTable)
	if (hashIndex_ < 0)
	{
		hashIndex_ = -(hashIndex_ + 1) - 1;
	}
	else
	{
		// Try the next element on the local list
		elemIndex_++;

		if (elemIndex_ < hashTable_.objects_[hashIndex_].size())
		{
			return *this;
		}
	}

	// Step to the next table entry
	elemIndex_ = 0;

	while
		(
			++hashIndex_ < hashTable_.objects_.size()
			&& !hashTable_.objects_[hashIndex_].size()
			)
	{
	}


	if (hashIndex_ >= hashTable_.objects_.size())
	{
		// make end iterator
		hashIndex_ = hashTable_.keys_.size();
	}

	return *this;
}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
typename tnbLib::StaticHashTable<T, Key, Hash>::template Iterator
<
	TRef,
	TableRef
>
tnbLib::StaticHashTable<T, Key, Hash>::Iterator
<
	TRef,
	TableRef
>::operator++
(
	int
	)
{
	iterator tmp = *this;
	++*this;
	return tmp;
}


template<class T, class Key, class Hash>
template<class TRef, class TableRef>
const Key&
tnbLib::StaticHashTable<T, Key, Hash>::Iterator<TRef, TableRef>::key() const
{
	return hashTable_.keys_[hashIndex_][elemIndex_];
}


template<class T, class Key, class Hash>
typename tnbLib::StaticHashTable<T, Key, Hash>::iterator
tnbLib::StaticHashTable<T, Key, Hash>::begin()
{
	// Find first non-empty entry
	forAll(keys_, hashIdx)
	{
		if (keys_[hashIdx].size())
		{
			return iterator(*this, hashIdx, 0);
		}
	}

#   ifdef FULLDEBUG
	if (debug)
	{
		Info << "StaticHashTable is empty\n";
	}
#   endif

	return StaticHashTable<T, Key, Hash>::endIter_;
}


template<class T, class Key, class Hash>
const typename tnbLib::StaticHashTable<T, Key, Hash>::iterator&
tnbLib::StaticHashTable<T, Key, Hash>::end()
{
	return StaticHashTable<T, Key, Hash>::endIter_;
}


template<class T, class Key, class Hash>
typename tnbLib::StaticHashTable<T, Key, Hash>::const_iterator
tnbLib::StaticHashTable<T, Key, Hash>::cbegin() const
{
	// Find first non-empty entry
	forAll(keys_, hashIdx)
	{
		if (keys_[hashIdx].size())
		{
			return const_iterator(*this, hashIdx, 0);
		}
	}

#   ifdef FULLDEBUG
	if (debug)
	{
		Info << "StaticHashTable is empty\n";
	}
#   endif

	return StaticHashTable<T, Key, Hash>::endConstIter_;
}


template<class T, class Key, class Hash>
const typename tnbLib::StaticHashTable<T, Key, Hash>::const_iterator&
tnbLib::StaticHashTable<T, Key, Hash>::cend() const
{
	return StaticHashTable<T, Key, Hash>::endConstIter_;
}


template<class T, class Key, class Hash>
typename tnbLib::StaticHashTable<T, Key, Hash>::const_iterator
tnbLib::StaticHashTable<T, Key, Hash>::begin() const
{
	return this->cbegin();
}


template<class T, class Key, class Hash>
const typename tnbLib::StaticHashTable<T, Key, Hash>::const_iterator&
tnbLib::StaticHashTable<T, Key, Hash>::end() const
{
	return StaticHashTable<T, Key, Hash>::endConstIter_;
}


// ************************************************************************* //