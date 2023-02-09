How to test Pygments CMake lexer
================================

1. Install ``poetry`` if needed:

  .. code-block:: console

    $ pip install [--user] poetry

2. Install developer packages

  .. code-block:: console

    $ poetry install
    Creating virtualenv sphinx-cmake-AJdp1NRs-py3.11 in ~/.cache/pypoetry/virtualenvs
    Updating dependencies
    Resolving dependencies... (19.9s)

    Writing lock file

    Package operations: 69 installs, 0 updates, 0 removals

      • Installing certifi (2022.12.7): Installing...
      • Installing charset-normalizer (3.0.1): Pending...
      • Installing certifi (2022.12.7)
      • Installing charset-normalizer (3.0.1)
      • Installing docutils (0.19)
    …

3. Enter venv and (optionally setup shorter aliases for commands ;-)

  .. code-block:: console

    $ poetry shell
    $ alias pti='poetry install'
    $ alias ptl='poetry lock'
    $ alias ptr='poetry run'
    $ alias pts='poetry shell'
    $ alias ptt='poetry run poe'

4. Run tests (using aliases above ;-)

  .. code-block:: console

    $ ptt test -v
    Poe => pytest -vv
    ============= test session starts =============================
    platform linux -- Python 3.11.1, pytest-7.2.1, pluggy-1.0.0 -- …
    cachedir: .pytest_cache
    rootdir: …/Utilities/Sphinx, configfile: pyproject.toml, testpaths: test
    collected 46 items

    test/cmakelexer_test.py::CMakeLexerTest::simple_test[command_no_args.cmake] PASSED [  2%]
    test/cmakelexer_test.py::CMakeLexerTest::simple_test[command_int_arg.cmake] PASSED [  4%]
    test/cmakelexer_test.py::CMakeLexerTest::simple_test[command_str_arg.cmake] PASSED [  6%]
    …

    $ ptt lint-check
    Poe => flakeheaven lint colors.py cmake.py cmakelexer.py test/cmakelexer_test.py

    $ ptt dead-check
    Poe => vulture cmake.py
