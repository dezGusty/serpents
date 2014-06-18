#ifndef GUS_LIB_FILE_HELPER_H
#define GUS_LIB_FILE_HELPER_H

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
//   File handling helper class.
//
//   Last change:  $LastChangedDate: 2013-06-20 23:19:27 +0200 (J, 20 iun. 2013) $
//   Revision:    $Revision: 574 $

//
// Includes
//

//
// Platform specific definitions and overall build options for the library.
//
#include <guslib/guslibbuildopts.h>

//
// C++ system headers
//

#include <map>
#include <string>
#include <vector>

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
    GUSLIB_EXPORT_SYMBOL bool CopyBinaryFile(const std::string& sourceFile, const std::string& destinationFile);

    /**
      Creates a directory.
    */
    GUSLIB_EXPORT_SYMBOL bool CreateDirectoryByName(const std::string& directoryName);

    /**
      Deletes a directory.
    */
    GUSLIB_EXPORT_SYMBOL bool DeleteDirectory(const std::string& directoryName, bool includeSubDirectories);

    /**
      Deletes a file.
    */
    GUSLIB_EXPORT_SYMBOL bool DeleteFileByName(const std::string& fileName);

    /**
      Get file contents and store them into a string.
      If the file does not exist, an exception is thrown.
    */
    GUSLIB_EXPORT_SYMBOL std::string GetFileContents(const std::string& fileName);

    /**
      Test whether a file is accessible on disk.
      @param fileName The name of the file to test for.
      @return true if the file is accessible, false otherwise.
    */
    GUSLIB_EXPORT_SYMBOL bool IsFileAccessible(const std::string& fileName);

    /**
      Get the name of the available WinCC OA managers
      @param folderList The list of folders to verify.
      @param mandatoryFilters Store the mandatory filter entries. Files must match one of these substrings to be
      included in the results.
      @param forbiddenFilters Store the denied filter entries. Files must not match any of these substrings to be
      included in the results.
    */
    GUSLIB_EXPORT_SYMBOL std::vector <std::string> GetListOfMatchingFileNames(
        const std::vector <std::string> folderList,
        const std::vector <std::string> mandatoryFilters,
        const std::vector <std::string> forbiddenFilters);

    /**
      Ensure that the given path contains a trailing slash or backslash char.
    */
    GUSLIB_EXPORT_SYMBOL std::string GetNameWithPlatformTrailingSlash(const std::string& name, bool with_slash = true);

    /**
      Ensure that all the separators in the specified file or folder name are using the same separator.
      @param fileFolderName The file or folder to bring to a normal form.
      @param useForwardSlash Specify whether a forward slash should be used as a separator (true), or a backslash (false).
      @return The parsed string.
    */
    GUSLIB_EXPORT_SYMBOL std::string NormalizeFileOrFolderName(
        const std::string& fileFolderName,
        bool useForwardSlash);

    /**
      Ensure that all the separators in the specified file or folder name are using the same separator.
      @param fileFolderName The file or folder to bring to a normal form.
      @param useForwardSlash Specify whether a forward slash should be used as a separator (true), or a backslash (false).
      @return The parsed string.
    */
    GUSLIB_EXPORT_SYMBOL std::string NormalizeFileOrFolderNameForCurrentPlatform(const std::string& fileFolderName);

    /**
      Ensure that a path contains a trailing separator (forward or back-slash character), or not, based on the given parameters.
      @param directoryName The name of the directory to ensure for.
      @param useForwardSlash Specify Specify whether a forward slash should be used as a separator (true), or a backslash (false).
      @param useTrailingSeparator If set to true, the path is returned with a trailing separator. Otherwise, the path is returned without one.
      @return The parsed string, containing a trailing separator.
    */
    GUSLIB_EXPORT_SYMBOL std::string EnsureTrailingSeparator(
        const std::string& directoryName,
        bool useForwardSlash,
        bool useTrailingSeparator);

    /**
      Remove the last folder from a folder name.
      @param name The name of the path to remove the folder from.
      @param keepTrailingSeparator Specify whether to use the trailing separator or not.
      @return The path without the last folder
    */
    GUSLIB_EXPORT_SYMBOL std::string RemoveLastFolderFromName(
        const std::string& name,
        bool keepTrailingSeparator);

    /**
      Verify whether a string (content) has any other substrings in it (matchlist)
      @param content The content.
      @param matchingList The vector of substrings to test for
      @param mustMatch Specify whether the matchlist should be found (at least one item) or not (at most 0 items)
      @param emptyContentMeansMatch Specify whether an empty "matchingList" parameter means
             we treat it as we have a match or not.
      in the given content
    */
    GUSLIB_EXPORT_SYMBOL bool SubstringListMatch(
        const std::string& content,
        const std::vector <std::string> matchingList,
        bool mustMatch = true,
        bool emptyContentMeansMatch = true);

    /**
      Transcopy a file (Copy with transformations).
      Copy the contents of source_file into dest_file, taking care in the course of this to apply the transformations.
    */
    GUSLIB_EXPORT_SYMBOL bool TranscopyFile(
        const std::string& source_file_name,
        const std::string& dest_file_name,
        std::map<std::string, std::string> transformations);

  }
}

#endif
