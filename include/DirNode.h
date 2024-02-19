#pragma once

#include <string>
#include <vector>
#include <sstream>

#include "FileNode.h"

namespace LotusLib {
	namespace FileEntries {
		class DirNode {
			std::string m_name;
			DirNode* m_parentNode;
			int m_tocOffset;
			std::vector<DirNode*> m_childDirs;
			std::vector<FileNode*> m_childFiles;
			std::vector<FileNode*> m_childFileDupes;

		public:
			DirNode();
			DirNode(DirNode* parentNode, std::string name, int tocOffset);

			void setData(DirNode* parentNode, std::string name, int tocOffset);

			const std::string& getName() const;
			const DirNode* getParent() const;
			int getTocOffset() const;
			size_t getDirCount() const;
			size_t getFileCount() const;

			std::string getFullPath() const;
			std::stringstream getFullpathStream() const;

			const DirNode* getChildDir(int index) const;
			const DirNode* getChildDir(const std::string& name) const;
			const FileNode* getChildFile(int index) const;
			const FileNode* getChildFile(const std::string& name) const;

			const DirNode* findChildDir(const std::string& path, size_t start, size_t len) const;
			const FileNode* findChildFile(const std::string& path, size_t start, size_t len) const;

			void addChildDir(DirNode* node);
			void addChildFile(FileNode* node, bool isDupe = false);

		private:
			void getPathRecursive(std::stringstream& stream) const;
		};
	}
}
