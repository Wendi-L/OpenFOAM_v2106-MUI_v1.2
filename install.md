Instructions for compiling OpenFOAM v2106 with MUI v1.2 embeded for Ubuntu
======================================================

These instructions are for compiling OpenFOAM [v2106](https://www.openfoam.com/news/main-news/openfoam-v2106) with MUI [v1.2] (https://github.com/MxUI/MUI/tree/1.2) embeded on Ubuntu, based on instructions from OpenFOAM [WiKi](http://openfoamwiki.net/index.php/Installation/Linux/OpenFOAM-v1806/Ubuntu).

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
cd ${INSTALL_FOLDER}/OpenFOAM/V2106/embedMUI
cp OpenFOAMv2106-MUIv1.2-gcc/* ./
chmod +777 patch_OF2106-MUI
./patch_OF2106-MUI
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

Build MUI v1.2:
---------------------------------------------
```bash
ln -s /path/to/MUI-v1.2 MUI
./makeMUI
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
./Allwmake
```

Run it a second time for getting a summary of the installation:  
---------------------------------------------
```bash
./Allwmake > makeSummary.log 2>&1
```

Check if OpenFOAM v2106 is working:  
---------------------------------------------
```bash
icoFoam -help
```

If OpenFOAM v2106 itself is working, message sililar with below will be printed on screen:

**********************************************************************************************
Usage: icoFoam [OPTIONS]

Options:

  -case <dir>       Specify case directory to use (instead of cwd)
  
  -decomposeParDict <file>

                    Use specified file for decomposePar dictionary

  -dry-run          Check case set-up only using a single time step

  -dry-run-write    Check case set-up and write only using a single time step

  -parallel         Run in parallel

  -postProcess      Execute functionObjects only

  -doc              Display documentation in browser

  -help             Display short help and exit

  -help-full        Display full help and exit


Transient solver for incompressible, laminar flow of Newtonian fluids.

Using: OpenFOAM-v2106 (2106) - visit www.openfoam.com

Build: v2106

Arch:  LSB;label=64;scalar=64

**********************************************************************************************

Check if MUI communication is working:  
---------------------------------------------
```bash
cd $WM_PROJECT_DIR/tutorials/basic/laplacianFoam/MUI_couple_example
./AllrunCoupled
```

If MUI communication inside OpenFOAM v2106 is working, message sililar with below will be occur at the very begining of the log:

**********************************************************************************************
/*---------------------------------------------------------------------------*\

| =========                 |                                                 |

| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |

|  \\    /   O peration     | Version:  v2106                                 |

|   \\  /    A nd           | Website:  www.openfoam.com                      |

|    \\/     M anipulation  |                                                 |

\*---------------------------------------------------------------------------*/

Build  : v2106 OPENFOAM=2106

Arch   : "LSB;label=64;scalar=64"

Exec   : laplacianFoam -parallel

Date   : Dec 24 2021

Time   : 13:54:21

Host   : ****

PID    : ****

I/O    : uncollated

Case   : ****/OpenFOAM-v2106/tutorials/basic/laplacianFoam/MUI_couple_example

nProcs : 4

Hosts  :

(

    (*****)

)

Pstream initialized with:

    floatTransfer      : 0

    nProcsSimpleSum    : 0

    commsType          : nonBlocking

    polling iterations : 0

trapFpe: Floating point exception trapping enabled (FOAM_SIGFPE).

fileModificationChecking : Monitoring run-time modified files using timeStampMaster (fileModificationSkew 5, maxFileModificationPolls 20)

allowSystemOperations : Allowing user-supplied system call operations


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Create time

Create mesh for time = 0

MUI [lib_mpi_multidomain]: Rank 0 "domain1" registered interface interface_1 as a1c24ebc

MUI [lib_mpi_multidomain]: Rank 1 "domain1" registered interface interface_1 as a1c24ebc

MUI [lib_mpi_multidomain]: Rank 2 "domain2" registered interface interface_1 as a1c24ebc

MUI [lib_mpi_multidomain]: Rank 3 "domain2" registered interface interface_1 as a1c24ebc

MUI Info [lib_mpi_multidomain]: 1 distinct interface(s) found

MUI [lib_mpi_multidomain]: Setting up interface interface_1 [a1c24ebc] (rank ids are local to each interface)

MUI [comm_mpi.h]: rank 1	identifier mpi://domain1/interface_1	domain size 2	peer number 2	

MUI [comm_mpi.h]: rank 0	identifier mpi://domain1/interface_1	domain size 2	peer number 2	

MUI [comm_mpi.h]: rank 3	identifier mpi://domain2/interface_1MUI [comm_mpi.h]: rank 2	identifier mpi://domain2/interface_1	domain size 2	peer number 2	

	domain size 2	peer number 2	

MUI [lib_mpi_multidomain]: Rank 0 "domain1" registered interface interface_2 as 6abfb875

MUI [lib_mpi_multidomain]: Rank 2 "domain2" registered interface interface_2 as 6abfb875

MUI [lib_mpi_multidomain]: Rank 1 "domain1" registered interface interface_2 as 6abfb875

MUI [lib_mpi_multidomain]: Rank 3 "domain2" registered interface interface_2 as 6abfb875

MUI Info [lib_mpi_multidomain]: 1 distinct interface(s) found

MUI [lib_mpi_multidomain]: Setting up interface interface_2 [6abfb875] (rank ids are local to each interface)

MUI [comm_mpi.h]: rank 1	identifier mpi://domain1/interface_2	domain size 2	peer number 2	

MUI [comm_mpi.h]: rank 0	identifier mpi://domain1/interface_2	domain size 2	peer number 2	

MUI [comm_mpi.h]: rank 2	identifier mpi://domain2/interface_2	domain size 2	peer number 2	

MUI [comm_mpi.h]: rank 3	identifier mpi://domain2/interface_2	domain size 2	peer number 2	


SIMPLE: No convergence criteria found

[MUI] Received value through 3D interface (interface_1): 100

[MUI] Received value through 3D interface (interface_1): 200

[MUI] Received value through 3D interface (interface_2): 100

[MUI] Received value through 3D interface (interface_2): 200


SIMPLE: No convergence criteria found

[MUI] Received value through 3D interface (interface_1): 100

[MUI] Received value through 3D interface (interface_1): 200

[MUI] Received value through 3D interface (interface_2): 100

[MUI] Received value through 3D interface (interface_2): 200


Reading field T

Reading transportProperties

........

**********************************************************************************************
