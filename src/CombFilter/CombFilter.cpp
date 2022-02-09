//
//  CombFilter.cpp
//  MUSI6106
//
//  Created by Noel  Alben on 2/7/22.
//
// standard headers
#include <limits>

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"
#include "RingBuffer.h"

#include "CombFilterIf.h"
#include "CombFilter.h"

CCombFilterBase::CCombFilterBase(int iMaxDelayLengthInFrames, int iNumChannels)
{
    
}

CCombFilterBase::~CCombFilterBase()
{
    
}

Error_t CCombFilterBase::resetComb(){
    
}
Error_t CCombFilterBase::setParam(<#CCombFilterIf::FilterParam_t eParam#>, <#float fParamValue#>)
{
    
}
Error_t CCombFilterBase::getParam(<#CCombFilterIf::FilterParam_t eParam#>)
{
    
}

Error_t CCombFilterFir::process(<#float **ppfInputBuffer#>, <#float **ppfOutputBuffer#>, <#int iNumberOfFrames#>)
{
    
}
Error_t CCombFilterIir::process(<#float **ppfInputBuffer#>, <#float **ppfOutputBuffer#>, <#int iNumberOfFrames#>)
{
    
}
