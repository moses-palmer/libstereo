#!/bin/sh
# compile-glsl.sh path.glsl

path="$1"
base="${path%%.*}"
destination="$base.h"
name="`echo ${base##*/} | tr - _`"

rm -f "$destination"

documentation="`awk '
BEGIN {
    k = 0;
}

/\\s*\\*\\*\\// {
    if (k == 1) k++;
}

/\\/\\*\\*/ {
    k++;
}

{
    if (k == 1) print;
}
' \"$path\"`"

echo "documentation=$documentation" >&2
if [ ! -z "$documentation" ]; then
    echo "$documentation"                                >> "$destination"
    echo                                                 >> "$destination"
fi

echo "#ifndef $name"                                     >> "$destination"
echo "#define $name \\"                                  >> "$destination"
cpp -P "$path" | sed -e 's/"/\\"/g;s/^/\"/g;s/$/\" \\/g' >> "$destination"
echo "/* End of $path */"                                >> "$destination"
echo "#endif"                                            >> "$destination"
