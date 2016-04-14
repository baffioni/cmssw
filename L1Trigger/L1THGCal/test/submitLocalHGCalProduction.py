#!/usr/bin/env python

import os,sys
import optparse
import commands
import time


usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
parser.add_option('-s', '--seed'      ,    dest='seed'              , help='initial seed'                                        , default=1,  type=int)
parser.add_option('-n', '--njobs'      ,    dest='njobs'              , help='number of jobs'                                     , default=1,  type=int)
parser.add_option('-e', '--nevents'      ,    dest='nevents'              , help='number of events per job'                                     , default=1,  type=int)
parser.add_option('-o', '--out'        ,    dest='output'             , help='output directory'                                   , default='/data_CMS2/cms/SLHC18_SingleElectronPt35_PU50/')
parser.add_option('-c', '--cfg'        ,    dest='cfg'                , help='cfg file'                                           , default='SingleElectronPt35_PU0_DIGI_RAW_step2_cfg.py')
parser.add_option('-r', '--rep'        ,    dest='customReplacements' , help='sed replacements for cfg  key1:val1,key2:val2,...'  , default=None)
(opt, args) = parser.parse_args()


#prepare output
if not os.path.isdir(opt.output):
    os.makedirs(opt.output)
cmsswBase = os.environ['CMSSW_BASE']
scram = os.environ['SCRAM_ARCH']
jobsDir = opt.output

def replfunc(match):
    return repldict[match.group(0)]


#loop over the required number of jobs
for n in xrange(opt.seed,opt.seed+opt.njobs):
    jobSeed = n
    #sed the cfg template 
    inCfg = open(opt.cfg).read()
    outCfg = open('%s/cmssw_%d_cfg.py'%(jobsDir,jobSeed), 'w')
    replacements = {
        'XXX_SEED_XXX':str(jobSeed),
        'XXX_SKIP_XXX':str(jobSeed*20 - 20),
        'XXX_EVENTS_XXX':str(opt.nevents),
    }
    if opt.customReplacements is not None:
        for rep in opt.customReplacements.split(','):
            repKeys=rep.split(':')
            replacements[repKeys[0]]=repKeys[1]
    
    for i in replacements.keys():
        inCfg = inCfg.replace(i, replacements[i])
    outCfg.write(inCfg)
    outCfg.close()
    
    scriptFile = open('%s/runJob_%d.sh'%(jobsDir,jobSeed), 'w')
    scriptFile.write('#!/bin/bash\n')
    scriptFile.write('source /cvmfs/cms.cern.ch/cmsset_default.sh\n')
    scriptFile.write('export SCRAM_ARCH=%s\n'%scram)
    scriptFile.write('cd %s/src\n'%cmsswBase)
    scriptFile.write('eval `scram r -sh`\n')
    scriptFile.write('cd %s\n'%jobsDir)
    scriptFile.write('cmsRun cmssw_%d_cfg.py &> job_%d.log\n'%(jobSeed,jobSeed))
    scriptFile.write('echo "All done for job %d" \n'%jobSeed)
    scriptFile.close()
    os.system('chmod u+rwx %s/runJob_%d.sh'%(jobsDir,jobSeed))


    os.system("/opt/exp_soft/cms/t3/t3submit -k eo -q cms \'%s/runJob_%d.sh\'"%(jobsDir,jobSeed))
    
