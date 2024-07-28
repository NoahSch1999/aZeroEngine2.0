#pragma once
#include <functional>
#include <queue>
#include <mutex>

#include "NoneCopyable.h"

namespace aZero
{

	using CommandTask = std::function<void()>;

	class CommandProcessor : public Helpers::NoneCopyable
	{
	private:
		std::deque<CommandTask> m_Tasks;
		std::mutex m_Lock;

	public:
		CommandProcessor() = default;

		void Enqueue(CommandTask&& task)
		{
			std::unique_lock<std::mutex> tempLock(m_Lock);
			m_Tasks.push_back(std::forward<CommandTask>(task));
		}

		void ExecuteNext()
		{
			CommandTask callable;
			{
				std::unique_lock<std::mutex> tempLock(m_Lock);

				if (m_Tasks.empty())
				{
					return;
				}

				callable = m_Tasks.front();
				m_Tasks.pop_front();
			}
			callable();
		}

		void Flush()
		{
			std::unique_lock<std::mutex> tempLock(m_Lock);
			while (!m_Tasks.empty())
			{
				CommandTask callable = m_Tasks.front();
				m_Tasks.pop_front();
				callable();
			}
		}

		bool IsEmpty()
		{
			std::unique_lock<std::mutex> tempLock(m_Lock);
			return m_Tasks.empty();
		}

		void Clear()
		{
			std::unique_lock<std::mutex> tempLock(m_Lock);
			m_Tasks.clear();
		}
	};
}