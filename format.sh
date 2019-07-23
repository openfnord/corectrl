#!/bin/bash

echo "Formatting code..."

find . -type d -name 3rdparty -prune -o -type f \( -name "*\.h" -o -name "*\.cpp" \) -print0 | xargs -0 -I '{}' sh -c "clang-format -i {}; echo -n '.'"

echo "Done."
