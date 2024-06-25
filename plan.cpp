#include <iostream>
#include "plan.h"

#include <memory>


Plan::Plan(const OFFilename& rtPlanFilename, const OFFilename& rtStructFilename, const OFFilename& rtDoseFilename)
{
        std::cout << "RTPlan: " << rtPlanFilename  << '\n'
                  << "RTStruct: " << rtStructFilename << '\n'
                  << "RTDose: " << rtDoseFilename     << '\n' ;

        loadStruct(rtStructFilename);

		// TODO: remove later
        for(const auto& contour: m_contours)
        {
            std::cout << contour.getContourName() << '\n';
        }

		loadPlan(rtPlanFilename);
		 
}

Plan::~Plan()
{
}

static void printVector(OFVector<double> data)
{
    if(data[2] < 555 && data[2] > 554)           
    {
        std::cout << "[";
        double x, y, z;
        for(int i =0; i < data.size(); i++)
        {
            if ((i+1)%3==1)
                x = data[i]; 
            else if ((i+1)%3==2)
                y = data[i];
            else
                z = data[i]; 

            if(z < 555 && z > 554)
                std::cout << "(" << x << ',' << y << ',' << z << "), "; 

        }
    std::cout << "]\n";
    }
}

void Plan::loadStruct(const OFFilename& rtStructFilename)
{
    DcmFileFormat fileformat; 
    OFCondition status = fileformat.loadFile(rtStructFilename);
    if (status.good())
    {
        DRTStructureSetIOD rtstruct;
        status = rtstruct.read(*fileformat.getDataset());
        if (status.good())
        {
            OFString name;
            rtstruct.getPatientName(name);
			// TODO: check if the names match on all the files
            std::cout << "Patient Name: " << name << '\n';

            auto contourSequence = rtstruct.getStructureSetROISequence();       // rtstruct -> getStructureSetROISequence();
            auto ROIcontourSequence = rtstruct.getROIContourSequence();

            for(int i = 0; i < contourSequence.getNumberOfItems(); i++)
            {
                //StructureSetROISequencen
                auto ROI = contourSequence[i];                      //  Item 

                OFString ROIName;
                ROI.getROIName(ROIName);                               // item->GetName();
                int roiNumber;
                ROI.getROINumber(roiNumber);

                //ROIContourSequence
                auto ROI1 = ROIcontourSequence[i];
                int refNumber;
                ROI1.getReferencedROINumber(refNumber);
                if(roiNumber != refNumber)
                    std::cerr << "Waring : ROINumber and refNumber not matching!\n";

                auto contour = ROI1.getContourSequence();
				Contour m_contour(ROIName.c_str(), contour);
				m_contours.push_back(m_contour);
            }
        }
    }
}


void Plan::loadPlan(const OFFilename& filename)
{

	Beam beam;

	beam.setContourPointerToBeam(&m_contours);	
	

	// TODO: the rest of the control points


}
