#!/bin/bash
# Створюємо директорію build (якщо її немає), переходимо туди і білдимо
mkdir -p build
cd build
cmake ..
make
