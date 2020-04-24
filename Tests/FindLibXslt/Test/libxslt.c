#include <assert.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <string.h>

int main()
{
  xsltInit();

  xsltStylesheet* style = xsltNewStylesheet();
  xsltFreeStylesheet(style);

  assert(strstr(CMAKE_EXPECTED_LibXslt_VERSION, LIBXSLT_DOTTED_VERSION));

  xsltCleanupGlobals();

  return 0;
}
