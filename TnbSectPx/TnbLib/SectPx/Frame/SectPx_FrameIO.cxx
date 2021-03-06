#include <SectPx_Frame.hxx>

#include <SectPx_Registry.hxx>
#include <SectPx_Makers.hxx>

DECLARE_SAVE_IMP(tnbLib::SectPx_Frame::EntityMaker)
{
	ar & Par;
	ar & Pnt;
	ar & GeoMap;
	ar & CmptProfile;
	ar & FieldFun;
}

DECLARE_LOAD_IMP(tnbLib::SectPx_Frame::EntityMaker)
{
	ar & Par;
	ar & Pnt;
	ar & GeoMap;
	ar & CmptProfile;
	ar & FieldFun;
}

DECLARE_SAVE_IMP(tnbLib::SectPx_Frame)
{
	ar & boost::serialization::base_object<SectPx_Entity>(*this);
	ar & theRegistry_;
	ar & theMakers_;
}

DECLARE_LOAD_IMP(tnbLib::SectPx_Frame)
{
	ar & boost::serialization::base_object<SectPx_Entity>(*this);
	ar & theRegistry_;
	ar & theMakers_;
}

BOOST_CLASS_EXPORT_IMPLEMENT(tnbLib::SectPx_Frame);