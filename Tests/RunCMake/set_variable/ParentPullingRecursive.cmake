function(report where)
    message("----------")
    message("variable values at ${where}:")
    foreach(var IN ITEMS
            top_implicit_inner_set top_implicit_inner_unset
            top_explicit_inner_set top_explicit_inner_unset top_explicit_inner_tounset
            top_implicit_outer_set top_explicit_outer_unset
            top_explicit_outer_set top_explicit_outer_unset top_explicit_outer_tounset

            outer_implicit_inner_set outer_implicit_inner_unset
            outer_explicit_inner_set outer_explicit_inner_unset outer_explicit_inner_tounset)
        if(DEFINED ${var})
            message("${var}: -->${${var}}<--")
        else()
            message("${var}: <undefined>")
        endif()
    endforeach()
    message("----------")
endfunction()

macro(set_values upscope downscope value)
    # Pull the value in implicitly.
    set_variable(dummy VALUES ${${upscope}_implicit_${downscope}_set})
    set_variable(dummy VALUES ${${upscope}_implicit_${downscope}_unset})
    # Pull it down explicitly.
    set_variable(${upscope}_explicit_${downscope}_set SCOPE PARENT VALUES "${value}")
    set_variable(${upscope}_explicit_${downscope}_unset SCOPE PARENT VALUES "${value}")
    unset_variable(${upscope}_explicit_${downscope}_tounset SCOPE PARENT)
endmacro()

function(inner)
    report("inner start")

    set_values(top inner inner)
    set_values(outer inner inner)

    report("inner end")
endfunction()

function(outer)
    report("outer start")

    set_values(top outer outer)

    # Set values for inner to manipulate.
    set_variable(outer_implicit_inner_set VALUES outer)
    unset_variable(outer_implicit_inner_unset)
    set_variable(outer_explicit_inner_set VALUES outer)
    unset_variable(outer_explicit_inner_unset)
    set_variable(outer_explicit_inner_tounset VALUES outer)

    report("outer before inner")

    inner()

    report("outer after inner")

    # Do what inner does so that we can test the values that inner should have
    # pulled through to here.
    set_values(top inner outer)

    report("outer end")
endfunction()

# variable name is:
#
#    <upscope>_<pulltype>_<downscope>_<settype>
#
# where the value is the name of the scope it was set in. The scopes available
# are "top", "outer", and "inner". The pull type may either be "implicit" or
# "explicit" based on whether the pull is due to a variable dereference or a
# PARENT_SCOPE setting. The settype is "set" where both scopes set a value,
# "unset" where upscope unsets it and downscope sets it, and "tounset" where
# upscope sets it and downscope unsets it.
#
# We test the following combinations:
#
#   - outer overriding top's values;
#   - inner overriding top's values;
#   - inner overriding outer's values; and
#   - outer overriding inner's values in top after inner has run.

# Set values for inner to manipulate.
set_variable(top_implicit_inner_set VALUES top)
unset_variable(top_implicit_inner_unset)
set_variable(top_explicit_inner_set VALUES top)
unset_variable(top_explicit_inner_unset)
set_variable(top_explicit_inner_tounset VALUES top)

# Set values for outer to manipulate.
set_variable(top_implicit_outer_set VALUES top)
unset_variable(top_implicit_outer_unset)
set_variable(top_explicit_outer_set VALUES top)
unset_variable(top_explicit_outer_unset)
set_variable(top_explicit_outer_tounset VALUES top)

report("top before calls")

outer()

report("top after calls")
