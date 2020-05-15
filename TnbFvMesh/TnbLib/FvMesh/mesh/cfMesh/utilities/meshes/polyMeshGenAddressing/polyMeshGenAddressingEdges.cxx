#include <polyMeshGenAddressing.hxx>

#include <DynList.hxx>
#include <helperFunctions.hxx>
#include <demandDrivenData.hxx>

# ifdef USE_OMP
#include <omp.h>
# endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

	void polyMeshGenAddressing::calcEdges() const
	{
		if (edgesPtr_)
		{
			FatalErrorIn("polyMeshGenAddressing::calcEdges() const")
				<< "edges already calculated"
				<< abort(FatalError);
		}
		else
		{
			const faceListPMG& faces = mesh_.faces();
			const VRWGraph& pointFaces = this->pointFaces();

			edgesPtr_ = new edgeList();
			label nEdges(0);
			labelList nfe(faces.size());

# ifdef USE_OMP
			const label nThreads = 3 * omp_get_num_procs();
			labelList nEdgesForThread(nThreads);
# else
			labelList nEdgesForThread(1);
# endif

# ifdef USE_OMP
# pragma omp parallel num_threads(nThreads) if( faces.size() > 1000 )
# endif
			{
				LongList<edge> edgesHelper;

# ifdef USE_OMP
# pragma omp for schedule(static)
# endif
				forAll(faces, faceI)
				{
					const face& f = faces[faceI];

					forAll(f, pI)
					{
						const edge fe = f.faceEdge(pI);
						const label s = fe.start();
						const label e = fe.end();

						DynList<label> edgeFaces;

						bool store(true);

						//- find all faces attached to this edge
						//- store the edge in case the face faceI is the face
						//- with the smallest label
						forAllRow(pointFaces, s, pfI)
						{
							const label ofI = pointFaces(s, pfI);
							const face& of = faces[ofI];

							if (of.which(e) < 0)
								continue;
							if (ofI < faceI)
							{
								store = false;
								break;
							}

							edgeFaces.append(ofI);
						}

						if (store)
							edgesHelper.append(fe);
					}
				}

				//- this enables other threads to see the number of edges
				//- generated by each thread
# ifdef USE_OMP
				nEdgesForThread[omp_get_thread_num()] = edgesHelper.size();
# else
				nEdgesForThread[0] = edgesHelper.size();
# endif

# ifdef USE_OMP
# pragma omp critical
# endif
				nEdges += edgesHelper.size();

# ifdef USE_OMP
# pragma omp barrier

# pragma omp master
# endif
				edgesPtr_->setSize(nEdges);

# ifdef USE_OMP
# pragma omp barrier
# endif

				//- find the starting position of the edges generated by this thread
				//- in the global list of edges
				label localStart(0);
# ifdef USE_OMP
				const label threadI = omp_get_thread_num();
# else
				const label threadI = 0;
# endif
				for (label i = 0; i < threadI; ++i)
					localStart += nEdgesForThread[i];

				//- store edges into the global list
				forAll(edgesHelper, i)
					edgesPtr_->operator[](localStart + i) = edgesHelper[i];
			}
		}
	}

	// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

	const edgeList& polyMeshGenAddressing::edges() const
	{
		if (!edgesPtr_)
		{
# ifdef USE_OMP
			if (omp_in_parallel())
				FatalErrorIn
				(
					"const edgeList& polyMeshGenAddressing::edges() const"
				) << "Calculating addressing inside a parallel region."
				<< " This is not thread safe" << exit(FatalError);
# endif

			calcEdges();
		}

		return *edgesPtr_;
	}

	void polyMeshGenAddressing::clearOutEdges()
	{
		deleteDemandDrivenData(edgesPtr_);
	}


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// ************************************************************************* //