IF EXIST DF_PATH.txt SET /P _DF_PATH=<DF_PATH.txt
IF NOT EXIST DF_PATH.txt SET _DF_PATH=%CD%\DF
mkdir VC2017
cd VC2017
echo Pre-generating a build folder
cmake ..\..\.. -G"Visual Studio 15 Win64" -T v141 -DCMAKE_INSTALL_PREFIX="%_DF_PATH%"
cmake-gui .
