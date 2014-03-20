mkdir build
cd build
cmake ..
make -j
cp ../check_result.py .
cp ../generator.ml .
ocamlopt generator.ml -o generator
