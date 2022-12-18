#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <sstream>

namespace LotusLib {
	namespace FileEntries {
		// Forward Declaration
		class DirNode;

		class FileNode {
			std::string m_name;
			DirNode* m_parentDir;
			int64_t m_cacheOffset;
			int64_t m_timeStamp;
			int32_t m_compLen;
			int32_t m_len;
			int m_tocOffset;

		public:
			FileNode();
			FileNode(std::string name, DirNode* parentDir, int64_t offset, int64_t timeStamp, int32_t compressedLength, int32_t length, int tocOffset);

			void setData(std::string name, DirNode* parentDir, int64_t offset, int64_t timeStamp, int32_t compressedLength, int32_t length, int toOffset);

			const std::string& getName() const;
			const DirNode* getParent() const;
			int32_t getLen() const;
			int32_t getCompLen() const;
			int64_t getOffset() const;
			int64_t getTimeStamp() const;
			int getTocOffset() const;

			std::string getFullPath() const;
		};
	}	
}
