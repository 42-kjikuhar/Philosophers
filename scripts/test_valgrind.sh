#!/usr/bin/env bash
# valgrind による philo/philosophers のメモリリーク検査。
#   - 複数シナリオを must_eat 付き（自然終了）で走らせ、valgrind がリーク検出したら fail
#   - macOS arm64 では valgrind 非対応のためスキップ
#   - CI（Linux）で実行することを想定
#
# 使い方: bash scripts/test_valgrind.sh

set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/philo/philosophers"

if ! command -v valgrind >/dev/null 2>&1; then
  echo "✘ valgrind が見つかりません。Linux で実行してください。"
  exit 1
fi

if [[ ! -x "$BIN" ]]; then
  ( cd "$ROOT/philo" && make >/dev/null )
fi

PASS=0
FAIL=0
FAILED=()

# 引数: $1=name, $2..=philo args（must_eat を入れて自然終了させる）
run_valgrind() {
  local name="$1"
  shift
  local args=("$@")
  echo "── $name : ${args[*]}"
  if valgrind --leak-check=full \
      --show-leak-kinds=definite,indirect,possible \
      --errors-for-leak-kinds=definite,indirect,possible \
      --track-origins=yes --error-exitcode=1 --quiet \
      "$BIN" "${args[@]}" >/dev/null 2>"$ROOT/.valgrind_$name.log"; then
    echo "  ✔ no leaks"
    PASS=$((PASS + 1))
    rm -f "$ROOT/.valgrind_$name.log"
  else
    echo "  ✘ leaks detected:"
    sed 's/^/    /' "$ROOT/.valgrind_$name.log" | head -40
    FAIL=$((FAIL + 1))
    FAILED+=("$name")
    rm -f "$ROOT/.valgrind_$name.log"
  fi
}

echo "=== valgrind memory leak tests ==="
# 自然終了する must_eat 付き構成で計測。死亡シナリオ含む。
run_valgrind "5_must_eat_3"  5 800 200 200 3
run_valgrind "4_must_eat_3"  4 410 200 200 3
run_valgrind "1_solo_die"    1 800 200 200
run_valgrind "4_one_dies"    4 310 200 100

echo
echo "──────────────────────"
echo "  passed: $PASS"
echo "  failed: $FAIL"
if (( FAIL > 0 )); then
  printf '  failures:\n'
  for n in "${FAILED[@]}"; do echo "    - $n"; done
  exit 1
fi
echo "  ✔ all valgrind tests passed"
