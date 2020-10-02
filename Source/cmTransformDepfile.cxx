/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmTransformDepfile.h"

#include <string>

#include <cm/optional>

#include "cmsys/FStream.hxx"

#include "cmGccDepfileReader.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"

namespace {
void WriteFilenameGcc(cmsys::ofstream& fout, const std::string& filename)
{
  for (auto c : filename) {
    switch (c) {
      case ' ':
        fout << "\\ ";
        break;
      case '\\':
        fout << "\\\\";
        break;
      default:
        fout << c;
        break;
    }
  }
}

void WriteGccDepfile(cmsys::ofstream& fout, const cmGccDepfileContent& content)
{
  for (auto const& dep : content) {
    bool first = true;
    for (auto const& rule : dep.rules) {
      if (!first) {
        fout << " \\\n  ";
      }
      first = false;
      WriteFilenameGcc(fout, rule);
    }
    fout << ':';
    for (auto const& path : dep.paths) {
      fout << " \\\n  " << path;
    }
    fout << '\n';
  }
}

std::string GetTlogPath(const std::string& baseDir,
                        const std::string& filename)
{
  std::string f = filename;
  if (!cmSystemTools::FileIsFullPath(f)) {
    f = cmStrCat(baseDir, '/', filename);
  }
  return cmSystemTools::ConvertToOutputPath(
    cmSystemTools::CollapseFullPath(f));
}

void WriteVsTlog(const std::string& baseDir, cmsys::ofstream& fout,
                 const cmGccDepfileContent& content)
{
  for (auto const& dep : content) {
    fout << '^';
    bool first = true;
    for (auto const& rule : dep.rules) {
      if (!first) {
        fout << '|';
      }
      fout << GetTlogPath(baseDir, rule);
    }
    fout << "\r\n";
    for (auto const& path : dep.paths) {
      fout << GetTlogPath(baseDir, path) << "\r\n";
    }
  }
}
}

bool cmTransformDepfile(const std::string& baseDir, cmDepfileFormat format,
                        const std::string& infile, const std::string& outfile)
{
  cmGccDepfileContent content;
  if (cmSystemTools::FileExists(infile)) {
    auto result = cmReadGccDepfile(infile.c_str());
    if (!result) {
      return false;
    }
    content = *std::move(result);
  }
  cmsys::ofstream fout(outfile.c_str());
  if (!fout) {
    return false;
  }
  switch (format) {
    case cmDepfileFormat::GccDepfile:
      WriteGccDepfile(fout, content);
      break;
    case cmDepfileFormat::VsTlog:
      WriteVsTlog(baseDir, fout, content);
      break;
  }
  return true;
}
