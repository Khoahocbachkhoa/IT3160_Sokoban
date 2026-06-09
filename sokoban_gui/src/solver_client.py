import re
import shutil
import subprocess
from dataclasses import dataclass

from config import ASTAR_DIR, CPP_SOLVER_EXE, SOLVER_BIN_DIR, SOLVER_TIMEOUT_SECONDS


ALGORITHMS = ("A Star", "DFS", "BFS", "UCS")


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
        if algorithm != "A Star":
            return SolverResult(
                solved=False,
                algorithm=algorithm,
                message=f"{algorithm} is not connected yet. Current C++ solver supports A Star only.",
            )

        build_result = self._ensure_cpp_solver()
        if build_result is not None:
            return build_result

        level_input = self._grid_to_solver_input(grid)

        try:
            completed = subprocess.run(
                [str(CPP_SOLVER_EXE)],
                input=level_input,
                text=True,
                encoding="utf-8",
                errors="replace",
                capture_output=True,
                timeout=SOLVER_TIMEOUT_SECONDS,
                cwd=str(ASTAR_DIR),
            )
        except subprocess.TimeoutExpired:
            return SolverResult(False, algorithm, message="C++ solver timed out")
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

    def _ensure_cpp_solver(self):
        if CPP_SOLVER_EXE.exists():
            return None

        SOLVER_BIN_DIR.mkdir(parents=True, exist_ok=True)
        build_dir = self._prepare_build_sources()

        sources = [
            build_dir / "src" / "main.cpp",
            build_dir / "src" / "board.cpp",
            build_dir / "src" / "search.cpp",
            build_dir / "src" / "heuristic.cpp",
            build_dir / "src" / "distance.cpp",
            build_dir / "src" / "deadlock.cpp",
        ]

        command = ["g++", "-std=c++14", "-O2", *map(str, sources), "-o", str(CPP_SOLVER_EXE)]

        try:
            completed = subprocess.run(
                command,
                text=True,
                encoding="utf-8",
                errors="replace",
                capture_output=True,
                timeout=60,
                cwd=str(ASTAR_DIR),
            )
        except FileNotFoundError:
            return SolverResult(False, "A Star", message="g++ was not found. Install MinGW or add g++ to PATH.")
        except subprocess.TimeoutExpired:
            return SolverResult(False, "A Star", message="C++ solver build timed out.")

        if completed.returncode != 0:
            return SolverResult(
                False,
                "A Star",
                message="Failed to build C++ solver.",
                stdout=completed.stdout,
                stderr=completed.stderr,
            )

        shutil.rmtree(build_dir, ignore_errors=True)
        return None

    def _prepare_build_sources(self):
        build_dir = SOLVER_BIN_DIR / "cpp_solver_build"
        src_dir = build_dir / "src"
        include_dir = build_dir / "include"

        if build_dir.exists():
            shutil.rmtree(build_dir)

        shutil.copytree(ASTAR_DIR / "src", src_dir)
        shutil.copytree(ASTAR_DIR / "include", include_dir)

        distance_cpp = src_dir / "distance.cpp"
        text = distance_cpp.read_text(encoding="utf-8")
        text = text.replace(
            "auto [curr_p, dist] = q.front();",
            "int curr_p = q.front().first;\n        int dist = q.front().second;",
        )
        text = text.replace("q.push({start_p, 0});", "q.push(std::make_pair(start_p, 0));")
        text = text.replace("q.push({next_p, dist + 1});", "q.push(std::make_pair(next_p, dist + 1));")
        distance_cpp.write_text(text, encoding="utf-8")

        return build_dir

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
