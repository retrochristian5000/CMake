# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# Third party packages
from pygments.lexer import RegexLexer, bygroups, include
from pygments.token import (
    Comment,
    Name,
    Number,
    Operator,
    Punctuation,
    String,
    Whitespace
)


class CMakeLexer(RegexLexer):
    tokens = {
        'root': [
            (
                r'(\s*)([A-Za-z_][A-Za-z0-9_]*)(\s*)'
              , bygroups(Whitespace, Name.Function, Whitespace)
              )
          , (r'\(', Name.Function, 'args-top-level')
          , (r'\.\.\.', Punctuation)                        # NOTE `block()`
          , include('angle-bracket')                        # NOTE `block()`, `if()`
          , include('ws')
          , include('comments')
            # ALERT This is not valid CMake but genex snippets in the
            # file scope are used in `cmake-generator-expressions(7)`
            # manual ... ;-()
          , include('genex')
          ]
      , 'args-top-level': [
            (r'\)', Name.Function, '#pop')
          , include('args-nested')
          ]
      , 'args-nested': [
            (r'\[=*\[', String.Heredoc, 'inside-bracket-string')
          , (r'\[', Punctuation, '#push')
          , (r'\]', Punctuation, '#pop')
          , (r'\(', Punctuation, 'inside-parenthis')
          , (r'"', String, 'inside-string')
          , include('unquoted-args')
          , include('ws')
          , include('comments')
          , include('interpolation')
          , include('genex')
          , include('angle-bracket')
          ]
      , 'angle-bracket': [
            # NOTE Special case for `cmake_policy`:
            # render `<NNNN>` as placeholder
            (r'(<)(NNNN)(>)', bygroups(Punctuation, Name.Label, Punctuation))
            # NOTE Special case for `find_package`:
            # render `<PackageName>` as placeholder
          , (r'(<)(PackageName)(>)', bygroups(Punctuation, Name.Label, Punctuation))
            # NOTE Special case for `project`:
            # render `<PROJECT-NAME>` as placeholder
          , (r'(<)(PROJECT-NAME)(>)', bygroups(Punctuation, Name.Label, Punctuation))
          , (r'<', Punctuation, 'inside-angle-bracket')
          ]
      , 'inside-angle-bracket': [
            (r'>', Punctuation, '#pop')
          , (r'\|', Punctuation)
          , (r'[a-z][A-Za-z0-9_\-]*', Name.Label)           # parameter <placeholder>
          , include('args-nested')
          ]
      , 'inside-parenthis': [
            (r'\)', Punctuation, '#pop')
          , include('args-nested')
          ]
      , 'unquoted-args': [
            (r'(32_64|64_32|32|64)', Name.Constant)         # NOTE `cmake_host_system_information()`
            # NOTE Special case for `try_compile`/`try_run`:
            # render `<LANG>_BLAH_BLAH` as named keyword w/ placeholder
          , (
                r'(<)([A-Z_]+)(>)(_[A-Z_]+)'
              , bygroups(Punctuation, Name.Label, Punctuation, Name.Constant)
              )
          , (r'\b[0-9][0-9\.]*\b', Number)
          , (
                # NOTE Special case for `set(ENV{<var} ...)` and the similar `unset`
                r'(ENV|CACHE|DATA)(\{)(?:(<)([a-z][a-z0-9_\-]*)(>))(\})'
              , bygroups(Name.Tag, String.Interpol, Punctuation, Name.Label, Punctuation, String.Interpol)
              )
          , (
                r'(ENV|CACHE|DATA)(\{)'
              , bygroups(Name.Tag, String.Interpol)
              , 'inside-interpolation'
              )
          , (
                r'([A-Za-z][A-Za-z0-9_]*)(?:(::)([A-Za-z][A-Za-z0-9_]*))+'
              , bygroups(Name.Namespace, Operator, Name.Entity)
              )
          , (r'@?[A-Z][A-Z0-9_]*\b', Name.Constant)
          , (r'\.\.\.', Punctuation)
          , (r'[<>]=', String.Other)                        # used in FindPkgConfig.cmake
          , (r'\|', Punctuation)                            # TODO Fix `add_custom_command`
          , (r'\\[; \\]', String.Escape)
          , (r';', Punctuation)
          , (r'[\w.+-/:@\*=]+', String.Other)
          ]
      , 'inside-string': [
            (r'"', String, '#pop')
          , (r'\\[\\trn";\$]', String.Escape)               # cmake-language(7): Escape Sequences
          , (r'\\[\.\*\+]', String)                         # Most likely escape for regex special symbols
          , (r';', String.Symbol)                           # `;` in the middle of quoted string
          , (r'\\$', String.Symbol)                         # line continuation
          , include('interpolation')
          , (r'\$', String)                                 # if no interpolation found...
          , (r'[^\\";\$]+', String)                         # all other characters
          ]
      , 'inside-bracket-string': [
            # NOTE This may not really match the open brackets,
            # but CMake docs wouldn't have incorrect examples, right? ;-)
            (r'.*\]=*\]', String.Heredoc, '#pop')
          , (r'[^\]]+', String.Heredoc)
          ]
      , 'interpolation': [
            (r'\$\{', String.Interpol, 'inside-interpolation')
          , (
                r'(\$)(ENV|CACHE|DATA)(\{)'
              , bygroups(String.Interpol, Name.Tag, String.Interpol)
              , 'inside-interpolation'
              )
          ]
      , 'inside-interpolation': [
            (r'\}', String.Interpol, '#pop')
          , (r'[A-Za-z_][A-Za-z_0-9\.]*', Name.Variable)
          , include('interpolation')
          ]
      , 'genex': [
            (r'(\$<)([A-Z_]+)(>)', bygroups(Operator, Operator.Word, Operator))
          , (r'\$<', Operator, 'inside-genex')
          ]
      , 'inside-genex': [
            (r'>', Operator, '#pop')
          , (r'([A-Z_]+)(:)', bygroups(Operator.Word, Operator))
          , (r':', Operator)
          , (r',', Punctuation)
          , include('args-nested')
          ]
      , 'comments': [
            (r'#\[=*\[', Comment, 'inside-bracket-comment')
          , (r'#.*\n', Comment)
          ]
      , 'inside-bracket-comment': [
            # NOTE This may not really match the open brackets,
            # but CMake docs wouldn't have incorrect examples, right? ;-)
            (r'.*\]=*\]', Comment, '#pop')
          , (r'[^\]]+', Comment)                            # all other characters
          ]
      , 'ws': [
            (r'[ \t\n]+', Whitespace)
          ]
      }
