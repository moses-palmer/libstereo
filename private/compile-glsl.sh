#!/bin/sh
# compile-glsl.sh path.glsl

path="$1"
base="${path%%.*}"
destination="$base.h"
name="`echo ${base##*/} | tr - _`"

# Remove any previous version
rm -f "$destination"

echo "#ifndef $name"                                           >> "$destination"
echo "#define $name \\"                                        >> "$destination"
sed -e 's/.*/"&\\n" \\/g' "$path"                             >> "$destination"
echo "\"\""                                                    >> "$destination"
echo "#endif"                                                  >> "$destination"
