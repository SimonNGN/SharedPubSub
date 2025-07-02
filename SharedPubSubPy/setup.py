from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'SharedPubSubPy',
        ['src/pybind_module.cpp'],
        include_dirs=[pybind11.get_include(), 'src', '..','../util'],
        language='c++'
    ),
]

setup(
    name='SharedPubSubPy',
    version='0.1',
    author='Your Name',
    author_email='your.email@example.com',
    description='Python bindings for the SharedPubSub library',
    ext_modules=ext_modules,
    zip_safe=False,
)