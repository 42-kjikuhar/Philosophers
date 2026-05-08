#!/usr/bin/env bash
# 42 ヘッダの整合性チェック。
#   1. 全 .c / .h に 42 ヘッダが付いているか
#   2. すべての `By:` 行が同一ユーザー名を指しているか（複数アカウント混在の検出）
#   3. ヘッダの日付整合性
#       3-a. Created <= Updated （時系列の逆転がないか）
#       3-b. Updated >= 直近コミット時刻 - 60s（ファイル変更時にヘッダが更新されているか）
#
# norminette はヘッダの「存在と構文」を見るが、ユーザー名混在や日付整合性は捕捉しないため別途検査する。
#
# 使い方: scripts/check_header.sh [対象ディレクトリ ...]   (既定: philo)
# 終了コード: 0 = OK / 1 = 違反あり

set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TARGETS=("${@:-philo}")
FAIL=0
USERS=()
NO_HEADER=()
DATE_BAD_ORDER=()
DATE_STALE=()

# 日付パース: macOS と Linux の両対応で "YYYY/MM/DD HH:MM:SS" → epoch
to_epoch() {
  local s="$1"
  if date -j -f "%Y/%m/%d %H:%M:%S" "$s" +%s 2>/dev/null; then
    return 0
  fi
  # GNU date は "YYYY/MM/DD HH:MM:SS" を直接受けないので "YYYY-MM-DD" に変換
  local norm="${s//\//-}"
  date -d "$norm" +%s 2>/dev/null
}

for dir in "${TARGETS[@]}"; do
  if [[ ! -d "$ROOT/$dir" ]]; then
    echo "  (ディレクトリが見つからないためスキップ: $dir)"
    continue
  fi

  while IFS= read -r f; do
    # ヘッダ存在チェック: 1 行目が 42 ヘッダの罫線か
    first="$(sed -n '1p' "$f")"
    if [[ "$first" != "/* ************************************************************************** */" ]]; then
      NO_HEADER+=("$f")
      continue
    fi

    # By: 行からユーザー名を抽出
    by_user="$(grep -m1 -E '^/\*   By: [^ ]+ ' "$f" | awk '{print $3}')"
    [[ -n "$by_user" ]] && USERS+=("$by_user::$f")

    # Created / Updated を抽出
    created_str="$(grep -m1 -E '^/\*   Created: ' "$f" | sed -E 's|^/\*   Created: ([0-9/]+ [0-9:]+) by .*|\1|')"
    updated_str="$(grep -m1 -E '^/\*   Updated: ' "$f" | sed -E 's|^/\*   Updated: ([0-9/]+ [0-9:]+) by .*|\1|')"

    if [[ -z "$created_str" || -z "$updated_str" ]]; then
      continue
    fi

    created_epoch="$(to_epoch "$created_str" || true)"
    updated_epoch="$(to_epoch "$updated_str" || true)"

    if [[ -z "$created_epoch" || -z "$updated_epoch" ]]; then
      continue
    fi

    # 3-a: Created <= Updated
    if (( updated_epoch < created_epoch )); then
      DATE_BAD_ORDER+=("$f::Created=$created_str / Updated=$updated_str")
    fi

    # 3-b: Updated が直近コミット時刻と乖離していないか
    rel="${f#$ROOT/}"
    last_commit="$(git -C "$ROOT" log -1 --format=%ct -- "$rel" 2>/dev/null || true)"
    if [[ -n "$last_commit" ]]; then
      # 60 秒の猶予（pre-commit 整形と CI の時刻差を許容）
      if (( last_commit - updated_epoch > 60 )); then
        diff_min=$(( (last_commit - updated_epoch) / 60 ))
        DATE_STALE+=("$f::Updated=$updated_str / last commit=$(date -r "$last_commit" '+%Y/%m/%d %H:%M:%S' 2>/dev/null || date -d @"$last_commit" '+%Y/%m/%d %H:%M:%S') (差 ${diff_min}分)")
      fi
    fi
  done < <(find "$ROOT/$dir" -type f \( -name '*.c' -o -name '*.h' \))
done

# 1) ヘッダ欠如
if [[ ${#NO_HEADER[@]} -gt 0 ]]; then
  echo "✘ 42 ヘッダ欠如:"
  for f in "${NO_HEADER[@]}"; do echo "    $f"; done
  FAIL=1
fi

# 2) ユーザー名混在
unique_users=""
if [[ ${#USERS[@]} -gt 0 ]]; then
  unique_users="$(printf '%s\n' "${USERS[@]}" | awk -F'::' '{print $1}' | sort -u)"
  count="$(printf '%s\n' "$unique_users" | grep -cE '.' || true)"
  if [[ "$count" -gt 1 ]]; then
    echo "✘ 42 ヘッダの By: ユーザーが混在しています:"
    printf '%s\n' "$unique_users" | sed 's/^/    - /'
    echo "  内訳:"
    printf '%s\n' "${USERS[@]}" | awk -F'::' '{print "    "$1": "$2}'
    FAIL=1
  fi
fi

# 3-a) 日付の逆転
if [[ ${#DATE_BAD_ORDER[@]} -gt 0 ]]; then
  echo "✘ Created > Updated（時系列逆転）:"
  for x in "${DATE_BAD_ORDER[@]}"; do
    file="${x%%::*}"; rest="${x#*::}"
    echo "    $file"
    echo "      $rest"
  done
  FAIL=1
fi

# 3-b) Updated が直近コミットより古い
if [[ ${#DATE_STALE[@]} -gt 0 ]]; then
  echo "✘ Updated が直近コミット時刻より古い（ヘッダ未更新の疑い）:"
  for x in "${DATE_STALE[@]}"; do
    file="${x%%::*}"; rest="${x#*::}"
    echo "    $file"
    echo "      $rest"
  done
  FAIL=1
fi

if [[ $FAIL -ne 0 ]]; then
  exit 1
fi
echo "✔ 42 ヘッダ整合性 OK (user: $unique_users / files: ${#USERS[@]})"
