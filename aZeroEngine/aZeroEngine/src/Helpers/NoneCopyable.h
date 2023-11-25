#pragma once

namespace aZero
{
	namespace Helpers
	{
		class NoneCopyable
		{
		private:
			NoneCopyable(const NoneCopyable&) = delete;
			NoneCopyable(NoneCopyable&&) = delete;
			NoneCopyable operator= (const NoneCopyable&) = delete;
			NoneCopyable operator= (NoneCopyable&&) = delete;
		public:
			NoneCopyable() = default;
		};
	}
}