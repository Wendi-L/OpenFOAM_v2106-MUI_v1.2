Instructions for compiling OpenFOAM v2106 with MUI v1.1.3 embeded for Ubuntu
======================================================

These instructions are for compiling OpenFOAM [v2106](https://www.openfoam.com/news/main-news/openfoam-v2106) with MUI [v1.1.3] (https://github.com/MxUI/MUI/tree/1.1.3) embeded on Ubuntu, based on instructions from OpenFOAM [WiKi](http://openfoamwiki.net/index.php/Installation/Linux/OpenFOAM-v1806/Ubuntu).

Set installation folder:
---------------------------------------------
```bash
  export INSTALL_FOLDER=`pwd`
  cd ${INSTALL_FOLDER}/OpenFOAM/V2106/embedMUI
```

Update and install the necessary packages:
---------------------------------------------
```bash
sudo -s

apt-get update

apt-get install libglu1-mesa-dev

apt-get install build-essential flex bison cmake zlib1g-dev libboost-system-dev libboost-thread-dev libopenmpi-dev openmpi-bin gnuplot libreadline-dev libncurses-dev libxt-dev qt5-default libqt5x11extras5-dev libqt5help5 qtdeclarative5-dev qttools5-dev freeglut3-dev libqt5opengl5-dev texinfo libscotch-dev libcgal-dev

exit

```

Download and unpack:
---------------------------------------------
```bash
wget "https://develop.openfoam.com/Development/openfoam/-/archive/OpenFOAM-v2106/openfoam-OpenFOAM-v2106.tar.gz" -O OpenFOAM-v2106.tar.gz
wget "https://develop.openfoam.com/Development/ThirdParty-common/-/archive/v2106/ThirdParty-common-v2106.tar.gz" -O ThirdParty-v2106.tar.gz
 
tar -xzf OpenFOAM-v2106.tar.gz
tar -xzf ThirdParty-v2106.tar.gz
mv openfoam-OpenFOAM-v2106 OpenFOAM-v2106
mv ThirdParty-common-v2106 ThirdParty-v2106
```

Make a few symbolic links to ensure the correct global MPI installation is used by this OpenFOAM+ installation:
---------------------------------------------
```bash
ln -s /usr/bin/mpicc.openmpi OpenFOAM-v2106/bin/mpicc
ln -s /usr/bin/mpirun.openmpi OpenFOAM-v2106/bin/mpirun
```

Patch OpenFOAM and ThirdParty with MUI:
---------------------------------------------
```bash
cd ${INSTALL_FOLDER}/OpenFOAM/V6/embedMUI
cp ${INSTALL_FOLDER}/OpenFOAM/embedMUIPatch/V6/* ./
chmod +777 patch_OF6-MUI
./patch_OF6-MUI
```

For building OpenFOAM+ itself, it depends on whether you have installed the i686 or x86_64 architecture of Ubuntu. To check this, run:
---------------------------------------------
```bash
uname -m
```

Setup building parameters:
---------------------------------------------
For x86_64 with 64-bit integer support:
```bash
source $HOME/working/apps/OpenFOAM/V2106/embedMUI/OpenFOAM-v2106/etc/bashrc WM_LABEL_SIZE=64
```
For x86_64 with 32-bit integer support:
```bash
source $HOME/working/apps/OpenFOAM/V2106/embedMUI/OpenFOAM-v2106/etc/bashrc
```
For i686: 
```bash
source $HOME/working/apps/OpenFOAM/V2106/embedMUI/OpenFOAM-v2106/etc/bashrc WM_ARCH_OPTION=32
```

Save an alias in the personal .bashrc file:
---------------------------------------------
```bash
echo "alias of2106_mui='source \$HOME/working/apps/OpenFOAM/V2106/embedMUI/OpenFOAM-v2106/etc/bashrc $FOAM_SETTINGS'" >> $HOME/.bashrc
```

Go to the ThirdParty folder:
---------------------------------------------
```bash
cd $WM_PROJECT_DIR/../ThirdParty-v2106
```

Build SCOTCH v6.1.0:
---------------------------------------------
```bash
wget "https://gforge.inria.fr/frs/download.php/file/38352/scotch_6.1.0.tar.gz"
tar zxvf scotch_6.1.0.tar.gz
./makeSCOTCH
```

Build KAHIP v2.12:
---------------------------------------------
```bash
wget "http://algo2.iti.kit.edu/schulz/software_releases/KaHIP_2.12.tar.gz"
tar zxvf KaHIP_2.12.tar.gz
mv KaHIP_v2.12 kahip-2.12
./makeKAHIP
```

Build METIS v5.1.0:
---------------------------------------------
```bash
wget "http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz"
tar zxvf metis-5.1.0.tar.gz
./makeMETIS
```

Build BOOST v1.66.0:
---------------------------------------------
```bash
wget "https://boostorg.jfrog.io/artifactory/main/release/1.66.0/source/boost_1_66_0.tar.gz"
tar zxvf boost_1_66_0.tar.gz
```

Build CGAL v4.12.2:
---------------------------------------------
```bash
wget "https://github.com/CGAL/cgal/archive/refs/tags/releases/CGAL-4.12.2.tar.gz"
tar zxvf CGAL-4.12.2.tar.gz
mv cgal-releases-CGAL-4.12.2 CGAL-4.12.2
./makeCGAL
```

Build ADIOS2 v2.6.0:
---------------------------------------------
```bash
wget "https://github.com/ornladios/ADIOS2/archive/refs/tags/v2.6.0.tar.gz"
tar zxvf v2.6.0.tar.gz
./makeAdios2
```

Build ThirdParty:
---------------------------------------------
```bash
./Allwmake
```

Go into OpenFOAM's main source folder and be certain that the correct Qt version is being used:
---------------------------------------------
```bash
cd $WM_PROJECT_DIR
export QT_SELECT=qt5
```

Compile OpenFOAM in somewhere between 30 minutes to 3-6 hours: 
---------------------------------------------
```bash
./Allwmake -j 4
```

Run it a second time for getting a summary of the installation:  
---------------------------------------------
```bash
./Allwmake -j 4 > makeSummary.log 2>&1
```

Check if icoFoam is working:  
---------------------------------------------
```bash
icoFoam -help

cd $WM_PROJECT_DIR/tutorials/basic/laplacianFoam/coupled_example
./AllrunCoupled
```