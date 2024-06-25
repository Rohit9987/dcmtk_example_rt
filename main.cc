#include <iostream>
#include <vector>
#include <cmath>

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"

// not sure what this is
#include "dcmtk/ofstd/ofconapp.h"

// rt headers
#include "dcmtk/dcmrt/drtdose.h"
#include "dcmtk/dcmrt/drtimage.h"
#include "dcmtk/dcmrt/drtplan.h"
#include "dcmtk/dcmrt/drtstrct.h"
#include "dcmtk/dcmrt/drttreat.h"
#include "dcmtk/dcmrt/drtionpl.h"
#include "dcmtk/dcmrt/drtiontr.h"

// time
#include <time.h>

// itk headers
#include "itkMesh.h"


// local header
#include "plan.h"


/*
TODO:
    1. open a rt-plan
    2. extract the information needed to calculate the dose                     - in progress, need data - rtplan, rtdose, rtstruct
        a. MU
        b. SSD
        c. field size
            - X and Y jaws
            - MLCs
        d. Energy
        e. depth inside the patient												- ongoing
        f.

    3. create the appropriate classes/files required
        - create files with PDD, OF and OAR data

    4. calculate the dose
        a. use simple 2d calculation
        b. use clarkson method
        c. pencil beam 
    5. verify the dose from the rt-dose file

    6. make UI

*/

int main(int argc, char** argv)
{
    clock_t clkStart, clkEnd;
    clkStart = clock();
        

    using namespace std::literals;

    OFFilename planFilename, doseFilename, structFilename;         // TODO: use char array to reduce memory
    if(argc < 2)
    {
        planFilename = "data/rtplan.dcm";
        structFilename = "data/rtstruct.dcm";
        doseFilename = "data/rtdose.dcm";
    }
    if (argc>3) 
    {
        planFilename = argv[1];
        structFilename = argv[2];
        doseFilename = argv[3];
    }

    Plan plan(planFilename, structFilename, doseFilename);


    clkEnd = clock();
    std::cout << (float)(clkEnd - clkStart)/CLOCKS_PER_SEC << "SECONDS\n";
    
    return 0;
}
