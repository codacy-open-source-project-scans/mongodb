#!/usr/bin/env python3
"""
This script checks to make sure poetry.lock is synced with pyproject.toml
It will return non zero if poetry.lock and pyproject.toml are not synced
"""

POETRY_LOCK_V151 = """# This file is automatically @generated by Poetry 1.5.1 and should not be changed by hand."""

import subprocess

# This has a great error message as part of the failure case
subprocess.run(["poetry", "lock", "--check"], check=True)

# Check if the poetry lock file was generated with poetry 1.5.1
with open("poetry.lock", "r") as poetry_lock:
    if POETRY_LOCK_V151 not in poetry_lock.read(len(POETRY_LOCK_V151)):
        raise Exception("""Poetry lockfile was not generated by poetry 1.5.1.
    Make sure to have poetry 1.5.1 installed when running poetry lock.
    If you are seeing this message please follow the poetry install steps in docs/building.md.""")
