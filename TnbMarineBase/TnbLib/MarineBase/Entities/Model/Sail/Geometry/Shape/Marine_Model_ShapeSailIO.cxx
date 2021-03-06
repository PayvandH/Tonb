#include <Marine_Model_ShapeSail.hxx>

#include <Marine_Body_Sail.hxx>

DECLARE_SAVE_IMP(tnbLib::marineLib::Model_ShapeSail)
{
	ar & boost::serialization::base_object<Marine_GeoSailModel>(*this);
	ar & theBody_;
}

DECLARE_LOAD_IMP(tnbLib::marineLib::Model_ShapeSail)
{
	ar & boost::serialization::base_object<Marine_GeoSailModel>(*this);
	ar & theBody_;
}