#pragma once
#ifndef _Cad2d_Plane_Manager_Header
#define _Cad2d_Plane_Manager_Header

#include <Standard_TypeDef.hxx>
#include <Global_Serialization.hxx>
#include <Cad2d_Module.hxx>

#include <memory>
#include <vector>

namespace tnbLib
{

	// Forward Declarations
	template<class EntityType>
	class Cad_EntityManager;

	class Pln_Entity;
	class Pln_Edge;
	class Pln_Vertex;

	namespace cad2dLib
	{

		class Plane_Manager
		{

			/*Private Data*/

			std::shared_ptr<Cad_EntityManager<Pln_Vertex>> theVertices_;
			std::shared_ptr<Cad_EntityManager<Pln_Edge>> theEdges_;

			/*private functions and operators*/
			DECLARE_SAVE_LOAD_HEADER(TnbCad2d_EXPORT);

		protected:

			Plane_Manager()
			{}


			auto& ChangeCorners()
			{
				return theVertices_;
			}

			auto& ChangeSegments()
			{
				return theEdges_;
			}

		public:

			Standard_Integer NbCorners() const;

			Standard_Integer NbSegments() const;

			const auto& Corners() const
			{
				return theVertices_;
			}

			const auto& Segments() const
			{
				return theEdges_;
			}

			void RetrieveCornersTo
			(
				std::vector<std::shared_ptr<Pln_Entity>>& theEntities
			) const;

			void RetrieveSegmentsTo
			(
				std::vector<std::shared_ptr<Pln_Entity>>& theEntities
			) const;
		};
	}
}

#endif // !_Cad2d_Plane_Manager_Header
