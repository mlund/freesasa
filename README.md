FreeSASA
========

[![DOI](https://zenodo.org/badge/18467/mittinatten/freesasa.svg)](https://zenodo.org/badge/latestdoi/18467/mittinatten/freesasa)
[![Build Status](https://travis-ci.org/mittinatten/freesasa.svg?branch=master)](https://travis-ci.org/mittinatten/freesasa)
[![Coverage Status](https://coveralls.io/repos/github/mittinatten/freesasa/badge.svg?branch=master)](https://coveralls.io/github/mittinatten/freesasa?branch=master)

FreeSASA is a C library and command line tool for calculating Solvent
Accessible Surface Area (SASA) of biomolecules. It is designed to be
simple to use with defaults, but allows customization of all
parameters of the calculation and provides a few different tools to
analyze the results. Python bindings are also included in the
repository.

By default Lee & Richards' algorithm is used, but Shrake & Rupley's is
also available. Both can be parameterized to arbitrary precision, and
for high resolution versions of the algorithms, the calculations give
identical results.

FreeSASA assigns a radius and a class to each atom. The atomic radii
are by default the _ProtOr_ radii defined by Tsai et
al. ([JMB 1999, 290: 253](http://www.ncbi.nlm.nih.gov/pubmed/10388571))
for standard amino acids and nucleic acids, and the van der Waals
radius of the element for other atoms. Each atom is also classified as
either polar or apolar.

Users can provide their own atomic radii and classifications via
configuration files. The input format for configuration files is
described in the
[online documentation](http://freesasa.github.io/doxygen/Config-file.html),
and the `share/` directory contains some sample configurations,
including one for the NACCESS parameters
([Hubbard & Thornton 1993](http://www.bioinf.manchester.ac.uk/naccess/)).

Version 2.0 adds some new features and breaks a few parts of the
interface from 1.x (mainly the API), see CHANGELOG.md for detailed
information.

Building and installing
------------------------

FreeSASA can be compiled and installed using the following

    ./configure
    make && make install

NB: If the source is downloaded from the git repository the
configure-script needs to be set up first using `autoreconf -i`. Users
who don't have autotools installed, can download a tarball that
includes the autogenerated scripts from http://freesasa.github.io/ or
from the latest
[GitHub-release](https://github.com/mittinatten/freesasa/releases).

The above commands build and install the command line tool `freesasa`
(built in `src/`), the commands

    freesasa -h

and, if installed,

    man freesasa

give an overview of options. To run a calculation from PDB-file input
using the defaults, simply type

    freesasa <pdb-file>

In addition, `make install` installs the header `freesasa.h` and the
library `libfreesasa`.

The configuration can be changed with these options:
* `--disable-json` build without support for JSON output.
* `--disable-xml` build without support for XML output.
* `--disable-threads` build without multithreaded calculations
* `--enable-doxygen` activates building of Doxygen documentation

For developers:
* `--enable-check` enables unit-testing using the Check framework
* `--enable-gcov` adds compiler flags for measuring coverage of tests
    using gcov
* `--enable-parser-generator` rebuild parser/lexer source from
    Flex/Bison sources (the autogenerated code is included in the
    repository, so no need to do this if you are not going to change
    the parser).

Python module
-------------

The Python bindings are available from PyPi and can be installed using

    pip install freesasa

This module is found in a separate repository
https://github.com/freesasa/freesasa-python
The PyPi module has binaries for Mac OS X and Windows, for a number
of Python versions and [separate documentation](http://freesasa.github.io/python/).

Documentation
-------------

Enabling Doxygen builds a [full reference
manual](http://freesasa.github.io/doxygen/), documenting both CLI and
API in the folder `doc/html/doxygen/`, also available on the web at
http://freesasa.github.io/.

After building the package, calling

    freesasa -h

explains how the commandline tool can be used.

Compatibility and dependencies
------------------------------

The library has been tested successfully with several versions of GNU
C Compiler and Clang/LLVM. It can be built using only
standard C and GNU libraries. The standard build depends on
[json-c](https://github.com/json-c/json-c) and
[libxml2](http://xmlsoft.org/). These can be disabled by configuring
with `--disable-json` and `--disable-xml` respectively.

Developers who want to do testing need to install the Check unit
testing framework. Building the full reference manual requires Doxygen
(version > 1.8.8). Changing the selection parser and lexer requires Flex and
Bison. These build options, which add extra dependencies, are disabled
by default to simplify installation for users only interested in the
command line tool and and/or C Library.

The C API can be built using MSVC, see:
https://github.com/mittinatten/freesasa/issues/22#issuecomment-374661526
(no project files provided though), but probably not the command line tool.
It should be relatively straightforwad to build the command line tool for 
Windows using MinGW or Cygwin, but this hasn't been tested (let me know if 
you've got it to work).

Citing FreeSASA
---------------

FreeSASA can be cited using the following publication

* Simon Mitternacht (2016) FreeSASA: An open source C library for
  solvent accessible surface area calculations. _F1000Research_
  5:189. (doi:
  [10.12688/f1000research.7931.1](http://dx.doi.org/10.12688/f1000research.7931.1))

The [DOI numbers from Zenodo](https://zenodo.org/badge/latestdoi/18467/mittinatten/freesasa)
can be used to cite a specific version of FreeSASA.
