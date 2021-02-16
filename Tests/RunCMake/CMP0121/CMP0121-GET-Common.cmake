set(listvar a b c d e)

list(GET listvar 0 invalid out)
message("GET: -->${out}<--")
