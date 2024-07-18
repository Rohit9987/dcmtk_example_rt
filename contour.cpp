#include "contour.h"

#include <iostream>
#include <cstring>

Contour::Contour(const char* contourName, DRTContourSequence& contour)
{
    m_contourName = new char[std::strlen(contourName) + 1];
    std::strcpy(m_contourName, contourName);

    
	m_mesh = MeshType::New();
	convertContourToMesh(contour);
}

void Contour::convertContourToMesh(DRTContourSequence& contour)
{
	using PointType = MeshType::PointType;

	PointType point;

	if (m_mesh == nullptr)
	{
		std::cerr << "No mesh created!\n";
		return;	
	}

	int contourPlane = 0;
	int dataPoint = 0;

	//TODO: delete
	int totalPoints = 0;

	while(contour[contourPlane++].isValid())
	{
		OFVector<double> data;
		contour[contourPlane].getContourData(data);
		totalPoints += data.size();

		for(int i = 0; 3*i < data.size(); i++)
		{
				 point[0]= data[3*i];
				 point[1] = data[3*i+1];
				 point[2] = data[3*i+2];

				 m_mesh->SetPoint(dataPoint++, point);
		}
	}
		std::cout << "Points : " << m_mesh->GetNumberOfPoints() << '\n';
		std::cout << "Total Points: " << totalPoints/3.0 << '\n';


		

}


Contour::~Contour() 
{
    //delete[] m_contourName;
	//memset(m_contourName, '\0', sizeof(m_contourName));
	//todo:: delete the names after the deletion of objects 
    std::cout << "Erased\n";
}

double Contour::calculateDepth(/* isocenter, sourcePoint*/) const
{
	/*
		isocenter x, y, z;
		source xs, ys, zs;

		x = x - xs;
		y = y - ys;
		z = z - zs;		// first one
		first find the z coordinate of the isocenter first
		then find the closest point;




	*/

	return 100;		// mm
}
