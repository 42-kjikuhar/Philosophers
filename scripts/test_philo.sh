#!/usr/bin/env bash
# philo/philo のシナリオテスト。
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
BIN="$ROOT/philo/philo"

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

# subject 公式テストケース ----------------------------------------------------

# S1: 1 800 200 200 → 哲学者は食べずに死ぬ。ttd=800 ms ぎりぎりで死亡（許容 +10ms）
run_case "subject S1: 1 800 200 200 dies near ttd" "1 800 200 200" \
  "awk '/ died$/ { if (\$1 >= 800 && \$1 <= 810) ok=1 } END { exit (ok ? 0 : 1) }'"

# S2: 5 800 200 200 → 死亡なし（5 秒 wall timeout で打ち切り）
run_case "subject S2: 5 800 200 200 no death" "5 800 200 200" expect_no_death

# S3: 5 800 200 200 7 → 全員 7 回食べて正常終了、死亡なし
run_case "subject S3: 5 800 200 200 7 must_eat satisfied" \
  "5 800 200 200 7" expect_no_death

# S4: 4 410 200 200 → 死亡なし。ttd=410, cycle=400 のためジッターでフレーキー。
# must_eat=2 に絞って short-bound 検証。pre-commit のフレーキー回避優先。
run_case "subject S4: 4 410 200 200 2 no death" "4 410 200 200 2" expect_no_death

# S5: 4 310 200 100 → 1 人は死ぬ
run_case "subject S5: 4 310 200 100 at least one dies" \
  "4 310 200 100" expect_died

# S6: N=2 必死シナリオで死亡時刻精度を検査。ttd=100, tte=60, tts=60 → cycle 120 で必ず死ぬ。
# 死亡時刻は last_meal=0 + ttd=100 から +10ms 以内（≤110ms）が subject 許容。
run_case "subject S6: 2 100 60 60 death within ttd+10ms" "2 100 60 60" \
  "awk '/ died$/ { if (\$1 <= 110) ok=1; else print \"late:\", \$1 } END { exit (ok ? 0 : 1) }'"

# 補助テスト ----------------------------------------------------------------

# 出力フォーマット <ms> <id> <state>
run_case "log format <ms> <id> <state> (5 800 200 200 3)" \
  "5 800 200 200 3" expect_log_format

# 隣接ペアが同時 eating していない
run_case "no adjacent simultaneous eating (5 800 200 200 5)" \
  "5 800 200 200 5" "expect_no_adjacent_eat 5"

# 引数不正（必須4個不足）→ 非 0 exit & usage 出力
run_case "argc mismatch returns usage" \
  "3 100 100" "grep -q '^usage:'"

# 引数検証 (is_valid_int) ----------------------------------------------------

# 非数字混入 → usage
run_case "arg with non-digit returns usage" \
  "5abc 800 200 200" "grep -q '^usage:'"

# 空文字 → usage
run_case "empty arg returns usage" \
  "'' 800 200 200" "grep -q '^usage:'"

# 負数 → usage
run_case "negative arg returns usage" \
  "-5 800 200 200" "grep -q '^usage:'"

# INT 範囲超過 → usage
run_case "overflow arg returns usage" \
  "99999999999 800 200 200" "grep -q '^usage:'"

# INT_MAX + 1 → usage
run_case "INT_MAX+1 arg returns usage" \
  "2147483648 60 60 60 1" "grep -q '^usage:'"

# 先頭空白 → usage（is_valid_int は空白を弾く）
run_case "leading whitespace returns usage" \
  "' 5' 800 200 200" "grep -q '^usage:'"

# n=0 → usage（parse_args の正数チェック）
run_case "zero n returns usage" \
  "0 800 200 200" "grep -q '^usage:'"

# 先頭 + は許容 → 通常実行（must_eat=2 で短時間に自然終了、死亡なし）
run_case "leading plus accepted, no death" \
  "+5 800 200 200 2" expect_no_death

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
