echo "************************"
echo "Compiling tests"
echo "************************"
make clean
make -j 4 tests

echo "************************"
echo "Testing Genetic Algorithm methods"
echo "************************"
#res=$(./bin/test_ga)
./bin/test_ga

echo "************************"
echo "Testing logging functions"
echo "************************"
# res=$(./bin/test_prints)
./bin/test_prints

echo "************************"
echo "Testing Island Model functions"
echo "************************"
# res=$(./bin/test_im)
./bin/test_im
