#include "ErrorD3dkmt.h"
#include "mempulse/mempulse.h"


namespace mempulse {

ErrorD3dkmt::ErrorD3dkmt(const std::string& message)
: Error(MEMPULSE_ERROR_D3DKMT, message)
{
}

} // namespace mempulse
