

cargo new edit_distance --lib
cd edit_distance
// add rust code...

mkdir pytest
python -m venv .venv
source .venv/bin/activate.fish (for fish shells)
pip install maturin

maturin develop
call rust code from python

enjoy :)
