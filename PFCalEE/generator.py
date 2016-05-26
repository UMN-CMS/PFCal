import math
import random
import numpy

def genPosition(numVertices):
    
    for j in range(0, numVertices):

        x = random.uniform(1,1)
        y = random.uniform(1,1)
        z = numpy.random.exponential(1,1)

return x, y, z

def genDirection(numVertices):

    for k in range(0, numVertices):

        theta = random.uniform(0,math.pi)
        phi   = random.uniform(0,2*math.pi)
        
        px = math.sin(theta)*math.sin(phi)
        py = math.sin(theta)*math.cos(phi)
        pz = math.cos(theta)

return px, py, pz
