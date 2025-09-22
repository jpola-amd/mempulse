#pragma once

namespace mempulse {
class File {
public:
	enum class Mode { ReadOnly, ReadWrite };

	File() noexcept;
	File(const File& file) noexcept;
	File(File&& file) noexcept;

	explicit File(const char* path, Mode mode = Mode::ReadOnly);

	virtual ~File() noexcept;

	[[nodiscard]]
	bool Open(const char* path, Mode mode = Mode::ReadOnly) noexcept;

	[[nodiscard]]
	bool IsOpen() const noexcept { return m_fd >= 0; }

	void Close() noexcept;

	operator int() const noexcept { return m_fd; }

	File& operator=(const File& file) noexcept;
	File& operator=(File&& file) noexcept;
private:
	int m_fd;
};
}
