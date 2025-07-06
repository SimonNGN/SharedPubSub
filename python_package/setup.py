from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'SharedPubSub',
        ['module.cpp'],
        include_dirs=[pybind11.get_include(), '..','../util'],
        language='c++',
        extra_compile_args=['-std=c++20'],  # Needs this for some reason..
        cxx_std=20
    ),
]

setup(
    name='SharedPubSub',
    version='1.0',
    author='Simon Nguyen',
    description='Python bindings for the SharedPubSub library',
    ext_modules=ext_modules,
    zip_safe=False,
)