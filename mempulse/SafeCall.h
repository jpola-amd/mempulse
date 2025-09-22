#pragma once

#include "mempulse.h"
#include "Error.h"
#include "Logging.h"
#include <functional>
#include <expected>

namespace mempulse {

template <class Function,
    class... Params>
[[nodiscard("Please, don't ignore result")]] inline auto safeCall(Function function, Params&&... params) noexcept -> std::expected<std::invoke_result_t<Function, Params&&...>, MempulseError>
{
    try {
        if constexpr (!std::is_same<std::invoke_result_t<Function, Params&&...>, void>::value) {
            return std::invoke(function, std::forward<Params>(params)...);
        } else {
            std::invoke(function, std::forward<Params>(params)...);
            return {};
        }
    }  catch (const Error& e) {
        MEMPULSE_LOG_ERROR(e.what());
        return std::unexpected(static_cast<MempulseError>(e.getErrorCode()));
	} catch (const std::exception& e) {
        MEMPULSE_LOG_ERROR(e.what());
        return std::unexpected(MEMPULSE_ERROR_INTERNAL);
    } catch (...) {
        MEMPULSE_LOG_ERROR("unkown error");
        return std::unexpected(MEMPULSE_ERROR_INTERNAL);
    }
}

#define check(status)        \
    if (!status.has_value()) \
        return status.error();



} // namespace mempulse
