# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# Standard imports
import io
import pathlib

# Third party packages
import pytest
from pygments import highlight
from pygments.formatter import Formatter
from pygments.lexers import load_lexer_from_file
from pygments.token import Comment, String, Whitespace

_data_dir = pathlib.Path(__file__).parent


class TestFormatter(Formatter):
    name = 'Test'
    aliases = ['test']

    def format(self, tokensource, outfile):  # NOQA disable=flake8-builtins
        for ttype, value in tokensource:
            if ttype in [Comment, Whitespace, String] and '\n' in value:
                value = value.replace('\n', '<LF>')
            if ttype in [String] and '`' in value:
                value = value.replace('`', '\\`')
            outfile.write(f'{ttype}:`{value}`\n')

        outfile.flush()


class CMakeLexerTest:  # NOQA too-few-public-methods

    lexer = load_lexer_from_file(_data_dir.parent / 'cmakelexer.py', 'CMakeLexer')
    formatter = TestFormatter()

    @pytest.mark.parametrize(
        'input_file'
      , [
            'command_no_args.cmake'
          , 'command_int_arg.cmake'
          , 'command_str_arg.cmake'
          , 'command_str_arg_esc.cmake'
          , 'command_kw_arg.cmake'
          , 'command_subst_arg.cmake'
          , 'command_subst_str_arg.cmake'
          , 'command_dbl_subst_arg.cmake'
          , 'command_special_arg.cmake'
          , 'command_genex_arg.cmake'
          , 'command_aliased_tgt_arg.cmake'
          , 'command_path_arg.cmake'
          , 'command_url_arg.cmake'
          , 'signature_one_arg.cmake'
          , 'signature_opt_arg.cmake'
          , 'signature_multi_arg.cmake'
          , 'signature_multi_arg_ws.cmake'
          , 'signature_multi_opt_arg.cmake'
          , 'signature_multi_opt_arg_ws.cmake'
          , 'signature_alt_arg.cmake'
          , 'signature_alt_kw_arg.cmake'
          , 'comments.cmake'
            # Special cases
          , 'source_group.cmake'
          , 'pkg_check_modules.cmake'
          , 'configure_file.cmake'
          , 'if_parentheses_cond.cmake'
          , 'foreach_items.cmake'
          , 'set_list.cmake'
          , 'check_source_compiles.cmake'
            # Problems found after rewriting lexer
          , 'add_custom_command.cmake'
          , 'add_library.cmake'
          , 'block.cmake'
          , 'cmake_host_system_information.cmake'
          , 'cmake_language.cmake'
          , 'file.cmake'
          , 'set_ENV.cmake'
          , 'target_compile_definitions.cmake'
          , 'genex_at_global_context.cmake'
          , 'string_cont.cmake'
          , 'CMAKE_CXX_CLANG_TIDY.cmake'
          , 'ProcessorCount.cmake'
          , 'message.cmake'
          , 'add_custom_target_genex.cmake'
          , 'cmake_policy.cmake'
          , 'try_compile.cmake'
          , 'project.cmake'
          ]
      )
    def simple_test(self, input_file):
        filename = _data_dir / input_file

        outstream = io.StringIO()
        highlight(filename.read_text(), self.lexer, self.formatter, outstream)
        actual_lines = outstream.getvalue().split('\n')[:-1]

        expected_filename = filename.with_suffix('.out')
        if expected_filename.exists():
            expected_lines = expected_filename.read_text().split('\n')[:-1]
            assert actual_lines == expected_lines
            # Also, make sure there are no error tokens
            assert all((lambda line: 'Token.Error' not in line, actual_lines))

        else:
            expected_filename.write_text('\n'.join(actual_lines) + '\n')
            raise pytest.skip(f'{expected_filename} not found. Writing actual output to it...')
