#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EX="$ROOT/example.stella"

if [[ ! -f "$EX" ]]; then
  echo "Example not found: $EX" >&2
  exit 1
fi

# Build (BNFC -> compile -> build/stella-interpreter)
make -C "$ROOT"

# Run type checker on the example
"$ROOT/build/stella-interpreter" typecheck "$EX"
echo "Typecheck OK"