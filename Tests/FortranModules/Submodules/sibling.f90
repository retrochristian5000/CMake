! Empty submodule for checking disambiguation of
! nodes at the same vertical level in the tree
submodule ( parent ) sibling
contains
  module logical function sibling_function() result(sibling_stuff)
    sibling_stuff=.true.
  end function
end submodule sibling
