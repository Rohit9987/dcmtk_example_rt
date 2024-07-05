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
		
			// TODO: convert the beam parameters to the class/instance
			// beam has a container with control points

			try
			{
				auto nbeams = rtplan.getBeamSequence().getNumberOfItems();
				for(int j = 0; j < nbeams; j++)
				{
					auto beam = rtplan.getBeamSequence().getItem(j);
					OFString beamType;
					beam.getBeamType(beamType);

					double mu;
					beam.getFinalCumulativeMetersetWeight(mu);

					int numcp;
					beam.getNumberOfControlPoints(numcp);


					auto cp = beam.getControlPointSequence();

					OFVector<double> isocenter;
					cp[0].getIsocenterPosition(isocenter);

					double energy;
					cp[0].getNominalBeamEnergy(energy);

					double colAngle;
					cp[0].getBeamLimitingDeviceAngle(colAngle);

					// initialize a new beam object
					Beam beam1{Beam::BeamType::STATIC, mu, numcp, energy, {isocenter[0], isocenter[1], isocenter[2]}};

					for(int i = 0; i < numcp - 1; i++)
					{

						double angle;
						cp[i].getGantryAngle(angle);

						// TODO: need data storage for x1, x2, y1, y2, and mlcpoints;

						double x1, x2, y1, y2, mlc[120];

						auto blds = cp[i].getBeamLimitingDevicePositionSequence();
						for(int j = 0; j < blds.getNumberOfItems(); j++)
						{
							OFString bldType;
							blds[j].getRTBeamLimitingDeviceType(bldType);
							std::cout << "BLD Type: " << bldType << '\n';;
							OFVector<double> positions;
							blds[j].getLeafJawPositions(positions);
							int m = 0;
							for(auto position: positions)
								std::cout << j << ": "  << m++ << " : "  << position << "\n";

							if(j == 0)
							{
								x1 = positions[0];
								x2 = positions[1];
							}
							else if(j == 1)
							{
								y1 = positions[0];
								y2 = positions[1];
							}

							else
							{
								int k = 0;
								for(auto position: positions)
									mlc[k++] = position;
							}
						}

						ControlPoint(angle, x1, x2, y1, y2, mlc);

					}

					m_beams.push_back(beam1);

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
