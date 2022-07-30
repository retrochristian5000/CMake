program main
  use parent, only : child_function,grandchild_subroutine
  use parent, only : sibling_function,GreatGrandChild_subroutine
  use obfuscated_parent, &
      only : obfuscated_child_function => child_function, &
             grandchild_subroutine_obf => grandchild_subroutine
  implicit none
  if (child_function())            call grandchild_subroutine
  if (sibling_function())          call GreatGrandChild_subroutine
  if (obfuscated_child_function()) call grandchild_subroutine_obf
end program
