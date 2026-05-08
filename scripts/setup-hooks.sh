#!/usr/bin/env bash
# 初回セットアップ用: リポジトリローカルの git hooks を有効化する。
set -eu
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

chmod +x .githooks/* scripts/*.sh
git config core.hooksPath .githooks
echo "✔ core.hooksPath = .githooks"
echo "  hooks: $(ls .githooks | tr '\n' ' ')"
