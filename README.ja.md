*このプロジェクトは 42 のカリキュラムの一環として kjikuhar によって作成されました。*

# Philosophers

> *"哲学がこんなにも致命的だとは思わなかった。"*

> **注記**: このファイルは `README.md`（subject 必須、英語版）の作者用日本語訳です。提出時には削除し、英語版のみを残します。

## 概要 (Description)

`Philosophers` は 42 School の課題で、C 言語によるマルチスレッドと同期処理の基礎を学ぶことを目的としています。**Dining Philosophers Problem**（食事する哲学者問題）を題材に、`N` 人の哲学者が円卓に座り、**eating（食事）/ sleeping（睡眠）/ thinking（思考）** の 3 状態を循環しながら、限られた本数の fork を共有して **デッドロック** と **starvation** を回避します。

本リポジトリは **mandatory part のみ** を実装しています。各哲学者は POSIX スレッド、各 fork は `pthread_mutex_t`、独立した monitor スレッドが死亡判定と `must_eat` 完了判定を行います。subject の厳密な要件に従う必要があります：

- data race を発生させない。
- メモリリークを発生させない。
- 死亡時刻のログは実際の死亡から 10 ms 以内に出す。
- 哲学者が死亡したら、それ以降のログを出してはいけない。

## 手順 (Instructions)

### ビルド

```sh
cd philo
make
```

`philo/philosophers` 実行ファイルが生成される。コンパイルは `cc -Wall -Wextra -Werror -pthread`。

### 実行

```
./philosophers number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

| 引数 | 単位 | 意味 |
|------|------|------|
| `number_of_philosophers` | — | 哲学者の数（= fork の数） |
| `time_to_die` | ms | 最後に食事を開始してからこの時間を超えると死亡 |
| `time_to_eat` | ms | 食事に要する時間（fork 2 本を保持） |
| `time_to_sleep` | ms | 睡眠に要する時間 |
| `number_of_times_each_philosopher_must_eat` | 回 | 任意。全員がこの回数食べたら simulation 終了 |

例：

```sh
./philosophers 5 800 200 200       # 死亡が起きるまで実行
./philosophers 5 800 200 200 7     # 全員 7 回食べたら終了
./philosophers 1 800 200 200       # fork 1 本のみ → 必ず死亡
```

### 出力フォーマット

```
<timestamp_in_ms> <philosopher_id> <state>
```

状態は `has taken a fork`、`is eating`、`is sleeping`、`is thinking`、`died`。哲学者の id は 1 〜 `number_of_philosophers` の 1-origin。

### Make ターゲット

| ターゲット | 用途 |
|-----------|------|
| `make`（または `make all`） | 通常ビルド |
| `make clean` | object ファイル削除 |
| `make fclean` | object ファイル + バイナリ削除 |
| `make re` | `fclean` 後 `all` |
| `make dev` | 厳格警告フラグ（`-Wpedantic` 等）+ デバッグシンボル |
| `make asan` | AddressSanitizer + UBSan（出力 `philosophers_asan`） |
| `make tsan` | ThreadSanitizer（出力 `philosophers_tsan`） |
| `make valgrind [ARGS="..."]` | valgrind でメモリリーク検査（Linux 専用） |

### テスト

```sh
bash scripts/test_philo.sh       # subject 公式テスト + 補助シナリオ
bash scripts/test_valgrind.sh    # valgrind による複数シナリオ leak 検査（Linux）
```

CI（`.github/workflows/ci.yml`）では 5 ジョブを並列実行：norminette、forbidden function 検査、42 ヘッダ整合性、シナリオテスト、valgrind。

## 参考資料 (Resources)

### 古典的文献

- E. W. Dijkstra, *Hierarchical ordering of sequential processes*, Acta Informatica, 1971 — Dining Philosophers Problem の原典。
- C. A. R. Hoare, *Communicating Sequential Processes*, 1978。
- M. Chandy and J. Misra, *The drinking philosophers problem*, ACM TOPLAS, 1984 — トークンパッシングによる deadlock-free な別解。
- *Operating System Concepts*（Silberschatz, Galvin, Gagne）— 同期処理と古典的問題の章。

### 技術ドキュメント

- POSIX `pthread_create(3)`、`pthread_mutex_init(3)`、`pthread_mutex_lock(3)`、`pthread_join(3)`、`gettimeofday(2)`、`usleep(3)` の Linux man page。
- *The Linux Programming Interface*（Michael Kerrisk）の thread / mutex 章。
- Apple *Threading Programming Guide*。
- LLVM ThreadSanitizer / AddressSanitizer ドキュメント。
- valgrind manual — `--tool=memcheck`、`--tool=helgrind`。

### AI の利用について

本リポジトリでは生成 AI（Anthropic Claude）を **コードジェネレータとしてではなく、対話相手・補助ツール** として使用しました。利用範囲は以下に限定：

- **ツール周り**: Makefile ターゲット、git hooks（pre-commit / commit-msg）、GitHub Actions CI ジョブ、42 ヘッダ整合性 checker、禁止関数 checker のドラフト作成。哲学者アルゴリズム本体ではなくビルド環境側。
- **概念ディスカッション**: deadlock 回避戦略（min-first / odd-even / Chandy-Misra）のトレードオフ、`PTHREAD_MUTEX_ERRORCHECK` 等の mutex 属性の意味、TSan / ASan / valgrind の違いの整理。
- **テストシナリオ設計**: subject の公式テスト（`1 800 200 200`、`5 800 200 200`、`5 800 200 200 7`、`4 410 200 200`、`4 310 200 100`、N=2 死亡時刻精度）を bash テストハーネスに落とし込む作業。
- **ドキュメント作成**: README、コード内コメント、`.github/copilot-instructions.md` のレビュー指針。

`philo/*.c` と `philo/philo.h` の設計判断 — 4 種類の mutex（`forks[]` / `print_mutex` / `death_mutex` / philo ごとの `meal_mutex`）の役割分担、id 偶奇による fork 取得順 + 初期 stagger、奇数 N 用の明示的 think 遅延、N=1 特殊扱い、monitor ループ — は author が逐行レビュー・commit。AI は **退屈なツール作業** と **peer review 的な議論** にのみ使い、**理解の代替** にはしないという方針です。

## 技術的選択 (Technical choices)

### ファイル構成

```
philo/
├── philo.h        構造体定義（t_sim, t_philo）と関数プロトタイプ
├── main.c         エントリーポイント：引数受け取り、スレッド生成・join・解放
├── init.c         引数検証、mutex 初期化、哲学者初期化
├── routine.c      哲学者スレッドのルーチン：take_forks / do_eat / release_forks / do_sleep_think
├── monitor.c      監視スレッド：死亡判定と must_eat 完了判定
├── utils.c        ft_atoi、current_time_ms、log_event（mutex 保護 printf）
└── Makefile
```

### 同期プリミティブ

| Mutex | 守るデータ |
|-------|-----------|
| `sim->forks[i]`（N 個） | i 番目の fork の取得権 |
| `sim->print_mutex` | `printf` の直列化（ログが混ざらないように） |
| `sim->death_mutex` | `sim->finished` 終了フラグ |
| `philos[i].meal_mutex`（哲学者ごと） | その哲学者の `last_meal_time` と `meals_eaten` |

### Deadlock 回避

`take_forks` で、**奇数 id** は **左 fork 先取り**、**偶数 id** は **右 fork 先取り**。全員が同じ方向に取らないので循環待ちが発生せず deadlock しない。

### 公平性

`number_of_philosophers` が奇数のとき、odd/even ルール単独だと wrap-around 哲学者が非対称な contention に置かれる。追加で 2 つの工夫：

- **初期 stagger** — 偶数 id の哲学者はループ開始前に `time_to_eat / 2` ms 待つ。開始時の同時 fork 争奪を崩す。
- **明示的 think 遅延** — `time_to_die − time_to_eat − time_to_sleep > 100 ms` のとき、`is thinking` ログ後に slack の半分待つ。隣の哲学者が解放された fork を取る時間を与える。

### N=1 のエッジケース

`number_of_philosophers == 1` のとき左右の fork は同一 mutex になる。同じ mutex を 2 回 lock すると自スレッドデッドロックになるため、routine 側で検出して `has taken a fork` を 1 回だけログし、`time_to_die + 1 ms` 待ってから return する。monitor が `died` を出し、スレッドは clean に join される。

### Monitor ループ

```
while (!finished) {
    if (check_all_satisfied()) return;   // must_eat 到達 → finished
    if (check_one_death())     return;   // 誰かが time_to_die 超過 → "died" ログ → finished
    usleep(1000);
}
```

`check_all_satisfied` を `check_one_death` より先に評価することで、`must_eat` 完了とぎりぎりの starvation 判定が同 poll に重なった場合に **正常終了を優先** する。

### ログ race の回避

`log_event` は `print_mutex` → `death_mutex` の順に取得し、`sim->finished` が false の場合のみ印字。`declare_death` も同じ順序で取得するため、`died` 後にどの哲学者の状態ログも印字されないことを保証する。

## 適合性サマリ (Conformance)

| 42 grading flag | 状態 |
|-----------------|------|
| Norminette | clean（norminette 3.3.55） |
| 禁止関数 | 使用なし（`nm` でバイナリの未定義シンボルを subject の許可リストと突合） |
| クラッシュ・未定義動作 | 観測なし |
| Data race | ThreadSanitizer で報告なし |
| メモリリーク | valgrind（`--errors-for-leak-kinds=definite,indirect,possible`）で報告なし |
| 10 ms 以内の死亡通知 | クリア（`2 100 60 60` で死亡 101–102 ms） |
| Subject 公式テスト（mandatory） | すべて通過 — `scripts/test_philo.sh` 参照 |

## License / Author

42 Tokyo カリキュラム提出物。
Author: `kjikuhar`
