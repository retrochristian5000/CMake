/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmSourceGroup_h
#define cmSourceGroup_h

#include "cmConfigure.h" // IWYU pragma: keep

#include <memory>
#include <string>
#include <vector>

class cmSourceFile;

/** \class cmSourceGroup
 * \brief Hold a group of sources as specified by a SOURCE_GROUP command.
 *
 * cmSourceGroup holds a regular expression and a list of files.  When
 * local generators are about to generate the rules for a target's
 * files, the set of source groups is consulted to group files
 * together.  A file is placed into the last source group that lists
 * the file by name.  If no group lists the file, it is placed into
 * the last group whose regex matches it.
 */
class cmSourceGroup
{
public:
  cmSourceGroup(std::string name, const char* regex,
                const char* parentName = nullptr);
  cmSourceGroup(cmSourceGroup const&);
  cmSourceGroup(cmSourceGroup&&) noexcept;
  ~cmSourceGroup();

  cmSourceGroup& operator=(cmSourceGroup const&);
  cmSourceGroup& operator=(cmSourceGroup&&) noexcept;

  /**
   * Set the regular expression for this group.
   */
  void SetGroupRegex(const char* regex);

  /**
   * Add a file name to the explicit list of files for this group.
   */
  void AddGroupFile(std::string const& name);

  /**
   * Add child to this sourcegroup
   */
  void AddChild(cmSourceGroup const& child);

  /**
   * Looks up child and returns it
   */
  cmSourceGroup* LookupChild(std::string const& name);

  /**
   * Get the name of this group.
   */
  std::string const& GetName() const;

  /**
   * Get the full path name for group.
   */
  std::string const& GetFullName() const;

  /**
   * Check if the given name matches this group's regex.
   */
  bool MatchesRegex(std::string const& name);

  /**
   * Check if the given name matches this group's explicit file list.
   */
  bool MatchesFiles(std::string const& name) const;

  /**
   * Check if the given name matches this group's explicit file list
   * in children.
   */
  cmSourceGroup* MatchChildrenFiles(std::string const& name);

  /**
   * Check if the given name matches this group's regex in children.
   */
  cmSourceGroup* MatchChildrenRegex(std::string const& name);

  /**
   * Assign the given source file to this group.  Used only by
   * generators.
   */
  void AssignSource(cmSourceFile const* sf);

  /**
   * Get the list of the source files that have been assigned to this
   * source group.
   */
  std::vector<cmSourceFile const*> const& GetSourceFiles() const;

  /**
   * Get the list of the children source groups.
   */
  std::vector<cmSourceGroup> const& GetGroupChildren() const;

private:
  class Internal;
  std::unique_ptr<Internal> Impl;
};

#endif
