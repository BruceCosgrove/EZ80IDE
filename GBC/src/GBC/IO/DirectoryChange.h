#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <thread>

namespace gbc::DirectoryChange
{
	using NotificationType = uint8_t;
	enum NotificationType_
	{
		NotificationType_FileNameChanged      = 1 << 0, // Renaming, creating, or deleting a file.
		NotificationType_DirectoryNameChanged = 1 << 1, // Renaming, creating, or deleting a directory.
		NotificationType_AttributesChanged    = 1 << 2, // Changing the attributes of a file or directory.
		NotificationType_SizeChanged          = 1 << 3, // Changing the size of a file or directory.
		NotificationType_LastWriteChanged     = 1 << 4, // Writing to a file or directory
		NotificationType_SecurityChanged      = 1 << 5, // Changing the security of a file or directory.
		NotificationType_NameChanged = NotificationType_FileNameChanged | NotificationType_DirectoryNameChanged,
		NotificationType_Any = NotificationType_FileNameChanged | NotificationType_DirectoryNameChanged
								| NotificationType_AttributesChanged | NotificationType_SizeChanged
								| NotificationType_LastWriteChanged | NotificationType_SecurityChanged
	};

	// Parameter: if everything went smoothly and a notification was received
	//	if false, an error occurred and this notifier will close.
	// Return value: if you want to continue waiting for notifications
	using NotificationFunc = std::function<bool(bool)>;

	class Notifier
	{
	public:
		Notifier(const NotificationFunc& notificationFunc, const std::filesystem::path& directoryPath, NotificationType notificationType, bool checkSubdirectories);
		Notifier() noexcept = default;
		Notifier(Notifier&& notifier) noexcept;
		Notifier& operator=(Notifier&& notifier) noexcept;
		~Notifier();

		void Remove();
	private:
		std::thread thread;
		void* closeHandle = nullptr;
	};
}
