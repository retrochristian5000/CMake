foreach(cmd
    function endfunction
    macro endmacro
    if elseif else endif
    while endwhile
    foreach endforeach
    add_subdirectory
    enable_language
    project
    return
    subdirs
    )
  cmake_language(DEFER CALL ${cmd})
endforeach()
