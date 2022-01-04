##  OpenFOAM-v2106 MUI Patch README

Author: Wendi Liu (wendi.liu@stfc.ac.uk) (derived from OpenFOAM MUI Patch by Stephen M. Longshaw)

Date: 24/12/2021

This patch integrates the Multiscale Universal Interface (MUI) library 
into OpenFOAM-v2106 and the associated ThirdParty-v2106 folder (NOTE if the 
previous version of this patch has already been applied then please use
the patch_OF2106-MUI-Patched script and use the patch file 
"OpenFOAM-v2106-MUI-Patched.patch" instead) 

MUI can be obtained from: https://github.com/MxUI/MUI

All files changed (or created) by each patch are listed in the 
corresponding .changes file.

In order to use this patch: 

1. create a copy of the base OpenFOAM-v2106 and ThirdParty-v2106 folders.
2. copy the script "patch_OF2106-MUI" and patch files "OpenFOAM-v2106-MUI.patch" 
   and "ThirdParty-v2106-MUI.patch" into the base directory containing the folders.
3. Execute the "patch_OF2106-MUI" script to patch both folders.
4. Build and install OpenFOAM as usual, taking note that a copy of MUI needs to 
   be placed into the "MUI" folder in the base of the ThirdParty-v2106 folder first.
5. Please then refer to the new README-MUI file in the base of the OpenFOAM-v2106 folder.
