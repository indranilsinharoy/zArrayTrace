from __future__ import print_function, division
import ctypes as ct
import numpy as np
from numpy.ctypeslib import load_library


def main(dllName, dllpath):
    # load the arrayTrace library
    arrayTraceLib = load_library(dllName, dllpath)
    arrayTrace = arrayTraceLib.arrayTrace
    # specify argtypes and restype
    arrayTrace.restype = ct.c_int
    arrayTrace.argtypes = [ct.c_int]
    ret = arrayTrace(0)

    print(ret)
    print("OK!")

if __name__ == '__main__':
    import os as os
    dllDir = "x64\\Release\\"
    dllName = "ArrayTrace.dll"
    dllpath = os.path.join(os.path.dirname(os.path.realpath(__file__)), dllDir)
    main(dllName, dllpath)
