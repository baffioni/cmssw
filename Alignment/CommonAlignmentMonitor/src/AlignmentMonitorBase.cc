// -*- C++ -*-
//
// Package:     CommonAlignmentMonitor
// Class  :     AlignmentMonitorBase
// 
// Implementation:
//     <Notes on implementation>
//
// Original Author:  Jim Pivarski
//         Created:  Fri Mar 30 12:21:07 CDT 2007
// $Id: AlignmentMonitorBase.cc,v 1.5 2007/12/04 23:29:27 ratnik Exp $
//

// system include files

// user include files
#include "Alignment/CommonAlignmentMonitor/interface/AlignmentMonitorBase.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h" 
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//

// AlignmentMonitorBase::AlignmentMonitorBase(const AlignmentMonitorBase& rhs)
// {
//    // do actual copying here;
// }

AlignmentMonitorBase::AlignmentMonitorBase(const edm::ParameterSet& cfg, std::string name)
   : m_iteration(0), mp_tracker(0), mp_muon(0), mp_store(0) {
   edm::Service<TFileService> tFileService;
   m_baseDirMap[std::vector<std::string>()] = new TFileDirectory(tFileService->mkdir(name));
}

//
// assignment operators
//
// const AlignmentMonitorBase& AlignmentMonitorBase::operator=(const AlignmentMonitorBase& rhs)
// {
//   //An exception safe implementation is
//   AlignmentMonitorBase temp(rhs);
//   swap(rhs);
//
//   return *this;
// }

//
// member functions
//

void AlignmentMonitorBase::beginOfJob(AlignableTracker *pTracker, AlignableMuon *pMuon, AlignmentParameterStore *pStore) {
   mp_tracker = pTracker;
   mp_muon = pMuon;
   mp_store = pStore;

   if (!pMuon)          mp_navigator = new AlignableNavigator(pTracker);
   else if (!pTracker)  mp_navigator = new AlignableNavigator(pMuon);
   else                 mp_navigator = new AlignableNavigator(pTracker, pMuon);
}

void AlignmentMonitorBase::startingNewLoop() {
   m_iteration++;

   for (std::map<std::vector<std::string>, TFileDirectory*>::const_iterator i = m_iterDirMap.begin();  i != m_iterDirMap.end();  ++i) {
      delete i->second;
   }
   m_iterDirMap.clear();

   std::ostringstream dirName;
   dirName << "iter" << iteration();
   m_iterDirMap[std::vector<std::string>()] = new TFileDirectory(m_baseDirMap[std::vector<std::string>()]->mkdir(dirName.str()));

   book();
}

void AlignmentMonitorBase::duringLoop(const edm::EventSetup &iSetup, const ConstTrajTrackPairCollection &iTrajTracks) {
   event(iSetup, iTrajTracks);
}

void AlignmentMonitorBase::endOfLoop(const edm::EventSetup &iSetup) {
   afterAlignment(iSetup);
}

void AlignmentMonitorBase::endOfJob() {
}

TFileDirectory *AlignmentMonitorBase::directory(std::string dir) {
   std::string::size_type lastPos = dir.find_first_not_of("/", 0);
   std::string::size_type pos = dir.find_first_of("/", lastPos);
   std::vector<std::string> dirs;

   bool isIter = false;
   if (dir.substr(lastPos, pos - lastPos) == std::string("iterN")) {
      isIter = true;
      lastPos = dir.find_first_not_of("/", pos);
      pos = dir.find_first_of("/", lastPos);
   }

   while (std::string::npos != pos  ||  std::string::npos != lastPos) {
      dirs.push_back(dir.substr(lastPos, pos - lastPos));
      lastPos = dir.find_first_not_of("/", pos);
      pos = dir.find_first_of("/", lastPos);
   }

   std::map<std::vector<std::string>, TFileDirectory*> *theMap;
   if (isIter) theMap = &m_iterDirMap;
   else theMap = &m_baseDirMap;

   std::vector<std::string> partial;
   TFileDirectory *last = (*theMap)[partial];
   for (unsigned int i = 0;  i < dirs.size();  i++) {
      partial.push_back(dirs[i]);
      if (theMap->find(partial) == theMap->end()) {
	 (*theMap)[partial] = new TFileDirectory(last->mkdir(dirs[i]));
      }
      last = (*theMap)[partial];
   }
   return last;
}

TH1F *AlignmentMonitorBase::book1D(std::string dir, std::string name, std::string title, int nchX, double lowX, double highX) {
   return directory(dir)->make<TH1F>(name.c_str(), title.c_str(), nchX, lowX, highX);
}

TProfile *AlignmentMonitorBase::bookProfile(std::string dir, std::string name, std::string title, int nchX, double lowX, double highX, int nchY, double lowY, double highY, char *option) {
   if (lowY == highY) {
      return directory(dir)->make<TProfile>(name.c_str(), title.c_str(), nchX, lowX, highX, option);
   }
   else {
      return directory(dir)->make<TProfile>(name.c_str(), title.c_str(), nchX, lowX, highX, lowY, highY, option);
   }
}

TTree *AlignmentMonitorBase::bookTree(std::string dir, std::string name, std::string title) {
   return directory(dir)->make<TTree>(name.c_str(), title.c_str());
}

//
// const member functions
//

//
// static member functions
//
