import FWCore.ParameterSet.Config as cms

process = cms.Process('DigiNtuplizer')


process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.Geometry.GeometryExtended2023DevReco_cff')
process.load('Configuration.Geometry.GeometryExtended2023Dev_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('Configuration.StandardSequences.VtxSmearedNoSmear_cff')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')



process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)
files = ['file:signal.root']

# Input source
process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring()
                            )
process.source.fileNames = cms.untracked.vstring(files)
process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')

process.options = cms.untracked.PSet(

)

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("digintuple.root")
    )



process.hgcaltriggerntuplizer = cms.EDAnalyzer(
    "HGCalTriggerDigiNtuplizer",
    gen     = cms.InputTag("genParticles"),
    eeDigis = cms.InputTag('mix:HGCDigisEE'),
    fhDigis = cms.InputTag('mix:HGCDigisHEfront'),
    bhDigis = cms.InputTag('mix:HGCDigisHEback'),
    TriggerGeometry = cms.PSet(
        TriggerGeometryName = cms.string('HGCalTriggerGeometryImp1'),
        L1TCellsMapping = cms.FileInPath("L1Trigger/L1THGCal/data/cellsToTriggerCellsMap.txt"),
        eeSDName = cms.string('HGCalEESensitive'),
        fhSDName = cms.string('HGCalHESiliconSensitive'),
        bhSDName = cms.string('HGCalHEScintillatorSensitive'),
        ),
    )

process.p = cms.Path(process.hgcaltriggerntuplizer)


