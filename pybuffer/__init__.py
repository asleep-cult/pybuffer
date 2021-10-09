import ctypes as _ctypes
import os as _os
import string as _string
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


char_p = _ctypes.POINTER(_ctypes.c_char)

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
    char_p,
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
    'setcharat',
    buffer=_ctypes.c_void_p,
    char=_ctypes.c_ubyte,
    index=_ctypes.c_size_t
)
_pybuffer.decl_function(
    'read',
    char_p,
    buffer=_ctypes.c_void_p,
    start=_ctypes.c_size_t,
    stop=_ctypes.c_size_t,
    error=True
)
_pybuffer.decl_function(
    'write',
    _ctypes.c_size_t,
    buffer=_ctypes.c_void_p,
    data=_ctypes.c_char_p,
    size=_ctypes.c_size_t,
    offset=_ctypes.c_size_t,
    error=True
)
_pybuffer.decl_function(
    'fill',
    buffer=_ctypes.c_void_p,
    char=_ctypes.c_char,
    error=True
)
_pybuffer.decl_function(
    'writeuint8',
    _ctypes.c_size_t,
    buffer=_ctypes.c_void_p,
    number=_ctypes.c_uint8,
    offset=_ctypes.c_size_t,
    byteorder=_ctypes.c_int,
    error=True
)
_pybuffer.decl_function(
    'writeint8',
    _ctypes.c_size_t,
    buffer=_ctypes.c_void_p,
    number=_ctypes.c_int8,
    offset=_ctypes.c_size_t,
    byteorder=_ctypes.c_int,
    error=True
)
_pybuffer.decl_function(
    'writeuint16',
    _ctypes.c_size_t,
    buffer=_ctypes.c_void_p,
    number=_ctypes.c_uint16,
    offset=_ctypes.c_size_t,
    byteorder=_ctypes.c_int,
    error=True
)
_pybuffer.decl_function(
    'writeint16',
    _ctypes.c_size_t,
    buffer=_ctypes.c_void_p,
    number=_ctypes.c_int16,
    offset=_ctypes.c_size_t,
    byteorder=_ctypes.c_int,
    error=True
)
_pybuffer.decl_function(
    'writeuint32',
    _ctypes.c_size_t,
    buffer=_ctypes.c_void_p,
    number=_ctypes.c_uint32,
    offset=_ctypes.c_size_t,
    byteorder=_ctypes.c_int,
    error=True
)
_pybuffer.decl_function(
    'writeint32',
    _ctypes.c_size_t,
    buffer=_ctypes.c_void_p,
    number=_ctypes.c_int32,
    offset=_ctypes.c_size_t,
    byteorder=_ctypes.c_int,
    error=True
)
_pybuffer.decl_function(
    'writeuint64',
    _ctypes.c_size_t,
    buffer=_ctypes.c_void_p,
    number=_ctypes.c_uint64,
    offset=_ctypes.c_size_t,
    byteorder=_ctypes.c_int,
    error=True
)
_pybuffer.decl_function(
    'writeint64',
    _ctypes.c_size_t,
    buffer=_ctypes.c_void_p,
    number=_ctypes.c_int64,
    offset=_ctypes.c_size_t,
    byteorder=_ctypes.c_int,
    error=True
)


def _typename(obj):
    return repr(type(obj).__name__)


def charconv(char):
    if isinstance(char, (str, bytes)):
        return ord(char)
    elif isinstance(char, int):
        if char < 0 or char > 255:
            raise ValueError('char should be in range(0, 256)')
        return char
    else:
        raise TypeError(f'char should be a string, byte string or integer, got {_typename(char)}')


def bytesconv(data):
    if isinstance(data, bytes):
        return data
    elif isinstance(data, str):
        return data.encode('utf-8')
    elif isinstance(data, int):
        return bchr(data)
    elif isinstance(data, memoryview):
        return data.tobytes()
    elif isinstance(data, bytearray):
        return bytes(data)
    else:
        raise TypeError(f'data should be a string or bytes-like object, got {_typename(data)}')


def bchr(char):
    if char < 0 or char > 255:
        raise ValueError('char should be in range(0, 256)')
    return char.to_bytes(1, _sys.byteorder)


class _BufferChar(int):
    def __lt__(self, other):
        return super().__lt__(charconv(other))

    def __le__(self, other):
        return super().__le__(charconv(other))

    def __eq__(self, other):
        return super().__eq__(charconv(other))

    def __ne__(self, other):
        return super().__ne__(charconv(other))

    def __ge__(self, other):
        return super().__ge__(charconv(other))

    def __gt__(self, other):
        return super().__gt__(charconv(other))


_chartable = []
for i in range(256):
    _chartable.append(_BufferChar(i))


class Buffer:
    __slots__ = ('__buffer__',)

    def __new__(cls, initializer):
        try:
            data = bytesconv(initializer)
        except TypeError:
            try:
                data = tuple(charconv(char) for char in initializer)
            except TypeError:
                raise TypeError(f'cannot convert {_typename(initializer)} to Buffer') from None

            self = cls.from_size(len(data))
            for i in range(len(data)):
                self[i] = data[i]

            return self
        else:
            self = object.__new__(cls)
            self.__buffer__ = _pybuffer.from_string(initializer, len(initializer))
            return self

    @classmethod
    def from_size(cls, size, fill=None):
        self = object.__new__(cls)
        self.__buffer__ = _pybuffer.new(size)

        if fill is not None:
            self.fill(fill)

        return self

    def fill(self, char):
        _pybuffer.fill(self.__buffer__, charconv(char))

    def read(self, start=None, stop=None):
        if start is None:
            start = 0
        else:
            if not isinstance(start, int):
                raise TypeError(f'start should be an integer, got {_typename(start)}')

            if start < 0:
                start = len(self) + start

        if stop is None:
            stop = len(self)
        else:
            if not isinstance(stop, int):
                raise TypeError(f'stop should be an integer, got {_typename(stop)}')

            if stop < 0:
                stop = len(self) + stop

        return _pybuffer.read(self.__buffer__, start, stop)[:stop - start]

    def _get_offset(self, offset):
        if offset is None:
            return 0
        else:
            if not isinstance(offset, int):
                raise TypeError(f'offset should be an integer or None, got {_typename(offset)}')

            if offset < 0:
                return len(self) + offset

            return offset

    def write(self, data, offset=None):
        try:
            data = bytesconv(data)
        except TypeError:
            try:
                data = bytes(charconv(char) for char in data)
            except TypeError:
                raise TypeError(f'cannot write {_typename(data)} object to Buffer') from None

        return _pybuffer.write(self.__buffer__, data, len(data), self._get_offset(offset))

    def _get_byteorder(self, byteorder):
        if byteorder == 'little':
            return _pybuffer.PYBUFFER_LITTLE_ENDIAN
        elif byteorder == 'big':
            return _pybuffer.PYBUFFER_BIG_ENDIAN
        else:
            raise ValueError('byteorder should be \'little\' or \'big\'')

    def _check_sign(self, number):
        if number < 0:
            raise OverflowError('can\'t convert negative int to unsigned')

    def writeint8(self, number, byteorder, offset=None, *, signed=False):
        byteorder = self._get_byteorder(byteorder)
        offset = self._get_offset(offset)
        if signed:
            return _pybuffer.writeint8(self.__buffer__, number, offset, byteorder)
        else:
            self._check_sign(number)
            return _pybuffer.writeuint8(self.__buffer__, number, offset, byteorder)

    def writeint16(self, number, byteorder, offset=None, *, signed=False):
        byteorder = self._get_byteorder(byteorder)
        offset = self._get_offset(offset)
        if signed:
            return _pybuffer.writeint16(self.__buffer__, number, offset, byteorder)
        else:
            self._check_sign(number)
            return _pybuffer.writeuint16(self.__buffer__, number, offset, byteorder)

    def writeint32(self, number, byteorder, offset=None, *, signed=False):
        byteorder = self._get_byteorder(byteorder)
        offset = self._get_offset(offset)
        if signed:
            return _pybuffer.writeint32(self.__buffer__, number, offset, byteorder)
        else:
            self._check_sign(number)
            return _pybuffer.writeuint32(self.__buffer__, number, offset, byteorder)

    def writeint64(self, number, byteorder, offset=None, *, signed=False):
        byteorder = self._get_byteorder(byteorder)
        offset = self._get_offset(offset)
        if signed:
            return _pybuffer.writeint64(self.__buffer__, number, offset, byteorder)
        else:
            self._check_sign(number)
            return _pybuffer.writeuint64(self.__buffer__, number, offset, byteorder)

    def decode(self, encoding='utf-8', errors='strict'):
        data = _pybuffer.data(self.__buffer__)[:len(self)]
        return data.decode(encoding, errors)

    def __len__(self):
        return _pybuffer.size(self.__buffer__)

    def __getitem__(self, indice):
        if not isinstance(indice, int):
            raise TypeError(f'Buffer indices should be integers, got {_typename(indice)}')

        if indice < 0:
            indice = len(self) + indice

        return _chartable[_pybuffer.charat(self.__buffer__, indice)]

    def __setitem__(self, indice, char):
        if not isinstance(indice, int):
            raise TypeError(f'Buffer indices should be integers, got {_typename(indice)}')

        if indice < 0:
            indice = len(self) + indice

        _pybuffer.setcharat(self.__buffer__, indice, char)

    def __repr__(self):
        # Mainly copied from bytes.__repr__
        # https://github.com/python/cpython/blob/main/Objects/bytesobject.c#L1291-L1364
        size = 0
        for char in self:
            if char in (b'\'', b'\\', b'\t', b'\n', b'\r'):
                size += 2
            elif char < b' ' or char >= 0x7F:
                size += 4
            else:
                size += 1

        buffer = Buffer.from_size(size)

        i = 0
        for char in self:
            if char == b'\'' or char == b'\\':
                i += buffer.write((b'\\', char), i)
            elif char == b'\t':
                i += buffer.write(b'\\t', i)
            elif char == b'\n':
                i += buffer.write(b'\\n', i)
            elif char == b'\r':
                i += buffer.write(b'\\r', i)
            elif char < b' ' or char >= 0x7F:
                i += buffer.write(b'\\x', i)
                i += buffer.write(_string.hexdigits[(char & 0xF0) >> 4], i)
                i += buffer.write(_string.hexdigits[char & 0xF], i)
            else:
                i += buffer.write(char, i)

        return f'<Buffer \'{buffer.decode()}\'>'

    def __del__(self):
        if self.__buffer__ is not None:
            _pybuffer.free(self.__buffer__)


buffer = Buffer.from_size(10)
buffer.writeint16(255, 'little', signed=True)
print(buffer)
