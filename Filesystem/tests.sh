cd cmake-build-debug
cmake --build .
echo "FILESYSTEM COMPILED"

echo "RUNNING TESTS"

printf "\n"
echo "██████████████████████████████████████████████████████████████████████████"
echo "TEST 1 - creating and removing virtual disk"

./lab6 < ../tests/creating_disk
ls -al ./virtual_disk
./lab6 < ../tests/removing_disk
ls -al ./virtual_disk

printf "\n"
echo "██████████████████████████████████████████████████████████████████████████"
echo "TEST 2 - adding, coping and removing file"
./lab6 < ../tests/adding_removing_file
diff -s bf.txt copied_file.txt

printf "\n"
echo "██████████████████████████████████████████████████████████████████████████"
echo "TEST 3 - external fragmentation"
./lab6 < ../tests/external_fragmentation
diff -s img.png test2.png


printf "\n"
echo "██████████████████████████████████████████████████████████████████████████"
echo "TEST 4 - internal fragmentation"
./lab6 < ../tests/internal_fragmentation
