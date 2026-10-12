# The program units in this file consist of a module/submodule
# tree represented by the following graph:
#
#        parent
#          |
#         / \
#        /   \
#    child   sibling
#      |
#  grandchild
#      |
#  GreatGrandChild
#
# where the parent node is a module and all other nodes are submodules.

add_executable(submod
  # Place these first so that we do not get "lucky" and build them
  # after their ancestors even if their dependencies are missed.
  greatgrandchild.f90
  grandchild.f90
  main.f90
  parent.f90
  obfuscated_parent.f90
  child.f90
  sibling.f90
  )
