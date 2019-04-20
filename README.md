# LIVLZ

Liverpool group LZ Analysis code, which is fully compiled code using automake to work out dependencies and requires common rqlib

Others are welcome to use this too and in the future the individual analyses (e.g. `MyAnalysis`) will be split into a spearate pkg from the framework

# Checkout

```
git clone git@lz-git.ua.edu:analysis/MDC2Analysis.git
cd MDC2Analysis
git checkout liv
```

# Structure

The package has the following elements:

* `include`: header files
* `src`: source files
* `prg`: main programs
* `config`: config files
* `bin`: output executables
* `lib`: compiled so library
* `tmp`: temporary objects and dependencies
* `scripts`: shell scripts
* `python`: python scripts
* `inputs`: input file lists
* `Makefile` 

# Components

## ConfigSvc

Deals with steering or configuing the analysis.  The options are defined in ConfigSvc class, which is a singleton, and can be specicified either:

* In a config file, whose name is specificed the first time the ConfigSvc is instantiated in the prg file
* At the command line with either a long (--) or short option (-)

The latter has precedence over the former

## HistSvc

Deals with histogramming and writing them out to a file.  The histograms are booked and filled in one line, with the filling taking place the first time the call in encountered

## Logger

Print out with various levels such that only those above the level specified in the `ConfigSvc::Initialise()->Level` via `-l` are printed

## Event

Class for event quantities, which deals with reading from RQ to allow the user to calculate common derived quantities and isolate them from RQ in case of future changes.
All access to RQ should go via functions defined in there ideally but direct access to RQ allowed for testing but may open you up to changes in RQ

## Cuts

Class for defining common cuts that a user can call.  Knows abnout the Event class.

## Analysis

Base analysis class that deals with loading the LZap files and running the event loop.  Defines functions for `Initialize()`, `Execute()` and `Finalize()`, which are
called before the event loop, once per event, and after the event loop respecivelty

## MyAnalysis

Example analysis.  Users should create their own in `XXAnalysis.{h,cxx}` imnplemnting `Initialize()`, `Exectute()` and `Finalize()`.

## test.cxx

Example main program reading `config/test.config` and calling `MyAnalysis`.  Users should make their own xxx.cxx which is steered by `config/xxx.config`


# Compiling

Go to the correct directory

```
cd LIVMDC2
```

and setup root via

```
source scripts/setup.sh
```

Then copy the data file used for the common config:

```
mkdir data
scp <NERSC_USER>@pdsf.nersc.gov:/projecta/projectdirs/lz/data/MDC2/calibration/LZAP-3.6.0-PHYSICS-3.6.0/Kr83/lz_201703010000_000010_0000_lzap.root data/.
```

Please edit the main file to specify the name of the file to be used to generate `rqlib` and the location of the `common` package and then run `make`
which compiles all the code and the underlying rqlib.

To remove all the packages components compiled call

```
make clean

```

To remove the rqlib call

```
make rqclean

```

To remove everything call

```
make purge
```

# Running

If you want to access files remotely from UKDC setup your proxy via:

```
voms-proxy-init -voms lz -valid 96:00
```

To run the example put the filename list in `inputs/RQfile.list` or another file with the name specified via `fileList` in the config file, then do


```
bin/test
```


To only run over a certain number of events e.g. 10000 call

```
bin/test -n 10000
```

To run with verbose output call

```
bin/test -l 5
```

You can change the print interval with `-i` and the outputfile name with `-o` (or in the config file)

# Extending

To make your own analysis

* Copy `MyAnalysiss.{h,cxx}` changing the name for the files and the class name by replacing `My` with `XXX`
* Copy `prg/test.cxx` to `prg/YYY.cxx` and change it to use your XXXAnalysis and the config file `config/YYY.config`, which you should make
* Compile and run

# TODO

* Extend example analysis
* Add plotting code (python)
