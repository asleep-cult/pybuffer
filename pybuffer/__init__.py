import ctypes as _ctypes
import os as _os
import sys as _sys


class _CFunction:
    def __init__(self, name, restype, arguments, function, error):
        self.error = error
        self.name = name
        self.function = function

        self.function.argtypes = arguments
        if self.error:
            arguments.append(_ctypes.POINTER(_ctypes.c_int))

        self.function.restype = restype

    def _check_error(self, error):
        if error == _pybuffer.PYBUFFER_NOMEM:
            raise MemoryError()
        elif error == _pybuffer.PYBUFFER_OUT_OF_BOUNDS:
            raise IndexError('Buffer index out of bounds')

    def __call__(self, *args, **kwargs):
        if self.error:
            error = _ctypes.c_int(_pybuffer.PYBUFFER_OK)
            try:
                return self.function(*args, _ctypes.pointer(error), **kwargs)
            finally:
                self._check_error(error.value)
        else:
            return self.function(*args, **kwargs)


class _CBinding:
    def __init__(self, path):
        if _sys.platform == 'win32':
            self.lib = _ctypes.windll.LoadLibrary(path)
        else:
            self.lib = _ctypes.cdll.LoadLibrary(path)

    def define(self, name, value):
        setattr(self, name, value)

    def decl_function(self, name, restype=None, **kwargs):
        error = kwargs.pop('error', False)

        function = getattr(self.lib, 'pybuffer_' + name)
        cfunc = _CFunction(name, restype, list(kwargs.values()), function, error)

        setattr(self, name, cfunc)


_pybuffer = _CBinding(_os.path.join(_os.path.dirname(__file__), 'pybuffer.o'))

_pybuffer.define('PYBUFFER_OK', 0)
_pybuffer.define('PYBUFFER_NOMEM', -1)
_pybuffer.define('PYBUFFER_OUT_OF_BOUNDS', -2)

_pybuffer.define('PYBUFFER_BIG_ENDIAN', 0)
_pybuffer.define('PYBUFFER_LITTLE_ENDIAN', 1)

_pybuffer.decl_function(
    'new',
    _ctypes.c_void_p,
    size=_ctypes.c_size_t,
    error=True
)
_pybuffer.decl_function(
    'from_string',
    _ctypes.c_void_p,
    data=_ctypes.c_char_p,
    size=_ctypes.c_size_t,
    error=True
)
_pybuffer.decl_function(
    'free',
    buffer=_ctypes.c_void_p
)
_pybuffer.decl_function(
    'size',
    _ctypes.c_size_t,
    buffer=_ctypes.c_void_p
)
_pybuffer.decl_function(
    'data',
    _ctypes.c_char_p,
    buffer=_ctypes.c_void_p
)
_pybuffer.decl_function(
    'charat',
    _ctypes.c_ubyte,
    buffer=_ctypes.c_void_p,
    index=_ctypes.c_size_t,
    error=True
)
_pybuffer.decl_function(
    'read',
    _ctypes.c_char_p,
    buffer=_ctypes.c_void_p,
    start=_ctypes.c_size_t,
    stop=_ctypes.c_size_t,
    error=True
)
_pybuffer.decl_function(
    'write',
    buffer=_ctypes.c_void_p,
    data=_ctypes.c_char_p,
    start=_ctypes.c_size_t,
    stop=_ctypes.c_size_t,
    error=True
)
_pybuffer.decl_function(
    'fill',
    buffer=_ctypes.c_void_p,
    char=_ctypes.c_char,
    error=True
)


class Buffer:
    def __init__(self):
        pass

    @classmethod
    def from_size(cls, size):
        self = cls.__new__(cls)
        self.__buffer__ = _pybuffer.new(size)
        return self

    def fill(self, char):
        if isinstance(char, (bytes, str)):
            char = ord(char)
        elif isinstance(char, int):
            if char < 0 or char > 255:
                raise ValueError('char should be in range(0, 256)')
        else:
            raise TypeError(f'char should be a str, byte string or int, got {type(char)!r}')

        _pybuffer.fill(self.__buffer__, char)

    def __len__(self):
        return _pybuffer.size(self.__buffer__)

    def __getitem__(self, index):
        if index < 0:
            index = len(self) + index
        return _pybuffer.charat(self.__buffer__, index)

    def __del__(self):
        if self.__buffer__ is not None:
            _pybuffer.free(self.__buffer__)
