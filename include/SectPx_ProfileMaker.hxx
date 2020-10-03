#pragma once
#ifndef _SectPx_ProfileMaker_Header
#define _SectPx_ProfileMaker_Header

#include <SectPx_Maker.hxx>
#include <Global_Done.hxx>

namespace tnbLib
{

	// Forward Declarations
	class SectPx_Pnt;
	class SectPx_Edge;
	class SectPx_Registry;
	class SectPx_ProfileQ;
	class SectPx_TopoProfile;
	class SectPx_Node;

	namespace maker
	{

		class Profile
			: public SectPx_Maker
			, public Global_Done
		{

			/*Private Data*/

			std::shared_ptr<SectPx_TopoProfile> theProfile_;


			Standard_Integer
				MakeEdge
				(
					const std::shared_ptr<SectPx_Pnt>& theP0,
					const std::shared_ptr<SectPx_Pnt>& theP1
				);

			std::shared_ptr<SectPx_Edge>
				RemoveEdge
				(
					const Standard_Integer theIndex
				);

			void RemoveEdge
			(
				const std::shared_ptr<SectPx_Edge>& theEdge
			);

			static void AddEdgeToPnt
			(
				const std::shared_ptr<SectPx_Edge>& theSeg,
				const std::shared_ptr<SectPx_Pnt>& thePnt
			);

		public:

			Profile
			(
				const std::shared_ptr<SectPx_TopoProfile>& theProfile,
				const std::shared_ptr<SectPx_Registry>& theRegistry
			)
				: SectPx_Maker(theRegistry)
				, theProfile_(theProfile)
			{}


			const auto& TopoProfile() const
			{
				return theProfile_;
			}

			const std::shared_ptr<SectPx_Node>& Node0() const;

			const std::shared_ptr<SectPx_Node>& Node1() const;

			Standard_Integer Init();

			Standard_Boolean
				IsOnBoundary
				(
					const std::shared_ptr<SectPx_Pnt>& thePnt
				) const;

			std::shared_ptr<SectPx_Edge>
				SelectEdge
				(
					const Standard_Integer theIndex
				) const;

			std::tuple<Standard_Integer, Standard_Integer>
				ImportPnt
				(
					const std::shared_ptr<SectPx_Pnt>& thePnt,
					const std::shared_ptr<SectPx_Edge>& theEdge
				);

			Standard_Integer
				RemovePnt
				(
					const std::shared_ptr<SectPx_Pnt>& thePnt
				);

			std::shared_ptr<SectPx_ProfileQ>
				RetrieveProfileQ() const;
		};
	}
}

#endif // !_SectPx_ProfileMaker_Header
