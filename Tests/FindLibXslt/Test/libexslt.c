#include <assert.h>
#include <libexslt/exslt.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <string.h>

int main()
{
  xsltInit();

  xsltStylesheet* style = xsltNewStylesheet();
  exsltRegisterAll();
  xsltFreeStylesheet(style);

  assert(strstr(CMAKE_EXPECTED_LibExslt_VERSION, LIBEXSLT_DOTTED_VERSION));

  xsltCleanupGlobals();

  return 0;
}
