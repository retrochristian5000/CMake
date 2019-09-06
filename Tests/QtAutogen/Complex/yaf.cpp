/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "yaf.h"
#include "yaf_p.h"

#include <stdio.h>

Yaf::Yaf()
{
}

void Yaf::doYaf()
{
  YafP yafP;
  yafP.doYafP();
}

#include "moc_yaf_p.cpp"
