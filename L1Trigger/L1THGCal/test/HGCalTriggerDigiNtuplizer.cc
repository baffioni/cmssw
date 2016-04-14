#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"
//#include "DataFormats/ForwardDetId/interface/HGCEEDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCTriggerDetId.h"
#include "DataFormats/ForwardDetId/interface/ForwardSubdetector.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerFECodecBase.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerBackendProcessor.h"


#include "TTree.h"

#include <memory>
#include <fstream>

namespace 
{  
  template<typename T>
  struct array_deleter
  {
    void operator () (T* arr) { delete [] arr; }
  };
}

class HGCalTriggerDigiNtuplizer : public edm::EDAnalyzer
{  
    public:    
        HGCalTriggerDigiNtuplizer(const edm::ParameterSet&);
        ~HGCalTriggerDigiNtuplizer() { }

        virtual void beginRun(const edm::Run&, const edm::EventSetup&);
        virtual void analyze(const edm::Event&, const edm::EventSetup&);

    private:
        void setTreeGenSize(const size_t);
        void setTreeCellSize(const size_t);
        //void setTreeTriggerCellSize(const size_t);
        // inputs
        HGCalTriggerGeometryBase::es_info es_info_;
        edm::EDGetToken inputgen_, inputee_, inputfh_, inputbh_;//, inputfet_;
        // tools
        std::unique_ptr<HGCalTriggerGeometryBase> triggerGeometry_;
        //HGCalBestChoiceCodecImpl codec_;
        // Output
        edm::Service<TFileService> fs_;
        TTree* tree_;
        int run_;
        int event_;
        int lumi_;
        // gen information
        int genN_;
        std::shared_ptr<int>   genId_;
        std::shared_ptr<int>   genStatus_;
        std::shared_ptr<float> genEnergy_;
        std::shared_ptr<float> genPt_;
        std::shared_ptr<float> genEta_;
        std::shared_ptr<float> genPhi_;
        // cells information
        int                    cellN_     ;
        std::shared_ptr<int>   cellId_     ;
        std::shared_ptr<int>   cellSubdet_   ;
        std::shared_ptr<int>   cellSide_   ;
        std::shared_ptr<int>   cellLayer_  ;
        std::shared_ptr<int>   cellWafer_ ;
        std::shared_ptr<int>   cellWaferType_ ;
        std::shared_ptr<int>   cellCell_ ;
        std::shared_ptr<int>   cellModuleId_ ;
        std::shared_ptr<int>   cellTriggerCellId_ ;
        std::shared_ptr<int>   cellDigi_ ;
        std::shared_ptr<float> cellX_      ;
        std::shared_ptr<float> cellY_      ;
        std::shared_ptr<float> cellZ_      ; 
        std::shared_ptr<float> cellEta_    ;
        std::shared_ptr<float> cellPhi_    ;
        // trigger cells information
        //int                    triggerCellN_     ;
        //std::shared_ptr<int>   triggerCellId_     ;
        //std::shared_ptr<int>   triggerCellSide_   ;
        //std::shared_ptr<int>   triggerCellLayer_  ;
        //std::shared_ptr<int>   triggerCellSector_ ;
        //std::shared_ptr<int>   triggerCellCell_ ;
        //std::shared_ptr<int>   triggerCellModule_ ;
        //std::shared_ptr<int>   triggerCellModuleId_ ;
        //std::shared_ptr<int>   triggerCellDigi_ ;
        //std::shared_ptr<float> triggerCellX_      ;
        //std::shared_ptr<float> triggerCellY_      ;
        //std::shared_ptr<float> triggerCellZ_      ; 
        //std::shared_ptr<float> triggerCellEta_    ;
        //std::shared_ptr<float> triggerCellPhi_    ;
};

DEFINE_FWK_MODULE(HGCalTriggerDigiNtuplizer);

/*****************************************************************/
HGCalTriggerDigiNtuplizer::HGCalTriggerDigiNtuplizer(const edm::ParameterSet& conf):
    inputgen_(consumes<reco::GenParticleCollection>(conf.getParameter<edm::InputTag>("gen"))),
    inputee_ (consumes<HGCEEDigiCollection>(conf.getParameter<edm::InputTag>("eeDigis"))),
    inputfh_ (consumes<HGCHEDigiCollection>(conf.getParameter<edm::InputTag>("fhDigis"))), 
    inputbh_ (consumes<HGCHEDigiCollection>(conf.getParameter<edm::InputTag>("bhDigis")))
    //inputfet_(consumes<HGCHEDigiCollection>(conf.getParameter<edm::InputTag>("fetDigis")))
    //codec_(conf.getParameterSet("FECodec"))
/*****************************************************************/
{
    //setup geometry configuration
    const edm::ParameterSet& geometryConfig = conf.getParameterSet("TriggerGeometry");
    const std::string& trigGeomName = geometryConfig.getParameter<std::string>("TriggerGeometryName");
    HGCalTriggerGeometryBase* geometry = HGCalTriggerGeometryFactory::get()->create(trigGeomName,geometryConfig);
    triggerGeometry_.reset(geometry);



    tree_ = fs_->make<TTree>("Tree","Tree of HGC digis");
    genId_              .reset(new int[1],   array_deleter<int>());
    genStatus_          .reset(new int[1],   array_deleter<int>());
    genEnergy_          .reset(new float[1], array_deleter<float>());
    genPt_              .reset(new float[1], array_deleter<float>());
    genEta_             .reset(new float[1], array_deleter<float>());
    genPhi_             .reset(new float[1], array_deleter<float>());
    cellId_             .reset(new int[1],   array_deleter<int>());
    cellSubdet_         .reset(new int[1],   array_deleter<int>());
    cellSide_           .reset(new int[1],   array_deleter<int>());
    cellLayer_          .reset(new int[1],   array_deleter<int>());
    cellWafer_          .reset(new int[1],   array_deleter<int>());
    cellWaferType_      .reset(new int[1],   array_deleter<int>());
    cellCell_           .reset(new int[1],   array_deleter<int>());
    cellModuleId_       .reset(new int[1],   array_deleter<int>());
    cellTriggerCellId_  .reset(new int[1],   array_deleter<int>());
    cellDigi_           .reset(new int[1],   array_deleter<int>());
    cellX_              .reset(new float[1], array_deleter<float>());
    cellY_              .reset(new float[1], array_deleter<float>());
    cellZ_              .reset(new float[1], array_deleter<float>());
    cellEta_            .reset(new float[1], array_deleter<float>());
    cellPhi_            .reset(new float[1], array_deleter<float>());
    //triggerCellId_      .reset(new int[1],   array_deleter<int>());
    //triggerCellSide_    .reset(new int[1],   array_deleter<int>());
    //triggerCellLayer_   .reset(new int[1],   array_deleter<int>());
    //triggerCellSector_  .reset(new int[1],   array_deleter<int>());
    //triggerCellCell_    .reset(new int[1],   array_deleter<int>());
    //triggerCellModule_  .reset(new int[1],   array_deleter<int>());
    //triggerCellModuleId_.reset(new int[1],   array_deleter<int>());
    //triggerCellDigi_    .reset(new int[1],   array_deleter<int>());
    //triggerCellX_       .reset(new float[1], array_deleter<float>());
    //triggerCellY_       .reset(new float[1], array_deleter<float>());
    //triggerCellZ_       .reset(new float[1], array_deleter<float>());
    //triggerCellEta_     .reset(new float[1], array_deleter<float>());
    //triggerCellPhi_     .reset(new float[1], array_deleter<float>());

    tree_->Branch("run"                       , &run_                            , "run/I");
    tree_->Branch("event"                     , &event_                          , "event/I");
    tree_->Branch("lumi"                      , &lumi_                           , "lumi/I");
    tree_->Branch("gen_n"                     , &genN_                           , "gen_n/I");
    tree_->Branch("gen_id"                    , genId_                    .get() , "gen_id[gen_n]/I");
    tree_->Branch("gen_status"                , genStatus_                .get() , "gen_status[gen_n]/I");
    tree_->Branch("gen_energy"                , genEnergy_                .get() , "gen_energy[gen_n]/F");
    tree_->Branch("gen_pt"                    , genPt_                    .get() , "gen_pt[gen_n]/F");
    tree_->Branch("gen_eta"                   , genEta_                   .get() , "gen_eta[gen_n]/F");
    tree_->Branch("gen_phi"                   , genPhi_                   .get() , "gen_phi[gen_n]/F");
    tree_->Branch("cell_n"                    , &cellN_                          , "cell_n/I");
    tree_->Branch("cell_id"                   , cellId_                   .get() , "cell_id[cell_n]/I");
    tree_->Branch("cell_subdet"               , cellSubdet_               .get() , "cell_subdet[cell_n]/I");
    tree_->Branch("cell_zside"                , cellSide_                 .get() , "cell_zside[cell_n]/I");
    tree_->Branch("cell_layer"                , cellLayer_                .get() , "cell_layer[cell_n]/I");
    tree_->Branch("cell_wafer"                , cellWafer_                .get() , "cell_wafer[cell_n]/I");
    tree_->Branch("cell_wafertype"            , cellWaferType_            .get() , "cell_wafertype[cell_n]/I");
    tree_->Branch("cell_cell"                 , cellCell_                 .get() , "cell_cell[cell_n]/I");
    tree_->Branch("cell_moduleid"             , cellModuleId_             .get() , "cell_moduleid[cell_n]/I");
    tree_->Branch("cell_triggercellid"        , cellTriggerCellId_        .get() , "cell_triggercellid[cell_n]/I");
    tree_->Branch("cell_digi"                 , cellDigi_                 .get() , "cell_digi[cell_n]/I");
    tree_->Branch("cell_x"                    , cellX_                    .get() , "cell_x[cell_n]/F");
    tree_->Branch("cell_y"                    , cellY_                    .get() , "cell_y[cell_n]/F");
    tree_->Branch("cell_z"                    , cellZ_                    .get() , "cell_z[cell_n]/F");
    tree_->Branch("cell_eta"                  , cellEta_                  .get() , "cell_eta[cell_n]/F");
    tree_->Branch("cell_phi"                  , cellPhi_                  .get() , "cell_phi[cell_n]/F");
    //tree_->Branch("triggercell_n"             , &triggerCellN_                   , "triggercell_n/I");
    //tree_->Branch("triggercell_id"            , triggerCellId_            .get() , "triggercell_id[triggercell_n]/I");
    //tree_->Branch("triggercell_zside"         , triggerCellSide_          .get() , "triggercell_zside[triggercell_n]/I");
    //tree_->Branch("triggercell_layer"         , triggerCellLayer_         .get() , "triggercell_layer[triggercell_n]/I");
    //tree_->Branch("triggercell_sector"        , triggerCellSector_        .get() , "triggercell_sector[triggercell_n]/I");
    //tree_->Branch("triggercell_cell"          , triggerCellCell_          .get() , "triggercell_cell[triggercell_n]/I");
    //tree_->Branch("triggercell_module"        , triggerCellModuleId_      .get() , "triggercell_module[triggercell_n]/I");
    //tree_->Branch("triggercell_moduleid"      , triggerCellModuleId_      .get() , "triggercell_moduleid[triggercell_n]/I");
    //tree_->Branch("triggercell_digi"          , triggerCellDigi_          .get() , "triggercell_digi[triggercell_n]/I");
    //tree_->Branch("triggercell_x"             , triggerCellX_             .get() , "triggercell_x[triggercell_n]/F");
    //tree_->Branch("triggercell_y"             , triggerCellY_             .get() , "triggercell_y[triggercell_n]/F");
    //tree_->Branch("triggercell_z"             , triggerCellZ_             .get() , "triggercell_z[triggercell_n]/F");
    //tree_->Branch("triggercell_eta"           , triggerCellEta_           .get() , "triggercell_eta[triggercell_n]/F");
    //tree_->Branch("triggercell_phi"           , triggerCellPhi_           .get() , "triggercell_phi[triggercell_n]/F");

}

/*****************************************************************/
void HGCalTriggerDigiNtuplizer::beginRun(const edm::Run& /*run*/, const edm::EventSetup& es) 
/*****************************************************************/
{
    triggerGeometry_->reset();
    const std::string& ee_sd_name = triggerGeometry_->eeSDName();
    const std::string& fh_sd_name = triggerGeometry_->fhSDName();
    const std::string& bh_sd_name = triggerGeometry_->bhSDName();
    es.get<IdealGeometryRecord>().get(ee_sd_name,es_info_.geom_ee);
    es.get<IdealGeometryRecord>().get(fh_sd_name,es_info_.geom_fh);
    es.get<IdealGeometryRecord>().get(bh_sd_name,es_info_.geom_bh);
    es.get<IdealGeometryRecord>().get(ee_sd_name,es_info_.topo_ee);
    es.get<IdealGeometryRecord>().get(fh_sd_name,es_info_.topo_fh);
    es.get<IdealGeometryRecord>().get(bh_sd_name,es_info_.topo_bh);
    triggerGeometry_->initialize(es_info_);
}


/*****************************************************************/
void HGCalTriggerDigiNtuplizer::analyze(const edm::Event& e, const edm::EventSetup& es) 
/*****************************************************************/
{

    run_    = e.id().run();
    lumi_   = e.luminosityBlock();
    event_  = e.id().event();

    //generator level particles
    edm::Handle<reco::GenParticleCollection> genParticles_h;
    e.getByToken(inputgen_, genParticles_h);
    const reco::GenParticleCollection& genParticles = *genParticles_h;

    genN_ = genParticles.size();
    setTreeGenSize(genN_);
    size_t ip = 0;
    for(const auto& particle : genParticles)
    {
        genId_      .get()[ip] = particle.pdgId();
        genStatus_  .get()[ip] = particle.status();
        genEnergy_  .get()[ip] = particle.energy();
        genPt_      .get()[ip] = particle.pt();
        genEta_     .get()[ip] = particle.eta();
        genPhi_     .get()[ip] = particle.phi();
        ip++;
    }


    edm::Handle<HGCEEDigiCollection> ee_digis_h;
    edm::Handle<HGCHEDigiCollection> fh_digis_h;
    e.getByToken(inputee_,ee_digis_h);
    e.getByToken(inputfh_,fh_digis_h);

    const HGCEEDigiCollection& ee_digis = *ee_digis_h;
    const HGCHEDigiCollection& fh_digis = *fh_digis_h;

    cellN_ = ee_digis.size() + fh_digis.size();
    setTreeCellSize(cellN_);
    size_t ic = 0;
    for(const auto& eedata : ee_digis)
    {
        //const HGCEEDetId id(eedata.id());
        const HGCalDetId id(eedata.id());
        //const auto& modulePtr = triggerGeometry_->getModuleFromCell(id);
        //const auto& triggerCellPtr = triggerGeometry_->getTriggerCellFromCell(id);
        const GlobalPoint& center = es_info_.geom_ee->getPosition(id); 
        cellId_            .get()[ic] = id.rawId();
        cellSubdet_        .get()[ic] = ForwardSubdetector::HGCEE;
        cellSide_          .get()[ic] = id.zside();
        cellLayer_         .get()[ic] = id.layer();
        cellWafer_         .get()[ic] = id.wafer();
        cellWaferType_     .get()[ic] = id.waferType();
        cellCell_          .get()[ic] = id.cell();
        cellModuleId_      .get()[ic] = 0;//modulePtr->moduleId();
        cellTriggerCellId_ .get()[ic] = 0;//triggerCellPtr->triggerCellId();
        cellDigi_          .get()[ic] = eedata[2].data(); 
        cellX_             .get()[ic] = center.x();
        cellY_             .get()[ic] = center.y();
        cellZ_             .get()[ic] = center.z();
        cellEta_           .get()[ic] = center.eta();
        cellPhi_           .get()[ic] = center.phi();
        ic++;
    }
    for(const auto& fhdata : fh_digis)
    {
        const HGCalDetId id(fhdata.id());
        //const auto& modulePtr = triggerGeometry_->getModuleFromCell(id);
        //const auto& triggerCellPtr = triggerGeometry_->getTriggerCellFromCell(id);
        const GlobalPoint& center = es_info_.geom_fh->getPosition(id); 
        cellId_            .get()[ic] = id.rawId();
        cellSubdet_        .get()[ic] = ForwardSubdetector::HGCHEF;
        cellSide_          .get()[ic] = id.zside();
        cellLayer_         .get()[ic] = id.layer();
        cellWafer_         .get()[ic] = id.wafer();
        cellWaferType_     .get()[ic] = id.waferType();
        cellCell_          .get()[ic] = id.cell();
        cellModuleId_      .get()[ic] = 0;//modulePtr->moduleId();
        cellTriggerCellId_ .get()[ic] = 0;//triggerCellPtr->triggerCellId();
        cellDigi_          .get()[ic] = fhdata[2].data(); 
        cellX_             .get()[ic] = center.x();
        cellY_             .get()[ic] = center.y();
        cellZ_             .get()[ic] = center.z();
        cellEta_           .get()[ic] = center.eta();
        cellPhi_           .get()[ic] = center.phi();
        ic++;
    }

    //edm::Handle<l1t::HGCFETriggerDigiCollection> fet_digis_h;
    //e.getByToken(inputfet_,fet_digis_h);
    //const l1t::HGCFETriggerDigiCollection& fet_digis = *fet_digis_h;
    //// loop on trigger digis
    //triggerCellN_ = 0;
    //// first count the number of trigger cells
    //for( const auto& digi : fet_digis ) 
    //{
        //HGCalBestChoiceCodecImpl::data_type data;
        //data.reset();
        //digi.decode(codec_, data);
        //for(const auto& value : data.payload)
        //{
            //if(value==0) continue;
            //triggerCellN_++;
        //}
    //}
    //setTreeTriggerCellSize(triggerCellN_);
    //size_t itc = 0;
    //for( const auto& digi : fet_digis ) 
    //{
        //HGCalBestChoiceCodecImpl::data_type data = codec_.decode(digi.data());
        //const HGCTriggerDetId& moduleId = digi.getDetId<HGCTriggerDetId>();
        //uint32_t cell = 1; // trigger cell numbering starts at 1
        //for(const auto& value : data.payload)
        //{
            //if(value==0) continue;
            //HGCTriggerDetId triggerId(HGCTrigger,
                    //moduleId.zside(),
                    //moduleId.layer(),
                    //moduleId.sector(),
                    //moduleId.module(),
                    //cell
                    //);
            //const auto& triggerCellPtr = triggerGeometry_->triggerCells().at(triggerId);
            //const GlobalPoint& center = triggerCellPtr->position(); 
            //triggerCellId_            .get()[itc] = triggerId.rawId();
            //triggerCellSide_          .get()[itc] = triggerId.zside();
            //triggerCellLayer_         .get()[itc] = triggerId.layer();
            //triggerCellSector_        .get()[itc] = triggerId.sector();
            //triggerCellCell_          .get()[itc] = triggerId.cell();
            //triggerCellModule_        .get()[itc] = triggerId.module();
            //triggerCellModuleId_      .get()[itc] = moduleId;
            //triggerCellDigi_          .get()[itc] = value; 
            //triggerCellX_             .get()[itc] = center.x();
            //triggerCellY_             .get()[itc] = center.y();
            //triggerCellZ_             .get()[itc] = center.z();
            //triggerCellEta_           .get()[itc] = center.eta();
            //triggerCellPhi_           .get()[itc] = center.phi();
            //cell++;
        //}
    //}

    tree_->Fill();
}

/*****************************************************************/
void HGCalTriggerDigiNtuplizer::setTreeGenSize(const size_t n) 
/*****************************************************************/
{
    genId_              .reset(new int[n],   array_deleter<int>());
    genStatus_          .reset(new int[n],   array_deleter<int>());
    genEnergy_          .reset(new float[n], array_deleter<float>());
    genPt_              .reset(new float[n], array_deleter<float>());
    genEta_             .reset(new float[n], array_deleter<float>());
    genPhi_             .reset(new float[n], array_deleter<float>());

    tree_->GetBranch("gen_id")            ->SetAddress(genId_              .get());
    tree_->GetBranch("gen_status")        ->SetAddress(genStatus_          .get());
    tree_->GetBranch("gen_energy")        ->SetAddress(genEnergy_          .get());
    tree_->GetBranch("gen_pt")            ->SetAddress(genPt_              .get());
    tree_->GetBranch("gen_eta")           ->SetAddress(genEta_             .get());
    tree_->GetBranch("gen_phi")           ->SetAddress(genPhi_             .get());

}

/*****************************************************************/
void HGCalTriggerDigiNtuplizer::setTreeCellSize(const size_t n) 
/*****************************************************************/
{

    cellId_             .reset(new int[n],   array_deleter<int>());
    cellSubdet_         .reset(new int[n],   array_deleter<int>());
    cellSide_           .reset(new int[n],   array_deleter<int>());
    cellLayer_          .reset(new int[n],   array_deleter<int>());
    cellWafer_          .reset(new int[n],   array_deleter<int>());
    cellWaferType_      .reset(new int[n],   array_deleter<int>());
    cellCell_           .reset(new int[n],   array_deleter<int>());
    cellModuleId_       .reset(new int[n],   array_deleter<int>());
    cellTriggerCellId_  .reset(new int[n],   array_deleter<int>());
    cellDigi_           .reset(new int[n],   array_deleter<int>());
    cellX_              .reset(new float[n], array_deleter<float>());
    cellY_              .reset(new float[n], array_deleter<float>());
    cellZ_              .reset(new float[n], array_deleter<float>());
    cellEta_            .reset(new float[n], array_deleter<float>());
    cellPhi_            .reset(new float[n], array_deleter<float>());

    tree_->GetBranch("cell_id")           ->SetAddress(cellId_             .get());
    tree_->GetBranch("cell_subdet")       ->SetAddress(cellSubdet_         .get());
    tree_->GetBranch("cell_zside")        ->SetAddress(cellSide_           .get());
    tree_->GetBranch("cell_layer")        ->SetAddress(cellLayer_          .get());
    tree_->GetBranch("cell_wafer")        ->SetAddress(cellWafer_          .get());
    tree_->GetBranch("cell_wafertype")    ->SetAddress(cellWaferType_      .get());
    tree_->GetBranch("cell_cell")         ->SetAddress(cellCell_           .get());
    tree_->GetBranch("cell_moduleid")     ->SetAddress(cellModuleId_       .get());
    tree_->GetBranch("cell_triggercellid")->SetAddress(cellTriggerCellId_  .get());
    tree_->GetBranch("cell_digi")         ->SetAddress(cellDigi_           .get());
    tree_->GetBranch("cell_x")            ->SetAddress(cellX_              .get());
    tree_->GetBranch("cell_y")            ->SetAddress(cellY_              .get());
    tree_->GetBranch("cell_z")            ->SetAddress(cellZ_              .get());
    tree_->GetBranch("cell_eta")          ->SetAddress(cellEta_            .get());
    tree_->GetBranch("cell_phi")          ->SetAddress(cellPhi_            .get());
}

/*****************************************************************/
//void HGCalTriggerDigiNtuplizer::setTreeTriggerCellSize(const size_t n) 
/*****************************************************************/
//{

    //triggerCellId_             .reset(new int[n],   array_deleter<int>());
    //triggerCellSide_           .reset(new int[n],   array_deleter<int>());
    //triggerCellLayer_          .reset(new int[n],   array_deleter<int>());
    //triggerCellSector_         .reset(new int[n],   array_deleter<int>());
    //triggerCellCell_           .reset(new int[n],   array_deleter<int>());
    //triggerCellModule_         .reset(new int[n],   array_deleter<int>());
    //triggerCellModuleId_       .reset(new int[n],   array_deleter<int>());
    //triggerCellDigi_           .reset(new int[n],   array_deleter<int>());
    //triggerCellX_              .reset(new float[n], array_deleter<float>());
    //triggerCellY_              .reset(new float[n], array_deleter<float>());
    //triggerCellZ_              .reset(new float[n], array_deleter<float>());
    //triggerCellEta_            .reset(new float[n], array_deleter<float>());
    //triggerCellPhi_            .reset(new float[n], array_deleter<float>());

    //tree_->GetBranch("triggercell_id")           ->SetAddress(triggerCellId_             .get());
    //tree_->GetBranch("triggercell_zside")        ->SetAddress(triggerCellSide_           .get());
    //tree_->GetBranch("triggercell_layer")        ->SetAddress(triggerCellLayer_          .get());
    //tree_->GetBranch("triggercell_sector")       ->SetAddress(triggerCellSector_         .get());
    //tree_->GetBranch("triggercell_cell")         ->SetAddress(triggerCellCell_           .get());
    //tree_->GetBranch("triggercell_module")       ->SetAddress(triggerCellModule_         .get());
    //tree_->GetBranch("triggercell_moduleid")     ->SetAddress(triggerCellModuleId_       .get());
    //tree_->GetBranch("triggercell_digi")         ->SetAddress(triggerCellDigi_           .get());
    //tree_->GetBranch("triggercell_x")            ->SetAddress(triggerCellX_              .get());
    //tree_->GetBranch("triggercell_y")            ->SetAddress(triggerCellY_              .get());
    //tree_->GetBranch("triggercell_z")            ->SetAddress(triggerCellZ_              .get());
    //tree_->GetBranch("triggercell_eta")          ->SetAddress(triggerCellEta_            .get());
    //tree_->GetBranch("triggercell_phi")          ->SetAddress(triggerCellPhi_            .get());
//}
