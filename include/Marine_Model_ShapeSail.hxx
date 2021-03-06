#pragma once
#ifndef _Marine_Model_ShapeSail_Header
#define _Marine_Model_ShapeSail_Header

#include <Marine_GeoSailModel.hxx>

namespace tnbLib
{

	namespace marineLib
	{

		// Forward Declarations
		class Body_Sail;

		class Model_ShapeSail
			: public Marine_GeoSailModel
		{

			/*Private Data*/

			std::shared_ptr<Body_Sail> theBody_;


			DECLARE_SAVE_LOAD_HEADER(TnbMarine_EXPORT);

		protected:

			Model_ShapeSail()
			{}

		public:

			Model_ShapeSail
			(
				const std::shared_ptr<Body_Sail>& theSail
			);

			Model_ShapeSail
			(
				std::shared_ptr<Body_Sail>&& theSail
			);

			Model_ShapeSail
			(
				const Standard_Integer theIndex,
				const std::shared_ptr<Body_Sail>& theSail
			);

			Model_ShapeSail
			(
				const Standard_Integer theIndex, 
				std::shared_ptr<Body_Sail>&& theSail
			);

			Model_ShapeSail
			(
				const Standard_Integer theIndex, 
				const word& theName, 
				const std::shared_ptr<Body_Sail>& theSail
			);

			Model_ShapeSail
			(
				const Standard_Integer theIndex, 
				const word& theName,
				std::shared_ptr<Body_Sail>&& theSail
			);

			Marine_SailModelType SailType() const override
			{
				return Marine_SailModelType::shape;
			}

			const auto& Body() const
			{
				return theBody_;
			}
		};
	}
}

#endif // !_Marine_Model_ShapeSail_Header
