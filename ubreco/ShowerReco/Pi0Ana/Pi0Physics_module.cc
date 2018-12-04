////////////////////////////////////////////////////////////////////////
// Class:       Pi0Physics
// Plugin Type: analyzer (art v2_09_06)
// File:        Pi0Physics_module.cc
//
// Generated at Wed Apr 11 15:32:15 2018 by David Caratelli using cetskelgen
// from cetlib version v3_01_03.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "art/Framework/Services/Optional/TFileService.h"

// larsoft data-products
#include "nusimdata/SimulationBase/MCTruth.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"

// selection algorithms
#include "Selection/SelectionAlg.h"
#include "Selection/TruncMean.h"

#include "TTree.h"
#include "TVector3.h"

class Pi0Physics;


class Pi0Physics : public art::EDAnalyzer {
public:
  explicit Pi0Physics(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  Pi0Physics(Pi0Physics const &) = delete;
  Pi0Physics(Pi0Physics &&) = delete;
  Pi0Physics & operator = (Pi0Physics const &) = delete;
  Pi0Physics & operator = (Pi0Physics &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;

  // Selected optional functions.
  void beginJob() override;
  void endJob() override;

private:

  std::string fShrProducer, fTrkProducer, fCaloProducer;

  TruncMean _TruncMean;

  void SetTTree();

  // Declare member data here.
  TTree* _tree;
  float _delta_px  = 0;
  float _delta_py  = 0;
  float _delta_pz  = 0;
  float _pi0_px    = 0;
  float _pi0_py    = 0;
  float _pi0_pz    = 0;
  float _proton0mc_px = 0;
  float _proton0mc_py = 0;
  float _proton0mc_pz = 0;
  float _proton1mc_px = 0;
  float _proton1mc_py = 0;
  float _proton1mc_pz = 0;
  float _proton2mc_px = 0;
  float _proton2mc_py = 0;
  float _proton2mc_pz = 0;
  int   _npi0mc;
  int   _nprotonmc;
  int   _ndelta;

  TTree* _trk_tree;
  float _len, _tmean;

  TTree* _delta_tree;
  int   _nproton;
  int   _npi0;
  float _muon_len;
  float _muon_px;
  float _muon_py;
  float _muon_pz;
  float _proton0_len;
  float _proton0_px;
  float _proton0_py;
  float _proton0_pz;
  float _proton1_len;
  float _proton1_px;
  float _proton1_py;
  float _proton1_pz;
  float _proton2_len;
  float _proton2_px;
  float _proton2_py;
  float _proton2_pz;
  float _shr1_e;
  float _shr1_px;
  float _shr1_py;
  float _shr1_pz;
  float _shr2_e;
  float _shr2_px;
  float _shr2_py;
  float _shr2_pz;
  float _rcangle, _rcmass;


  double _rc_vtx_x, _rc_vtx_y, _rc_vtx_z;  

  selection::SelectionAlg _pi0selection;

};


Pi0Physics::Pi0Physics(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p)  // ,
 // More initializers here.
{

  fShrProducer  = p.get<std::string>("ShrProducer" );
  fTrkProducer  = p.get<std::string>("TrkProducer" );
  fCaloProducer = p.get<std::string>("CaloProducer");

  SetTTree();
}

void Pi0Physics::analyze(art::Event const & e)
{

  auto const& mct_h = e.getValidHandle<std::vector<simb::MCTruth> >("generator");

  // load input showers
  auto const& shr_h = e.getValidHandle<std::vector<recob::Shower>>(fShrProducer);
  // load input tracks
  auto const& trk_h = e.getValidHandle<std::vector<recob::Track>>(fTrkProducer);
  // load input vertices
  auto const& vtx_h = e.getValidHandle<std::vector<recob::Vertex>>("ccvertex");

  // grab calorimetry objects associated to tracks
  art::FindMany<anab::Calorimetry> trk_calo_assn_v(trk_h, e, fCaloProducer);

  auto mct = mct_h->at(0);
  size_t npart = mct.NParticles();

  _npi0mc    = 0;
  _nprotonmc = 0;
  _ndelta  = 0;

  for (size_t i=0; i < npart; i++){
    auto const& part = mct.GetParticle(i);
    if ( (part.PdgCode() == 111) and (part.StatusCode() == 1) ){
      _npi0mc += 1;
      _pi0_px = part.Trajectory().Px(0);
      _pi0_py = part.Trajectory().Py(0);
      _pi0_pz = part.Trajectory().Pz(0);
    }
    if ( (part.PdgCode() == 2212) and (part.StatusCode() == 1) ){
      if (_nprotonmc == 0){
	_proton0mc_px = part.Trajectory().Px(0);
	_proton0mc_py = part.Trajectory().Py(0);
	_proton0mc_pz = part.Trajectory().Pz(0);
      }
      if (_nprotonmc == 1){
	_proton1mc_px = part.Trajectory().Px(0);
	_proton1mc_py = part.Trajectory().Py(0);
	_proton1mc_pz = part.Trajectory().Pz(0);
      }
      if (_nprotonmc == 2){
	_proton2mc_px = part.Trajectory().Px(0);
	_proton2mc_py = part.Trajectory().Py(0);
	_proton2mc_pz = part.Trajectory().Pz(0);
      }
      _nprotonmc += 1;
    }
    if ( part.PdgCode() == 2214){
      _ndelta += 1;
      _delta_px = part.Trajectory().Px(0);
      _delta_py = part.Trajectory().Py(0);
      _delta_pz = part.Trajectory().Pz(0);
    }
  }


  if (_npi0mc==1)
    _tree->Fill();

  // reconstruction section
  // find tracks near vertex
  
  // store reco vertex info

  if (vtx_h->size() == 1){
    Double_t rcxyz[3] = {};
    auto const& vtx = vtx_h->at(0);
    vtx.XYZ(rcxyz);
    _rc_vtx_x = rcxyz[0];
    _rc_vtx_y = rcxyz[1];
    _rc_vtx_z = rcxyz[2];
  }
  
  recob::Track::Point_t nuvtx(_rc_vtx_x,_rc_vtx_y,_rc_vtx_z);

  // apply pi0 selection to event showers
  auto pi0candidate = _pi0selection.ApplySelection(shr_h);

  if (pi0candidate.mass > 0) {
    _npi0 = 1;
    _rcmass = pi0candidate.mass;
    _rcangle = pi0candidate.angle;
    _shr1_e = pi0candidate.e1;
    _shr2_e = pi0candidate.e2;
    auto shr1 = shr_h->at(pi0candidate.idx1);
    auto shr2 = shr_h->at(pi0candidate.idx2);
    _shr1_px = shr1.Direction().Unit().X();
    _shr1_py = shr1.Direction().Unit().Y();
    _shr1_pz = shr1.Direction().Unit().Z();
    _shr2_px = shr2.Direction().Unit().X();
    _shr2_py = shr2.Direction().Unit().Y();
    _shr2_pz = shr2.Direction().Unit().Z();
  }
  else
    _npi0 = 0;


  _nproton = 0;

  // find longest track -> will be muon
  int longesttrkidx = 0;
  double longesttrklen = 0;
  
  for (size_t t=0; t < trk_h->size(); t++) {
    
    auto const& trk = trk_h->at(t);
    auto const& beg = trk.Vertex();
    auto const& end = trk.End();
    double dvtx = 1000.;
    //bool flipped = false;
    // which one is closest?
    if ( (nuvtx-beg).R() < (nuvtx-end).R() ) {
      dvtx = (nuvtx-beg).R();
      //flipped = false;
    }
    else {
      dvtx = (nuvtx-end).R();
      //flipped = true;
    }

    if (trk.Length() > longesttrklen){
      longesttrkidx = t;
      longesttrklen = trk.Length();
    }

    if (dvtx < 2.0) {

      // fill calorimetry info for this track
      // grab the associated calorimetry object
      const std::vector<const anab::Calorimetry*>& Calo_v = trk_calo_assn_v.at(t);

      for (size_t pl=0; pl < Calo_v.size(); pl++){
	
	auto const& calo = Calo_v.at(pl);
	
	auto const& plane = calo->PlaneID().Plane;
	
	if (plane == 2){
	  
	  // grab point-by-point information
	  auto const& dqdx_v = calo->dQdx();
	  std::vector<float> dqdx_vv;
	  for (auto const& dqdx : dqdx_v)
	    dqdx_vv.push_back((float)dqdx);
	  //auto const& xyz_v = calo->XYZ();
	  
	  if (dqdx_vv.size() < 3) continue;

	  _tmean = _TruncMean.CalcIterativeTruncMean(dqdx_vv,0,1,0,10,0.2,3);
	  _len = trk.Length();

	  _trk_tree->Fill();
	  
	  if( (_tmean < 450+600*exp(-_len/40.)) && (_tmean > 380+280*exp(-_len/30.)) ) {

	    if (_nproton == 0){
	      _proton0_len = _len;
	      _proton0_px = trk.VertexDirection().X();
	      _proton0_py = trk.VertexDirection().Y();
	      _proton0_pz = trk.VertexDirection().Z();
	    }
	    if (_nproton == 1){
	      _proton1_len = _len;
	      _proton1_px = trk.VertexDirection().X();
	      _proton1_py = trk.VertexDirection().Y();
	      _proton1_pz = trk.VertexDirection().Z();
	    }
	    if (_nproton == 2){
	      _proton2_len = _len;
	      _proton2_px = trk.VertexDirection().X();
	      _proton2_py = trk.VertexDirection().Y();
	      _proton2_pz = trk.VertexDirection().Z();
	    }
	    
	    _nproton += 1;

	  }// if proton

	}// if collection plane
      }// for all planes
    }// if within 2 cm of vertex
  }// for all tracks

  if (trk_h->size() > 0) {
    auto const& muon = trk_h->at(longesttrkidx);
    _muon_len = muon.Length();
    _muon_px = muon.VertexDirection().X();
    _muon_py = muon.VertexDirection().Y();
    _muon_pz = muon.VertexDirection().Z();
  }

  _delta_tree->Fill();

  return;
}

void Pi0Physics::beginJob()
{
  // Implementation of optional member function here.
}

void Pi0Physics::endJob()
{
  // Implementation of optional member function here.
}


void Pi0Physics::SetTTree() {

  art::ServiceHandle<art::TFileService> tfs;

  _tree = tfs->make<TTree>("_tree","TTree");
  _tree->Branch("_delta_px",&_delta_px,"delta_px/F");
  _tree->Branch("_delta_py",&_delta_py,"delta_py/F");
  _tree->Branch("_delta_pz",&_delta_pz,"delta_pz/F");
  _tree->Branch("_pi0_px",&_pi0_px,"pi0_px/F");
  _tree->Branch("_pi0_py",&_pi0_py,"pi0_py/F");
  _tree->Branch("_pi0_pz",&_pi0_pz,"pi0_pz/F");
  _tree->Branch("_proton0mc_px",&_proton0mc_px,"proton0mc_px/F");
  _tree->Branch("_proton0mc_py",&_proton0mc_py,"proton0mc_py/F");
  _tree->Branch("_proton0mc_pz",&_proton0mc_pz,"proton0mc_pz/F");
  _tree->Branch("_proton1mc_px",&_proton1mc_px,"proton1mc_px/F");
  _tree->Branch("_proton1mc_py",&_proton1mc_py,"proton1mc_py/F");
  _tree->Branch("_proton1mc_pz",&_proton1mc_pz,"proton1mc_pz/F");
  _tree->Branch("_proton2mc_px",&_proton2mc_px,"proton2mc_px/F");
  _tree->Branch("_proton2mc_py",&_proton2mc_py,"proton2mc_py/F");
  _tree->Branch("_proton2mc_pz",&_proton2mc_pz,"proton2mc_pz/F");
  _tree->Branch("_npi0mc",&_npi0mc,"npi0mc/I");
  _tree->Branch("_nprotonmc",&_nprotonmc,"nprotonmc/I");
  _tree->Branch("_ndelta",&_ndelta,"ndelta/I");

  _trk_tree = tfs->make<TTree>("_trk_tree","Track Tree");
  _trk_tree->Branch("_len",&_len,"len/F");
  _trk_tree->Branch("_tmean",&_tmean,"tmean/F");

  _delta_tree = tfs->make<TTree>("_delta_tree","Delta TTree");
  _delta_tree->Branch("_nproton",&_nproton,"nproton/I");
  _delta_tree->Branch("_proton0_len",&_proton0_len,"proton0_len/F");
  _delta_tree->Branch("_proton0_px",&_proton0_px,"proton0_px/F");
  _delta_tree->Branch("_proton0_py",&_proton0_py,"proton0_py/F");
  _delta_tree->Branch("_proton0_pz",&_proton0_pz,"proton0_pz/F");
  _delta_tree->Branch("_proton1_len",&_proton1_len,"proton1_len/F");
  _delta_tree->Branch("_proton1_px",&_proton1_px,"proton1_px/F");
  _delta_tree->Branch("_proton1_py",&_proton1_py,"proton1_py/F");
  _delta_tree->Branch("_proton1_pz",&_proton1_pz,"proton1_pz/F");
  _delta_tree->Branch("_proton2_len",&_proton2_len,"proton2_len/F");
  _delta_tree->Branch("_proton2_px",&_proton2_px,"proton2_px/F");
  _delta_tree->Branch("_proton2_py",&_proton2_py,"proton2_py/F");
  _delta_tree->Branch("_proton2_pz",&_proton2_pz,"proton2_pz/F");
  _delta_tree->Branch("_muon_len",&_muon_len,"muon_len/F");
  _delta_tree->Branch("_muon_px",&_muon_px,"muon_px/F");
  _delta_tree->Branch("_muon_py",&_muon_py,"muon_py/F");
  _delta_tree->Branch("_muon_pz",&_muon_pz,"muon_pz/F");
  _delta_tree->Branch("_shr1_e",&_shr1_e,"shr1_e/F");
  _delta_tree->Branch("_shr1_px",&_shr1_px,"shr1_px/F");
  _delta_tree->Branch("_shr1_py",&_shr1_py,"shr1_py/F");
  _delta_tree->Branch("_shr1_pz",&_shr1_pz,"shr1_pz/F");
  _delta_tree->Branch("_shr2_e",&_shr2_e,"shr2_e/F");
  _delta_tree->Branch("_shr2_px",&_shr2_px,"shr2_px/F");
  _delta_tree->Branch("_shr2_py",&_shr2_py,"shr2_py/F");
  _delta_tree->Branch("_shr2_pz",&_shr2_pz,"shr2_pz/F");
  _delta_tree->Branch("_rcmass",&_rcmass,"rcmass/F");
  _delta_tree->Branch("_rcangle",&_rcangle,"rcangle/F");
  _delta_tree->Branch("_npi0",&_npi0,"npi0/I");

  return;
}

DEFINE_ART_MODULE(Pi0Physics)
