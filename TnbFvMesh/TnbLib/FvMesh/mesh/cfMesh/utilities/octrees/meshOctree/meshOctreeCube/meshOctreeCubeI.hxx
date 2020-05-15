#pragma once
#include <triSurface.hxx>
#include <meshOctreeSlot.hxx>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace tnbLib
{

	inline meshOctreeCube::meshOctreeCube()
		:
		meshOctreeCubeBasic(),
		activeSlotPtr_(NULL),
		subCubesPtr_(NULL),
		cubeLabel_(-1),
		containedElementsLabel_(-1),
		containedEdgesLabel_(-1)
	{}

	inline meshOctreeCube::meshOctreeCube(const meshOctreeCube& moc)
		:
		meshOctreeCubeBasic(moc.coordinates(), moc.cubeType(), moc.procNo()),
		activeSlotPtr_(moc.activeSlotPtr_),
		subCubesPtr_(moc.subCubesPtr_),
		cubeLabel_(moc.cubeLabel_),
		containedElementsLabel_(moc.containedElementsLabel_),
		containedEdgesLabel_(moc.containedEdgesLabel_)
	{}

	inline const meshOctreeSlot* meshOctreeCube::slotPtr() const
	{
		return activeSlotPtr_;
	}

	inline bool meshOctreeCube::isLeaf() const
	{
		if (!subCubesPtr_)
			return true;

		return false;
	}

	inline meshOctreeCube* meshOctreeCube::subCube(const label scI) const
	{
		return subCubesPtr_[scI];
	}

	inline label meshOctreeCube::cubeLabel() const
	{
		return cubeLabel_;
	}

	inline bool meshOctreeCube::hasContainedElements() const
	{
		if (containedElementsLabel_ == -1)
			return false;

		return true;
	}

	inline label meshOctreeCube::containedElements() const
	{
		return containedElementsLabel_;
	}

	inline bool meshOctreeCube::hasContainedEdges() const
	{
		if (containedEdgesLabel_ == -1)
			return false;;

		return true;
	}

	inline label meshOctreeCube::containedEdges() const
	{
		return containedEdgesLabel_;
	}

	inline void meshOctreeCube::operator=(const meshOctreeCube& moc)
	{
		static_cast<meshOctreeCubeCoordinates&>(*this) = moc.coordinates();
		activeSlotPtr_ = moc.activeSlotPtr_;
		subCubesPtr_ = moc.subCubesPtr_;
		cubeLabel_ = moc.cubeLabel_;
		containedElementsLabel_ = moc.containedElementsLabel_;
		containedEdgesLabel_ = moc.containedEdgesLabel_;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace tnbLib

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //