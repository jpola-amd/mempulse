#include "File.h"
#include "ErrorDrm.h"
#include "mempulse/Logging.h"

#include <unistd.h>
#include <fcntl.h>


namespace mempulse {

inline int to_flags(File::Mode mode) {
	switch (mode) {
		case File::Mode::ReadOnly:
			return O_RDONLY;
		case File::Mode::ReadWrite:
			return O_RDWR;
		default:
			break;
	}
	MEMPULSE_LOG_ERROR("file: to_flags. Unkown File::mode. Force flag 0");
	return 0;
}

File::File() noexcept : m_fd(-1) {
	MEMPULSE_LOG_TRACE();
}

File::File(File&& file) noexcept : m_fd(file.m_fd) {
	MEMPULSE_LOG_TRACE();

	file.m_fd = -1;
}

File::File(const char* path, File::Mode mode):
 m_fd(open(path, to_flags(mode))) {
	MEMPULSE_LOG_TRACE();

	if (m_fd < 0)
		throw ErrorDrm("can't open file " + std::string(path));
}

File::File(const File& file) noexcept {
	MEMPULSE_LOG_TRACE();

	if (file.m_fd >= 0)
		m_fd = dup(file.m_fd);
	else
		m_fd = -1;
}

File& File::operator=(const File& file) noexcept {
	MEMPULSE_LOG_TRACE();

	if (this == &file)
		return *this;

	Close();

	if (file.m_fd >= 0)
		m_fd = dup(file.m_fd);

	return *this;
}

File& File::operator=(File&& file) noexcept {
	MEMPULSE_LOG_TRACE();

	if (this == &file)
		return *this;

	m_fd = file.m_fd;
	file.m_fd = -1;

	return *this;
}

File::~File() noexcept {
	MEMPULSE_LOG_TRACE();

	Close();
}

bool File::Open(const char* path, Mode mode) noexcept {
	MEMPULSE_LOG_TRACE();

	m_fd = ::open(path, to_flags(mode));
	return m_fd >= 0;
}

void File::Close() noexcept {
	MEMPULSE_LOG_TRACE();

	if (m_fd < 0)
		return;

	close(m_fd);
	m_fd = -1;
}

}
