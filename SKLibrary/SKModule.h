#include <string>

namespace SkeletonKey
{
	class SKModule
	{
	public:
		static std::wstring output;
		virtual std::wstring run(bool isAdmin, std::wstring userHome, std::wstring outputPath);
		void logLine(std::wstring line);
	};
}