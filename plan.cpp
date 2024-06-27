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
	// open the rtplan file
	DcmFileFormat fileformat;
	OFCondition status = fileformat.loadFile(filename);

	if(status.good())
	{
		DRTPlanIOD rtplan;
		status = rtplan.read(*fileformat.getDataset());
		
		if(status.good())
		{
			OFString patientName;
			status = rtplan.getPatientName(patientName);
			if(status.good())
				std::cout << "Patient name (rtplan): " << patientName << '\n';
			else
				std::cerr << "Error: Cannot access patient name (rtplan) (" << status.text() << ")\n";
		
			try
			{
				auto nbeams = rtplan.getBeamSequence().getNumberOfItems();
				for(int j = 0; j < nbeams; j++)
				{
					auto beam = rtplan.getBeamSequence().getItem(j);
					OFString beamType;
					beam.getBeamType(beamType);
					std::cout << "Beam Type: " << beamType <<'\n';

					double mu;
					beam.getFinalCumulativeMetersetWeight(mu);
					std::cout << "MU : " << mu << '\n';

					int numcp;
					beam.getNumberOfControlPoints(numcp);
					std::cout << "Number of Control points: " << numcp << '\n';

					auto cp = beam.getControlPointSequence();

					for(int i = 0; i < numcp - 1; i++)
					{
						double angle;
						cp[i].getGantryAngle(angle);
						std::cout << "Gantry angle: " << angle <<'\n';

						double colAngle;
						cp[i].getBeamLimitingDeviceAngle(colAngle);
						std::cout << "Col Angle: " << colAngle <<'\n';

						double energy;
						cp[i].getNominalBeamEnergy(energy);
						std::cout << "Beam energy: " << energy << '\n';

						double ssd;
						cp[i].getSourceToSurfaceDistance(ssd);
						std::cout << "SSD: " << ssd << '\n';

						float ssd1;
						cp[i].getSourceToExternalContourDistance(ssd1);
						std::cout << "SSD1: " << ssd1 << '\n';

						OFString entryPoint;
						cp[i].getSurfaceEntryPoint(entryPoint);
						std::cout << "EntryPoint" << entryPoint << '\n';

						OFVector<double> isocenter;
						cp[i].getIsocenterPosition(isocenter);
						std::cout << "Isocenter:"<< isocenter[0] << '\n';
						std::cout << "Isocenter:"<< isocenter[1] << '\n';
						std::cout << "Isocenter:"<< isocenter[2] << '\n';

						OFVector<double> surface;
						cp[i].getSurfaceEntryPoint(surface);
						std::cout << "Surface: " << surface[0] << '\n';
						std::cout << "Surface: " << surface[1] << '\n';
						std::cout << "Surface: " << surface[2] << '\n';

	/*
						auto blds = cp[i].getBeamLimitingDevicePositionSequence();
						for(int j = 0; j < blds.getNumberOfItems(); j++)
						{
							OFString bldType;
							blds[j].getRTBeamLimitingDeviceType(bldType);
							std::cout << "BLD Type: " << bldType << '\n';;
							OFVector<double> positions;
							blds[j].getLeafJawPositions(positions);
							for(auto position: positions)
								std::cout << j << ": " << position << "\n";
						}
						*/

					}

				}
			}
			catch(...)
			{}

		}
	}

	Beam beam;

	beam.setContourPointerToBeam(&m_contours);	
	

	// TODO: the rest of the control points


}
