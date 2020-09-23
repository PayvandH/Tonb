#pragma once
// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
inline tnbLib::label tnbLib::UPtrList<T>::size() const
{
	return ptrs_.size();
}


template<class T>
inline bool tnbLib::UPtrList<T>::empty() const
{
	return ptrs_.empty();
}


template<class T>
inline T& tnbLib::UPtrList<T>::first()
{
	return this->operator[](0);
}


template<class T>
inline const T& tnbLib::UPtrList<T>::first() const
{
	return this->operator[](0);
}


template<class T>
inline T& tnbLib::UPtrList<T>::last()
{
	return this->operator[](this->size() - 1);
}


template<class T>
inline const T& tnbLib::UPtrList<T>::last() const
{
	return this->operator[](this->size() - 1);
}


template<class T>
inline void tnbLib::UPtrList<T>::resize(const label newSize)
{
	this->setSize(newSize);
}


template<class T>
inline bool tnbLib::UPtrList<T>::set(const label i) const
{
	return ptrs_[i] != nullptr;
}


template<class T>
inline T* tnbLib::UPtrList<T>::set(const label i, T* ptr)
{
	T* old = ptrs_[i];
	ptrs_[i] = ptr;
	return old;
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class T>
inline const T& tnbLib::UPtrList<T>::operator[](const label i) const
{
	if (!ptrs_[i])
	{
		FatalErrorInFunction
			<< "hanging pointer at index " << i
			<< " (size " << size()
			<< "), cannot dereference"
			<< abort(FatalError);
	}

	return *(ptrs_[i]);
}


template<class T>
inline T& tnbLib::UPtrList<T>::operator[](const label i)
{
	if (!ptrs_[i])
	{
		FatalErrorInFunction
			<< "hanging pointer at index " << i
			<< " (size " << size()
			<< "), cannot dereference"
			<< abort(FatalError);
	}

	return *(ptrs_[i]);
}


template<class T>
inline const T* tnbLib::UPtrList<T>::operator()(const label i) const
{
	return ptrs_[i];
}


// * * * * * * * * * * * * * * * * STL iterator  * * * * * * * * * * * * * * //

template<class T>
inline tnbLib::UPtrList<T>::iterator::iterator(T** ptr)
	:
	ptr_(ptr)
{}


template<class T>
inline bool tnbLib::UPtrList<T>::iterator::operator==(const iterator& iter) const
{
	return ptr_ == iter.ptr_;
}


template<class T>
inline bool tnbLib::UPtrList<T>::iterator::operator!=(const iterator& iter) const
{
	return ptr_ != iter.ptr_;
}


template<class T>
inline T& tnbLib::UPtrList<T>::iterator::operator*()
{
	return **ptr_;
}


template<class T>
inline T& tnbLib::UPtrList<T>::iterator::operator()()
{
	return operator*();
}


template<class T>
inline typename tnbLib::UPtrList<T>::iterator
tnbLib::UPtrList<T>::iterator::operator++()
{
	++ptr_;
	return *this;
}


template<class T>
inline typename tnbLib::UPtrList<T>::iterator
tnbLib::UPtrList<T>::iterator::operator++(const int)
{
	iterator tmp = *this;
	++ptr_;
	return tmp;
}


template<class T>
inline typename tnbLib::UPtrList<T>::iterator
tnbLib::UPtrList<T>::iterator::operator--()
{
	--ptr_;
	return *this;
}


template<class T>
inline typename tnbLib::UPtrList<T>::iterator
tnbLib::UPtrList<T>::iterator::operator--(const int)
{
	iterator tmp = *this;
	--ptr_;
	return tmp;
}


template<class T>
inline typename tnbLib::UPtrList<T>::iterator
tnbLib::UPtrList<T>::iterator::operator+=(const label n)
{
	ptr_ += n;
	return *this;
}


template<class T>
inline typename tnbLib::UPtrList<T>::iterator
tnbLib::UPtrList<T>::iterator::operator-=(const label n)
{
	ptr_ -= n;
	return *this;
}


template<class T>
inline typename tnbLib::UPtrList<T>::iterator
tnbLib::UPtrList<T>::iterator::operator+(const label n) const
{
	typename UPtrList<T>::iterator tmp = *this;
	return tmp += n;
}


template<class T>
inline typename tnbLib::UPtrList<T>::iterator
tnbLib::UPtrList<T>::iterator::operator-(const label n) const
{
	typename UPtrList<T>::iterator tmp = *this;
	return tmp -= n;
}


template<class T>
inline tnbLib::label tnbLib::UPtrList<T>::iterator::operator-
(
	const typename UPtrList<T>::iterator& iter
	) const
{
	return (ptr_ - iter.ptr_);
}


template<class T>
inline T& tnbLib::UPtrList<T>::iterator::operator[](const label n)
{
	return *(*this + n);
}


template<class T>
inline bool tnbLib::UPtrList<T>::iterator::operator<(const iterator& iter) const
{
	return ptr_ < iter.ptr_;
}


template<class T>
inline bool tnbLib::UPtrList<T>::iterator::operator>(const iterator& iter) const
{
	return ptr_ > iter.ptr_;
}


template<class T>
inline bool tnbLib::UPtrList<T>::iterator::operator<=(const iterator& iter) const
{
	return ptr_ <= iter.ptr_;
}


template<class T>
inline bool tnbLib::UPtrList<T>::iterator::operator>=(const iterator& iter) const
{
	return ptr_ >= iter.ptr_;
}


template<class T>
inline typename tnbLib::UPtrList<T>::iterator
tnbLib::UPtrList<T>::begin()
{
	return ptrs_.begin();
}


template<class T>
inline typename tnbLib::UPtrList<T>::iterator
tnbLib::UPtrList<T>::end()
{
	return ptrs_.end();
}


// * * * * * * * * * * * * * * * STL const_iterator  * * * * * * * * * * * * //

template<class T>
inline tnbLib::UPtrList<T>::const_iterator::const_iterator(const T* const* ptr)
	:
	ptr_(ptr)
{}


template<class T>
inline tnbLib::UPtrList<T>::const_iterator::const_iterator(const iterator& iter)
	:
	ptr_(iter.ptr_)
{}


template<class T>
inline bool tnbLib::UPtrList<T>::const_iterator::operator==
(
	const const_iterator& iter
	) const
{
	return ptr_ == iter.ptr_;
}


template<class T>
inline bool tnbLib::UPtrList<T>::const_iterator::operator!=
(
	const const_iterator& iter
	) const
{
	return ptr_ != iter.ptr_;
}


template<class T>
inline const T& tnbLib::UPtrList<T>::const_iterator::operator*()
{
	return **ptr_;
}


template<class T>
inline const T& tnbLib::UPtrList<T>::const_iterator::operator()()
{
	return operator*();
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::const_iterator::operator++()
{
	++ptr_;
	return *this;
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::const_iterator::operator++(const int)
{
	const_iterator tmp = *this;
	++ptr_;
	return tmp;
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::const_iterator::operator--()
{
	--ptr_;
	return *this;
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::const_iterator::operator--(const int)
{
	const_iterator tmp = *this;
	--ptr_;
	return tmp;
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::const_iterator::operator+=(const label n)
{
	ptr_ += n;
	return *this;
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::const_iterator::operator-=(const label n)
{
	ptr_ -= n;
	return *this;
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::const_iterator::operator+(const label n) const
{
	typename UPtrList<T>::const_iterator tmp = *this;
	return tmp += n;
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::const_iterator::operator-(const label n) const
{
	typename UPtrList<T>::const_iterator tmp = *this;
	return tmp -= n;
}


template<class T>
inline tnbLib::label tnbLib::UPtrList<T>::const_iterator::operator-
(
	const typename UPtrList<T>::const_iterator& iter
	) const
{
	return (ptr_ - iter.ptr_);
}


template<class T>
inline const T& tnbLib::UPtrList<T>::const_iterator::operator[](const label n)
{
	return *(*this + n);
}


template<class T>
inline bool tnbLib::UPtrList<T>::const_iterator::operator<
(
	const const_iterator& iter
	) const
{
	return ptr_ < iter.ptr_;
}


template<class T>
inline bool tnbLib::UPtrList<T>::const_iterator::operator>
(
	const const_iterator& iter
	) const
{
	return ptr_ > iter.ptr_;
}


template<class T>
inline bool tnbLib::UPtrList<T>::const_iterator::operator<=
(
	const const_iterator& iter
	) const
{
	return ptr_ <= iter.ptr_;
}


template<class T>
inline bool tnbLib::UPtrList<T>::const_iterator::operator>=
(
	const const_iterator& iter
	) const
{
	return ptr_ >= iter.ptr_;
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::begin() const
{
	return ptrs_.begin();
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::end() const
{
	return ptrs_.end();
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::cbegin() const
{
	return ptrs_.begin();
}


template<class T>
inline typename tnbLib::UPtrList<T>::const_iterator
tnbLib::UPtrList<T>::cend() const
{
	return ptrs_.end();
}


// ************************************************************************* //