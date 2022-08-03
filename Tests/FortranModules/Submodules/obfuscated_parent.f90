! This module is equal to the "parent module"

module &
         obfuscated_parent   ! True module declaration
  implicit none

  interface

    module logical &
                     function child_function() result(child_stuff)
    end function;  module subroutine &
                                       grandchild_subroutine()
    end subroutine; end interface; contains

  module logical function child_function() result(child_stuff)
    child_stuff=.true.
  end function

  module subroutine grandchild_subroutine()
    print *,"Test passed."
  end subroutine

end module obfuscated_parent
