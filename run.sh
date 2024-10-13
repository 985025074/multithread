clear
if [ "$1" = "run" ]
then
    echo "run the project"
    cd build
    make && ./hello
elif [ $1 = "objdump" ]
then
    echo "objdump the project"
    cd build
    objdump -d -j .text ./hello | c++filt
elif [ $1 = "clean" ]
then
    echo "clean the project"
    rm -rf build
elif [ $1 = "rebuilt" ]
then
    echo "rebuild the project"
    rm -rf build
    mkdir build
    cd build
    cmake .. && make && ./hello
fi