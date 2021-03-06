## OpenFOAM-v2106 with MUI-v1.2 README 

Author: Wendi Liu (wendi.liu@stfc.ac.uk) (derived from OpenFOAM MUI Patch by Stephen M. Longshaw)
Date: 24/12/2021 (Merry Christmas!)

The MUI coupling library (MUI v1.2)* is integrated into this OpenFOAM (v2106) installation (and can therefore be used in any application).

General MUI coupling is achieved using the following new source files:

src/finiteVolume/cfdTools/general/include:
	* fvCoupling.H (includes coupling[n]d header files for general use)

src/OpenFOAM/include:
	* createCouplingData.H (creates 2 coupling objects that contain 2d and 3d coupling interfaces)
	* createCouplings.H (reads coupling data from a new dictionary file at system/couplingDict (example provided in appendix A below) and creates MUI interfaces, storing them into the objects created in createCouplingData.H)

src/OpenFOAM/coupling:
	* coupling2d/coupling2d.C (creates 1 or more 2D MUI coupling interfaces with URIs equal to "mpi://[couplingName name from couplingDict]/[interfaceNames values from couplingDict TwoDInterfaces section]")
	* coupling2d/coupling2d.H
	* coupling3d/coupling3d.C (creates 1 or more 3D MUI coupling interfaces with URIs equal to "mpi://[couplingName name from couplingDict]/[interfaceNames values from couplingDict ThreeDInterfaces section]")
	* coupling3d/coupling3d.H

A number of modifications have also been made to the ThirdParty scripts to allow the MUI library to be included in all required paths. Effectively however, inclusion of MUI in a build is achieved by:
	* Ensuring a copy of the MUI folder is in the base of the ThirdParty folder before ./Allwmake is run (this will create a copy in platforms/$ARCH$WM_COMPILER/MUI, the name of the MUI folder can be changed if required in the makeMUI script and also in the OpenFOAM script at $WM_PROJECT_DIR/etc/config/MUI.sh (or MUI.csh if not using bash), this follows the standard OpenFOAM way of working with libraries).
	* The basic build rules have been changed to include a new switch -DUSE_MUI, without this, any new coupling libraries will become dummies.

To use MUI in an OpenFOAM application, the general additions are as follows:
	1) Add MUI and MPI library paths to your applications "options" Make file at Make/options, this has been done for all default solvers already (this requires the ThirdParty folder to have been successfully run and to have found MUI as this sets system variables). An example updated file for building the standard laplacianFoam solver (applications/solvers/basic/laplacianFoam) can be seen in Appendix B.
	2) Include the fvCoupling.H header file in your application. This should be done after the main OpenFOAM header (i.e. fvCFD.H in most cases):
		2.1) #ifdef USE_MUI
		     	#include "fvCoupling.H"
		     #endif
	3) Include the mui.h header file in your application, this should be done AFTER any existing header includes and should be wrapped in an ifdef to ensure it is not included if -DUSE_MUI is not passed during compilation:
		3.1) #ifdef USE_MUI
		     	#include "mui.h"
		     #endif
	4) Include the new data and couplings creation headers, these should be included AFTER any existing "createXXX.H" files such as createTime.H (not that a new runtime parameter is parsed to determine whether to create couplings or not, this is actually a new switch "-coupled", which allows a MUI-compiled application to be run either with or without coupling enabled:
		4.1) #ifdef USE_MUI
		     	#include "createCouplingData.H"

				if (args.cplRunControl().cplRun())
				{
					#include "createCouplings.H"
				}
		     #endif
	5) Important final step: Include the coupling deletion header at the END of the application. Without this the MUI interface objects will not be deleted on application exit, meaning MPI_Finalize will never be called if MUI is handling that. This should be called before the final "return 0" of the application:
		5.1) #ifdef USE_MUI
			 	#include "deleteCouplings.H"
		     #endif

Assuming the above are added then you now have access to MUI interfaces in the objects created by "createCouplingData.H", which are named "couplingInterface2d" and "couplingInterface3d" respectively (these will be empty if no interfaces are specified in the corresponding section of the couplingDict dictionary). The objects data structures are defined in the header files coupling2d.H and coupling3d.H respectively.
	
/*--------------------------------*- C++ -*----------------------------------*\
| =========                 |                                                 |
| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |
|  \\    /   O peration     | Version:  v2106                                 |
|   \\  /    A nd           | Website:  www.openfoam.com                      |
|    \\/     M anipulation  |                                                 |
\*---------------------------------------------------------------------------*/
FoamFile
{
    version     2.0;
    format      ascii;
    class       dictionary;
    location    "system";
    object      couplingDict;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

couplingName	domain1; //Coupling domain name
refLength	1;  //Length reference value for anything used in the coupling interfaces
refTime		1;  //Time reference value for anything used in the coupling interfaces

couplingConfigurations
( 
    TwoDInterfaces //2D coupling interfaces
    {
        interface_1_2d
	{
	    sending	yes;
	    receiving	yes;
	    smart_send	yes;
	}    
    }

    ThreeDInterfaces //3D coupling interfaces
    {
        interface_1_3d
	{
	    sending	yes;
	    receiving	yes;
	    smart_send	yes;
	}      
    }
);

// ************************************************************************* //

::::::::::::::::::::::: Appendix B - Make/options example :::::::::::::::::::::::

sinclude $(GENERAL_RULES)/mplib$(WM_MPLIB) << This is new 
sinclude $(RULES)/mplib$(WM_MPLIB) << This is new 
sinclude $(GENERAL_RULES)/MUI << This is new 

EXE_INC = \
    $(PFLAGS) \ << This is new 
    $(PINC) \ << This is new 
    ${MUI_INC} \ << This is new 
    -I$(LIB_SRC)/finiteVolume/lnInclude \
    -I$(LIB_SRC)/meshTools/lnInclude

EXE_LIBS = \
    $(PLIBS) \ << This is new 
    -lfiniteVolume \
    -lmeshTools

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

* Tang Y.-H., Kudo, S., Bian, X., Li, Z., & Karniadakis, G. E. Multiscale Universal Interface: A Concurrent Framework for Coupling Heterogeneous Solvers, Journal of Computational Physics, 2015, 297.15, 13-31.
