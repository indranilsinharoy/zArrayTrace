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
#   4. "want_opd" - used to specify the last surface (-1 for image, or any valid surface number)
class DdeArrayData(Structure):
    _fields_ = [(  'x', c_double), ('y', c_double), ('z', c_double), 
                (  'l', c_double), ('m', c_double), ('n', c_double),
                ('opd', c_double), ('intensity', c_double),  
                ('Exr', c_double), ('Exi', c_double), 
                ('Eyr', c_double), ('Eyi', c_double),
                ('Ezr', c_double), ('Ezi', c_double),
                ('wave', c_int),   ('error', c_int), 
                ('vigcode', c_int), ('want_opd', c_int)]


def main(dllName, dllpath):
    # load the arrayTrace library
    arrayTraceLib = load_library(dllName, dllpath)
    arrayTrace = arrayTraceLib.arrayTrace
    # specify argtypes and restype
    arrayTrace.restype = c_int
    arrayTrace.argtypes = [c_int]
    #ret = arrayTrace(0)

    print(ret)
    print("OK!")

if __name__ == '__main__':
    import os as os
    dllDir = "x64\\Release\\"
    dllName = "ArrayTrace.dll"
    dllpath = os.path.join(os.path.dirname(os.path.realpath(__file__)), dllDir)
    main(dllName, dllpath)
