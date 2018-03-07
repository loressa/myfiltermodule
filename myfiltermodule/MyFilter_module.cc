////////////////////////////////////////////////////////////////////////
// Class:       MyFilter
// Module Type: filter
// File:        MyFilter_module.cc
//
// Generated at Mon Oct  3 13:17:19 2016 by Lorena Escudero Sanchez using artmod
// from cetpkgsupport v1_10_02.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"

#include <memory>

#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCTrajectory.h"
#include "larcore/Geometry/Geometry.h"


class MyFilter;
class MyFilter : public art::EDFilter
{
public:
    explicit MyFilter(fhicl::ParameterSet const & p);
    MyFilter(MyFilter const &) = delete;
    MyFilter(MyFilter &&) = delete;
    MyFilter & operator = (MyFilter const &) = delete;
    MyFilter & operator = (MyFilter &&) = delete;

    bool filter(art::Event & e) override;

private:
    int   m_interactionType;        ///< The interaction type (NUANCE code) required to pass the filter
    float m_detectorHalfLengthX;    ///< Half the detector length in X
    float m_detectorHalfLengthY;    ///< Half the detector length in Y
    float m_detectorHalfLengthZ;    ///< Half the detector length in Z
    float m_coordinateOffsetX;      ///< Position of X = 0 wrt global coordinates
    float m_coordinateOffsetY;      ///< Position of Y = 0 wrt global coordinates
    float m_coordinateOffsetZ;      ///< Position of Z = 0 wrt global coordinates
    float m_selectedBorderX;        ///< Minimum fiducial distance from detector edge in X
    float m_selectedBorderY;        ///< Minimum fiducial distance from detector edge in Y
    float m_selectedBorderZ;        ///< Minimum fiducial distance from detector edge in Z
};

// -----------------------------------------------------------------------------------------------------------------------------------------

MyFilter::MyFilter(fhicl::ParameterSet const & p) :
    m_interactionType(p.get<int>("InteractionType", -1)),
    m_detectorHalfLengthX(p.get<float>("DetectorHalfLengthX")),
    m_detectorHalfLengthY(p.get<float>("DetectorHalfLengthY")),
    m_detectorHalfLengthZ(p.get<float>("DetectorHalfLengthZ")),
    m_coordinateOffsetX(p.get<float>("CoordinateOffsetX")),
    m_coordinateOffsetY(p.get<float>("CoordinateOffsetY")),
    m_coordinateOffsetZ(p.get<float>("CoordinateOffsetZ")),
    m_selectedBorderX(p.get<float>("SelectedBorderX")),
    m_selectedBorderY(p.get<float>("SelectedBorderY")),
    m_selectedBorderZ(p.get<float>("SelectedBorderZ"))
{
}

// -----------------------------------------------------------------------------------------------------------------------------------------

bool MyFilter::filter(art::Event & e)
{
    art::Handle< std::vector< simb::MCTruth > > mct_handle;
    e.getByLabel("generator", mct_handle );

    // Check the event has truth information
    if (!mct_handle.isValid()) return false;
    if (mct_handle->empty()) return false;

    for(auto const& mct : (*mct_handle))
    {
        // Check this truth block is from a generated beam neutrino
        if(mct.Origin() != simb::kBeamNeutrino) continue;

        // Check the interaction type is correct (if one has been supplied in fcl)
        if(m_interactionType != -1 && mct.GetNeutrino().InteractionType() != m_interactionType) return false;
  
        // Check the neutrino vertex is fiducial
        float nuVtxX = mct.GetNeutrino().Nu().Vx();                                                                      
        float nuVtxY = mct.GetNeutrino().Nu().Vy();                                                                             
        float nuVtxZ = mct.GetNeutrino().Nu().Vz(); 

        if ((nuVtxX > (m_detectorHalfLengthX - m_coordinateOffsetX - m_selectedBorderX)) || (nuVtxX < (-m_coordinateOffsetX + m_selectedBorderX)) ||
            (nuVtxY > (m_detectorHalfLengthY - m_coordinateOffsetY - m_selectedBorderY)) || (nuVtxY < (-m_coordinateOffsetY + m_selectedBorderY)) ||
            (nuVtxZ > (m_detectorHalfLengthZ - m_coordinateOffsetZ - m_selectedBorderZ)) || (nuVtxZ < (-m_coordinateOffsetZ + m_selectedBorderZ)) ) 
            return false;

        return true;
    }

    return false;
}

DEFINE_ART_MODULE(MyFilter)
