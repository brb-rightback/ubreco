////////////////////////////////////////////////////////////////////////
// Class:       T0RecoAnodeCathodePiercingAna
// Module Type: analyzer
// File:        T0RecoAnodeCathodePiercingAna_module.cc
//
// Generated at Wed Jul 13 20:22:39 2016
// Authors: David Caratelli & Chris Barnes
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// services etc...
#include "larcore/Geometry/Geometry.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"

// data-products
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/AnalysisBase/T0.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "lardataobj/MCBase/MCTrack.h"
#include "lardataobj/RecoBase/OpFlash.h"
#include "lardata/Utilities/AssociationUtil.h"

// C++
#include <memory>
#include <iostream>
#include <math.h>

// ROOT
#include <TTree.h>

class T0RecoAnodeCathodePiercingAna;

class T0RecoAnodeCathodePiercingAna : public art::EDAnalyzer {
public:
  explicit T0RecoAnodeCathodePiercingAna(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  T0RecoAnodeCathodePiercingAna(T0RecoAnodeCathodePiercingAna const &) = delete;
  T0RecoAnodeCathodePiercingAna(T0RecoAnodeCathodePiercingAna &&) = delete;
  T0RecoAnodeCathodePiercingAna & operator = (T0RecoAnodeCathodePiercingAna const &) = delete;
  T0RecoAnodeCathodePiercingAna & operator = (T0RecoAnodeCathodePiercingAna &&) = delete;

  void beginJob();

  // Required functions.
  void analyze(art::Event const & e) override;


private:

  // Declare member data here.
  std::string fTrackProducer;
  std::string fT0Producer;
  std::string fFlashProducer;
  std::string fCaloProducer;
  std::string fHitProducer;
  std::string fCosmicTagProducer;

  bool        fUseMC;

  // Declare the two resolution values
  double      fTPCResolution; // cm resolution on the faces of the TPC to determine if the track pierced that face
  double      fMCResolution; // cm resolution to allow mc-reco track matching. [Y,Z] must be within this distance
  
  // debug mode?
  bool _debug;

  // define top, bottom, front and back boundaries of TPC
  double _TOP, _BOTTOM, _FRONT, _BACK;

  double fDriftVelocity;

  TTree* _tree;
  double _mc_time;
  double _rc_time;
  double _t_match;
  double _dt_flash;
  double _pe_flash;
  double _length;
  double _rc_x_start, _rc_x_end;
  double _rc_y_start, _rc_y_end;
  double _rc_z_start, _rc_z_end;
  double _mc_x_start, _mc_x_end;
  double _mc_y_start, _mc_y_end;
  double _mc_z_start, _mc_z_end;
  int    _matched;
  int    _run, _subrun, _event;

  TTree* _mucs_tree;
  double _reco_time;

  TTree* _calo_tree;
  float _x, _y, _z;
  float _x_corr;
  float _px, _py, _pz;
  float _dqds;
  double _t0;

  TTree* _2dhit_tree;
  float _mag;
  float _2d_adc;

  // Define the variables for which face of the TPC that the particle pierces a bit more completely
  
  // Y-Faces of the TPC
  int    _enters_top;
  int    _exits_bottom;

  // Z-Faces of the TPC
  int    _enters_front;
  int    _enters_back;
  int    _exits_front;
  int    _exits_back;

  // X-Faces of the TPC
  int    _enters_anode;
  int    _enters_cathode;
  int    _exits_anode;
  int    _exits_cathode;

  // Set a boolean variable for if one of the conditionals for which face of the TPC the track entered has been set yet.
  bool if_loop_has_already_been_entered;

  // functions to be used throughout module
  bool   TrackEntersTop     (const std::vector<TVector3>& sorted_trk);
  bool   TrackEntersFront   (const std::vector<TVector3>& sorted_trk);
  bool   TrackEntersBack    (const std::vector<TVector3>& sorted_trk);
  bool   TrackEntersAnode   (const std::vector<TVector3>& sorted_trk);
  bool   TrackEntersSide    (const std::vector<TVector3>& sorted_trk);
  bool   TrackExitsBottom   (const std::vector<TVector3>& sorted_trk);
  bool   TrackExitsFront    (const std::vector<TVector3>& sorted_trk);
  bool   TrackExitsBack     (const std::vector<TVector3>& sorted_trk);
  bool   TrackExitsAnode    (const std::vector<TVector3>& sorted_trk);
  bool   TrackExitsSide     (const std::vector<TVector3>& sorted_trk);

  // functions to be used for organization in the module
  void   SortTrackPoints      (const recob::Track& track,
			       std::vector<TVector3>& sorted_trk);

  bool MatchTracks(const recob::Track& track, const sim::MCTrack& mctrk, const double& res);

};


T0RecoAnodeCathodePiercingAna::T0RecoAnodeCathodePiercingAna(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p)
{
  fTrackProducer     = p.get<std::string>("TrackProducer"    );
  fT0Producer        = p.get<std::string>("T0Producer"       );
  fFlashProducer     = p.get<std::string>("FlashProducer"    );
  fCaloProducer      = p.get<std::string>("CaloProducer"     );
  fHitProducer       = p.get<std::string>("HitProducer"      );
  fCosmicTagProducer = p.get<std::string>("CosmicTagProducer");
  fUseMC             = p.get<bool>       ("UseMC"            );
  fTPCResolution     = p.get<double>     ("TPCResolution"       );
  fMCResolution      = p.get<double>     ("MCResolution"        );
  _debug             = p.get<bool>       ("debug"            );

  // get boundaries based on detector bounds
  auto const* geom = lar::providerFrom<geo::Geometry>();

  _TOP    =   geom->DetHalfHeight() - fTPCResolution;
  _BOTTOM = - geom->DetHalfHeight() + fTPCResolution;
  _FRONT  =   fTPCResolution;
  _BACK   =   geom->DetLength() - fTPCResolution;

}

void T0RecoAnodeCathodePiercingAna::beginJob()
{

  // '_tree'
  art::ServiceHandle<art::TFileService> tfs;
  _tree = tfs->make<TTree>("_tree","T0 reco performance");
  _tree->Branch("_mc_time",&_mc_time,"mc_time/D");
  _tree->Branch("_rc_time",&_rc_time,"rc_time/D");
  _tree->Branch("_t_match",&_t_match,"t_match/D");
  _tree->Branch("_dt_flash",&_dt_flash,"dt_flash/D");
  _tree->Branch("_pe_flash",&_pe_flash,"pe_flash/D");
  _tree->Branch("_length", &_length, "length/D");
  _tree->Branch("_matched",&_matched,"matched/I");
  // Add branches for the first and last x, y, and z coordinates of the rc tracks and the mc tracks
  _tree->Branch("_mc_x_start",&_mc_x_start,"mc_x_start/D");
  _tree->Branch("_mc_y_start",&_mc_y_start,"mc_y_start/D");
  _tree->Branch("_mc_z_start",&_mc_z_start,"mc_z_start/D");
  _tree->Branch("_mc_x_end",&_mc_x_end,"mc_x_end/D");
  _tree->Branch("_mc_y_end",&_mc_y_end,"mc_y_end/D");
  _tree->Branch("_mc_z_end",&_mc_z_end,"mc_z_end/D");
  _tree->Branch("_rc_x_start",&_rc_x_start,"rc_x_start/D");
  _tree->Branch("_rc_y_start",&_rc_y_start,"rc_y_start/D");
  _tree->Branch("_rc_z_start",&_rc_z_start,"rc_z_start/D");
  _tree->Branch("_rc_x_end",&_rc_x_end,"rc_x_end/D");
  _tree->Branch("_rc_y_end",&_rc_y_end,"rc_y_end/D");
  _tree->Branch("_rc_z_end",&_rc_z_end,"rc_z_end/D");
  
  _tree->Branch("_run",&_run,"run/I");
  _tree->Branch("_subrun",&_subrun,"subrun/I");
  _tree->Branch("_event",&_event,"event/I");

  // Define the branches for which face of the TPC that the track pierced.
  // Y-Faces of TPC
  _tree->Branch("_enters_top", &_enters_top, "enters_top/I");
  _tree->Branch("_exits_bottom", &_exits_bottom, "exits_bottom/I");

  // Z-Faces of the TPC
  _tree->Branch("_enters_front", &_enters_front, "enters_front/I");
  _tree->Branch("_enters_back", &_enters_back, "enters_back/I");
  _tree->Branch("_exits_front", &_exits_front, "exits_front/I");
  _tree->Branch("_exits_back", &_exits_back, "exits_back/I");

  // X-Faces of the TPC
  _tree->Branch("_enters_anode", &_enters_anode, "enters_anode/I");
  _tree->Branch("_enters_cathode", &_enters_cathode, "enters_cathode/I");
  _tree->Branch("_exits_anode", &_exits_anode, "exits_anode/I");
  _tree->Branch("_exits_cathode", &_exits_cathode, "exits_cathode/I");

  // '_mucs_tree'
  _mucs_tree = tfs->make<TTree>("_mucs_tree","MuCS tagged tracks tree");
  _mucs_tree->Branch("_reco_time",&_reco_time,"reco_time/D");
  _mucs_tree->Branch("_length", &_length, "length/D");
  _mucs_tree->Branch("_t_match",&_t_match,"t_match/D");
  _mucs_tree->Branch("_dt_flash",&_dt_flash,"dt_flash/D");
  _mucs_tree->Branch("_pe_flash",&_pe_flash,"pe_flash/D");
  _mucs_tree->Branch("_rc_x_start",&_rc_x_start,"rc_x_start/D");
  _mucs_tree->Branch("_rc_y_start",&_rc_y_start,"rc_y_start/D");
  _mucs_tree->Branch("_rc_z_start",&_rc_z_start,"rc_z_start/D");
  _mucs_tree->Branch("_rc_x_end",&_rc_x_end,"rc_x_end/D");
  _mucs_tree->Branch("_rc_y_end",&_rc_y_end,"rc_y_end/D");
  _mucs_tree->Branch("_rc_z_end",&_rc_z_end,"rc_z_end/D");
  _mucs_tree->Branch("_run",&_run,"run/I");
  _mucs_tree->Branch("_subrun",&_subrun,"subrun/I");
  _mucs_tree->Branch("_event",&_event,"event/I");

  // Define the branches for which face of the TPC that the track pierced.                                                                                                   
  // Y-Faces of TPC                                                                                                                                                           
  _mucs_tree->Branch("_enters_top", &_enters_top, "enters_top/I");
  _mucs_tree->Branch("_exits_bottom", &_exits_bottom, "exits_bottom/I");

  // Z-Faces of the TPC                                                                                                                                                       
  _mucs_tree->Branch("_enters_front", &_enters_front, "enters_front/I");
  _mucs_tree->Branch("_enters_back", &_enters_back, "enters_back/I");
  _mucs_tree->Branch("_exits_front", &_exits_front, "exits_front/I");
  _mucs_tree->Branch("_exits_back", &_exits_back, "exits_back/I");

  // X-Faces of the TPC                                                                                                                                                    
  _mucs_tree->Branch("_enters_anode", &_enters_anode, "enters_anode/I");
  _mucs_tree->Branch("_enters_cathode", &_enters_cathode, "enters_cathode/I");
  _mucs_tree->Branch("_exits_anode", &_exits_anode, "exits_anode/I");
  _mucs_tree->Branch("_exits_cathode", &_exits_cathode, "exits_cathode/I");

  // '_calo_tree'
  _calo_tree = tfs->make<TTree>("_calo_tree","Track Calorimetry TTree");
  _calo_tree->Branch("_x",&_x,"x/F");
  _calo_tree->Branch("_x_corr",&_x_corr,"x_corr/F");
  _calo_tree->Branch("_y",&_y,"y/F");
  _calo_tree->Branch("_z",&_z,"z/F");
  _calo_tree->Branch("_px",&_px,"px/F");
  _calo_tree->Branch("_py",&_py,"py/F");
  _calo_tree->Branch("_pz",&_pz,"pz/F");
  _calo_tree->Branch("_t0",&_t0,"t0/D");
  _calo_tree->Branch("_run",&_run,"run/I");
  _calo_tree->Branch("_dqds",&_dqds,"dqds/F");

  // '_2dhit_tree'
  _2dhit_tree = tfs->make<TTree>("_2dhit_tree","2D Track Calorimetry TTree");
  _2dhit_tree->Branch("_mag",&_mag,"mag/F"); // track length measured as distance from entering to exiting point
  _2dhit_tree->Branch("_2d_adc",&_2d_adc,"2d_adc/F");
  _2dhit_tree->Branch("_rc_x_start",&_rc_x_start,"rc_x_start/D");
  _2dhit_tree->Branch("_rc_y_start",&_rc_y_start,"rc_y_start/D");
  _2dhit_tree->Branch("_rc_z_start",&_rc_z_start,"rc_z_start/D");
  _2dhit_tree->Branch("_rc_x_end",&_rc_x_end,"rc_x_end/D");
  _2dhit_tree->Branch("_rc_y_end",&_rc_y_end,"rc_y_end/D");
  _2dhit_tree->Branch("_rc_z_end",&_rc_z_end,"rc_z_end/D");
  _2dhit_tree->Branch("_t0",&_t0,"t0/D");

  // Use '_detp' to find 'efield' and 'temp'
  auto const* _detp = lar::providerFrom<detinfo::DetectorPropertiesService>();
  double efield = _detp -> Efield();
  double temp   = _detp -> Temperature();
  // Determine the drift velocity from 'efield' and 'temp'
  fDriftVelocity = _detp -> DriftVelocity(efield,temp);

}

void T0RecoAnodeCathodePiercingAna::analyze(art::Event const & e)
{
  _event  = e.event();
  _subrun = e.subRun();
  _run    = e.run();

  // Implementation of required member function here.

  // load tracks previously created for which T0 reconstruction is requested
  art::Handle<std::vector<recob::Track> > track_h;
  e.getByLabel( fTrackProducer , track_h );
  
  // make sure tracks look good
  if(!track_h.isValid()) {
    std::cerr<<"\033[93m[ERROR]\033[00m ... could not locate Track!"<<std::endl;
    throw std::exception();
  }

  // load MCtracks
  art::Handle<std::vector<sim::MCTrack> > mctrk_h;
  e.getByLabel("mcreco",mctrk_h);

  // if we should use MCTrack
  if (fUseMC){
    // make sure tracks look good
    if(!mctrk_h.isValid()) {
      std::cerr<<"\033[93m[ERROR]\033[00m ... could not locate MCTrack!"<<std::endl;
      throw std::exception();
    }
  }// if use MCTrack

  // grab T0 objects associated with tracks
  art::FindMany<anab::T0> trk_t0_assn_v(track_h, e, fT0Producer );

  // grab flashes associated with tracks
  art::FindMany<recob::OpFlash> trk_flash_assn_v(track_h, e, fFlashProducer );

  // grab calorimetry associated with tracks
  art::FindMany<anab::Calorimetry> trk_calo_assn_v(track_h, e, fCaloProducer );

  // grab 2d hits associated with tracks
  art::FindMany<recob::Hit> trk_hit_assn_v(track_h, e, fHitProducer);

  if (_debug)
    std::cout << "There are " << trk_flash_assn_v.size() << " track -> flash associations" << std::endl;

  // grab CosmicTag objects associated with tracks
  art::FindMany<anab::CosmicTag> trk_cosmictag_assn_v(track_h, e, fCosmicTagProducer);

  if (_debug)
    std::cout << "There are " << trk_cosmictag_assn_v.size() << " track -> cosmictag associations" << std::endl;

  for (size_t i=0; i < track_h->size(); i++){

    auto const& track = track_h->at(i);

    _matched = 0;

    const std::vector<const anab::T0*>& T0_v = trk_t0_assn_v.at(i);

    if (_debug)
      std::cout << T0_v.size() << "\tT0 objects associated to this track" << std::endl;

    const std::vector<const recob::OpFlash*>& flash_v = trk_flash_assn_v.at(i);

    if (_debug)
      std::cout << flash_v.size() << "\tflash objects associated to this track" << std::endl;

    // grab T0 object
    if (T0_v.size() == 1){

      auto t0 = T0_v.at(0);

      // get sorted points for the track object [assuming downwards going]
      std::vector<TVector3> sorted_trk;
      SortTrackPoints(track,sorted_trk);

      auto const& top    = sorted_trk.at(0);
      auto const& bottom = sorted_trk.at(sorted_trk.size() - 1);
      
      _rc_x_start = top.X();
      _rc_y_start = top.Y();
      _rc_z_start = top.Z();
      _rc_x_end   = bottom.X();
      _rc_y_end   = bottom.Y();
      _rc_z_end   = bottom.Z();
      _length     = track.Length();
      _mag = sqrt( ( (_rc_x_end - _rc_x_start) * (_rc_x_end - _rc_x_start) ) +
		   ( (_rc_y_end - _rc_y_start) * (_rc_y_end - _rc_y_start) ) +
		   ( (_rc_z_end - _rc_z_start) * (_rc_z_end - _rc_z_start) ) );
      _px = (_rc_x_end - _rc_x_start) / _mag;
      _py = (_rc_y_end - _rc_y_start) / _mag;
      _pz = (_rc_z_end - _rc_z_start) / _mag;

      // Set all of the variables for which face of the TPC that the track pierced equal to 0

      _enters_top = _exits_bottom = _enters_front = _enters_back = _exits_front = _exits_back = _enters_anode = _enters_cathode = _exits_anode = _exits_cathode = 0;

      // Set 'if_loop_has_already_been_entered' to false.
      if_loop_has_already_been_entered = false;

      // These conditionals handle the cases of the top-entering and bottom-exiting tracks.
      // Anode to Bottom
      if ( (TrackExitsBottom(sorted_trk) == true) && (TrackEntersAnode(sorted_trk) == true)  ) { _exits_bottom = 1; _enters_anode = 1;   if_loop_has_already_been_entered = true;  }

      // Cathode to Bottom
      if ( (TrackExitsBottom(sorted_trk) == true) && (TrackEntersAnode(sorted_trk) == false) ) { _exits_bottom = 1; _enters_cathode = 1; if_loop_has_already_been_entered = true;  }
      
      // Top to Anode
      if ( (TrackEntersTop(sorted_trk) == true)   && (TrackExitsAnode(sorted_trk) == true)   ) { _enters_top = 1;   _exits_anode = 1;    if_loop_has_already_been_entered = true;  }

      // Top to Cathode
      if ( (TrackEntersTop(sorted_trk) == true)   && (TrackExitsAnode(sorted_trk) == false)  ) { _enters_top = 1;   _exits_cathode = 1;  if_loop_has_already_been_entered = true;  }

      // These conditionals handle the cases of the endcap-piercing tracks (tracks can either enter or exit these faces of the TPC).
      // Anode to Front
      if ( (TrackExitsFront(sorted_trk) == true) && (TrackEntersAnode(sorted_trk) == true)  && (if_loop_has_already_been_entered == false)  ) { _exits_front = 1;  _enters_anode = 1;    }

      // Cathode to Front
      if ( (TrackExitsFront(sorted_trk) == true) && (TrackEntersAnode(sorted_trk) == false) && (if_loop_has_already_been_entered == false)  ) { _exits_front = 1;  _enters_cathode = 1;  }

      // Anode to Back
      if ( (TrackExitsBack(sorted_trk) == true) && (TrackEntersAnode(sorted_trk) == true)   && (if_loop_has_already_been_entered == false)  ) { _exits_back = 1;   _enters_anode = 1;    }

      // Cathode to Back
      if ( (TrackExitsBack(sorted_trk) == true) && (TrackEntersAnode(sorted_trk) == false)  && (if_loop_has_already_been_entered == false)  ) { _exits_back = 1;   _enters_cathode = 1;  }

      // Front to Anode
      if ( (TrackEntersFront(sorted_trk) == true) && (TrackExitsAnode(sorted_trk) == true)  && (if_loop_has_already_been_entered == false)  ) { _enters_front = 1; _exits_anode = 1;     }
      
      // Front to Cathode
      if ( (TrackEntersFront(sorted_trk) == true) && (TrackExitsAnode(sorted_trk) == false) && (if_loop_has_already_been_entered == false)  ) { _enters_front = 1; _exits_cathode = 1;   }

      // Back to Anode
      if ( (TrackEntersBack(sorted_trk) == true) && (TrackExitsAnode(sorted_trk) == true)   && (if_loop_has_already_been_entered == false)  ) { _enters_back = 1;  _exits_anode = 1;     }

      // Back to Cathode
      if ( (TrackEntersBack(sorted_trk) == true) && (TrackExitsAnode(sorted_trk) == false)  && (if_loop_has_already_been_entered == false)  ) { _enters_back = 1;  _exits_cathode = 1;   }

      // reconstructed time comes from T0 object
      _rc_time = t0->Time();

      // if there is an associated optical flash
      if (flash_v.size() == 1){
	
	auto flash = flash_v.at(0);
	
	_pe_flash = flash->TotalPE();
	_t_match  = flash->Time();
	_dt_flash = fabs( flash->Time() - _rc_time );

      } // if there is an optical flash

      // if we should use MC info -> continue w/ MC validation
      if (fUseMC == true){
	// loop through MCTracks to find the one that matches.
	for (size_t j=0; j < mctrk_h->size(); j++){
	  
	  auto const& mctrk = mctrk_h->at(j);
	  
	  // try matching to MC
	  if (MatchTracks(track, mctrk, fMCResolution) == false)
	    continue;
	  
	  // matched -> get MCTrack time and reconstructed track reconstructed T0
	  _mc_time = mctrk.at(0).T() / 1000.;
	  _matched = 1;


	  // Here is where I'll set the coordinates for the start and end of the mc tracks
	  _mc_x_start = mctrk.at(0).X();
	  _mc_y_start = mctrk.at(0).Y();
	  _mc_z_start = mctrk.at(0).Z();
	  _mc_x_end   = mctrk.at(mctrk.size() - 1).X();
	  _mc_y_end   = mctrk.at(mctrk.size() - 1).Y();
	  _mc_z_end   = mctrk.at(mctrk.size() - 1).Z();
	  
	} // for all MCTracks
      }// if we should use MCTracks
      
      _tree->Fill();
      
      // check if there is a cosmic tag, if so assume MuCS-tagged track
      std::vector<const anab::CosmicTag*> CosmicTag_v = trk_cosmictag_assn_v.at(i);
      
      if (CosmicTag_v.size() == 1){
	_reco_time = _rc_time;
	_mucs_tree->Fill();
      }// if there is a cosmic tag

      
      // fill calorimetry info for this track
      // grab the associated calorimetry object
      const std::vector<const anab::Calorimetry*>& Calo_v = trk_calo_assn_v.at(i);

      //std::cout << "There are " << Calo_v.size() << " Calorimetry objects associated with track" << i << std::endl;

      for (size_t pl=0; pl < Calo_v.size(); pl++){
	
	auto const& calo = Calo_v.at(pl);
	
	auto const& plane = calo->PlaneID().Plane;

	if (plane == 2){
	  
	  _t0 = _rc_time;

	  //std::cout << "Plane ID is " << plane << std::endl;
	  
	  // grab point-by-point information
	  auto const& dqdx_v = calo->dQdx();
	  auto const& xyz_v = calo->XYZ();

	  if (dqdx_v.size() != xyz_v.size()){
	    std::cout << "dQdx and XYZ vectors diagreee in size! skip track..." << std::endl;
	    continue;
	  }

	  for (size_t n=0; n < dqdx_v.size(); n++){

	    _dqds = dqdx_v[n];
	    _x = xyz_v[n].X();
	    _y = xyz_v[n].Y();
	    _z = xyz_v[n].Z();
	    _x_corr = _x - (_t0 * fDriftVelocity);

	    _calo_tree->Fill();
	    
	  }// for all calorimetry info

	}// if collection plane
	
      }// for all planes


      // fill 2dhit info for this track
      // grab the associated 2d hit object
      const std::vector<const recob::Hit*>& Hit_v = trk_hit_assn_v.at(i);

      _2d_adc = 0;

      for (size_t h=0; h < Hit_v.size(); h++){
	
	auto const& hit = Hit_v.at(h);
	
	if (hit->View() != geo::_plane_proj::kW) continue;
	
	_2d_adc += hit->Integral();
	
      }// for all hits	

      _2dhit_tree->Fill();
      
    } // if there is a reconstructed T0
    
  } // for all reconstructed tracks
  
}


bool T0RecoAnodeCathodePiercingAna::MatchTracks(const recob::Track& track, const sim::MCTrack& mctrk, const double& res)
{
  //assumes both the reco track
  //and mctrack are downwards going
  
  if ( (mctrk.size() < 2) or (track.NumberTrajectoryPoints() < 2) )
    return false;
  
  auto const& mctrk_s = mctrk.at(0);
  auto const& mctrk_e = mctrk.at( mctrk.size() - 1 );
  auto const& track_s = track.Vertex();
  auto const& track_e = track.End();

  // if track start is above and mctrk start is above
  if ( ( track_s.Y() > track_e.Y() ) and ( mctrk_s.Y() > mctrk_e.Y() ) ){
    if ( (fabs(mctrk_s.Y()-track_s.Y()) < res) and (fabs(mctrk_s.Z()-track_s.Z()) < res) and (fabs(mctrk_e.Y()-track_e.Y()) < res) and (fabs(mctrk_e.Z()-track_e.Z()) < res) )
      return true;
  }
  // if tarck start is above and mctrk start is below
  if ( ( track_s.Y() > track_e.Y() ) and ( mctrk_s.Y() < mctrk_e.Y() ) ){
    if ( (fabs(mctrk_e.Y()-track_s.Y()) < res) and (fabs(mctrk_e.Z()-track_s.Z()) < res) and (fabs(mctrk_s.Y()-track_e.Y()) < res) and (fabs(mctrk_s.Z() - track_e.Z()) < res) )
      return true;
  }
  // if track start is below and mctrk start is above
  if ( ( track_s.Y() < track_e.Y() ) and ( mctrk_s.Y() > mctrk_e.Y() ) ){
    if ( (fabs(mctrk_s.Y()-track_e.Y()) < res) and (fabs(mctrk_s.Z()-track_e.Z()) < res) and (fabs(mctrk_e.Y()-track_s.Y()) < res) and (fabs(mctrk_e.Z()-track_s.Z()) < res) )
      return true;
  }
  // if track start is below and mctrk start is below
  if ( ( track_s.Y() < track_e.Y() ) and ( mctrk_s.Y() < mctrk_e.Y() ) ){
    if ( (fabs(mctrk_e.Y()-track_e.Y()) < res) and (fabs(mctrk_e.Z()-track_e.Z()) < res) and (fabs(mctrk_s.Y() - track_s.Y()) < res) and (fabs(mctrk_s.Z()-track_s.Z()) < res) )
      return true;
  }
  
  return false;
}


bool   T0RecoAnodeCathodePiercingAna::TrackEntersTop(const std::vector<TVector3>& sorted_trk)
{
  // check that the first point in the track
  // pierces the top boundary of the TPC
  // This track either will pierce the top of the TPC or is just about to (the '_TOP' variable is just below the actual coordinate position of the top in Y)

  if (sorted_trk.at(0).Y() > _TOP)
    return true;

  return false;
}


bool T0RecoAnodeCathodePiercingAna::TrackEntersFront(const std::vector<TVector3>& sorted_trk)
{

  // Determine if the track enters the                                                                                              
  // front of the TPC based on if the position                                                                                  
  // of its initial Z-coordinate is less than                                                                                     
  // the location of the front of the TPC in Z                                                                                                                                    

  // First define 'top_pt' to mean the point at the start of the track                                                                     
  auto const& top_pt = sorted_trk.at(0);

  if (top_pt.Z() < _FRONT)
    return true;

  // I may include the case in which I check                                                                                        
  // the y-coordinates as well, but I will not                                                                                                  
  // implement that at this time                                                                                                                                                

  // If this condition is not satisfied, then return 'false' (the track was not determined                                                                                
  // within resolution to enter the front of the TPC)                                                                                                             
  return false;
}


bool T0RecoAnodeCathodePiercingAna::TrackEntersBack(const std::vector<TVector3>& sorted_trk)
{

  // Determines if the track enters the                                                                                               
  // back of the TPC based on if the position                                                                                         
  // of its initial Z-coordinate is greater                                                                                                
  // than the location of the back of the                                                                                                                         
  // TPC in Z                                                                                                                                                                      

  // First define 'top_pt' to mean the point at the start of the track                                                                      
  auto const& top_pt = sorted_trk.at(0);

  if (top_pt.Z() > _BACK)
    return true;

  // If this condition is not satisfied, then return 'false' (the track was not determined                                                                                  
  // within resolution to enter the back of the TPC)                                                                                                                  
  return false;
}


bool   T0RecoAnodeCathodePiercingAna::TrackEntersAnode(const std::vector<TVector3>& sorted_trk)
{

  // we know the track enters either the                                                                                                                               
  // anode or cathode                                                                                                                                                        
  // at this point figure out                                                                                                                                                
  // if it ENTERS the ANODE or CATHODE                                                                                         
  // ANODE: top point must be at lower X-coord                                                                                                
  // than bottom point                                                                                              
  // CATHODE: top point must be at larger X-coord                                                                                                                              
  // than bottom point
  // assume track has already been sorted                                                                                                                                     
  // such that the 1st point is the most elevated in Y coord.                                                                                                                 
  // return TRUE if passes the ANODE                                                                                                                                                
  
  auto const& top    = sorted_trk.at(0);
  auto const& bottom = sorted_trk.at( sorted_trk.size() - 1 );

  if (top.X() < bottom.X())
    return true;
  
  return false;
}


bool   T0RecoAnodeCathodePiercingAna::TrackEntersSide(const std::vector<TVector3>& sorted_trk)
{
  
  // check that the top-most point                                                                                                                                            
  // is not on the top of the TPC                                                                                                                                              
  // nor on the front & back of the TPC                                                                                                                                           
  
  auto const& top_pt = sorted_trk.at(0);

  // if highest point above the TOP -> false                                                                                                                                   
  if (top_pt.Y() > _TOP)
    return false;

  // if highest point in Z close to front or back                                                                                                                              
  // -> FALSE                                                                                                                                                                 
  if ( (top_pt.Z() < _FRONT) or (top_pt.Z() > _BACK) )
    return false;

  // If the function makes it this far, then it will enter through one of the sides of the TPC
  return true;
}


bool   T0RecoAnodeCathodePiercingAna::TrackExitsBottom(const std::vector<TVector3>& sorted_trk)
{

  // check that the last point in the track                                                                                                                                    
  // pierces the bottom boundary of the TPC                                                                                                                                   
  if ( sorted_trk.at( sorted_trk.size() - 1).Y() < _BOTTOM )
    return true;

  return false;
}


bool   T0RecoAnodeCathodePiercingAna::TrackExitsFront(const std::vector<TVector3>& sorted_trk)
{

  // Determine if the track exits the                                                                                                                                       
  // front of the TPC based on if the position                                                                                                                          
  // of its final Z-coordinate is less than                                                                                                                      
  // the location of the front of the TPC in Z                                                                                                                                      

  // First define 'bottom_pt' to mean the point at the end of the track                                                                                                      
  auto const& bottom_pt = sorted_trk.at(sorted_trk.size() - 1);

  if (bottom_pt.Z() < _FRONT)
    return true;

  return false;
}


bool   T0RecoAnodeCathodePiercingAna::TrackExitsBack(const std::vector<TVector3>& sorted_trk)
{

  // Determine if the track exits the                                                                                                                           
  // front of the TPC based on if the position                                                                                                          
  // of its final Z-coordinate is less than                                                                                                                      
  // the location of the front of the TPC in Z                                                                                                                                     

  // First define 'bottom_pt' to mean the point at the end of the track                                                                                                  
  auto const& bottom_pt = sorted_trk.at(sorted_trk.size() - 1);

  if (bottom_pt.Z() > _BACK)
    return true;

  return false;
}


bool   T0RecoAnodeCathodePiercingAna::TrackExitsAnode(const std::vector<TVector3>& sorted_trk)
{

  // Check, once it's known that the track doesn't exit out of the bottom, whether it's the anode or                                                                           
  // the cathode that it exits out of                                                                                                                                         
  // This can be done by direct analogy with the 'Anode' function (shown in this file as the 'TrackEntersAnode') function written by D. Caratelli                             
  // Define 'top' as the point at the start of the track, and 'bottom' as the point at the end of the track                                                                     

  auto const& top    = sorted_trk.at(0);
  auto const& bottom = sorted_trk.at(sorted_trk.size() - 1);

  // Check to see which point has a lower x coordinate                                                                                                                         
  // If the bottom does, then it exits out of the anode                                                                                                                       
  // If the top does, then it exits out of the cathode                                                                                                                          
  if (bottom.X() < top.X()) 
    return true;

  return false; // Otherwise, the top is less than the bottom, so the track ended closer to the cathode and exited there                                                          
}


bool   T0RecoAnodeCathodePiercingAna::TrackExitsSide(const std::vector<TVector3>& sorted_trk)
{

  // check that the bottom-most point                                                                                                                                           
  // is not on the bottom of the TPC                                                                                                                                            
  // nor on the front & back of the TPC                                                                                                                                              

  auto const& bottom_pt = sorted_trk.at(sorted_trk.size() - 1);

  // if lowest point below the BOTTOM -> false                                                                                                            
  // Within this resolution, this means that it's likely that the track exited out of the bottom (at a point earlier on in the process than the last point) OR is just about to

  if (bottom_pt.Y() <  _BOTTOM)
    return false;

  // if lowest point in Z close to front or back                                                                                                         
  // -> FALSE                                                                                                                                              
  // If the the bottom point is less than the front, then the track has already pierced the front of the TPC and exited that way OR is likely just about to
  // If the bottom point is greater than the back, then the track has already pierced the back of the TPC and exited that way OR is likely just about to
  if ( (bottom_pt.Z() < _FRONT) or (bottom_pt.Z() > _BACK) )
    return false;

  return true;
}


void   T0RecoAnodeCathodePiercingAna::SortTrackPoints(const recob::Track& track, std::vector<TVector3>& sorted_trk)
{

  // vector to store 3D coordinates of                                                                                                                                           
  // ordered track                              
  sorted_trk.clear();

  // take the reconstructed 3D track                                                                                                                                           
  // and assuming it is downwards                                                                                                    
  // going, sort points so that                                                                                                              
  // the track starts at the top                                                                                                     
  // which point is further up in Y coord?                                                                                                                  
  // start or end?                                                                                                                 
  auto const&N = track.NumberTrajectoryPoints();
  auto const&start = track.LocationAtPoint(0);
  auto const&end   = track.LocationAtPoint( N - 1 );

  // if points are ordered correctly                                                                                                                                       
  if (start.Y() > end.Y()){
    for (size_t i=0; i < N; i++)
      sorted_trk.push_back( track.LocationAtPoint(i) );
  }
  
  // otherwise flip order                                                                                                                                                 
  else {
    for (size_t i=0; i < N; i++)
      sorted_trk.push_back( track.LocationAtPoint( N - i - 1) );
  }
}

DEFINE_ART_MODULE(T0RecoAnodeCathodePiercingAna)
