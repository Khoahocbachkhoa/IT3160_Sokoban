# IT3160 Sokoban

Project gồm chương trình tự động giải Sokoban với 4 thuật toán sử dụng: DFS, BFS, UCS và A*.

- **A*** là solver được tối ưu riêng cho Sokoban, dùng heuristic, phát hiện
  deadlock và tìm kiếm theo các lần đẩy hợp lệ.
- **BFS, DFS và UCS** triển khai thuật toán tìm kiếm cổ điển, không dùng heuristic hoặc tối ưu chuyên biệt
  Sokoban.

## Cấu trúc project

```text
IT3160_Sokoban/
|-- solver/
|   |-- include/                 # Khai báo thư viện
|   |-- src/                     # Mã nguồn C++
|   |-- Makefile
|-- sokoban_gui/
|   |-- assets/                  # Ảnh và font
|   |-- bin/                     # Solver do GUI tự build
|   |-- src/                     # Game, renderer, level loader, gui
|   |-- config.py
|   |-- main.py
|-- data/
|   |-- level1.txt ... level12.txt
|   |-- custom.txt
|   |-- BENCHMARK_RESULTS.md
|   |-- SOURCE_SOKOBAN_PYTHON_AI.md
|-- README_SOKOBAN.md
```

## Yêu cầu môi trường

- Python 3
- Pygame
- `g++` có trong `PATH`

Cài Pygame:

```powershell
python -m pip install pygame
```

Kiểm tra compiler:

```powershell
g++ --version
```

## Chạy GUI

Từ thư mục `IT3160_Sokoban`:

```powershell
python sokoban_gui\main.py
```

Điều khiển:

- `WASD` hoặc phím mũi tên: di chuyển;
- `R`: chơi lại level;
- `P`: pause/resume;
- `SOLVE`: chọn A*, BFS, DFS hoặc UCS;
- `STEP`: thực hiện một bước trong lời giải;
- `RETRY`: tải lại level;
- `NEXT`: chuyển level.

## Build và chạy solver trực tiếp

Từ thư mục `solver`:

```powershell
g++ -std=c++14 -O3 -Wall -Wextra -static-libgcc -static-libstdc++ `
    -Iinclude src\*.cpp -o sokoban.exe
```

Chạy một thuật toán:

```powershell
Get-Content ..\data\level1.txt | .\sokoban.exe astar
Get-Content ..\data\level1.txt | .\sokoban.exe bfs
Get-Content ..\data\level1.txt | .\sokoban.exe dfs
Get-Content ..\data\level1.txt | .\sokoban.exe ucs
```

Nếu không truyền đối số, solver mặc định chạy A*.

## Định dạng level

Ký hiệu Sokoban:

```text
#  tường
   ô trống
$  thùng
.  goal
@  người chơi
*  thùng trên goal
+  người chơi trên goal
```

Một level hợp lệ cần đúng một người chơi và thông thường có số thùng bằng số
goal. Bạn có thể tự tạo level theo ý mình và thử nghiệm