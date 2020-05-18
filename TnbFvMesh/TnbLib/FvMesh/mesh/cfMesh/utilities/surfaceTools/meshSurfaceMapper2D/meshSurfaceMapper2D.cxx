#include <meshSurfaceMapper2D.hxx>

#include <meshSurfaceEngine.hxx>
#include <meshSurfacePartitioner.hxx>
#include <polyMeshGen2DEngine.hxx>
#include <triSurf.hxx>
#include <triSurfacePartitioner.hxx>
#include <demandDrivenData.hxx>
#include <meshOctree.hxx>
#include <helperFunctions.hxx>

// #define DEBUGSearch

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

	void meshSurfaceMapper2D::findActiveBoundaryEdges()
	{
		const VRWGraph& edgeFaces = surfaceEngine_.edgeFaces();

		const polyMeshGen2DEngine& mesh2DEngine = this->mesh2DEngine();
		const boolList& activeFace = mesh2DEngine.activeFace();

		const label startFace = surfaceEngine_.mesh().boundaries()[0].patchStart();

		activeBoundaryEdges_.clear();

		//- check which edges are at the boundary
		forAll(edgeFaces, edgeI)
		{
			if (edgeFaces.sizeOfRow(edgeI) == 2)
			{
				const bool active0 = activeFace[startFace + edgeFaces(edgeI, 0)];
				const bool active1 = activeFace[startFace + edgeFaces(edgeI, 1)];

				if (active0 && active1)
					activeBoundaryEdges_.append(edgeI);
			}
		}

		if (Pstream::parRun())
		{
			const Map<label>& globalToLocal =
				surfaceEngine_.globalToLocalBndEdgeAddressing();
			const Map<label>& otherProc = surfaceEngine_.otherEdgeFaceAtProc();

			const DynList<label>& neiProcs = surfaceEngine_.beNeiProcs();

			std::map<label, labelLongList> exchangeData;
			forAll(neiProcs, i)
				exchangeData[neiProcs[i]].clear();

			forAllConstIter(Map<label>, globalToLocal, it)
			{
				const label beI = it();

				if (activeFace[startFace + edgeFaces(beI, 0)])
				{
					exchangeData[otherProc[beI]].append(it.key());
				}
			}

			labelLongList receivedData;
			help::exchangeMap(exchangeData, receivedData);

			forAll(receivedData, i)
			{
				const label beI = globalToLocal[receivedData[i]];

				if (activeFace[startFace + edgeFaces(beI, 0)])
					activeBoundaryEdges_.append(beI);
			}
		}
	}

	void meshSurfaceMapper2D::create2DEngine() const
	{
		polyMeshGen& mesh = const_cast<polyMeshGen&>(surfaceEngine_.mesh());
		mesh2DEnginePtr_ = new polyMeshGen2DEngine(mesh);
	}

	void meshSurfaceMapper2D::createTriSurfacePartitioner() const
	{
		surfPartitionerPtr_ = new triSurfacePartitioner(meshOctree_.surface());
	}

	void meshSurfaceMapper2D::createMeshSurfacePartitioner() const
	{
		meshPartitionerPtr_ = new meshSurfacePartitioner(surfaceEngine_);
	}

	void meshSurfaceMapper2D::clearOut()
	{
		deleteDemandDrivenData(mesh2DEnginePtr_);
		deleteDemandDrivenData(surfPartitionerPtr_);
		deleteDemandDrivenData(meshPartitionerPtr_);
	}

	// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

	meshSurfaceMapper2D::meshSurfaceMapper2D
	(
		const meshSurfaceEngine& mse,
		const meshOctree& octree
	)
		:
		surfaceEngine_(mse),
		meshOctree_(octree),
		mesh2DEnginePtr_(NULL),
		surfPartitionerPtr_(NULL),
		meshPartitionerPtr_(NULL)
	{
		if (Pstream::parRun())
		{
			//- allocate bpAtProcs and other addressing
			//- this is done here to prevent possible deadlocks
			surfaceEngine_.bpAtProcs();
		}

		findActiveBoundaryEdges();

		createMeshSurfacePartitioner();
	}

	// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

	meshSurfaceMapper2D::~meshSurfaceMapper2D()
	{
		clearOut();
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //