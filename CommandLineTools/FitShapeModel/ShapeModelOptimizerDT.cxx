#include "ShapeModelOptimizerDT.h"
#include "ShapeModel.h"
#include "ShapeModelImageFloat.h"
#include "VNLVTKConverters.h"
#include "ShapeModelVisualizer.h"

#include <vtkMath.h>

#include <climits>
#include <float.h>
#include <ctime>
#include <algorithm>

ShapeModelOptimizerDT::ShapeModelOptimizerDT( ShapeModel& shapeModel,
                                              ShapeModelImage& image )
: ShapeModelOptimizer( shapeModel, image )
{
}

ShapeModelOptimizerDT::~ShapeModelOptimizerDT()
{
}

void
ShapeModelOptimizerDT::beforeOptimization( double sigma )
{
  _itkImage = dynamic_cast< ShapeModelImageFloat& >( _image ).getImageITK();
  
  _imageInterpolator = FT::ImageInterpolatorType::New();
  _imageInterpolator->SetInputImage( _itkImage );
}

bool
ShapeModelOptimizerDT::transformPhysicalPointToIndex( const PointType& pt,
                                                      IndexType& pixelIndex )
{
  return _itkImage->TransformPhysicalPointToIndex( pt, pixelIndex );
}

bool 
ShapeModelOptimizerDT::updatePosition( const PointType& pt,
                                       const IndexType& idx,
                                       const PointType& prevPt,
                                       double& prevEval,
                                       PointType& qt, 
                                       const CovPixelType& normal, 
                                       double& maxEval,
                                       double& minEval,
                                       int j, int& minj )
{
  double curEval = _imageInterpolator->Evaluate( pt );
  bool updated = false;
  //std::cout << "curEval: " << curEval << " at j = " << j << std::endl;
  
  if (((curEval == 0) || (curEval < 0 && prevEval > 0)) && abs(j) < minj) // detect zero crossing
  {
    double a = fabs(prevEval); // distance to zero of previous pixel
    double b = fabs(curEval); // distance to zero of current pixel
    double w = a / (a + b); // weight of current pixel relative to previous pixel
    /*
    double a = exp( -fabs(prevEval) ); // closeness to zero of previous pixel (max 1 at 0)
    double b = exp( -fabs(curEval) ); // closeness to zero of current pixel (max 1 at 0)
    double w = b / (a + b); // weight of current pixel relative to previous pixel
    */
    for (int k = 0; k < 3; k++)
    {
      qt[k] = w * pt[k] + (1.0 - w) * prevPt[k]; // linear interpolation
    }
    minj = j;
    minEval = curEval;
    updated = true;
  }
  
  prevEval = curEval;
  return updated;
}

PointType 
ShapeModelOptimizerDT::determinePosition( unsigned int i, 
                                          const std::vector<PointType>& vecOrgPt, 
                                          const std::vector<PointType>& vecQt, 
                                          const std::vector<double>& vecMaxEval, 
                                          const std::vector<double>& vecMinEval,
                                          double maxMaxEval,
                                          double minMinEval )
{
  PointType qt = vecQt[i];
  
  return qt;
}

