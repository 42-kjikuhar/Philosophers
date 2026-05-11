# Philosophers

42 School の課題 **Dining Philosophers** の実装。pthread + mutex 版（mandatory only）。

## ビルド

```sh
cd philo
make
```

成果物: `philo/philosophers`

## 実行

```
./philosophers number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

| 引数 | 単位 | 意味 |
|------|------|------|
| `number_of_philosophers` | 個 | 哲学者と fork の数（1〜200） |
| `time_to_die` | ms | 最後に食事を始めてからこの時間を超えると死亡 |
| `time_to_eat` | ms | 食事に要する時間 |
| `time_to_sleep` | ms | 睡眠に要する時間 |
| `number_of_times_each_philosopher_must_eat` | 回 | 任意。全員がこの回数食べたら正常終了 |

### 例

```sh
./philosophers 5 800 200 200      # 死亡が起きるまで延々と回す
./philosophers 5 800 200 200 7    # 全員 7 回食べたら終了
./philosophers 1 800 200 200      # fork 1 本のみ → 800ms 後に死亡
```

### 出力フォーマット

```
<timestamp_ms> <philo_id> <state>
```

| state | 意味 |
|-------|------|
| `has taken a fork` | fork を 1 本取得 |
| `is eating` | 食事開始 |
| `is sleeping` | 睡眠開始 |
| `is thinking` | 思考開始 |
| `died` | 死亡（このログ以降は何も出力されない） |

`philo_id` は 1〜N の 1-origin。

## ファイル構成

```
philo/
├── philo.h        構造体・関数プロトタイプ
├── main.c         エントリーポイント、スレッド作成/join
├── init.c         引数パース、mutex 初期化、philo 初期化、解放
├── routine.c      philo スレッドのメインロジック（take_forks / do_eat / 等）
├── monitor.c      監視スレッド（死亡判定 / must_eat 完了判定）
├── utils.c        ft_atoi, current_time_ms, log_event
└── Makefile
```

## 設計

### 4 種類の mutex

| mutex | 守るデータ |
|-------|-----------|
| `sim->forks[i]` （N 個） | 各 fork の取得権 |
| `sim->print_mutex` | `printf` を直列化（ログが混ざらない） |
| `sim->death_mutex` | `sim->finished`（終了フラグ） |
| `philos[i].meal_mutex` | 各 philo の `last_meal_time`, `meals_eaten` |

### Deadlock 回避

`take_forks` で **偶数 id は右→左、奇数 id は左→右** の順に取得。全員が同じ方向に取らないことで循環待ち（古典 deadlock）が成立しない。

### 公平性向上

- **開始 stagger**: 偶数 id の philo は最初に `time_to_eat / 2` ms 待ってからループ開始。同時 fork ラッシュを崩す
- **明示的 think 遅延**: `do_sleep_think` の末尾で `time_to_die - time_to_eat - time_to_sleep > 100ms` なら半分だけ待つ。N が奇数のときの starvation を防ぐ

### N=1 特殊扱い

`philo_routine` で `n==1` を検出した場合、左 fork を 1 本取って `time_to_die + 1ms` 待つだけで return。自スレッドデッドロック（同一 mutex の二重 lock）を回避。

### 監視スレッド

`monitor_routine` が独立スレッドとして起動し、1ms 周期で全 philo を巡回：

1. `check_all_satisfied` — 全員 `must_eat` 回数到達なら `finished=1`
2. `check_one_death` — 誰かが `now - last_meal_time > time_to_die` なら `died` を印字して `finished=1`

`check_all_satisfied` を先に走らせることで、`must_eat` 完了直後の境界条件での誤死亡判定を抑止。

## ツールチェイン

リポジトリには Makefile に加えて以下が同梱：

| コマンド | 内容 |
|---------|------|
| `make` | 通常ビルド（`-Wall -Wextra -Werror -pthread`） |
| `make dev` | 厳格警告フラグ + デバッグシンボル |
| `make asan` | AddressSanitizer + UBSan ビルド → `philosophers_asan` |
| `make tsan` | ThreadSanitizer ビルド → `philosophers_tsan` |
| `make valgrind` | valgrind でメモリリーク検査（Linux 専用） |
| `make valgrind ARGS="..."` | 引数指定で valgrind 実行 |
| `make clean` / `make fclean` / `make re` | 標準クリーン操作 |

### サニタイザ実行例

```sh
make asan && ./philosophers_asan 5 800 200 200 7
make tsan && ./philosophers_tsan 5 800 200 200 7
make valgrind ARGS="5 800 200 200 3"
```

## テスト

```sh
bash scripts/test_philo.sh        # subject 6 ケース + 補助 3 ケース
bash scripts/test_valgrind.sh     # valgrind による複数シナリオ leak 検査（Linux）
```

### subject 公式テストケース

| # | コマンド | 期待 |
|---|---------|------|
| S1 | `1 800 200 200` | 哲学者は食べずに 800ms 直後に死亡 |
| S2 | `5 800 200 200` | 死亡なし |
| S3 | `5 800 200 200 7` | 全員 7 回食べて正常終了 |
| S4 | `4 410 200 200` | 死亡なし |
| S5 | `4 310 200 100` | 1 人死亡 |
| S6 | N=2 で死亡時刻の精度 | ttd から +10ms 以内 |

## CI

`.github/workflows/ci.yml` で `develop` / `main` に push / PR 時に 5 ジョブ並列実行：

| ジョブ | 内容 |
|--------|------|
| `norminette` | 42 Norm 準拠検証（norminette 3.3.55） |
| `forbidden` | バイナリの未定義シンボルを検査して subject 許可関数のみ使用していることを確認 |
| `header` | 42 ヘッダの整合性（存在 / ユーザー名統一 / 日付 / ファイル名） |
| `tests` | `scripts/test_philo.sh` を clang ビルドで実行 |
| `valgrind` | `scripts/test_valgrind.sh` を実行（メモリリーク検査） |

## 規約

### コミット

- **1 行のみ**（本文・空行なし）
- **日本語**
- **prefix 必須**: `add:` / `delete:` / `change:` / `fix:` / `docs:` / `chore:`
- **`Co-authored-by:` を含めない**

`.githooks/commit-msg` が pre-commit hook 経由で検証する。

### コード規約

42 Norm 準拠。`norminette` で機械検査、`c_formatter_42` で整形。

42 ヘッダの `Updated:` 行は **commit のたびに pre-commit hook が現在時刻に自動更新**する（`.githooks/pre-commit`）。

### 開発フロー

```sh
# 初回のみ: フックを有効化
./scripts/setup-hooks.sh

# 通常の開発
git checkout -b feature/xxx develop
# ... 編集 ...
git commit -m "add: 新機能"   # pre-commit が format + norm + build + 禁止関数 + テストを実行
git push origin feature/xxx
# develop に PR
```

base ブランチは `develop`。`main` への直接 push は原則しない。

## 禁止関数（subject 準拠）

許可関数のみ使用：
```
memset, printf, malloc, free, write, usleep, gettimeofday,
pthread_create, pthread_detach, pthread_join,
pthread_mutex_init, pthread_mutex_destroy,
pthread_mutex_lock, pthread_mutex_unlock
```

`scripts/check_forbidden.sh` がバイナリの未定義シンボルを `nm` で取り出し、許可リストと突合する。

## ライセンス / 著者

42 Tokyo の学生課題。著者: `kjikuhar`。
