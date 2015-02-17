#file: import_problem.py
#from __future__ import print_function, division
#import os as os
import time as time
#import pyzdde.zdde as pyz                  # PROBLEM
#import matplotlib.pyplot as plt            # PROBLEM
#import iutils.opticsutils.foptics as fou   # PROBLEM
#import iutils.plotutils.mplutils as mpu    # PROBLEM
#import iutils.pyutils.genutils as gu       # PROBLEM
#import iutils.pyutils.aputils as apu       # PROBLEM
#import scipy as sp                         # NO PROBLEM
#import numpy as np                         # NO PROBLEM
#import mayavi as mayavi                    # NO PROBLEM
#import iutils.pyutils.display as du        # PROBLEM SHOWS UP SOMETIME

from ctypes import  WinDLL, c_int, c_double, Structure, POINTER

class DdeArrayData(Structure):
    _fields_ = [(  'x', c_double), ('y', c_double), ('z', c_double),
                (  'l', c_double), ('m', c_double), ('n', c_double),
                ('opd', c_double), ('intensity', c_double),
                ('Exr', c_double), ('Exi', c_double),
                ('Eyr', c_double), ('Eyi', c_double),
                ('Ezr', c_double), ('Ezi', c_double),
                ('wave', c_int),   ('error', c_int),
                ('vigcode', c_int), ('want_opd', c_int)]

def my_function( ):
    num_rays = 441
    rd = (DdeArrayData * (num_rays + 1))()
    # Setup a basic ray data array for test
    rd[0].opd = 0.0
    rd[0].wave = 0
    rd[0].error = c_int(num_rays)
    rd[0].want_opd = -1
    k = 0
    for i in xrange(-10, 11, 1):
        for j in xrange(-10, 11, 1):
            k += 1
            rd[k].z = i/20.0
            rd[k].l = j/20.0
            rd[k].intensity = 1.0
            rd[k].wave = 1

    start_time = time.clock()
    ret = arrayTrace(rd)    # Call the C function
    end_time = time.clock()
    print "Time before tracing: ", (start_time)*10e6, "micro seconds"
    print "Ray tracing took", (end_time - start_time)*10e3, " milli seconds"

if __name__ == '__main__':
    array_trace_lib = WinDLL("C:\\tmp\\ArrayTrace.dll")
    arrayTrace = array_trace_lib.arrayTrace
    arrayTrace.restype = c_int
    arrayTrace.argtypes = [POINTER(DdeArrayData)]
    my_function()
