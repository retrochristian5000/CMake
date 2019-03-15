/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmFileTime_h
#define cmFileTime_h

#include "cmConfigure.h" // IWYU pragma: keep

#include <cstdint>
#include <string>

/** \class cmFileTime
 * \brief Abstract file time with support for comparison with other file times.
 *
 * The file time is stored in a signed 64bit integer that counts the
 * nanoseconds since the begin of a common epoch.
 */
class cmFileTime
{
public:
  // -- Types
  static constexpr std::int64_t ns_per_s = 1000000000;

public:
  /**
   * @brief Loads the file time of fileName from the file system
   * @return true on success
   */
  bool Load(std::string const& fileName);

  // -- Comparison in nanosecond resolution

  bool operator==(cmFileTime const& ftm) const { return this->NS == ftm.NS; }
  bool operator!=(cmFileTime const& ftm) const { return this->NS != ftm.NS; }
  bool operator>(cmFileTime const& ftm) const { return this->NS > ftm.NS; }
  bool operator>=(cmFileTime const& ftm) const { return this->NS >= ftm.NS; }
  bool operator<(cmFileTime const& ftm) const { return this->NS < ftm.NS; }
  bool operator<=(cmFileTime const& ftm) const { return this->NS <= ftm.NS; }

  /**
   * @brief Return true if this is older than ftm
   */
  bool Older(cmFileTime const& ftm) const { return ftm.NS > this->NS; }

  /**
   * @brief Return true if this is newer than ftm
   */
  bool Newer(cmFileTime const& ftm) const { return this->NS > ftm.NS; }

  /**
   * @brief Return true if this is the same as ftm
   */
  bool Equal(cmFileTime const& ftm) const { return this->NS == ftm.NS; }

  /**
   * @brief Return true if this is not the same as ftm
   */
  bool Differ(cmFileTime const& ftm) const { return this->NS != ftm.NS; }

  /**
   * @brief Compare file modification times.
   * @return -1, 0, +1 for this older, same, or newer than ftm.
   */
  int Compare(cmFileTime const& ftm)
  {
    if (this->Older(ftm)) {
      return -1;
    }
    return this->Newer(ftm) ? 1 : 0;
  }

  // -- Comparison in second resolution

  /**
   * @brief Return true if this is at least a second older than ftm
   */
  bool OlderS(cmFileTime const& ftm) const
  {
    return (ftm.NS > this->NS) &&
      ((ftm.NS - this->NS) >= cmFileTime::ns_per_s);
  }

  /**
   * @brief Return true if this is at least a second newer than ftm
   */
  bool NewerS(cmFileTime const& ftm) const
  {
    return (this->NS > ftm.NS) &&
      ((this->NS - ftm.NS) >= cmFileTime::ns_per_s);
  }

  /**
   * @brief Return true if this is within the same second as ftm
   */
  bool EqualS(cmFileTime const& ftm) const
  {
    return !this->OlderS(ftm) && !this->NewerS(ftm);
  }

  /**
   * @brief Return true if this is older or newer than ftm by at least a second
   */
  bool DifferS(cmFileTime const& ftm) const
  {
    return this->OlderS(ftm) || this->NewerS(ftm);
  }

  /**
   * @brief Compare file modification times.
   * @return -1: this at least a second older, 0: this within the same second
   *         as ftm, +1: this at least a second newer than ftm.
   */
  int CompareS(cmFileTime const& ftm) const
  {
    if (this->OlderS(ftm)) {
      return -1;
    }
    return this->NewerS(ftm) ? 1 : 0;
  }

  /**
   * @brief The file time in nanoseconds
   */
  std::int64_t GetNS() const { return NS; }

private:
  std::int64_t NS = 0;
};

#endif
