#include "gbcpch.h"
#include "GBC/IO/DirectoryChange.h"

namespace gbc::DirectoryChange
{
	static constexpr DWORD allTypes = FILE_NOTIFY_CHANGE_FILE_NAME  | FILE_NOTIFY_CHANGE_DIR_NAME
									| FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE
									| FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY;
	static constexpr DWORD GetWindowsNotificationType(NotificationType notificationType)
	{
		DWORD type = 0;

		if (notificationType & NotificationType_FileNameChanged)      type |= FILE_NOTIFY_CHANGE_FILE_NAME;
		if (notificationType & NotificationType_DirectoryNameChanged) type |= FILE_NOTIFY_CHANGE_DIR_NAME;
		if (notificationType & NotificationType_AttributesChanged)    type |= FILE_NOTIFY_CHANGE_ATTRIBUTES;
		if (notificationType & NotificationType_SizeChanged)          type |= FILE_NOTIFY_CHANGE_SIZE;
		if (notificationType & NotificationType_LastWriteChanged)     type |= FILE_NOTIFY_CHANGE_LAST_WRITE;
		if (notificationType & NotificationType_SecurityChanged)      type |= FILE_NOTIFY_CHANGE_SECURITY;

		GBC_CORE_ASSERT((type & allTypes) != 0, "Unknown Directory Change Notification Type!");
		return type;
	}

	Notifier::Notifier(Notifier&& notifier) noexcept
		: thread(std::move(notifier.thread)), closeHandle(notifier.closeHandle)
	{
		notifier.closeHandle = nullptr;
	}

	Notifier& Notifier::operator=(Notifier&& notifier) noexcept
	{
		if (this != &notifier)
		{
			Remove();

			thread = std::move(notifier.thread);
			closeHandle = notifier.closeHandle;
			notifier.closeHandle = nullptr;
		}
		return *this;
	}

	Notifier::~Notifier()
	{
		Remove();
	}

	void Notifier::Remove()
	{
		if (closeHandle)
		{
			SetEvent(closeHandle);
			thread.join();
			closeHandle = nullptr;
		}
	}

	Notifier::Notifier(const NotificationFunc& notificationFunc, const std::filesystem::path& directoryPath, NotificationType notificationType, bool checkSubdirectories)
	{
		// Notes:
		//	length of "GBCDirectoryChange" is 18
		//	there are 14 total null terminators because that is the number of base 36 digits that 2^64 - 1 is.
		//	14th null terminator is added by the compiler
		static uint64_t id = 0;
		static wchar_t eventNameBuffer[] = L"GBCDirectoryChange\0\0\0\0\0\0\0\0\0\0\0\0\0";
		_ui64tow_s(++id, eventNameBuffer + 18, 14, 36);

		std::atomic_bool closeHandleSet = false;
		std::atomic_bool waiting = true;

		std::thread thread = std::thread([&](const NotificationFunc& notificationFunc)
		{
			HANDLE handles[2];

			// Create the remove event
			handles[0] = CreateEventW(NULL, FALSE, FALSE, eventNameBuffer);
			if (handles[0] == NULL)
			{
				// The remove event could not be created
				waiting = false;
				return;
			}

			// Create the directory notification
			DWORD windowsNotificationType = GetWindowsNotificationType(notificationType);
			std::filesystem::path absolutePath = std::filesystem::absolute(directoryPath);
			handles[1] = FindFirstChangeNotificationW(absolutePath.c_str(), checkSubdirectories, windowsNotificationType);
			if (handles[1] == INVALID_HANDLE_VALUE)
			{
				// The notification could not be created
				FindCloseChangeNotification(handles[0]);
				waiting = false;
				return;
			}

			closeHandle = handles[0];
			closeHandleSet = true;
			waiting = false;

			bool running = true;
			while (running)
			{
				switch (WaitForMultipleObjects(2, handles, FALSE, INFINITE))
				{
					case WAIT_OBJECT_0:
						// User requested to remove the notification
						running = false;
						break;
					case WAIT_OBJECT_0 + 1:
						running = notificationFunc(false);
						if (FindNextChangeNotification(handles[1]))
							break;
						[[fallthrough]];
					case WAIT_FAILED:
						// Inform user that an error occurred and this notification will now close
						(void)notificationFunc(true);
						running = false;
						break;
				}
			}

			FindCloseChangeNotification(handles[1]);
			FindCloseChangeNotification(handles[0]);
		}, notificationFunc);

		// Wait for this notification's closeHandle to be set or for an error to occur
		// This was getting "optimized" out in release and dist if waiting wasn't atomic.
		while (waiting);

		if (closeHandleSet)
			this->thread = std::move(thread);
		else
			thread.join();
	}
}
