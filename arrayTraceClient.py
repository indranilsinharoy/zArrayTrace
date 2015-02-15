from __future__ import print_function, division
from ctypes import  c_int, c_double, Structure, POINTER
import numpy as np
from numpy.ctypeslib import load_library

# Ray data structure
# Following fields in the 0th array element have special meaning:
#   1. "opd" - used to specify ray-trace-mode (0 ==  GetTrace, 1 == GetTraceDirect, 
#                                              2 ==  GetPolTrace, 3 == GetPolTraceDirect) 
#   2. "wave" - used to specify real or paraxial mode (0 == real, 1 == paraxial) 
#   3. "error" - used to specify the number of rays in the array
#   4. "vigcode" - used to specify the start surface in mode 1, mode 3
#   5. "want_opd" - used to specify the last surface (-1 for image, or any valid surface number)
class DdeArrayData(Structure):
    _fields_ = [(  'x', c_double), ('y', c_double), ('z', c_double), 
                (  'l', c_double), ('m', c_double), ('n', c_double),
                ('opd', c_double), ('intensity', c_double),  
                ('Exr', c_double), ('Exi', c_double), 
                ('Eyr', c_double), ('Eyi', c_double),
                ('Ezr', c_double), ('Ezi', c_double),
                ('wave', c_int),   ('error', c_int), 
                ('vigcode', c_int), ('want_opd', c_int)]



def print_ray_data(rd):
    # Print the ray data for debugging
    print("Ray data info @ Python side before sending to C side:\n")
    num_rays = rd[0].error
    for i in range(num_rays + 1):
        print("rd[{}].x = ".format(i), rd[i].x)
        print("rd[{}].y = ".format(i), rd[i].y)
        print("rd[{}].z = ".format(i), rd[i].z)
        print("rd[{}].l = ".format(i), rd[i].l)
        print("rd[{}].m = ".format(i), rd[i].m)
        print("rd[{}].n = ".format(i), rd[i].n)
        print("rd[{}].opd = ".format(i), rd[i].opd)
        print("rd[{}].intensity = ".format(i), rd[i].intensity)
        print("rd[{}].Exr = ".format(i), rd[i].Exr)
        print("rd[{}].Exi = ".format(i), rd[i].Exi)
        print("rd[{}].Eyr = ".format(i), rd[i].Eyr)
        print("rd[{}].Eyi = ".format(i), rd[i].Eyi)
        print("rd[{}].Ezr = ".format(i), rd[i].Ezr)
        print("rd[{}].Ezi = ".format(i), rd[i].Ezi)
        print("rd[{}].wave = ".format(i), rd[i].wave)
        print("rd[{}].error = ".format(i), rd[i].error)
        print("rd[{}].vigcode = ".format(i), rd[i].vigcode)
        print("rd[{}].want_opd = ".format(i), rd[i].want_opd)
        print("\n")

def main(dllName, dllpath):
    # load the arrayTrace library
    array_trace_lib = load_library(dllName, dllpath)
    arrayTrace = array_trace_lib.arrayTrace
    # specify argtypes and restype
    arrayTrace.restype = c_int
    arrayTrace.argtypes = [POINTER(DdeArrayData)]
    
    # Set up ray data 
    num_rays = 9
    rd = (DdeArrayData * (num_rays + 1))()
    
    # Setup a basic ray data array for test 
    rd[0].opd = 0.0  # mode 0, like GetTrace
    rd[0].wave = 0   # real ray tracing
    rd[0].error = c_int(num_rays)  
    rd[0].want_opd = -1
    k = 0
    for i in [-10, 0, 10]:
        for j in [-10, 0, 10]:
            k += 1
            rd[k].z = i/20
            rd[k].l = j/20
            rd[k].intensity = 1.0
            rd[k].wave = 1


    #print_ray_data(rd) # for visual debugging


    ret = arrayTrace(rd)

    #print(ret)
    print("OK!")

if __name__ == '__main__':
    import os as os
    dllDir = "x64\\Release\\"
    dllName = "ArrayTrace.dll"
    dllpath = os.path.join(os.path.dirname(os.path.realpath(__file__)), dllDir)
    main(dllName, dllpath)
