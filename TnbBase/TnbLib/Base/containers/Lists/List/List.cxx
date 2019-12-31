#include <List.hxx>

#include <ListLoopM.hxx>
#include <FixedList.hxx>
#include <PtrList.hxx>
#include <SLList.hxx>
#include <IndirectList.hxx>
#include <UIndirectList.hxx>
#include <BiIndirectList.hxx>
#include <contiguous.hxx>

// * * * * * * * * * * * * * * * Static Members  * * * * * * * * * * * * * * //

template<class Type>
const tnbLib::List<Type> tnbLib::List<Type>::zero;


// * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * * //

// Construct with length specified
template<class T>
tnbLib::List<T>::List(const label s)
	:
	UList<T>(NULL, s)
{
	if (this->size_ < 0)
	{
		FatalErrorIn("List<T>::List(const label size)")
			<< "bad size " << this->size_
			<< abort(FatalError);
	}

	if (this->size_)
	{
		this->v_ = new T[this->size_];
	}
}


// Construct with length and single value specified
template<class T>
tnbLib::List<T>::List(const label s, const T& a)
	:
	UList<T>(NULL, s)
{
	if (this->size_ < 0)
	{
		FatalErrorIn("List<T>::List(const label size, const T&)")
			<< "bad size " << this->size_
			<< abort(FatalError);
	}

	if (this->size_)
	{
		this->v_ = new T[this->size_];

		List_ACCESS(T, (*this), vp);
		List_FOR_ALL((*this), i)
			List_ELEM((*this), vp, i) = a;
		List_END_FOR_ALL
	}
}


// Construct as copy
template<class T>
tnbLib::List<T>::List(const List<T>& a)
	:
	UList<T>(NULL, a.size_)
{
	if (this->size_)
	{
		this->v_ = new T[this->size_];

#       ifdef USEMEMCPY
		if (contiguous<T>())
		{
			memcpy(this->v_, a.v_, this->byteSize());
		}
		else
#       endif
		{
			List_ACCESS(T, (*this), vp);
			List_CONST_ACCESS(T, a, ap);
			List_FOR_ALL((*this), i)
				List_ELEM((*this), vp, i) = List_ELEM(a, ap, i);
			List_END_FOR_ALL
		}
	}
}


// Construct by transferring the parameter contents
template<class T>
tnbLib::List<T>::List(const Xfer< List<T> >& lst)
{
	transfer(lst());
}


// Construct as copy or re-use as specified.
template<class T>
tnbLib::List<T>::List(List<T>& a, bool reUse)
	:
	UList<T>(NULL, a.size_)
{
	if (reUse)
	{
		this->v_ = a.v_;
		a.v_ = 0;
		a.size_ = 0;
	}
	else if (this->size_)
	{
		this->v_ = new T[this->size_];

#       ifdef USEMEMCPY
		if (contiguous<T>())
		{
			memcpy(this->v_, a.v_, this->byteSize());
		}
		else
#       endif
		{
			List_ACCESS(T, (*this), vp);
			List_CONST_ACCESS(T, a, ap);
			List_FOR_ALL((*this), i)
				List_ELEM((*this), vp, i) = List_ELEM(a, ap, i);
			List_END_FOR_ALL
		}
	}
}


// Construct as subset
template<class T>
tnbLib::List<T>::List(const UList<T>& a, const unallocLabelList& map)
	:
	UList<T>(NULL, map.size())
{
	if (this->size_)
	{
		// Note:cannot use List_ELEM since third argument has to be index.

		this->v_ = new T[this->size_];

		forAll(*this, i)
		{
			this->v_[i] = a[map[i]];
		}
	}
}


// Construct given start and end iterators.
template<class T>
template<class InputIterator>
tnbLib::List<T>::List(InputIterator first, InputIterator last)
{
	label s = 0;
	for
		(
			InputIterator iter = first;
			iter != last;
			++iter
			)
	{
		s++;
	}

	setSize(s);

	s = 0;

	for
		(
			InputIterator iter = first;
			iter != last;
			++iter
			)
	{
		this->operator[](s++) = iter();
	}
}


// Construct as copy of FixedList<T, Size>
template<class T>
template<unsigned Size>
tnbLib::List<T>::List(const FixedList<T, Size>& lst)
	:
	UList<T>(NULL, Size)
{
	if (this->size_)
	{
		this->v_ = new T[this->size_];

		forAll(*this, i)
		{
			this->operator[](i) = lst[i];
		}
	}
}


// Construct as copy of PtrList<T>
template<class T>
tnbLib::List<T>::List(const PtrList<T>& lst)
	:
	UList<T>(NULL, lst.size())
{
	if (this->size_)
	{
		this->v_ = new T[this->size_];

		forAll(*this, i)
		{
			this->operator[](i) = lst[i];
		}
	}
}


// Construct as copy of SLList<T>
template<class T>
tnbLib::List<T>::List(const SLList<T>& lst)
	:
	UList<T>(NULL, lst.size())
{
	if (this->size_)
	{
		this->v_ = new T[this->size_];

		label i = 0;
		for
			(
				typename SLList<T>::const_iterator iter = lst.begin();
				iter != lst.end();
				++iter
				)
		{
			this->operator[](i++) = iter();
		}
	}
}


// Construct as copy of IndirectList<T>
template<class T>
tnbLib::List<T>::List(const IndirectList<T>& lst)
	:
	UList<T>(NULL, lst.size())
{
	if (this->size_)
	{
		this->v_ = new T[this->size_];

		forAll(*this, i)
		{
			this->operator[](i) = lst[i];
		}
	}
}


// Construct as copy of UIndirectList<T>
template<class T>
tnbLib::List<T>::List(const UIndirectList<T>& lst)
	:
	UList<T>(NULL, lst.size())
{
	if (this->size_)
	{
		this->v_ = new T[this->size_];

		forAll(*this, i)
		{
			this->operator[](i) = lst[i];
		}
	}
}


// Construct as copy of BiIndirectList<T>
template<class T>
tnbLib::List<T>::List(const BiIndirectList<T>& lst)
	:
	UList<T>(NULL, lst.size())
{
	if (this->size_)
	{
		this->v_ = new T[this->size_];

		forAll(*this, i)
		{
			this->operator[](i) = lst[i];
		}
	}
}


// * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * * //

// Destroy list elements
template<class T>
tnbLib::List<T>::~List()
{
	if (this->v_) delete[] this->v_;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
void tnbLib::List<T>::setSize(const label newSize)
{
	if (newSize < 0)
	{
		FatalErrorIn("List<T>::setSize(const label)")
			<< "bad set size " << newSize
			<< abort(FatalError);
	}

	if (newSize != this->size_)
	{
		if (newSize > 0)
		{
			T* nv = new T[label(newSize)];

			if (this->size_)
			{
				register label i = min(this->size_, newSize);

#               ifdef USEMEMCPY
				if (contiguous<T>())
				{
					memcpy(nv, this->v_, i * sizeof(T));
				}
				else
#               endif
				{
					register T* vv = &this->v_[i];
					register T* av = &nv[i];
					while (i--) *--av = *--vv;
				}
			}
			if (this->v_) delete[] this->v_;

			this->size_ = newSize;
			this->v_ = nv;
		}
		else
		{
			clear();
		}
	}
}


template<class T>
void tnbLib::List<T>::setSize(const label newSize, const T& a)
{
	label oldSize = this->size_;
	this->setSize(newSize);

	if (newSize > oldSize)
	{
		register label i = newSize - oldSize;
		register T* vv = &this->v_[newSize];
		while (i--) *--vv = a;
	}
}


template<class T>
void tnbLib::List<T>::clear()
{
	if (this->v_) delete[] this->v_;
	this->size_ = 0;
	this->v_ = 0;
}


// Transfer the contents of the argument List into this List
// and anull the argument list
template<class T>
void tnbLib::List<T>::transfer(List<T>& a)
{
	if (this->v_) delete[] this->v_;
	this->size_ = a.size_;
	this->v_ = a.v_;

	a.size_ = 0;
	a.v_ = 0;
}


// Transfer the contents of the argument DynamicList into this List
// and anull the argument list
template<class T>
template<unsigned SizeInc, unsigned SizeMult, unsigned SizeDiv>
void tnbLib::List<T>::transfer(DynamicList<T, SizeInc, SizeMult, SizeDiv>& a)
{
	// shrink the allocated space to the number of elements used
	a.shrink();
	transfer(static_cast<List<T>&>(a));
	a.clearStorage();
}


// Transfer the contents of the argument SortableList into this List
// and anull the argument list
template<class T>
void tnbLib::List<T>::transfer(SortableList<T>& a)
{
	// shrink away the sort indices
	a.shrink();
	transfer(static_cast<List<T>&>(a));
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

// Assignment to UList operator. Takes linear time.
template<class T>
void tnbLib::List<T>::operator=(const UList<T>& a)
{
	if (a.size_ != this->size_)
	{
		if (this->v_) delete[] this->v_;
		this->v_ = 0;
		this->size_ = a.size_;
		if (this->size_) this->v_ = new T[this->size_];
	}

	if (this->size_)
	{
#       ifdef USEMEMCPY
		if (contiguous<T>())
		{
			memcpy(this->v_, a.v_, this->byteSize());
		}
		else
#       endif
		{
			List_ACCESS(T, (*this), vp);
			List_CONST_ACCESS(T, a, ap);
			List_FOR_ALL((*this), i)
				List_ELEM((*this), vp, i) = List_ELEM(a, ap, i);
			List_END_FOR_ALL
		}
	}
}


// Assignment operator. Takes linear time.
template<class T>
void tnbLib::List<T>::operator=(const List<T>& a)
{
	if (this == &a)
	{
		FatalErrorIn("List<T>::operator=(const List<T>&)")
			<< "attempted assignment to self"
			<< abort(FatalError);
	}

	operator=(static_cast<const UList<T>&>(a));
}


// Assignment operator. Takes linear time.
template<class T>
void tnbLib::List<T>::operator=(const SLList<T>& lst)
{
	if (lst.size() != this->size_)
	{
		if (this->v_) delete[] this->v_;
		this->v_ = 0;
		this->size_ = lst.size();
		if (this->size_) this->v_ = new T[this->size_];
	}

	if (this->size_)
	{
		label i = 0;
		for
			(
				typename SLList<T>::const_iterator iter = lst.begin();
				iter != lst.end();
				++iter
				)
		{
			this->operator[](i++) = iter();
		}
	}
}


// Assignment operator. Takes linear time.
template<class T>
void tnbLib::List<T>::operator=(const IndirectList<T>& lst)
{
	if (lst.size() != this->size_)
	{
		if (this->v_) delete[] this->v_;
		this->v_ = 0;
		this->size_ = lst.size();
		if (this->size_) this->v_ = new T[this->size_];
	}

	forAll(*this, i)
	{
		this->operator[](i) = lst[i];
	}
}


// Assignment operator. Takes linear time.
template<class T>
void tnbLib::List<T>::operator=(const UIndirectList<T>& lst)
{
	if (lst.size() != this->size_)
	{
		if (this->v_) delete[] this->v_;
		this->v_ = 0;
		this->size_ = lst.size();
		if (this->size_) this->v_ = new T[this->size_];
	}

	forAll(*this, i)
	{
		this->operator[](i) = lst[i];
	}
}


// Assignment operator. Takes linear time.
template<class T>
void tnbLib::List<T>::operator=(const BiIndirectList<T>& lst)
{
	if (lst.size() != this->size_)
	{
		if (this->v_) delete[] this->v_;
		this->v_ = 0;
		this->size_ = lst.size();
		if (this->size_) this->v_ = new T[this->size_];
	}

	forAll(*this, i)
	{
		this->operator[](i) = lst[i];
	}
}

// * * * * * * * * * * * * * * * *  IOStream operators * * * * * * * * * * * //

#include <ListIO.cxx>