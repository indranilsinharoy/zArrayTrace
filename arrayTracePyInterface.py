from __future__ import print_function, division
#import pyzdde.zdde as pyz
#import matplotlib.pyplot as plt
import os as os
import sys as sys
from ctypes import  WinDLL, c_int, c_double, Structure, POINTER, c_char_p, create_string_buffer
import time as time
# ##############################
#       Setup
# ##############################
lensfile = "C:\\Users\\Indranil\\Documents\\ZEMAX\\Samples\\Sequential\\Objectives\\Cooke 40 degree field.zmx"



class DdeArrayData(Structure):
    _fields_ = [(  'x', c_double), ('y', c_double), ('z', c_double),
                (  'l', c_double), ('m', c_double), ('n', c_double),
                ('opd', c_double), ('intensity', c_double),
                ('Exr', c_double), ('Exi', c_double),
                ('Eyr', c_double), ('Eyi', c_double),
                ('Ezr', c_double), ('Ezi', c_double),
                ('wave', c_int),   ('error', c_int),
                ('vigcode', c_int), ('want_opd', c_int)]

def is64bit():
    return sys.maxsize > 2**31 - 1


dllDir = "x64\\Release\\" if is64bit() else "Release\\"
dllName = "ArrayTrace.dll"
dllpath = os.path.join(os.path.dirname(os.path.realpath(__file__)), dllDir)
# load the arrayTrace library
array_trace_lib = WinDLL(dllpath + dllName)
arrayTrace = array_trace_lib.arrayTrace
# specify argtypes and restype
arrayTrace.restype = c_int
arrayTrace.argtypes = [POINTER(DdeArrayData)]




# ##############
# Interface 1
# ##############

# In this method there are two main functions that are required to be used --
# getRayDataArray() and zArrayTrace()

# 1. User calls the function getRayDataArray() which creates the ctypes array
#    and returns the array after filling up the header element
# 2. The user then fills the rest of the data in the array
# 3. The user then calls zArrayTrace() with rd
# 4. The user retrieves the ray traced data from rd

# The advantage of this method:
# 1. I think it is probably more memory efficient.
# 2. Pure standard python ...

# the zArrayTrace() function will mostly likely be just imported from
# the appropriate module .... or be ready to use in pyzdde.zdde

def zArrayTrace(rd):
    """function to trace

    Parameters
    ----------
    rd : ctypes array
        array of ray data structure as specified in Zemax manual for array
        ray tracing. Use the helper function getRayDataArray() to generate ``rd``

    Returns
    -------
    ret : integer
        0 if successful,
    """
    return arrayTrace(rd)

def getRayDataArray(numRays, tType=0, mode=0, startSurf=None, endSurf=-1):
    """helper function to create the basic ray data array (rd). The caller
    must fill the appropriate fields of the array from elements rd[1] to
    rd[numRays]

    Parameters
    ----------
    numRays : integer
        number of rays that will be traced
    tType : integer (0-3)
        0 =  GetTrace (Default), 1 = GetTraceDirect, 2 = GetPolTrace,
        3 = GetPolTraceDirect
    mode : integer (0-1)
        0 = real (Default), 1 = paraxial
    startSurf : integer or None (Default)
        specify start surface in tType 1 and 3
    endSurf : integer
        specify end surface to trace. Default is image surface (-1)

    Returns
    -------
    rd : ctypes array
        array of ray data structure as specified in Zemax manual for array
        ray tracing.

    Notes
    -----
    Since the memory for the array is allocated by Python, the user doesn't
    need to worry about freeing the memory.
    """
    rd = (DdeArrayData * (numRays + 1))()
    # Setup a basic ray data array for test
    rd[0].opd = c_double(tType)
    rd[0].wave = c_int(mode)
    rd[0].error = c_int(numRays)
    if startSurf:
        rd[0].vigcode = c_int(startSurf)
    rd[0].want_opd = c_int(endSurf)
    return rd



# ###############################################################
# Test the different ideas for interfaces to get a feel for them
# ###############################################################

def test_interface_one():
    #ln = pyz.createLink()
    #ln.zLoadFile(lensfile)
    #print("Loaded zemax file:", ln.zGetFile())

    #ln.zPushLens(1)   # FOR SOME REASON, THE ARRAY RAY TRACING SEEMS TO
                      # BE WORKING ON THE LENS THAT IS IN THE MAIN ZEMAX APPLICATION WINDOW!!!!
    #ln.zNewLens()     # THIS IS JUST TO PROVE THE ABOVE POINT!!! RAY TRACING STILL ON THE LENS
                      # IN THE MAIN ZEMAX APPLICATION, EVENTHOUGH THE LENS IN THE DDE SERVER IS A "NEW LENS"
    numRays = 441
    #rd = getRayDataArray(numRays, tType=0.0, mode=0)
    rd = (DdeArrayData * (numRays + 1))()
    # Setup a basic ray data array for test
    rd[0].opd = 0.0
    rd[0].wave = 0
    rd[0].error = c_int(numRays)
    rd[0].want_opd = -1

    # Fill the rest of the ray data array
    k = 0
    for i in xrange(-10, 11, 1):
        for j in xrange(-10, 11, 1):
            k += 1
            rd[k].z = i/20                   # px
            rd[k].l = j/20                   # py
            rd[k].intensity = 1.0
            rd[k].wave = 1

    # Trace the rays
    start_time = time.clock()
    ret = zArrayTrace(rd)
    end_time = time.clock()
    print("Return value from array ray tracing", ret)
    print("Time before tracing: ", (start_time)*10e6, "micro seconds")
    print("Ray tracing took", (end_time - start_time)*10e3, " milli seconds")

    # Dump the ray trace data into a file
    tmp_file = os.path.join(os.path.dirname(os.path.realpath(__file__)), "rtd_py_interface_01.txt")
    if ret==0:
        #print_ray_data(rd, "Ray data info @ Python AFTER ray tracing:")
        k = 0
        with open(tmp_file, 'w') as f:
            f.write("Listing of Array trace data\n")
            f.write("     px      py error            xout            yout   trans\n")
            for i in xrange(-10, 11, 1):
                for j in xrange(-10, 11, 1):
                    k += 1
                    line = ("{:7.3f} {:7.3f} {:5d} {:15.6E} {:15.6E} {:7.4f}\n"
                            .format(i/20, j/20, rd[k].error, rd[k].x, rd[k].y, rd[k].intensity))
                    f.write(line)
        print("Done writing ray trace data to file")
    else:
        print("There was some problem in ray tracing")

    #print("Zemax version", ln.zGetVersion())  # To test if the PyZDDE based link still exist or not?

    # close PyZDDE link
    #ln.close()

if __name__ == '__main__':
    test_interface_one()

