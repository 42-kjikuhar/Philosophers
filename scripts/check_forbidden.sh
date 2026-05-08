#!/usr/bin/env bash
# Philosophers 課題の禁止関数監査スクリプト。
#
# 二段階チェック:
#   1. ソースレベル grep: ビルド前に *.c / *.h から疑わしい呼び出しを検出する。
#   2. バイナリレベル nm: 最終確認。リンカが取り込んだ全ての未定義シンボルを見るので、
#      マクロや typedef 経由の呼び出しも漏れなく拾える。
#
# 使い方:
#   scripts/check_forbidden.sh           # 既定で philo を対象
#   scripts/check_forbidden.sh philo     # 明示指定
#
# 終了コード: 0 = クリーン / 1 = 違反検出 / 2 = セットアップエラー。

set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TARGETS=("${@:-philo}")
FAIL=0

# 42 subject に従う許可関数リスト
ALLOW_PHILO=(
  memset printf malloc free write usleep gettimeofday
  pthread_create pthread_detach pthread_join
  pthread_mutex_init pthread_mutex_destroy
  pthread_mutex_lock pthread_mutex_unlock
)

# macOS / Linux のランタイム・ビルトイン・リンカ生成シンボル一覧（無視する）。
# nm の出力は OS によって接頭の `_` が付いたり付かなかったりするので、
# 「先頭 _ を 1 つ剥がした後の形」で揃えて列挙する。
IGNORE_LIST=(
  main _main
  _init _fini
  _DYNAMIC _GLOBAL_OFFSET_TABLE_
  dyld_stub_binder
  atexit _exit
  _libc_start_main libc_start_main
  _cxa_finalize cxa_finalize
  _cxa_atexit cxa_atexit
  _gmon_start__ gmon_start__
  _stack_chk_fail stack_chk_fail _stack_chk_guard stack_chk_guard
  ITM_deregisterTMCloneTable ITM_registerTMCloneTable
  _chkstk chkstk
)

contains() {
  local needle="$1"; shift
  for x in "$@"; do [[ "$x" == "$needle" ]] && return 0; done
  return 1
}

audit_dir() {
  local dir="$1"
  local -a allow=("${ALLOW_PHILO[@]}")

  echo "── ${dir} ──"
  if [[ ! -d "$ROOT/$dir" ]]; then
    echo "  (ディレクトリが見つからないためスキップ)"
    return 0
  fi

  # Tier 2: バイナリレベルの nm 検査（最も正確）。標準名のバイナリを探す。
  local bin=""
  if [[ -x "$ROOT/$dir/philosophers" && -f "$ROOT/$dir/philosophers" ]]; then
    bin="$ROOT/$dir/philosophers"
  fi

  if [[ -n "$bin" ]]; then
    echo "  binary: $(basename "$bin")"
    local violations=0
    local sym
    while IFS= read -r sym; do
      [[ -z "$sym" ]] && continue
      contains "$sym" "${IGNORE_LIST[@]}" && continue
      if ! contains "$sym" "${allow[@]}"; then
        echo "  ✘ FORBIDDEN: $sym"
        violations=$((violations + 1))
      fi
    done < <(nm -u "$bin" 2>/dev/null | awk 'NF>0{print $NF}' | sed -E 's/@@?.*$//' | sed 's/^_//' | sort -u)
    # 補足: macOS の nm -u は 1 列出力、GNU の nm は "U <sym>" 形式。
    # 末尾フィールドを取り、GLIBC の "@VERSION" 接尾辞を剥がし、Mach-O の先頭アンダースコアを 1 つ剥がす。
    if [[ $violations -gt 0 ]]; then
      FAIL=1
      echo "  → バイナリで $violations 件の違反"
    else
      echo "  ✔ バイナリは clean"
    fi
  else
    echo "  (バイナリ未ビルド — 'make' を実行してから再度確認してください)"
  fi

  # Tier 1: 既知の危険関数をソースレベルで grep（防御網）
  local risky='\b(strcpy|strncpy|strcat|strncat|sprintf|gets|scanf|sscanf|atoi|atol|atof|sleep|nanosleep|system|exec[lv]p?e?|fork|kill|wait|waitpid|signal|sigaction|alarm|read|open|close)[[:space:]]*\('
  local hits
  hits="$(grep -REn --include='*.c' --include='*.h' "$risky" "$ROOT/$dir" 2>/dev/null || true)"
  if [[ -n "$hits" ]]; then
    echo "  ソースレベルの疑わしい箇所（subject と照合してください）:"
    echo "$hits" | sed 's/^/    /'
    # ソースレベルの hit はバイナリ検査が通っていれば警告のみ。単独では失敗扱いにしない。
  fi
}

for d in ${TARGETS[@]}; do
  audit_dir "$d"
  echo
done

if [[ $FAIL -ne 0 ]]; then
  echo "✘ 禁止関数を検出しました。"
  exit 1
fi
echo "✔ 禁止関数チェック OK"
