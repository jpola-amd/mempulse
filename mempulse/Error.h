#pragma once

#include <stdexcept>
#include <string>

namespace mempulse {

class Error : public std::runtime_error {
public:
    explicit Error(int err, const std::string& message);
    int getErrorCode() const noexcept { return errorCode; }

private:
    int errorCode;
};

class ErrorInternal : public Error {
public:
    explicit ErrorInternal(const std::string& message);
};

class ErrorInvalidParameter : public Error {
public:
    explicit ErrorInvalidParameter(
			const std::string& parameterName,
			const std::string& message);
};

class ErrorInvalidContext : public Error {
public:
	explicit ErrorInvalidContext();
};

class ErrorInvalidMemory : public Error {
public:
	explicit ErrorInvalidMemory();
};

class ErrorInvalidDevice : public Error {
public:
    explicit ErrorInvalidDevice(int deviceId);
};

}
