#!/bin/bash

echo "-------------------------------------------"
echo "Doxygen documentation creation"
echo "-------------------------------------------"

echo "Automatic generation of the rqlib"
root -q -b configure.C
root -q -b load.C macro.C+
root -q -b plot_root.C+

echo "Generation of the Macro documentation"
python doc/GenerateReadmeMacro.py

echo "Generation of the Plot Macro documentation"
python doc/GenerateReadmePlotMacro.py

echo "Generation of documentation"
doxygen .Doxigen

echo "Generation of the pdf output"
cd doc/latex
make



echo "-------------------------------------------"
echo "Doxygen documentation creation: DONE !!!"
echo "-------------------------------------------"
