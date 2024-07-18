#pragma once

// local headers
#include "contour.h"
#include "beam.h"

// itk headers

// dcmtk headers


//#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"
//#include "dcmtk/ofstd/ofconapp.h"

#include "dcmtk/dcmrt/drtdose.h"
//#include "dcmtk/dcmrt/drtimage.h"
#include "dcmtk/dcmrt/drtplan.h"
#include "dcmtk/dcmrt/drtstrct.h"
//#include "dcmtk/dcmrt/drttreat.h"
//#include "dcmtk/dcmrt/drtionpl.h"
//#include "dcmtk/dcmrt/drtiontr.h"

class Plan
{

    //std::vector<std::unique_ptr<Contour>> m_contours;
	std::vector<Contour> m_contours;
    std::vector<Beam> m_beams;

    // TODO: some kind of implementation for dose as well


    // methods
    void loadPlan(const OFFilename& filename);
    void loadStruct(const OFFilename& filename);
    void loadDose(const OFFilename& filename) {};

    void loadCTImage(/*foldername*/) {};

    
public:
    // contructor to load the files
    Plan(const OFFilename& rtPlanFilename, const OFFilename& rtStructFilename, const OFFilename& rtDoseFilename); 

    // constructor with foldername to load all the plan details
    Plan(const char* foldername) {}

    //destructor
    ~Plan();
};
