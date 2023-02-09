try_compile(<resultVar>
            <SOURCES <srcfile...>                 |
             SOURCE_FROM_CONTENT <name> <content> |
             SOURCE_FROM_VAR <name> <var>         |
             SOURCE_FROM_FILE <name> <path>       >...
            [LOG_DESCRIPTION <text>]
            [NO_CACHE]
            [NO_LOG]
            [CMAKE_FLAGS <flags>...]
            [COMPILE_DEFINITIONS <defs>...]
            [LINK_OPTIONS <options>...]
            [LINK_LIBRARIES <libs>...]
            [OUTPUT_VARIABLE <var>]
            [COPY_FILE <fileName> [COPY_FILE_ERROR <var>]]
            [<LANG>_STANDARD <std>]
            [<LANG>_STANDARD_REQUIRED <bool>]
            [<LANG>_EXTENSIONS <bool>]
            )
