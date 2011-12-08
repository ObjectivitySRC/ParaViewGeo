#include "GhostTransaction.h"

//*****************************************************************************
ostream &operator<<(ostream &os, const GhostTransaction &gt)
{
  os
    << "(" << gt.GetSourceRank() << ", " << gt.GetSourceExtent() << ")->"
    << "(" << gt.GetDestinationRank() << ", " << gt.GetDestinationExtent() << ") "
    << gt.GetIntersectionExtent();
  return os;
}

