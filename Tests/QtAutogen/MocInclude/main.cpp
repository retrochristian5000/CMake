#include "ExternDot.hpp"
#include "ExternDotGenerated.hpp"
#include "None.hpp"
#include "OtherUnderscore.hpp"
#include "OtherUnderscoreSub.hpp"
#include "OwnDot.hpp"
#include "OwnDotUnderscore.hpp"
#include "OwnUnderscore.hpp"
#include "SingleSub/SubOwnDot.hpp"

int main(int argv, char** args)
{
  None objNone;
  OwnUnderscore objOwnUnderscore;
  OwnDot objOwnDot;
  OwnDotUnderscore objOwnDotUnderscore;

  OtherUnderscore objOtherUnderscore;
  OtherUnderscoreSub objOtherUnderscoreSub;

  ExternDot objExternDot;
  ExternDotGenerated objGeneratedExternDot;

  SingleSub::SubOwnDot subOwnDot;

  return 0;
}

// Header in global subdirectory
#include "SingleSub/moc_SubOwnDot.cpp"
