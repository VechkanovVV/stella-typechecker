#!/usr/bin/env bash
set -euo pipefail

ALL=/tmp/all_tests.txt
CMAKE=/tmp/cmake_tests.txt
MISSING=/tmp/missing_tests.txt

# собрать список тестов
find tests -type f -name '*.stella' | sed 's|^\./||' | sort > "$ALL"

# извлечь тесты из CMakeLists.txt (normalize ../tests/... -> tests/...)
grep -Po 'COMMAND\s+\S+\s+typecheck\s+\K\S+\.stella' CMakeLists.txt | sed 's|^\.\./||' | sort > "$CMAKE"

# показать отсутствующие в CMake
echo "Missing tests (full paths):"
comm -23 "$ALL" "$CMAKE" | tee "$MISSING"

# если среди них есть tests/.../ill-typed/... — вывести ещё и имена (basename без .stella)
if grep -q '/ill-typed/' "$MISSING"; then
  echo
  echo "Names of tests from 'ill-typed' (basename without .stella):"
  grep '/ill-typed/' "$MISSING" | while IFS= read -r p; do
    basename -s .stella "$p"
  done
fi

# сгенерировать add_test строки для вставки в CMakeLists.txt
echo
echo "Suggested add_test lines:"
comm -23 "$ALL" "$CMAKE" | while IFS= read -r f; do
  name=$(basename "$f" .stella)
  echo "add_test(NAME $name COMMAND stella-interpreter typecheck ../$f)"
done
