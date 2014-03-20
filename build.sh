mkdir build
cd build
cmake ..
make -j
cp ../check_result.py .
cp ../generator.ml .
cp ../run-test.sh .
cp ../script-plot.p .
ocamlopt generator.ml -o generator
