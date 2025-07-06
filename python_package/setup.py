from setuptools import setup, Extension
import pybind11

long_description = ""
with open("README.md", "r") as fh:
    long_description = fh.read()

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
    name="SharedPubSub",
    version="1.0",
    author="Simon Nguyen",
    description="Shared memory Publisher and Subscriber library",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/SimonNGN/SharedPubSub",
    ext_modules=ext_modules,
    zip_safe=False,
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: Linux",
    ],
)