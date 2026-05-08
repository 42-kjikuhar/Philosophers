# GitHub Copilot — Review & Generation Instructions

このリポジトリは **42 School "Philosophers" 課題**（食事する哲学者問題のスレッド/プロセス実装）です。
コード生成・PRレビューを行う際は以下の規約に従って指摘・提案してください。

---

## 1. プロジェクト構造
- `philo/` … 必須課題（pthread + mutex 実装）

> **bonus は当面対象外**: 初回提出は必須課題のみ。`philo_bonus/` は現在リポジトリに存在しません。bonus に関する提案・レビューは行わないでください。

最終提出時には `.github/`, `.githooks/`, `.gitignore`, `en.subject.pdf`, `scripts/` 等の補助ファイルは削除予定。**機能本体（philo/）は提出物として独立して動作する** ことを常に意識してレビューしてください。

---

## 2. 42 Norm（必ず指摘）

`norminette` で機械的にチェックされますが、以下は特によく見落とされます：

- **1行 ≤ 80 文字**（タブ含む実幅）
- **関数 ≤ 25 行**（中括弧除く）
- **1ファイル ≤ 5 関数**
- **関数引数 ≤ 4 個**
- **変数宣言は関数先頭**（C89 スタイル）。宣言と代入を同一行で行わない（ポインタ・配列を除く）
- `for` / `do-while` / `switch` / `case` / 三項演算子 / VLA / `typedef`内struct定義 は **禁止**
- `goto` 禁止
- 関数の中括弧は次行
- インデントはタブ（スペース禁止）
- グローバル変数（非 const）禁止
- マクロは大文字スネーク + 簡単な式のみ
- 構造体名は `t_xxx`、関数ポインタ型は `f_xxx`、enum は `e_xxx`
- すべての `.c` / `.h` の先頭に **42 ヘッダ**（`Created:` / `Updated:` 行を含む 11 行ブロック）を付ける

---

## 3. 禁止関数（subject 準拠）

レビュー時に以下のリスト外の libc / システム関数呼び出しを見つけたら **必ず指摘** してください。

### philo/ で許可される関数のみ
```
memset, printf, malloc, free, write, usleep, gettimeofday,
pthread_create, pthread_detach, pthread_join,
pthread_mutex_init, pthread_mutex_destroy,
pthread_mutex_lock, pthread_mutex_unlock
```

### よくある違反（必ず指摘）
- `atoi` → 自前 `ft_atoi` を実装する（オーバーフロー検出も）
- `sprintf` / `snprintf` → `printf` または `write` で置換
- `strcpy` / `strncpy` / `strcat` → 利用不可、自前で書く
- `sleep` / `nanosleep` → `usleep` のみ許可
- `bzero` → `memset` を使う
- `perror` / `strerror` → 直接 `write(2, ...)` で書く

---

## 4. 並行処理レビュー観点（philo/ の核心）

以下は機械チェックでは拾えないが **採点 / 評価で重視** されるポイント。レビュー時に積極的に指摘してください。

### データ競合
- 共有変数（`last_meal_time`, `is_dead`, `meal_count` 等）への読み書きは **対応する mutex で必ず保護されているか**
- `printf` のログ出力は **シリアライズ用 mutex（log_mutex / print_mutex）を取って** いるか — 取らないとログが破壊される
- `gettimeofday` 自体はスレッドセーフだが、その結果を構造体に書き込むときは保護必要

### デッドロック
- N 人哲学者が同時に「左 fork → 右 fork」と取ると **循環待ち**。回避策：
  - 偶数番号は右→左、奇数番号は左→右、で取得順を変える
  - もしくは ID 順に低い番号の fork から取るルール
- **N=1** の特殊ケース：fork が 1 本しかなく eat 不可能 → `time_to_die` 経過で死亡させる、`pthread_create` せず終了するなど明示的に処理する必要あり
- `pthread_join` / `pthread_detach` の選び方：監視スレッドとの兼ね合いで一貫している必要

### 時間精度
- `usleep(time_to_eat * 1000)` で **長く眠りすぎ** ない工夫（短い `usleep` ループで死亡時刻を超えないようチェック）
- `time_to_die` 直前の状態遷移（特に sleeping → thinking → eating の境界）でログの順序が乱れないか
- ms 単位の経過時間関数 `get_time_ms()` は `gettimeofday` 由来で、`tv_sec * 1000 + tv_usec / 1000` の **オーバーフロー** に注意（`long` 戻り値推奨）

### 死亡判定
- 死亡フラグを立てた後、**他のスレッドが追加ログを出さない** こと（フラグ確認 → ログ出力の間に mutex を保持）
- monitor スレッド方式 / ループ内チェック方式どちらも可だが、**「死んでから検出までの遅延」が time_to_die を大きく超えない** こと（10ms オーダー）
- `must_eat` 回数指定時は **全員が必要回数食べたら正常終了**（死亡判定なしで）

---

## 5. メモリ・リソース管理
- `malloc` の戻り値は必ず `NULL` チェック
- mutex は必ず `pthread_mutex_destroy` で解放、スレッドは必ず `join` か `detach`
- `valgrind --tool=helgrind` / `valgrind --tool=drd` でデータ競合検出されないこと（推奨）
- `valgrind --leak-check=full` でリーク 0

---

## 6. ヘッダ・ファイル運用
- `.c`/`.h` 新規作成時は 42 ヘッダを必ず付与
- `Updated:` 行は `c_formatter_42` で自動更新される（pre-commit hook 設定済み）
- ヘッダのユーザー名は一貫させる（混在しない）

---

## 7. コミットメッセージ規約

PR レビュー時、コミットが規約違反なら指摘してください。

- **1行のみ**（本文・空行を含めない）
- **日本語**
- **prefix 必須**: `add:` / `delete:` / `change:` / `fix:` / `docs:` / `chore:`
- `Co-authored-by:` を含めない（Copilot / Claude も co-author に入れない）
- 例: `add: time_to_die の判定処理を追加`

`Merge ...` / `Revert ...` は例外。

---

## 8. PR 規約

- ベースブランチは **`develop`**（`main` への直 push / 直マージは原則しない）
- PR テンプレート（`.github/PULL_REQUEST_TEMPLATE.md`）の以下を埋める：
  - 概要 / 変更点 / 背景・目的 / 動作確認 / レビュー観点
- マージ前に **`make` / `make norm` / `make forbidden`** がすべてクリーンであること
- 大量変更は分割推奨（並行処理ロジックの変更とリファクタを混ぜない）

---

## 9. レビュー時の優先度

レビューコメントを付ける際は以下の優先度で：

1. **🚨 致命的**: 禁止関数使用 / データ競合 / デッドロック条件 / メモリリーク
2. **⚠️ 重要**: Norm 違反 / N=1 等エッジケース未考慮 / 時間精度問題
3. **💡 改善**: 命名 / 関数分割 / コメント不足 / リファクタ提案

低優先度のコメントを大量に付けるより、**致命的・重要を確実に拾う** ことを優先してください。

---

## 10. やってはいけない提案

以下は **提案しないでください**：

- libft / 他課題の関数を import する案（philo は **完全自己完結** が前提）
- C++ 機能 / GNU 拡張（`__attribute__` 等は最小限）
- スレッドプール等の過剰な抽象化（subject の範囲を超える）
- `setjmp` / `longjmp`
- シグナルハンドラ（philo では原則使わない）
