#!/bin/bash

set -euo pipefail

readonly dir="$(dirname "$0")"
readonly very_long_c_txt="$dir/very_long.c.txt"
readonly hashes="$dir/hashes.cmake"

random_bytes() {
  python3 -c "import random; import sys; sys.stdout.buffer.write(random.Random(12345).randbytes(65536))"
}

cat > "$very_long_c_txt" <<EOF
#ifdef __cplusplus
extern "C" {
#endif

unsigned char imageBytes[] = {
EOF
random_bytes \
  | od -vtx1 -A none -w32 \
  | sed -E \
      -e 's/^ /  0x/g' \
      -e 's/([0-9a-f]) ([0-9a-f])/\1,0x\2/g' \
      -e 's/$/,/g' \
  >> "$very_long_c_txt"
cat >> "$very_long_c_txt" <<EOF
};

#ifdef __cplusplus
}
#endif
EOF

rm -f "$hashes"

write_hash() {
  name="$1"
  hash="$(sha256sum | cut -d' ' -f1)"
  echo "set(${name}_hash $hash)" >> "$hashes"
}

echo -en '\x00\x01\x02\x03\x04\x05\x06\x07' | write_hash basic
echo -en '' | write_hash empty
echo -en 'All work and no play makes Jack a dull boy.\n' | write_hash text_lf
echo -en 'All work and no play makes Jack a dull boy.\r\n' | write_hash text_crlf
echo -en 'This exactly 32 characters long!' | write_hash text_align
random_bytes | write_hash very_long
