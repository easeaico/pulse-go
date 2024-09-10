/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/utils/FileUtils.h"
#include <iterator>
#include <chrono>
#include <thread>

   // We haven't checked which filesystem to include yet
#ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL

// Check for feature test macro for <filesystem>
#   if defined(__cpp_lib_filesystem)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0

// Check for feature test macro for <experimental/filesystem>
#   elif defined(__cpp_lib_experimental_filesystem)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// We can't check if headers exist...
// Let's assume experimental to be safe
#   elif !defined(__has_include)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// Check if the header "<filesystem>" exists
#   elif __has_include(<filesystem>)

// If we're compiling on Visual Studio and are not compiling with C++17, we need to use experimental
#       ifdef _MSC_VER

// Check and include header that defines "_HAS_CXX17"
#           if __has_include(<yvals_core.h>)
#               include <yvals_core.h>

// Check for enabled C++17 support
#               if defined(_HAS_CXX17) && _HAS_CXX17
// We're using C++17, so let's use the normal version
#                   define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#               endif
#           endif

// If the marco isn't defined yet, that means any of the other VS specific checks failed, so we need to use experimental
#           ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
#               define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
#           endif

// Not on Visual Studio. Let's use the normal version
#       else // #ifdef _MSC_VER
#           define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#       endif

// Check if the header "<filesystem>" exists
#   elif __has_include(<experimental/filesystem>)
#       define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// Fail if neither header is available with a nice error message
#   else
#       error Could not find system header "<filesystem>" or "<experimental/filesystem>"
#   endif

// We priously determined that we need the exprimental version
#   if INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
// Include it
#       include <experimental/filesystem>

// We need the alias from std::experimental::filesystem to std::filesystem
namespace std {
  namespace filesystem = experimental::filesystem;
}

// We have a decent compiler and can use the normal version
#   else
// Include it
#       include <filesystem>
#   endif

#endif // #ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL

std::string Replace(const std::string& original, const std::string& replace, const std::string& withThis)
{
  size_t idx = 0;
  std::string s = original;
  while (true)
  {
    idx = s.find(replace);
    if (idx != std::string::npos)
    {
      s.erase(idx, replace.length());
      s.insert(idx, withThis);
    }
    else
      break;
  }
  return s;
}

void TrimFront(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
  {
    return !std::isspace(ch);
  }));
}
void TrimBack(std::string& s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
  {
    return !std::isspace(ch);
  }).base(), s.end());
}
void Trim(std::string& s)
{
  TrimFront(s);
  TrimBack(s);
}

bool CreatePath(const std::string& path)
{
  if (path.empty())
  {
    return false;
  }

  // Recursively create parents
  auto const sep = path.find_last_of("\\/");
  if (sep != std::string::npos && sep > 0)
  {
    if (!CreatePath(path.substr(0, sep)))
    {
      return false;
    }
  }

  MakeDirectory(path.c_str());
  return true;
}

bool CreateFilePath(const std::string& filenamePath)
{
  // Separate path from file, and create the path
  auto const sep = filenamePath.find_last_of("\\/");
  bool result = true;
  if (sep != std::string::npos && sep > 0)
  {
    std::error_code e;
    std::string dirs = filenamePath.substr(0, sep);
    if (!std::filesystem::exists(dirs))
    {
      // Sometimes create_directories returns false even when it makes it
      // Maybe only when there is a ending / in the dirs?
      std::filesystem::create_directories(dirs, e);
      result = std::filesystem::exists(dirs);
    }
  }
  return result; // Nothing to do...
}

bool WriteFile(const std::string& content, const std::string& filename)
{
  if (!CreateFilePath(filename))
    return false;
  std::ofstream ascii_ostream(filename, std::ios::out | std::ios::trunc);
  ascii_ostream << content;
  ascii_ostream.flush();
  ascii_ostream.close();
  return true;
}

bool ReadFile(const std::string& filename, std::string& content)
{
  if (!FileExists(filename))
    return false;

  std::ifstream input(filename);
  if (!input.is_open())
    return false;
  std::string buff((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  input.close();
  content = buff;
  return true;
}

bool IsDirectory(const std::string& dir)
{
  return std::filesystem::is_directory(dir);
}

bool IsRelativePath(const std::string& path)
{
  std::filesystem::path p(path);
  return p.is_relative();
}

void ListFiles(const std::string& dir, std::vector<std::string>& files, bool recursive, const std::string& mask, const std::string& exclusion)
{
  std::string filename;
  if (recursive)
  {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir))
    {
      if (std::filesystem::exists(entry.status()) && std::filesystem::is_regular_file(entry.status()))
      {
        filename = entry.path().string();
        std::replace(filename.begin(), filename.end(), '\\', '/');
        if (filename.find(mask) != std::string::npos)
        {
          if (exclusion.empty() || filename.find(exclusion) == std::string::npos)
            files.push_back(filename);
        }
      }

    }
  }
  else
  {
    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
      if (std::filesystem::exists(entry.status()) && std::filesystem::is_regular_file(entry.status()))
      {
        filename = entry.path().string();
        std::replace(filename.begin(), filename.end(), '\\', '/');
        if (filename.find(mask) != std::string::npos)
        {
          if (exclusion.empty() || filename.find(exclusion) == std::string::npos)
            files.push_back(filename);
        }
      }
    }
  }
}

void MakeDirectory(std::string const& dir)
{
  std::filesystem::create_directory(dir);
}

bool DeleteFile(const std::string &dir, short retry)
{
  if (std::remove(dir.c_str()) != 0)
  {
    for (int r=0; r<retry; r++)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      if (std::remove(dir.c_str()) == 0)
        return true;
    }
    return false;
  }
  return true;
}
bool DeleteDirectory(const std::string& dir)
{
  return std::filesystem::remove_all(dir);
}

std::string GetCurrentWorkingDirectory()
{
  return std::filesystem::current_path().string();
}

bool FileExists(const std::string& filename)
{
  return std::filesystem::exists(filename);
}

bool FindFileInFilePath(const std::string& filepath, const std::string& find, std::string& found)
{
  std::string filepathf = filepath;
  if (filepathf.back() != '/')
    filepathf += "/";

  // Treat empty string as current working directory
  if (filepathf.empty())
    filepathf = "./";

  if (!IsRelativePath(find))
  {
    if (FileExists(find))
    {
      found = find;
      return true;
    }
    return false;
  }

  std::string path;
  SplitPath(filepathf, path);

  while (!path.empty())
  {
    std::string possFilePath = path + find;
    if (FileExists(possFilePath))
    {
      found = possFilePath;
      return true;
    }

    size_t slash;
    if (path.length() < 2 || (slash = path.find_last_of("/", path.length() - 2)) == std::string::npos)
      break;
    path = path.substr(0, ++slash);
  }

  return false;
}

bool IsJSONFile(const std::string& filename)
{
  size_t ext = filename.find_last_of(".");
  if (ext == std::string::npos)
    return false;
  return filename.substr(ext) == ".json";
}

std::string CDM_DECL RelativePathFrom(const std::string& dir, const std::string& filepath)
{
  std::string dirf = dir;
  std::replace(dirf.begin(), dirf.end(), '\\', '/');

  std::string filepathf = filepath;
  std::replace(filepathf.begin(), filepathf.end(), '\\', '/');

  if (filepathf.rfind(dirf,0) != std::string::npos)
  {
    auto slash = filepathf.find_last_of("/");
    return filepathf.substr(dir.length(),slash-dir.length());
  }
  return "";
}

std::uintmax_t CDM_DECL FileSize(const std::string& filename)
{
  std::filesystem::path fp = filename;
  return std::filesystem::file_size(fp);
}

void SplitPath(const std::string& filepath, std::string& path)
{
  std::string filename;
  std::string ext;
  SplitPathFilenameExt(filepath, path, filename, ext);
}
void SplitFilename(const std::string& filepath, std::string& filename)
{
  std::string path;
  std::string ext;
  SplitPathFilenameExt(filepath, path, filename, ext);
  filename = filename + ext;
}
void SplitPathFilename(const std::string& filepath, std::string& path, std::string& filename)
{
  std::string ext;
  SplitPathFilenameExt(filepath, path, filename, ext);
  filename = filename + ext;
}
void SplitFilenameExt(const std::string& filepath, std::string& filename, std::string& ext)
{
  std::string path;
  SplitPathFilenameExt(filepath, path, filename, ext);
}
void SplitPathFilenameExt(const std::string& filepath, std::string& path, std::string& filename, std::string& ext)
{
  std::string filepathf = filepath;
  std::replace(filepathf.begin(), filepathf.end(), '\\', '/');

  // Get the filename
  auto slash = filepathf.find_last_of("/");
  (slash == std::string::npos) ?
    slash = 0 : slash++;
  filename = filepathf.substr(slash);

  // Split off the ext from the filename
  auto ePos = filename.find_last_of(".");
  if (ePos != std::string::npos)
  {
    ext = filename.substr(ePos);
    filename = filename.substr(0, ePos);
  }
  else
  {
    // No extension...
    ext = "";
  }

  // Get the path
  if (slash == 0)
    path = "./";
  else
    path = filepathf.substr(0, slash);
}
