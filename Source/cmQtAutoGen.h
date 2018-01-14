/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmQtAutoGen_h
#define cmQtAutoGen_h

#include "cmConfigure.h" // IWYU pragma: keep

#include <string>
#include <vector>

/** \class cmQtAutoGen
 * \brief Class used as namespace for QtAutoGen related types and functions
 */
class cmQtAutoGen
{
public:
  static std::string const listSep;
  static const unsigned int parallelMax = 64;

  /// @brief AutoGen generator type
  enum Generator
  {
    GEN, // General
    MOC,
    UIC,
    RCC
  };

  /// @brief Multiconfiguration type
  enum MultiConfig
  {
    SINGLE, // Single configuration
    WRAP,   // Multi configuration using wrapper files
    FULL    // Full multi configuration using per config sources
  };

public:
  /// @brief Returns the generator name
  static std::string const& GeneratorName(Generator genType);
  /// @brief Returns the generator name in upper case
  static std::string GeneratorNameUpper(Generator genType);

  /// @brief Returns the multi configuration name string
  static std::string const& MultiConfigName(MultiConfig config);
  /// @brief Returns the multi configuration type
  static MultiConfig MultiConfigType(std::string const& name);

  /// @brief Returns a the string escaped and enclosed in quotes
  static std::string Quoted(std::string const& text);

  static std::string QuotedCommand(std::vector<std::string> const& command);

  /// @brief Returns the parent directory of the file with a "/" suffix
  static std::string SubDirPrefix(std::string const& filename);

  /// @brief Appends the suffix to the filename before the last dot
  static std::string AppendFilenameSuffix(std::string const& filename,
                                          std::string const& suffix);

  /// @brief Merges newOpts into baseOpts
  static void UicMergeOptions(std::vector<std::string>& baseOpts,
                              std::vector<std::string> const& newOpts,
                              bool isQt5);

  /// @brief Merges newOpts into baseOpts
  static void RccMergeOptions(std::vector<std::string>& baseOpts,
                              std::vector<std::string> const& newOpts,
                              bool isQt5);

  /// @brief Parses the content of a qrc file
  ///
  /// Use when rcc does not support the "--list" option
  static void RccListParseContent(std::string const& content,
                                  std::vector<std::string>& files);

  /// @brief Parses the output of the "rcc --list ..." command
  static bool RccListParseOutput(std::string const& rccStdOut,
                                 std::string const& rccStdErr,
                                 std::vector<std::string>& files,
                                 std::string& error);

  /// @brief Converts relative qrc entry paths to full paths
  static void RccListConvertFullPath(std::string const& qrcFileDir,
                                     std::vector<std::string>& files);
};

#endif
