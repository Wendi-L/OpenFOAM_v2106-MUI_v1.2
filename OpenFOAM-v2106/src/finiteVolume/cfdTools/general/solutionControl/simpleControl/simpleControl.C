/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2017 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "simpleControl.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(simpleControl, 0);
}


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

bool Foam::simpleControl::read()
{
    solutionControl::read(true);
    return true;
}


bool Foam::simpleControl::criteriaSatisfied()
{
    if (residualControl_.empty())
    {
        return false;
    }

    bool achieved = true;
    bool checked = false;    // safety that some checks were indeed performed

    const dictionary& solverDict = mesh_.solverPerformanceDict();
    forAllConstIters(solverDict, iter)
    {
        const entry& solverPerfDictEntry = *iter;

        const word& fieldName = solverPerfDictEntry.keyword();
        const label fieldi = applyToField(fieldName);

        if (fieldi != -1)
        {
            Pair<scalar> residuals = maxResidual(solverPerfDictEntry);
            checked = true;

            const bool absCheck =
                (residuals.first() < residualControl_[fieldi].absTol);

            achieved = achieved && absCheck;

            if (debug)
            {
                Info<< algorithmName_ << " solution statistics:" << endl;

                Info<< "    " << fieldName << ": tolerance = "
                    << residuals.first()
                    << " (" << residualControl_[fieldi].absTol << ")"
                    << endl;
            }
        }
    }

    return checked && achieved;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::simpleControl::simpleControl
(
    fvMesh& mesh,
    const word& dictName,
    const bool verbose
)
:
    solutionControl(mesh, dictName),
    initialised_(false)
{
    read();

    if (verbose)
    {
        Info<< nl << algorithmName_;

        if (residualControl_.empty())
        {
            const scalar duration =
                mesh_.time().endTime().value()
              - mesh_.time().startTime().value();

            Info<< ": no convergence criteria found. "
                << "Calculations will run for " << duration << " steps."
                << nl;
        }
        else
        {
            Info<< ": convergence criteria" << nl;
            for (const fieldData& ctrl : residualControl_)
            {
                Info<< "    field " << ctrl.name << token::TAB
                    << " tolerance " << ctrl.absTol
                    << nl;
            }
        }
        Info<< endl;
    }
}

Foam::simpleControl::simpleControl
(
    fvMesh& mesh,
    const word& algorithmName,
	couplingInterface2d& twoDInterfaces,
	couplingInterface3d& threeDInterfaces
)
:
    solutionControl(mesh, algorithmName)
{
    read();

#ifdef USE_MUI
    //- If there are 2D interfaces defined in couplingDict then iterate through
    if(twoDInterfaces.interfaces != NULL)
    {
		if(twoDInterfaces.interfaces->size() != 0)
		{
			mui::point2d send_point, rcv_point;

			//Push values
			for(size_t i=0; i<twoDInterfaces.interfaces->size(); i++)
			{
				//- Create 2D sending point at location [0, 0]
				send_point[0] = 0;
				send_point[1] = 0;

				//- Push value of 100 to MUI interface with label "data" at sending point at the current solver time
				twoDInterfaces.interfaces->getInterface(i)->push("data", send_point, static_cast<scalar>(100));

				//- Create 2D sending point at location [0.01, 0.01]
				send_point[0] = 0.01;
				send_point[1] = 0.01;

				//- Push value of 200 to MUI interface with label "data" at sending point at the current solver time
				twoDInterfaces.interfaces->getInterface(i)->push("data", send_point, static_cast<scalar>(200));

				//- Commit the pushed value to the interface at current solver time
				twoDInterfaces.interfaces->getInterface(i)->commit(static_cast<scalar>(1));
			}

			mui::sampler_exact2d<scalar> spatial_sampler;
			mui::chrono_sampler_exact2d chrono_sampler;

			//Fetch values
			for(size_t i=0; i<twoDInterfaces.interfaces->size(); i++)
			{
				//- Create 2D receiving point at location [0, 0]
				rcv_point[0] = 0;
				rcv_point[1] = 0;

				//- Blocking fetch for value passed through interface from coupled solver
				scalar rcv_value = twoDInterfaces.interfaces->getInterface(i)->fetch("data", rcv_point, static_cast<scalar>(1), spatial_sampler, chrono_sampler);

				std::cout << "[MUI] Received value through 2D interface ("
						  << twoDInterfaces.interfaces->getInterfaceName(i) << "): "
						  << rcv_value << std::endl;

				//- Create 2D receiving point at location [0.01, 0.01]
				rcv_point[0] = 0.01;
				rcv_point[1] = 0.01;

				//- Blocking fetch for value passed through interface from coupled solver
				rcv_value = twoDInterfaces.interfaces->getInterface(i)->fetch("data", rcv_point, static_cast<scalar>(1), spatial_sampler, chrono_sampler);

				std::cout << "[MUI] Received value through 2D interface ("
						  << twoDInterfaces.interfaces->getInterfaceName(i) << "): "
						  << rcv_value << std::endl;
			}
		}
    }

    //- If there are 3D interfaces defined in couplingDict then iterate through
    if(threeDInterfaces.interfaces != NULL)
	{
		if(threeDInterfaces.interfaces->size() != 0)
		{
			mui::point3d send_point, rcv_point;

			//Push values
			for(size_t i=0; i<threeDInterfaces.interfaces->size(); i++)
			{
				//- Create 3D sending point at location [0, 0, 0]
				send_point[0] = 0;
				send_point[1] = 0;
				send_point[2] = 0;

				//- Push value of 100 to MUI interface with label "data" at sending point at the current solver time
				threeDInterfaces.interfaces->getInterface(i)->push("data", send_point, static_cast<scalar>(100));

				//- Create 3D sending point at location [0.01, 0.01, 0.01]
				send_point[0] = 0.01;
				send_point[1] = 0.01;
				send_point[2] = 0.01;

				//- Push value of 200 to MUI interface with label "data" at sending point at the current solver time
				threeDInterfaces.interfaces->getInterface(i)->push("data", send_point, static_cast<scalar>(200));

				//- Commit the pushed values to the interface at current solver time
				threeDInterfaces.interfaces->getInterface(i)->commit(static_cast<scalar>(1));
			}

			mui::sampler_exact3d<scalar> spatial_sampler;
			mui::chrono_sampler_exact3d chrono_sampler;

			//Fetch values
			for(size_t i=0; i<threeDInterfaces.interfaces->size(); i++)
			{
				//- Create 3D receiving point at location [0, 0, 0]
				rcv_point[0] = 0;
				rcv_point[1] = 0;
				rcv_point[2] = 0;

				//- Blocking fetch for value passed through interface from coupled solver
				scalar rcv_value = threeDInterfaces.interfaces->getInterface(i)->fetch("data", rcv_point, static_cast<scalar>(1), spatial_sampler, chrono_sampler);

				std::cout << "[MUI] Received value through 3D interface ("
						  << threeDInterfaces.interfaces->getInterfaceName(i) << "): "
						  << rcv_value << std::endl;

				//- Create 3D receiving point at location [0.01, 0.01, 0.01]
				rcv_point[0] = 0.01;
				rcv_point[1] = 0.01;
				rcv_point[2] = 0.01;

				//- Blocking fetch for value passed through interface from coupled solver
				rcv_value = threeDInterfaces.interfaces->getInterface(i)->fetch("data", rcv_point, static_cast<scalar>(1), spatial_sampler, chrono_sampler);

				std::cout << "[MUI] Received value through 3D interface ("
						  << threeDInterfaces.interfaces->getInterfaceName(i) << "): "
						  << rcv_value << std::endl;
			}
		}
	}
#endif
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::simpleControl::loop()
{
    solutionControl::setFirstIterFlag(true, true);

    read();

    Time& runTime = const_cast<Time&>(mesh_.time());

    if (initialised_ && criteriaSatisfied())
    {
        Info<< nl
            << algorithmName_
            << " solution converged in "
            << runTime.timeName() << " iterations" << nl << endl;

        // Set to finalise calculation
        runTime.writeAndEnd();
    }
    else
    {
        initialised_ = true;
        storePrevIterFields();
    }

    return runTime.loop();
}


// ************************************************************************* //
