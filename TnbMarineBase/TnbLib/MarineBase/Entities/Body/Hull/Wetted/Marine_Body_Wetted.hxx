#pragma once
#ifndef _Marine_Body_Wetted_Header
#define _Marine_Body_Wetted_Header

#include <Marine_HullBody.hxx>

namespace tnbLib
{

	namespace marineLib
	{

		class Body_Wetted
			: public Marine_HullBody
		{

			friend class Marine_DisctLib;

			/*Private Data*/

			
			std::shared_ptr<Marine_CmpSection> theMid_;


			//- private functions and operators


			DECLARE_SAVE_LOAD_HEADER(TnbMarine_EXPORT);

			auto & ChangeMid()
			{
				return theMid_;
			}

			void SetMid(const std::shared_ptr<Marine_CmpSection>& theMid)
			{
				theMid_ = theMid;
			}

			void SetMid(const std::shared_ptr<Marine_CmpSection>&& theMid)
			{
				theMid_ = std::move(theMid);
			}

		protected:

			template<class... _Types>
			Body_Wetted(_Types&&... _Args)
				: Marine_HullBody(_Args...)
			{}

		public:

			virtual Standard_Boolean ShapeType() const
			{
				return Standard_False;
			}

			Standard_Boolean IsWetted() const override
			{
				return Standard_True;
			}

			Marine_BodyType Type() const override
			{
				return Marine_BodyType::wetted;
			}	

			const auto& Mid() const
			{
				return theMid_;
			}

			std::shared_ptr<Marine_Body> Copy() const override;
		};
	}
}

#include <Marine_BodyConstructor.hxx>

namespace tnbLib
{
	namespace marineLib
	{

		template<>
		class BodyConstructor_Shape<Body_Wetted>
			: public Body_Wetted
		{

			/*Private Data*/

			std::shared_ptr<Marine_Shape> theShape_;

			std::shared_ptr<Marine_CmpSection> theWater_;

			//- private functions and operators

			DECLARE_SAVE_LOAD_HEADER(TnbMarine_EXPORT);

			auto& ChangeShape()
			{
				return theShape_;
			}

			auto& ChangeWL()
			{
				return theWater_;
			}

		public:

			template<class... _Types>
			BodyConstructor_Shape(_Types&&... _Args)
				: Body_Wetted(_Args...)
			{}

			Standard_Boolean ShapeType() const override
			{
				return Standard_True;
			}

			const auto& Shape() const
			{
				return theShape_;
			}

			void SetWL(const std::shared_ptr<Marine_CmpSection>& theWater)
			{
				theWater_ = theWater;
			}

			void SetWL(std::shared_ptr<Marine_CmpSection>&& theWater)
			{
				theWater_ = std::move(theWater);
			}

			void SetShape(const std::shared_ptr<Marine_Shape>& theShape)
			{
				theShape_ = theShape;
			}

			void SetShape(std::shared_ptr<Marine_Shape>&& theShape)
			{
				theShape_ = theShape;
			}

			const auto& WL() const
			{
				return theWater_;
			}
		};
	}
}

#endif // !_Marine_Body_Wetted_Header
