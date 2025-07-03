from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'SharedPubSub',
        ['src/module.cpp'],
        include_dirs=[pybind11.get_include(), 'src', '..','../util'],
        language='c++'
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