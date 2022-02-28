import numpy as np
import ctypes as C

GENERATIONS = 5000
GENOME_SIZE = 1000

CMPFUNC = C.CFUNCTYPE(C.c_float, C.POINTER(C.c_float*GENOME_SIZE), C.c_size_t)


def error(x, length):
    """!
    A simple test fitness function (Sphere).
    """
    address = C.addressof(x.contents)
    x = np.ctypeslib.as_array((C.c_float * length).from_address(address))
    tmp = (x**2).sum()
    return -float(tmp)


pygaim = C.cdll.LoadLibrary("/home/gdetorak/packages/gaim/lib/libgaim.so")
ga_optimization = pygaim.ga_optimization_python


pygaim.ga_optimization_python.argtypes = [CMPFUNC,
                                          C.c_size_t,
                                          C.c_size_t,
                                          C.c_size_t,
                                          C.c_size_t,
                                          C.c_size_t,
                                          C.c_size_t,
                                          C.POINTER(C.c_float*GENOME_SIZE),
                                          C.POINTER(C.c_float*GENOME_SIZE),
                                          C.c_char_p,
                                          C.c_char_p,
                                          C.c_char_p,
                                          C.c_float,
                                          C.c_size_t,
                                          C.c_size_t,
                                          C.c_size_t,
                                          C.c_bool,
                                          C.c_char_p,
                                          C.c_char_p,
                                          C.c_float,
                                          C.c_float,
                                          C.c_float,
                                          C.c_float,
                                          C.c_size_t,
                                          C.c_bool,
                                          C.c_bool,
                                          C.c_size_t,
                                          C.c_size_t,
                                          C.c_size_t,
                                          C.c_char_p,
                                          C.c_char_p,
                                          C.c_char_p,
                                          C.c_char_p,
                                          C.c_char_p,
                                          C.c_bool,
                                          C.c_bool,
                                          C.c_bool,
                                          C.c_bool,
                                          C.POINTER(C.POINTER(C.c_float)),
                                          C.POINTER(C.POINTER(C.c_float)),
                                          C.POINTER(C.POINTER(C.c_float))]
pygaim.ga_optimization_python.restype = None


callback = CMPFUNC(error)


if __name__ == '__main__':
    a = np.array([-1. for i in range(GENOME_SIZE)],
                 'f').ctypes.data_as(C.POINTER(C.c_float*GENOME_SIZE))
    b = np.array([1. for i in range(GENOME_SIZE)],
                 'f').ctypes.data_as(C.POINTER(C.c_float*GENOME_SIZE))

    clipping = "universal"
    b_clipping = clipping.encode('utf-8')
    clipping_fname = "clamp_fname.dat"
    b_clipping_fname = clipping_fname.encode('utf-8')
    selection_method = "ktournament"
    b_selection_method = selection_method.encode('utf-8')

    genome = np.zeros((GENOME_SIZE,))
    gp = genome.ctypes.data_as(C.POINTER(C.c_float))
    bsf = np.zeros((GENERATIONS,))
    bp = genome.ctypes.data_as(C.POINTER(C.c_float))
    avg = np.zeros((GENERATIONS,))
    ap = genome.ctypes.data_as(C.POINTER(C.c_float))

    res = pygaim.ga_optimization_python(callback,
                                        GENERATIONS,
                                        50,
                                        GENOME_SIZE,
                                        10,
                                        5,
                                        1,
                                        a,
                                        b,
                                        b'universal',
                                        b'clip_file.dat',
                                        b'ktournament',
                                        1.5,
                                        2,
                                        1,
                                        2,
                                        False,
                                        b'one_point',
                                        b'delta',
                                        0.2,
                                        0.1,
                                        0.0,
                                        1.0,
                                        1,
                                        True,
                                        False,
                                        5,
                                        3,
                                        500,
                                        b'elite',
                                        b'poor',
                                        b'graph.dat',
                                        b"experiment1",
                                        b"./data/",
                                        False,
                                        True,
                                        True,
                                        True,
                                        C.byref(gp),
                                        C.byref(bp),
                                        C.byref(ap))

    print(genome)

    import matplotlib.pylab as plt
    plt.plot(avg)
    plt.show()
