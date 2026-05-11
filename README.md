*This project has been created as part of the 42 curriculum by kjikuhar.*

# Philosophers

> *"I never thought philosophy would be so deadly."*

## Description

`Philosophers` is the 42 School project that introduces the basics of multithreading and synchronization in C. The goal is to simulate the classical
**Dining Philosophers Problem**, in which `N` philosophers sit around a round table alternating between three states — **eating**, **sleeping**, and
**thinking** — and must share a limited number of forks to avoid both **deadlock** and **starvation**.

This repository implements the **mandatory part** only: each philosopher is a POSIX thread, each fork is a `pthread_mutex_t`, and a separate monitor
thread watches for death and `must_eat` completion. The simulation must satisfy the strict rules of the subject:

- No data race.
- No memory leak.
- Death must be reported within 10 ms of its actual time.
- Once a philosopher dies, no further log line must be printed.

## Instructions

### Build

```sh
cd philo
make
```

This produces the executable `philo/philo`. Compilation uses `cc -Wall -Wextra -Werror -pthread`.

### Run

```
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

| Argument | Unit | Meaning |
|----------|------|---------|
| `number_of_philosophers` | — | Number of philosophers and number of forks |
| `time_to_die` | ms | A philosopher who has not started eating within this delay since their last meal dies |
| `time_to_eat` | ms | How long a philosopher spends eating (holding two forks) |
| `time_to_sleep` | ms | How long a philosopher spends sleeping |
| `number_of_times_each_philosopher_must_eat` | times | Optional. If every philosopher has eaten at least this many times, the simulation stops |

Examples:

```sh
./philo 5 800 200 200       # runs until somebody dies
./philo 5 800 200 200 7     # stops after every philosopher has eaten 7 times
./philo 1 800 200 200       # one philosopher with only one fork: must die
```

### Output format

```
<timestamp_in_ms> <philosopher_id> <state>
```

States: `has taken a fork`, `is eating`, `is sleeping`, `is thinking`, `died`. Philosopher IDs go from `1` to `number_of_philosophers`.

### Make targets

| Target | Purpose |
|--------|---------|
| `make` (or `make all`) | Standard build |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and binaries |
| `make re` | `fclean` then `all` |
| `make dev` | Build with stricter warnings (`-Wpedantic`, `-Wshadow`, `-Wformat=2`, etc.) plus debug symbols |
| `make asan` | Build with AddressSanitizer + UBSan (output `philosophers_asan`) |
| `make tsan` | Build with ThreadSanitizer (output `philosophers_tsan`) |
| `make valgrind [ARGS="..."]` | Run the binary under valgrind with strict leak checking (Linux only) |

### Tests

```sh
bash scripts/test_philo.sh       # subject test cases + auxiliary scenarios
bash scripts/test_valgrind.sh    # memory-leak scenarios (Linux only)
```

Continuous integration (`.github/workflows/ci.yml`) runs five jobs in parallel: norminette, forbidden-function audit, 42 header consistency, scenario
tests, and valgrind.

## Resources

### Classical references

- Wikipedia, [*Dining philosophers problem*](https://en.wikipedia.org/wiki/Dining_philosophers_problem) — concise overview of the problem and the
  classical solutions (resource hierarchy, Chandy/Misra, arbitrator).
- E. W. Dijkstra, *Hierarchical ordering of sequential processes*, Acta Informatica, 1971 — the original formulation of the Dining Philosophers
  Problem.
- C. A. R. Hoare, *Communicating Sequential Processes*, 1978.
- M. Chandy and J. Misra, *The drinking philosophers problem*, ACM TOPLAS, 1984 — alternative deadlock-free formulation by token passing.
- *Operating System Concepts* (Silberschatz, Galvin, Gagne) — chapter on synchronization and classical problems.

### Technical documentation

- POSIX `pthread_create(3)`, `pthread_mutex_init(3)`, `pthread_mutex_lock(3)`, `pthread_join(3)`, `gettimeofday(2)`, `usleep(3)` — Linux man pages.
- *The Linux Programming Interface* (Michael Kerrisk), chapters on threads and mutexes.
- Apple's *Threading Programming Guide*.
- LLVM ThreadSanitizer and AddressSanitizer documentation.
- Valgrind manual — `--tool=memcheck`, `--tool=helgrind`.

### Use of AI

Generative AI (Anthropic Claude) was used in this repository as a **collaborative thinking partner** rather than as a code generator. The use was
limited to the following tasks:

- **Tooling setup**: drafting the project's Makefile targets, git hooks (pre-commit, commit-msg), GitHub Actions CI jobs, the 42-header consistency
  checker, and the forbidden-function checker. These touch the build environment, not the philosophers algorithm itself.
- **Conceptual discussion**: clarifying the trade-offs between deadlock-avoidance strategies (min-first, even/odd alternation, Chandy/Misra),
  explaining the meaning of mutex attributes such as `PTHREAD_MUTEX_ERRORCHECK`, and walking through the difference between TSan, ASan, and valgrind.
- **Test scenario design**: enumerating the subject's official test cases (`1 800 200 200`, `5 800 200 200`, `5 800 200 200 7`, `4 410 200 200`,
  `4 310 200 100`, N=2 timing precision) and shaping them into a reproducible bash harness.
- **Documentation drafting**: this README, in-code comments, and the project's `.github/copilot-instructions.md` review rubric.

All design decisions in `philo/*.c` and `philo/philo.h` — the architecture of the four mutex categories (`forks[]`, `print_mutex`, `death_mutex`,
per-philosopher `meal_mutex`), the even/odd fork acquisition order with initial stagger, the explicit "think" delay for fairness on odd N, the `N=1`
special case, and the monitor loop — were reasoned through, reviewed line by line, and committed by the author. The intent has been to keep AI on the
side of *tedious tooling* and *peer-style discussion*, never as a substitute for understanding.

## Technical choices

### File layout

```
philo/
├── philo.h        Type definitions (t_sim, t_philo) and function prototypes
├── main.c         Entry point: argument parsing entry, thread creation, join, cleanup
├── init.c         Parameter validation, mutex initialization, philosopher setup
├── routine.c      Philosopher thread routine: take_forks / do_eat / release_forks / do_sleep_think
├── monitor.c      Monitor thread: death detection and must_eat completion check
├── utils.c        ft_atoi, current_time_ms, log_event (mutex-protected printf)
└── Makefile
```

### Synchronization primitives

| Mutex | Protects |
|-------|----------|
| `sim->forks[i]` (N entries) | Acquisition of the i-th fork |
| `sim->print_mutex` | Serializes `printf` so log lines never overlap |
| `sim->death_mutex` | The `sim->finished` simulation-stop flag |
| `philos[i].meal_mutex` (per philosopher) | `last_meal_time` and `meals_eaten` for one philosopher |

### Deadlock avoidance

In `take_forks`, philosophers with **odd id** lock the **left fork first**, those with **even id** lock the **right fork first**. Because the
philosophers do not all acquire in the same direction, no circular wait can form.

### Fairness

For odd `number_of_philosophers`, the odd/even rule alone leaves the wrap-around philosopher in an asymmetric contention pattern. Two additional
techniques are used:

- **Initial stagger** — even-id philosophers wait `time_to_eat / 2` ms before entering the loop, breaking the synchronous fork rush at start.
- **Explicit thinking delay** — when `time_to_die − time_to_eat − time_to_sleep > 100 ms`, every philosopher waits half of that slack after logging
  `is thinking`, giving neighbours time to acquire the released forks.

### Single-philosopher edge case

When `number_of_philosophers == 1`, the philosopher's left and right forks are the same mutex. The routine detects this and, instead of attempting to
lock the same mutex twice (which would self-deadlock), logs `has taken a fork` once and then sleeps for `time_to_die + 1 ms`. The monitor reports
`died` and joins the thread cleanly.

### Monitor loop

```
while (!finished) {
    if (check_all_satisfied()) return;   // must_eat reached → finished
    if (check_one_death())     return;   // any philo exceeded time_to_die → log "died" → finished
    usleep(1000);
}
```

`check_all_satisfied` is evaluated *before* `check_one_death` so that the simulation prefers terminating successfully when a `must_eat` target is
reached at exactly the same poll as a borderline starvation timer.

### Log race avoidance

`log_event` acquires `print_mutex` and then `death_mutex`, and prints only if `sim->finished` is still false. Combined with the same acquisition order
inside `declare_death`, this guarantees that no log line is printed after `died`.

## Conformance summary

| 42 grading flag | Status |
|-----------------|--------|
| Norminette | clean (norminette 3.3.55) |
| Forbidden function | none used (verified by `nm` against the subject's allow-list) |
| Crash / undefined behaviour | none observed |
| Data race | none reported by ThreadSanitizer |
| Memory leak | none reported by valgrind (`--errors-for-leak-kinds=definite,indirect,possible`) |
| Death announcement within 10 ms | yes (`2 100 60 60` → death at 101–102 ms) |
| Subject test cases (mandatory) | all pass — see `scripts/test_philo.sh` |

## License / Author

Submitted as part of the 42 Tokyo curriculum.
Author: `kjikuhar`
