//   This file is part of the guslib library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2014  Augustin Preda (thegusty999@gmail.com)
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//
//   Last change:  $LastChangedDate: 2013-06-20 23:19:27 +0200 (J, 20 iun. 2013) $
//   Revision:    $Revision: 574 $

//
// Includes
//

//
// Own header
//
#include "guslib/util/filehelper.h"

//
// C system headers.
//

// Platform specific definitions and overall build options for the library.
#include <guslib/guslibbuildopts.h>

#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
// Allow using the rmdir function
#  include <direct.h>
#elif GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_LINUX
#  include <unistd.h>
#endif

// Allow using the stat function.
#include <sys/stat.h>

//
// C++ system headers
//

// Allow using the std::replace function.
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

//
// Other and this library's headers
//

// A simple exception class.
#include "guslib/common/simpleexception.h"
// Directory entity listing (3rd party lib).
#include "guslib/thirdparty/dirent.h"


namespace guslib
{
  namespace filehelper
  {
    /**
      Copy one file to another.
      @param sourceFile       The name of the file that is copied.
      @param destinationFile  The name of the copy.
      @return True if successful, false otherwise.
    */
    bool CopyBinaryFile(const std::string& sourceFile, const std::string& destinationFile)
    {
      try
      {
        std::ifstream  src(sourceFile.c_str(),       std::ios::binary);
        std::ofstream  dst(destinationFile.c_str(),  std::ios::binary);

        dst << src.rdbuf();
        return true;
      }
      catch (const std::exception&)
      {
        return false;
      }
    }

    /**
      Creates a directory.
    */
    bool CreateDirectoryByName(const std::string& directoryName)
    {
      // The contents of the directory were deleted. Now delete the directory itself.
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
      if (_mkdir(directoryName.c_str()) != -1)
      {
        return true;
      }
#else
      // Modes for mkdir:
      // S_IRWXU 00700 mask for file owner permissions
      // S_IRUSR 00400 owner has read permission
      // S_IWUSR 00200 owner has write permission
      // S_IXUSR 00100 owner has execute permission
      // S_IRWXG 00070 mask for group permissions
      // S_IRGRP 00040 group has read permission
      // S_IWGRP 00020 group has write permission
      // S_IXGRP 00010 group has execute permission
      // S_IRWXO 00007 mask for permissions for others (not in group)
      // S_IROTH 00004 others have read permission
      // S_IWOTH 00002 others have write permisson
      // S_IXOTH 00001 others have execute permission
      if (mkdir(directoryName.c_str(),0777) != -1)
      {
        return true;
      }
#endif  // GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS

      return false;
    }

    /**
      Deletes a directory.
    */
    bool DeleteDirectory(const std::string& directoryName, bool include_sub_directories)
    {
      DIR *dir;

      // open the directory
      dir = opendir(directoryName.c_str());
      if (NULL == dir)
      {
        return false;
      }

      // Traverse the directory: this may be needed for handling recursive calls.
      struct dirent *ent;
      while ((ent = readdir (dir)) != NULL)
      {
        // Decide what to do with the directory entry
        switch (ent->d_type)
        {
        case DT_REG:
          {
            // DeleteFile
            std::string full_file_name(directoryName);
            full_file_name = GetNameWithPlatformTrailingSlash(full_file_name, true);
            full_file_name.append(ent->d_name);

            // It's a file. Try to remove it.
            int file_deletion_result = std::remove(full_file_name.c_str());
            if (file_deletion_result != 0)
            {
              // some error was encountered.
              closedir(dir);
              return false;
            }
          }
          break;

        case DT_DIR:
          if (include_sub_directories)
          {
            std::string sub_directoryName(ent->d_name);

            if (sub_directoryName != "." && sub_directoryName != "..")
            {
              std::string full_path_sub_dir(directoryName);
              full_path_sub_dir = GetNameWithPlatformTrailingSlash(full_path_sub_dir, true);
              full_path_sub_dir.append(sub_directoryName);

              // Call the function recursively
              if (!DeleteDirectory(full_path_sub_dir, true))
              {
                // failed to delete a subdirectory. Abort
                closedir(dir);
                return false;
              }
            }
          }
          // Could scan sub-directory recursively .. but do nothing
          break;

        default:
          // device entries
          // NOP
          break;
        }
      }
      closedir(dir);

      // The contents of the directory were deleted. Now delete the directory itself.
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
      if (_rmdir(directoryName.c_str()) != -1)
      {
        return true;
      }
      else
      {
        return false;
      }
#else
      if (rmdir(directoryName.c_str()) != -1)
      {
        return true;
      }
      else
      {
        return false;
      }
#endif  // GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS

      return true;
    }

    /**
      Deletes a file.
    */
    bool DeleteFileByName(const std::string& file_name)
    {
      int file_deletion_result = std::remove(file_name.c_str());
      if (file_deletion_result != 0)
      {
        // some error was encountered.
        return false;
      }

      return true;
    }

   

   /**
      Get file contents and store them into a string.
      If the file does not exist, an exception is thrown.
    */
    std::string GetFileContents(const std::string& fileName)
    {
      if (IsFileAccessible(fileName) != true)
      {
        throw guslib::SimpleException("Exception: file not found");
      }

      std::string contents("");

      std::ifstream in(fileName.c_str(), std::ios::binary);
      std::string line;
      if (in)
      {
        while (getline(in, line))
        {
          contents.append(line);
          contents.append("\n");
        }
        in.close();
      }
      return contents;
    }

    /**
      Get the name of the available WinCC OA managers
      @param folderList The list of folders to verify.
      @param mandatoryFilters Store the mandatory filter entries. Files must match one of these substrings to be
      included in the results.
      @param forbiddenFilters Store the denied filter entries. Files must not match any of these substrings to be
      included in the results.
    */
    std::vector <std::string> GetListOfMatchingFileNames(
      const std::vector <std::string> folder_list,
      const std::vector <std::string> mandatory_filters,
      const std::vector <std::string> forbidden_filters)
    {
      // Store the results here
      std::vector <std::string> result;
      std::string file_entry;

      // Run through all given folders.
      for (std::vector <std::string>::const_iterator folder_item = folder_list.begin();
        folder_item != folder_list.end(); ++folder_item)
      {
        // This is the dirent (directory entity) structure.
        DIR *dir;

        // open the directory
        dir = opendir(folder_item->c_str());

        if (0 != dir)
        {
          struct dirent *ent;

          // Read the data from the directory.
          while ((ent = readdir(dir)) != NULL)
          {
            // Decide what to do with the directory entry
            switch (ent->d_type)
            {
            case DT_REG:
              // Add the file to the list of candidates
              file_entry = std::string(ent->d_name);

              // Test the entry against the requirements:
              // - It must match the mandatory filters.
              // - It must NOT match any of the forbidden filters.
              if (SubstringListMatch(file_entry, mandatory_filters, true, true))
              {
                if (SubstringListMatch(file_entry, forbidden_filters, false, false))
                {
                  result.push_back(file_entry);
                }
              }

              break;

            case DT_DIR:
              // Could scan sub-directory recursively .. but do nothing
              break;

            default:
              // device entries
              // NOP
              break;
            }
          }

          // Don't forget to close the directory.
          closedir(dir);
        }
        else
        {
          // Folder could not be opened.
          // Either create a log message or change the last error message, but don't throw or break.
        }
      }

      return result;
    }

    /**
      Test whether a file is accessible on disk.
      @param fileName The name of the file to test for.
      @return true if the file is accessible, false otherwise.
    */
    bool IsFileAccessible(const std::string& fileName)
    {
      struct stat buffer;
      return (stat (fileName.c_str (), &buffer) == 0);
    }


    /**
      Ensure that the given path contains a trailing slash or backslash char.
    */
    std::string GetNameWithPlatformTrailingSlash(const std::string& name, bool with_slash)
    {
      if (name.length() <= 0)
      {
        return name;
      }

      char expected_character;
      char item_to_correct;
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
      expected_character = '\\';
      item_to_correct = '/';
#else
      expected_character = '/';
      item_to_correct = '\\';
#endif

      // Find out if we have the last char already.
      // An elegant way: dereference the reverse iterator. C++11 has support for back().

      const char last_character = *name.rbegin();
      if (last_character == expected_character || last_character == item_to_correct)
      {
        if (with_slash)
        {
          std::string result(name);
          std::replace(result.end() - 1, result.end(), item_to_correct, expected_character);
          return result;
        }
        else
        {
          // remove last char
          std::string result(name);
          if (name.length() > 1)
          {
            result = name.substr(0, name.length() - 1);
          }
          else
          {
            result = "";
          }
          return result;
        }
      }
      else
      {
        if (with_slash)
        {
          std::string result(name);
          result.push_back(expected_character);
          return result;
        }
        else
        {
          return name;
        }
      }

      // This should not be reached. Return the initial string unmodified.
      return name;
    }

    /**
      Ensure that all the separators in the specified file or folder name are using the same separator.
      @param fileFolderName The file or folder to bring to a normal form.
      @param useForwardSlash Specify whether a forward slash should be used 
                             as a separator (true), or a backslash (false).
      @return The parsed string.
    */
    std::string NormalizeFileOrFolderName(const std::string& fileFolderName, bool useForwardSlash)
    {
      std::string result(fileFolderName);

      char oldChar = '/';
      char newChar = '\\';

      if (useForwardSlash)
      {
        oldChar = '\\';
        newChar = '/';
      }

      // Replace the single instance.
      std::replace(result.begin(), result.end(), oldChar, newChar);

      // Get rid of the duplicates.
      // E.g. "C:/projects/my game/data/..//helpers.txt" => "C:/projects/my game/data/../helpers.txt"

      // Prepare the double instance.
      std::string doubleSeparator;
      doubleSeparator += newChar;
      doubleSeparator += newChar;

      std::string singleSeparator;
      singleSeparator += newChar;

      size_t foundPosition = 0;
      foundPosition = result.find(doubleSeparator, 0);

      while (foundPosition != std::string::npos)
      {
        result.replace(foundPosition, doubleSeparator.length(), singleSeparator);
        foundPosition = result.find(doubleSeparator, 0);
      }

      return result;
    }


    /**
      Ensure that a path contains a trailing separator (forward or back-slash character), or not, 
      based on the given parameters.
      @param directoryName The name of the directory to ensure for.
      @param useForwardSlash Specify Specify whether a forward slash should be used as a separator
                             (true), or a backslash (false).
      @param useTrailingSeparator If set to true, the path is returned with a trailing separator. 
                                  Otherwise, the path is returned without one.
      @return The parsed string, containing a trailing separator.
    */
    std::string EnsureTrailingSeparator(
        const std::string& directoryName,
        bool useForwardSlash,
        bool useTrailingSeparator)
    {
      const char slashFwd('/');
      const char slashBck('\\');

      // Find out if we have the last char already.
      // An elegant way: dereference the reverse iterator. C++11 has support for back().

      char lastChar = *directoryName.rbegin();
      if (lastChar == slashBck || lastChar == slashFwd)
      {
        // We already have a forward slash or a backslash character.

        if (useTrailingSeparator)
        {
          // Check to see if we have the same character.
          if (useForwardSlash)
          {
            // The user wants to use '/' as a separator.
            if (lastChar == slashFwd)
            {
              // Everything is fine.
              return directoryName;
            }
            else
            {
              // Replace the last char from '\' to '/'
              std::string result(directoryName);
              if (directoryName.length() > 1)
              {
                result.replace(result.length() - 1, 1, "/");
              }
              else
              {
                result = "/";
              }
              return result;
            }
          }
          else
          {
            // The user wants to use '\' as a separator.
            if (lastChar == slashBck)
            {
              // Everything is fine.
              return directoryName;
            }
            else
            {
              // Replace the last char from '/' to '\\'
              std::string result(directoryName);
              if (directoryName.length() > 1)
              {
                result.replace(result.length() - 1, 1, "\\");
              }
              else
              {
                result = "\\";
              }
              return result;
            }
          }
        }
        else
        {
          // We should have no trailing separator. Remove it.
          std::string result(directoryName);
          if (directoryName.length() > 1)
          {
            result = directoryName.substr(0, directoryName.length() - 1);
          }
          else
          {
            result = "";
          }
          return result;
        }
      }
      else
      {
        // We don't have a slash or backslash character at the end... so... just add one?
        if (useTrailingSeparator)
        {
          std::string result(directoryName);
          if (useForwardSlash)
          {
            result += '/';
          }
          else
          {
            result += '\\';
          }
          return result;
        }
        else
        {
          return directoryName;
        }
      }

      // This should not be reached. Return the initial string unmodified.
      return directoryName;
    }


    /**
      Remove the last folder from a folder name.
    */
    std::string RemoveLastFolderFromName(const std::string& name, bool useTrailingSeparator)
    {
      std::string result(name);

      // Get rid of any trailing item first.
      result = EnsureTrailingSeparator(result, true, false);

      // Now get rid of the next segment.
      size_t position = result.find_last_of("\\/");
      if (position != std::string::npos)
      {
        result = result.substr(0, position);
      }

      // Add the new trailing item, as needed.
      result = EnsureTrailingSeparator(result, true, useTrailingSeparator);

      return result;
    }


    /**
      Verify whether a string (content) has any other substrings in it (matchlist)
      @param content The content.
      @param matchingList The vector of substrings to test for
      @param mustMatch Specify whether the matchlist should be found (at least one item) or not (at most 0 items)
      @param emptyContentMeansMatch Specify whether an empty "matchingList" parameter means
             we treat it as we have a match or not.
      in the given content
    */
    bool SubstringListMatch(
      const std::string& content,
      const std::vector <std::string> matchingList,
      bool mustMatch,
      bool emptyContentMeansMatch)
    {
      bool foundMatch = false;

      if (matchingList.size() > 0)
      {
        for (std::vector <std::string>::const_iterator it = matchingList.begin();
          it != matchingList.end(); ++it)
        {
          if (std::string::npos != content.find(*it))
          {
            // match found
            foundMatch = true;
          }
        }
      }
      else
      {
        // empty match list.
        foundMatch = emptyContentMeansMatch;
      }

      // Return true if we have a match and needed a match
      // Also return true if we don't have a match and did not need one
      // Return false in other scenarios.

      return (mustMatch == foundMatch);
    }
  }
}
