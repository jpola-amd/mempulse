#include "File.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>

namespace mempulse {
	inline int to_flags(File::Mode mode) {
		switch (mode) {
			case File::Mode::ReadOnly:
				return O_RDONLY;
			case File::Mode::ReadWrite:
				return O_RDWR;
			default:
				return 0;
		}
		return 0;
	}

	File::File() noexcept : m_fd(-1) {
	}

	File::File(File&& file) noexcept : m_fd(file.m_fd) {
		file.m_fd = -1;
	}

	File::File(const char* path, File::Mode mode):
		m_fd(open(path, to_flags(mode))) {
		if (m_fd < 0)
			throw std::runtime_error("Could not open file");
	}

	File::File(const File& file) noexcept {
		if (file.m_fd >= 0)
			m_fd = dup(file.m_fd);
		else
			m_fd = -1;
	}

	File& File::operator=(const File& file) noexcept {
		if (this == &file)
			return *this;

		Close();

		if (file.m_fd >= 0)
			m_fd = dup(file.m_fd);

		return *this;
	}

	File& File::operator=(File&& file) noexcept {
		if (this == &file)
			return *this;

		m_fd = file.m_fd;
		file.m_fd = -1;

		return *this;
	}

	File::~File() noexcept {
		Close();
	}

	bool File::Open(const char* path, Mode mode) noexcept {
		m_fd = ::open(path, to_flags(mode));
		return m_fd >= 0;
	}

	void File::Close() noexcept {
		if (m_fd < 0)
			return;

		close(m_fd);
		m_fd = -1;
	}

}
