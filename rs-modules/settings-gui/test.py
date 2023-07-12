
import ctypes
from cffi import FFI
ffi = FFI()

dll_debug = "settings-gui\\target\\debug\\settings.dll"
dll_release = "settings-gui\\target\\release\\settings.dll"

# lib = ctypes.cdll.LoadLibrary(dll_release)
# lib.run()

ffi.cdef("void run();")
dll = ffi.dlopen(dll_release)
dll.run()