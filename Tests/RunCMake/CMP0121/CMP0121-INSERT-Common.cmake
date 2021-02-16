set(listvar a b c d e)

list(INSERT listvar invalid new)
message("INSERT: -->${listvar}<--")
