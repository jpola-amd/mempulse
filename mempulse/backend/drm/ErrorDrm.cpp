#include "ErrorDrm.h"
#include "mempulse/mempulse.h"

namespace mempulse {

ErrorDrm::ErrorDrm(const std::string& message)
: Error(MEMPULSE_ERROR_DRM, message) {
}

} // namepsace mempulse
