#pragma once
#ifndef _SectPx_PairedInterfaceAdaptor_Header
#define _SectPx_PairedInterfaceAdaptor_Header

#include <Standard_TypeDef.hxx>

#include <memory>

namespace tnbLib
{

	// Forward Declarations
	class SectPx_PairedInterface;
	
	namespace maker { class Interface; }

	class SectPx_PairedInterfaceAdaptor
	{

		friend class maker::Interface;

		/*Private Data*/

		std::weak_ptr<SectPx_PairedInterface> thePair_;

	protected:

		SectPx_PairedInterfaceAdaptor()
		{}

		void SetPair(const std::weak_ptr<SectPx_PairedInterface>& thePair)
		{
			thePair_ = thePair;
		}


	public:

		const auto& Pair() const
		{
			return thePair_;
		}
	};
}

#endif // !_SectPx_PairedInterfaceAdaptor_Header
