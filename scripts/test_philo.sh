#!/usr/bin/env bash
# philo/philosophers のシナリオテスト。
#
# 実装が直列 event-loop 版である現状に合わせ、最低限の挙動を検証する:
#   - 死ぬべきケースで "died" が出る
#   - 死なないケースで "died" が出ない
#   - 出力フォーマットが <ms> <id> <state> である
#   - 隣接ペアが同時刻に "is eating" を出していない（直列版でも守れる前提）
#
# pthread 化や must_eat 完了などの段階で、ここに新しいシナリオを足していく。

set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/philo/philosophers"

PASS=0
FAIL=0
FAILED=()

# ビルド確認（バイナリがなければ作る）
if [[ ! -x "$BIN" ]]; then
  ( cd "$ROOT/philo" && make >/dev/null )
fi

# gtimeout を優先（macOS coreutils 環境対策）。なければ bash ネイティブの fallback を使う。
TIMEOUT_BIN="$(command -v gtimeout || command -v timeout || true)"

# bash ネイティブ timeout: 子プロセス起動 → 監視 → 期限切れで TERM/KILL。
# 標準出力をキャプチャしたいので、出力先ファイル経由で受け渡す。
run_with_bash_timeout() {
  local secs="$1" outfile="$2"; shift 2
  "$@" >"$outfile" 2>&1 &
  local pid=$!
  ( sleep "$secs"; kill -TERM "$pid" 2>/dev/null; sleep 0.5; kill -KILL "$pid" 2>/dev/null ) >/dev/null 2>&1 &
  local watchdog=$!
  wait "$pid" 2>/dev/null
  local rc=$?
  kill "$watchdog" 2>/dev/null
  wait "$watchdog" 2>/dev/null
  return $rc
}

# 実行と判定のラッパ。
# 引数: $1=test name, $2=コマンドライン引数文字列, $3=判定関数名（出力をstdinで受け取り 0=pass / 非0=fail）
# must_eat なしのケースは binary が自然終了しないので、外側で 5 秒の壁時計タイムアウトを掛ける。
run_case() {
  local name="$1" args="$2" judge="$3"
  local out tmpfile
  tmpfile="$(mktemp)"
  set +e
  if [[ -n "$TIMEOUT_BIN" ]]; then
    "$TIMEOUT_BIN" 5 bash -c "$BIN $args" >"$tmpfile" 2>&1
  else
    run_with_bash_timeout 5 "$tmpfile" bash -c "$BIN $args"
  fi
  set -e
  out="$(cat "$tmpfile")"
  rm -f "$tmpfile"
  if eval "$judge" <<<"$out"; then
    echo "  ✔ $name"
    PASS=$((PASS + 1))
  else
    echo "  ✘ $name"
    echo "    args: $args"
    echo "    head:"
    echo "$out" | head -5 | sed 's/^/      /'
    FAIL=$((FAIL + 1))
    FAILED+=("$name")
  fi
}

# ===== 判定関数 =====

# "died" が含まれている
expect_died() { grep -q ' died$' || return 1; }

# "died" が含まれていない
expect_no_death() { ! grep -q ' died$' || return 1; }

# 出力 1 行目が "<ms> <id> <state...>" 形式
expect_log_format() {
  local first
  read -r first
  [[ "$first" =~ ^[0-9]+\ [0-9]+\ .+$ ]]
}

# 同時刻に隣接 ID が両方 "is eating" していないこと（直列版の整合確認）
expect_no_adjacent_eat() {
  local n="${1:-5}"
  awk -v n="$n" '
    /is eating$/ {
      ms=$1; id=$2;
      if (eat_at_ms[ms] == "") { eat_at_ms[ms] = id; next }
      split(eat_at_ms[ms], arr, ",")
      for (k in arr) {
        other = arr[k]+0
        diff = (id - other + n) % n
        if (diff == 1 || diff == n - 1) { print "ADJ " ms " " other " vs " id; bad=1 }
      }
      eat_at_ms[ms] = eat_at_ms[ms] "," id
    }
    END { exit (bad ? 1 : 0) }
  '
}

echo "=== philosophers behavior tests ==="

# Case 1: must_eat 指定で全員食べ切ったら死なずに正常終了
run_case "no death when must_eat satisfied (5 800 200 200 5)" \
  "5 800 200 200 5" expect_no_death

# Case 2: ttd < tte, 食事中に必ず死ぬ（5人版で確実）
run_case "must die when ttd < tte (5 100 200 200)" \
  "5 100 200 200" expect_died

# Case 3: 出力フォーマット <ms> <id> <state>
run_case "log format <ms> <id> <state> (5 800 200 200 3)" \
  "5 800 200 200 3" expect_log_format

# Case 4: 隣接ペアが同時 eating していない
run_case "no adjacent simultaneous eating (5 800 200 200 5)" \
  "5 800 200 200 5" "expect_no_adjacent_eat 5"

# Case 5: 引数不正（必須4個不足）→ 非 0 exit & usage 出力
run_case "argc mismatch returns usage" \
  "3 100 100" "grep -q '^usage:'"

# Case 6: must_eat 指定なし + 余裕パラメータ → 5 秒の wall timeout で打ち切られる
# 出力に eating があり、died が無いこと
run_case "no must_eat keeps eating without death (5 800 200 200)" \
  "5 800 200 200" \
  "grep -q 'is eating' && ! grep -q ' died$'"

echo
echo "──────────────────────"
echo "  passed: $PASS"
echo "  failed: $FAIL"
if (( FAIL > 0 )); then
  printf '  failures:\n'
  for n in "${FAILED[@]}"; do echo "    - $n"; done
  exit 1
fi
echo "  ✔ all tests passed"
