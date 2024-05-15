#include "contour.h"

#include <iostream>
#include <cstring>

Contour::Contour(const char* contourName, DRTContourSequence& contour)
{
    m_contourName = new char[std::strlen(contourName) + 1];
    std::strcpy(m_contourName, contourName);

    
}

void Contour::convertContourToMesh(DRTContourSequence& contour)
{}


Contour::~Contour() 
{
    delete[] m_contourName;
    std::cout << "Erased\n";
}

