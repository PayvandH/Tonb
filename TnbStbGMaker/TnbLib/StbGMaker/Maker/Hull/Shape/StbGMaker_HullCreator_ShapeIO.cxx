#include <StbGMaker_HullCreator_Shape.hxx>

#include <Marine_Shapes.hxx>

DECLARE_SAVE_IMP(tnbLib::stbGmakerLib::HullCreator_Shape)
{
	ar & boost::serialization::base_object<StbGMaker_HullCreator>(*this);
	ar & theHull_;
}

DECLARE_LOAD_IMP(tnbLib::stbGmakerLib::HullCreator_Shape)
{
	ar & boost::serialization::base_object<StbGMaker_HullCreator>(*this);
	ar & theHull_;
}