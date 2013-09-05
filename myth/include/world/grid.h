#if ! defined(__GRID__)
#define __GRID__

#include <asset.h>
#include <boost/shared_ptr.hpp>

namespace Grid
{
  boost::shared_ptr<Asset> Create(size_t size);
}

#endif // __GRID__
