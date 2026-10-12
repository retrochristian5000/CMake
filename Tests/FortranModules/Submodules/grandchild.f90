! Test the notation for an Nth-generation descendant
! for N>1, which necessitates the colon.
! Names are split across free-form continuation lines.
submodule ( par&
           &ent : chi&
                 &ld ) grand&
                      &child
contains
  module subroutine grandchild_subroutine()
    print *,"Test passed."
  end subroutine
end submodule grandchild
