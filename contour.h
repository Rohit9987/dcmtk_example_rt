#pragma once
#include "itkMesh.h"

#include "dcmtk/dcmrt/drtstrct.h"

class Contour
{

    using PixelType = float;
    constexpr static unsigned int Dimension = 3;
    using MeshType = itk::Mesh<PixelType, Dimension>;

    MeshType::Pointer m_mesh;

    char* m_contourName;

    // methods
	// method to convert contour from dcm to ITK mesh
    void convertContourToMesh(DRTContourSequence& contour);



public:

    Contour(const char* contourName, DRTContourSequence& contour);
    ~Contour();


	double calculateDepth(/*point*/) const;
    const char* getContourName() const { return m_contourName;}
};
