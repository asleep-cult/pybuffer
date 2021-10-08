import os

dirname = os.path.join(os.path.dirname(__file__), 'pybuffer')
os.system(
    f'gcc -shared -fPIC {dirname}/pybuffer.c -I{dirname} -o {dirname}/pybuffer.o'  # noqa
)
