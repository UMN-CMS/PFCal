#!/usr/bin/env python

import os,sys
import optparse
import commands
import math
import random
import subprocess

random.seed()

usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
parser.add_option('-v', '--version'     ,    dest='version'            , help='detector version'             , default=30,      type=int)
parser.add_option('-m', '--model'       ,    dest='model'              , help='detector model'               , default=0,      type=int)
parser.add_option('-b', '--Bfield'      ,    dest='Bfield'             , help='B field value in Tesla'       , default=0,      type=float)
parser.add_option('-n', '--nevts'       ,    dest='nevts'              , help='number of events to generate' , default=15,    type=int)
parser.add_option('-o', '--out'         ,    dest='out'                , help='output directory'             , default=os.getcwd() )
parser.add_option('-e', '--eos'         ,    dest='eos'                , help='eos path to save root file to EOS',         default='')
parser.add_option('-E', '--eosin'       ,    dest='eosin'              , help='eos path to read input root file from EOS',  default='')
parser.add_option('-g', '--gun'         ,    action="store_true",  dest='dogun'              , help='use particle gun.')
parser.add_option('-S', '--no-submit'   ,    action="store_true",  dest='nosubmit'           , help='Do not submit batch job.')
(opt, args) = parser.parse_args()


nSiLayers=2

#INPATHPU="root://eoscms//eos/cms/store/user/msun/V12/MinBias/"
INPATHPU="root://eoscms//eos/cms/store/cmst3/group/hgcal/Standalone/V12/MinBias/"

if opt.version==13:
    INPATHPU="root://eoscms//eos/cms/store/cmst3/group/hgcal/Standalone/V13/MinBias/"
elif opt.version==25:
    INPATHPU="root://eoscms//eos/cms/store/cmst3/group/hgcal/Standalone/V25/MinBias/"
elif opt.version==33:
    INPATHPU="root://eoscms//eos/cms/store/cmst3/group/hgcal/Standalone/V33/MinBias/pile/gitV00-03-07/e-/"

#nPuVtxlist=[0,140,200]
nPuVtxlist=[0]

#in %
interCalibList=[3];#0,1,2,3,4,5,10,15,20,50]

granularity='0-29:4,30-65:4'
noise='0-65:0.15'
threshold='0-65:5'

if (opt.version==8) :
    granularity='0-20:4,21-30:6'
    noise='0-30:0.14'
    threshold='0-30:2'
elif opt.version<20 :
    granularity='0-19:4,20-29:4'
    noise='0-29:0.14'
    threshold='0-29:5'
elif (opt.version==21 or opt.version==24):
    granularity='0-23:6,24-33:8'
    noise='0-33:0.14'
    threshold='0-33:2'
elif opt.version==22:
    granularity='0-9:8'
    noise='0-9:0.14'
    threshold='0-9:2'
elif opt.version==23:
    granularity='0-53:12'
    noise='0-53:0.14'
    threshold='0-53:2'
elif (opt.version==25 or opt.version==26):
    granularity='0-29:4,30-41:4,42-53:8'
    noise='0-41:0.14,42-53:0.2'
    threshold='0-53:5'
elif (opt.version==30 or opt.version==100 or opt.version==110):
    granularity='0-27:4'
    noise='0-27:0.14'
    threshold='0-27:5'
elif (opt.version==33):
    granularity='0-27:4,28-39:4,40-51:8'
    noise='0-39:0.14,40-51:0.2'
    threshold='0-51:5'
elif (opt.version==27 or opt.version==31):
    granularity='0-11:4,12-23:8'
    noise='0-11:0.14,12-23:0.2'
    threshold='0-23:5'
elif (opt.version==28 or opt.version==32):
    granularity='0-11:8'
    noise='0-11:0.2'
    threshold='0-11:5'
elif (opt.version==34):
    granularity='0-23:4'
    noise='0-23:0.14'
    threshold='0-23:5'
elif (opt.version==36):
    granularity='0-23:4,24-34:4,35-46:8'
    noise='0-34:0.14,35-46:0.2'
    threshold='0-46:5'
elif (opt.version==38):
    granularity='0-10:4,11-22:8'
    noise='0-10:0.14,11-22:0.2'
    threshold='0-22:5'
elif (opt.version==35):
    granularity='0-17:4'
    noise='0-17:0.14'
    threshold='0-17:5'
elif (opt.version==37):
    granularity='0-17:4,18-26:4,27-38:8'
    noise='0-26:0.14,27-38:0.2'
    threshold='0-38:5'
elif (opt.version==39):
    granularity='0-8:4,9-20:8'
    noise='0-8:0.14,9-20:0.2'
    threshold='0-20:5'
else:
    granularity='0-51:4'
    noise='0-51:0.15'
    threshold='0-51:5'

for nPuVtx in nPuVtxlist:

    for interCalib in interCalibList:
        if nPuVtx>0 :
           suffix='Pu%d_IC%d'%(nPuVtx,interCalib)
        else :
            suffix='IC%d'%(interCalib)
            
        if opt.model!=2 : suffix='%s_Si%d'%(suffix,nSiLayers)
            
        bval="BOFF"
        if opt.Bfield>0 : bval="BON" 
        
        outDir='%s/version_%d/model_%d/%s'%(opt.out,opt.version,opt.model,bval)
              
        #eosDirIn='%s'%(opt.eosin)
    
        if len(opt.eos)>0:
            eosDirIn='root://eoscms//eos/cms%s'%(opt.eosin)
        else:
            eosDir='%s/'%(outDir)
            eosDirIn='%s/'%(outDir)

        outlog='%s/digitizer%s.log'%(outDir,suffix)
        g4log='digijob%s.log'%(suffix)
        os.system('mkdir -p %s'%outDir)
        
        #wrapper
        scriptFile = open('%s/runDigiJob%s.sh'%(outDir,suffix), 'w')
        scriptFile.write('#!/bin/bash\n')
        scriptFile.write('source /data/cmszfs1/sw/HGCAL_SIM_A/setup.sh\n')
        #scriptFile.write('cd %s\n'%(outDir))
        outTag='version%d_model%d_%s'%(opt.version,opt.model,bval)
        scriptFile.write('localdir=`pwd`\n')
        scriptFile.write('%s/bin/digitizer %d %sHGcal_%s.root $localdir/ %s %s %s %d %d %d %s | tee %s\n'%(os.getcwd(),opt.nevts,eosDirIn,outTag,granularity,noise,threshold,interCalib,nSiLayers,nPuVtx,INPATHPU,outlog))
        scriptFile.write('echo "--Local directory is " $localdir >> %s\n'%(g4log))
        scriptFile.write('ls * >> %s\n'%(g4log))
        if len(opt.eos)>0:
            scriptFile.write('grep "alias eos=" /afs/cern.ch/project/eos/installation/cms/etc/setup.sh | sed "s/alias /export my/" > eosenv.sh\n')
            scriptFile.write('source eosenv.sh\n')
            scriptFile.write('$myeos mkdir -p %s\n'%eosDir)
            scriptFile.write('cmsStage -f DigiPFcal.root %s/Digi%s_%s.root\n'%(eosDir,suffix,outTag))
            scriptFile.write('if (( "$?" != "0" )); then\n')
            scriptFile.write('echo " --- Problem with copy of file DigiPFcal.root to EOS. Keeping locally." >> %s\n'%(g4log))
            scriptFile.write('else\n')
            scriptFile.write('eossize=`$myeos ls -l %s/Digi%s_%s.root | awk \'{print $5}\'`\n'%(eosDir,suffix,outTag))
            scriptFile.write('localsize=`ls -l DigiPFcal.root | awk \'{print $5}\'`\n')
            scriptFile.write('if (( "$eossize" != "$localsize" )); then\n')
            scriptFile.write('echo " --- Copy of digi file to eos failed. Localsize = $localsize, eossize = $eossize. Keeping locally..." >> %s\n'%(g4log))
            scriptFile.write('else\n')
            scriptFile.write('echo " --- Size check done: Localsize = $localsize, eossize = $eossize" >> %s\n'%(g4log))
            scriptFile.write('echo " --- File DigiPFcal.root successfully copied to EOS: %s/Digi%s_%s.root" >> %s\n'%(eosDir,suffix,outTag,g4log))
            scriptFile.write('rm DigiPFcal.root\n')
            scriptFile.write('fi\n')
            scriptFile.write('fi\n')
        else:
            scriptFile.write('mv DigiPFcal.root Digi%s_%s.root\n'%(suffix,outTag))

        scriptFile.write('echo "--deleting core files: too heavy!!"\n')
        #scriptFile.write('rm core.*\n')
        #scriptFile.write('cp * %s/\n'%(outDir))
        scriptFile.write('echo "All done"\n')
        scriptFile.close()
        
        #submit
        """
        os.system('chmod u+rwx %s/runDigiJob%s.sh'%(outDir,suffix))
        if opt.nosubmit : os.system('echo bsub -q %s %s/runDigiJob%s.sh'%(myqueue,outDir,suffix)) 
        else: os.system("bsub -q %s \'%s/runDigiJob%s.sh\'"%(myqueue,outDir,suffix))
        """


        #submit
        """
        condorSubmit = open('%s/condorSubmit'%(outDir,suffix), 'w')
        condorSubmit.write('Executable          =  %s\n' % scriptFile.name)
        condorSubmit.write('Universe            =  vanilla\n')
        condorSubmit.write('Requirements        =  Arch=="X86_64"  &&  (Machine  !=  "zebra01.spa.umn.edu")  &&  (Machine  !=  "zebra02.spa.umn.edu")  &&  (Machine  !=  "zebra03.spa.umn.edu")  &&  (Machine  !=  "zebra04.spa.umn.edu")\n')
        condorSubmit.write('+CondorGroup        =  "cmsfarm"\n')
        condorSubmit.write('getenv              =  True\n')
        condorSubmit.write('Log         =  %s.log\n' %(outDir)
        condorSubmit.write('Queue 1\n')
        condorSubmit.close()
        """
        os.system('chmod u+rwx %s/runDigiJob%s.sh'%(outDir,suffix))
        #command = "condor_submit " + condorSubmit.name + '\n'
        #if opt.nosubmit : os.system('echo ' + command) 
        #else: subprocess.call(command.split())
