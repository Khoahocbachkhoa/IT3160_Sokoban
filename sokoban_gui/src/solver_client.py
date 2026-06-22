import hashlib
import re
import subprocess
from dataclasses import dataclass

from config import (
    CPP_SOLVER_BUILD_STAMP,
    CPP_SOLVER_EXE,
    SOLVER_BIN_DIR,
    SOLVER_DIR,
    SOLVER_TIMEOUT_SECONDS,
)


ALGORITHMS = ("A Star", "DFS", "BFS", "UCS")
ALGORITHM_ARGUMENTS = {
    "A Star": "astar",
    "DFS": "dfs",
    "BFS": "bfs",
    "UCS": "ucs",
}

BUILD_FLAGS = (
    "-std=c++14",
    "-O3",
    "-Wall",
    "-Wextra",
    "-static-libgcc",
    "-static-libstdc++",
)
BUILD_VERSION = "multisolver-v1"


@dataclass
class SolverResult:
    solved: bool
    algorithm: str
    solution: str = ""
    message: str = ""
    stdout: str = ""
    stderr: str = ""


class SolverClient:
    def solve(self, grid, algorithm):
        solver_argument = ALGORITHM_ARGUMENTS.get(algorithm)
        if solver_argument is None:
            return SolverResult(
                solved=False,
                algorithm=algorithm,
                message=f"Unsupported algorithm: {algorithm}",
            )

        build_result = self._ensure_cpp_solver(algorithm)
        if build_result is not None:
            return build_result

        level_input = self._grid_to_solver_input(grid)

        try:
            completed = subprocess.run(
                [str(CPP_SOLVER_EXE), solver_argument],
                input=level_input,
                text=True,
                encoding="utf-8",
                errors="replace",
                capture_output=True,
                timeout=SOLVER_TIMEOUT_SECONDS,
                cwd=str(SOLVER_DIR),
            )
        except subprocess.TimeoutExpired:
            return SolverResult(False, algorithm, message=f"{algorithm} solver timed out")
        except OSError as exc:
            return SolverResult(False, algorithm, message=f"Cannot run C++ solver: {exc}")

        stdout = completed.stdout
        stderr = completed.stderr
        solution = self._parse_solution(stdout)

        if completed.returncode != 0:
            return SolverResult(False, algorithm, message="C++ solver exited with an error", stdout=stdout, stderr=stderr)

        if solution:
            return SolverResult(True, algorithm, solution=solution, message="Solved", stdout=stdout, stderr=stderr)

        reason = self._parse_failure_reason(stdout) or "No solution found"
        return SolverResult(False, algorithm, message=reason, stdout=stdout, stderr=stderr)

    def _ensure_cpp_solver(self, algorithm):
        sources = self._solver_sources()
        if not sources:
            return SolverResult(False, algorithm, message=f"No C++ source files found in {SOLVER_DIR / 'src'}")

        include_dir = SOLVER_DIR / "include"
        if not include_dir.is_dir():
            return SolverResult(False, algorithm, message=f"Solver include directory was not found: {include_dir}")

        build_signature = self._build_signature(sources)
        if (
            CPP_SOLVER_EXE.is_file()
            and CPP_SOLVER_BUILD_STAMP.is_file()
            and CPP_SOLVER_BUILD_STAMP.read_text(encoding="utf-8").strip() == build_signature
        ):
            return None

        SOLVER_BIN_DIR.mkdir(parents=True, exist_ok=True)
        temporary_exe = SOLVER_BIN_DIR / "cpp_solver.new.exe"
        command = [
            "g++",
            *BUILD_FLAGS,
            f"-I{include_dir}",
            *map(str, sources),
            "-o",
            str(temporary_exe),
        ]

        try:
            completed = subprocess.run(
                command,
                text=True,
                encoding="utf-8",
                errors="replace",
                capture_output=True,
                timeout=60,
                cwd=str(SOLVER_DIR),
            )
        except FileNotFoundError:
            return SolverResult(False, algorithm, message="g++ was not found. Install MinGW or add g++ to PATH.")
        except subprocess.TimeoutExpired:
            return SolverResult(False, algorithm, message="C++ solver build timed out.")

        if completed.returncode != 0:
            temporary_exe.unlink(missing_ok=True)
            return SolverResult(
                False,
                algorithm,
                message="Failed to build C++ solver.",
                stdout=completed.stdout,
                stderr=completed.stderr,
            )

        temporary_exe.replace(CPP_SOLVER_EXE)
        CPP_SOLVER_BUILD_STAMP.write_text(build_signature, encoding="utf-8")
        return None

    def _solver_sources(self):
        return sorted((SOLVER_DIR / "src").glob("*.cpp"))

    def _build_signature(self, sources):
        digest = hashlib.sha256(BUILD_VERSION.encode("utf-8"))
        for path in [*sources, *sorted((SOLVER_DIR / "include").glob("*.h"))]:
            digest.update(path.name.encode("utf-8"))
            digest.update(path.read_bytes())
        for flag in BUILD_FLAGS:
            digest.update(flag.encode("utf-8"))
        return digest.hexdigest()

    def _grid_to_solver_input(self, grid):
        rows = len(grid)
        cols = max(len(row) for row in grid) if grid else 0
        lines = ["".join(row).ljust(cols) for row in grid]
        return f"{rows} {cols}\n" + "\n".join(lines) + "\n"

    def _parse_solution(self, stdout):
        for line in stdout.splitlines():
            if ":" not in line:
                continue
            value = line.split(":", 1)[1].strip()
            if re.fullmatch(r"[lurdLURD]+", value):
                return value
        return ""

    def _parse_failure_reason(self, stdout):
        for line in stdout.splitlines():
            if ":" in line and ("do" in line.lower() or "reason" in line.lower()):
                return line.split(":", 1)[1].strip()
        return ""
