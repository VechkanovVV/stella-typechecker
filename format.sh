# установить clang-format (если не установлен)
sudo apt update && sudo apt install -y clang-format

# настроить стиль (по желанию)
cat > .clang-format <<'EOF'
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 120
EOF

# отформатировать один файл (in-place)
clang-format -i --style=file src/VisitTypeCheck.cpp

# или отформатировать все .cpp/.h рекурсивно
find . -type f \( -name '*.cpp' -o -name '*.cc' -o -name '*.c' -o -name '*.h' -o -name '*.hpp' \) -print0 \
  | xargs -0 clang-format -i --style=file

# посмотреть diff без записи (проверка)
clang-format --style=file src/VisitTypeCheck.cpp | diff -u src/VisitTypeCheck.cpp -
