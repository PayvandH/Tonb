#pragma once
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	template<class Cmpt, int length>
	class TensorN;

	// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

	template <class Cmpt, int length>
	const char* const VectorN<Cmpt, length>::typeName =
		("vector" + name(length)).c_str();

	template <class Cmpt, int length>
	const VectorN<Cmpt, length> VectorN<Cmpt, length>::zero(0);

	template <class Cmpt, int length>
	const VectorN<Cmpt, length> VectorN<Cmpt, length>::one(1);

	template <class Cmpt, int length>
	const VectorN<Cmpt, length> VectorN<Cmpt, length>::max(VGREAT);

	template <class Cmpt, int length>
	const VectorN<Cmpt, length> VectorN<Cmpt, length>::min(-VGREAT);


	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	// Construct null
	template <class Cmpt, int length>
	VectorN<Cmpt, length>::VectorN()
	{}


	// Construct given VectorSpace
	template <class Cmpt, int length>
	VectorN<Cmpt, length>::VectorN
	(
		const VectorSpace<VectorN<Cmpt, length>, Cmpt, length>& vs
	)
		: VectorSpace<VectorN<Cmpt, length>, Cmpt, length>(vs)
	{}


	// Construct given Cmpts
	template <class Cmpt, int length>
	VectorN<Cmpt, length>::VectorN(const Cmpt& vx)
	{
		VectorSpaceOps<VectorN<Cmpt, length>::nComponents, 0>::eqOpS
		(*this, vx, eqOp<Cmpt>());
	}


	// Construct from Istream
	template <class Cmpt, int length>
	VectorN<Cmpt, length>::VectorN(Istream& is)
		: VectorSpace<VectorN<Cmpt, length>, Cmpt, length>(is)
	{}


	// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

	template <class Cmpt, int length>
	const Cmpt& VectorN<Cmpt, length>::operator()(const direction i) const
	{
		return this->operator[](i);
	}


	template <class Cmpt, int length>
	Cmpt& VectorN<Cmpt, length>::operator()(const direction i)
	{
		return this->operator[](i);
	}


	//- Multiply components of VectorN by VectorN
	template<class Cmpt, int length>
	VectorN<Cmpt, length>
		VectorN<Cmpt, length>::cmptMultiply(const VectorN<Cmpt, length>& v)
	{
		VectorN<Cmpt, length> res;
		VectorSpaceOps<VectorN<Cmpt, length>::nComponents, 0>::op
		(res, *this, v, multiplyOp<Cmpt>());
		return res;
	}


	//- Transform the spherical tensor
	//- The components are assumed to be individual scalars
	//- i.e. transform has no effect
	template<class Cmpt, int length>
	VectorN<Cmpt, length> transform
	(
		const tensor& tt,
		const VectorN<Cmpt, length>& v
	)
	{
		return v;
	}


	// * * * * * * * * * * * * * * * Global Operators  * * * * * * * * * * * * * //

	template<class Cmpt, int length>
	class outerProduct<Cmpt, VectorN<Cmpt, length>>
	{
	public:

		typedef VectorN<Cmpt, length> type;
	};


	template<class Cmpt, int length>
	class outerProduct<VectorN<Cmpt, length>, Cmpt>
	{
	public:

		typedef VectorN<Cmpt, length> type;
	};


	template<class Cmpt, int length>
	class outerProduct<VectorN<Cmpt, length>, VectorN<Cmpt, length>>
	{
	public:

		typedef TensorN<Cmpt, length> type;
	};


	//- Inner-product between a vector and a vector
	template <class Cmpt, int length>
	Cmpt operator&
		(
			const VectorN<Cmpt, length>& v1,
			const VectorN<Cmpt, length>& v2
			)
	{
		Cmpt res = pTraits<Cmpt>::zero;
		VectorSpaceOps<VectorN<Cmpt, length>::nComponents, 0>::SopEqOpVV
		(res, v1, v2, plusEqOp<Cmpt>(), multiplyOp<Cmpt>());
		return res;
	}


	//- Componentwise division of scalar by VectorN
	template<class Cmpt, int length>
	VectorN<Cmpt, length>
		operator/(const scalar s, const VectorN<Cmpt, length>& v)
	{
		VectorN<Cmpt, length> res;
		VectorSpaceOps<VectorN<Cmpt, length>::nComponents, 0>::opSV
		(res, s, v, divideOp3<Cmpt, scalar, Cmpt>());
		return res;
	}


	//- Multiply components of VectorN by VectorN
	template<class Cmpt, int length>
	VectorN<Cmpt, length>
		cmptMultiply(const VectorN<Cmpt, length>& v1, const VectorN<Cmpt, length>& v2)
	{
		VectorN<Cmpt, length> res;
		VectorSpaceOps<VectorN<Cmpt, length>::nComponents, 0>::op
		(res, v1, v2, multiplyOp<Cmpt>());
		return res;
	}


	//- Multiply row of VectorN with a scalar
	template<class Cmpt, int length>
	VectorN<Cmpt, length>
		scaleRow(const VectorN<Cmpt, length>& v, const Cmpt& c)
	{
		return v * c;
	}


	//- Return the component sum
	// template <class Cmpt, int length>
	// Cmpt sum(const VectorN<Cmpt, length>& v)
	// {
	//     Cmpt result = pTraits<Cmpt>::zero;
	//     for(register label i=0; i<VectorN<Cmpt, length>::nComponents; i++)
	//     {
	//         result += v[i];
	//     }
	//     return result;
	// }


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //