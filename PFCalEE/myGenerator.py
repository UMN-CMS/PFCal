#!/usr/bin/env python
import math
import random
import optparse
import generator

from time import strftime 

outDir = '/local/cms/user/hiltbran/standalone_sim/PFCalEE/neutron_events/'

usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
parser.add_option('-f', '--filename', dest='filename', help='lhe file name', default='myFile1')
parser.add_option('-n', '--nevts', dest='nevts', help='number of events', default=15, type=int)
(opt, args) = parser.parse_args()

lhefile = open('%s%s.lhe'%(outDir,opt.filename), 'w')
lhefile.write('<header>\n')
lhefile.write('File created by Josh Hiltbrand on %s at %s\n'%(strftime('%Y-%m-%d'),strftime('%H:%M:%S')))
lhefile.write('</header>\n')

for i in range(0,opt.nevts):

    lhefile.write('<event>\n')
  """ myfunctionHere""" random.randint(1,5) 
    







    lhefile.write('</event>\n')


lhefile.close()
