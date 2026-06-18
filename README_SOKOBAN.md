# IT3160 Sokoban

Project Sokoban gồm 3 thành phần chính:

- `solver/`: C++ solver đa thuật toán (A*, BFS, DFS, UCS).
- `A_star/`: phiên bản A* trước đó và code tham khảo.
- `sokoban_gui/`: GUI Pygame cho phép chơi, xem, và gọi solver.
- `data/`: chứa các level mẫu và file level người dùng.

## Cấu trúc thư mục

```text
IT3160_Sokoban/
|-- A_star/                # Code A* ban đầu
|   |-- include/
|   |-- src/
|-- solver/                # C++ solver hiện tại
|   |-- include/
|   |-- src/
|   |-- Makefile
|   |-- sokoban.exe        # nếu đã build
|-- sokoban_gui/           # GUI Python + solver client
|   |-- bin/
|   |-- src/
|   |-- assets/
|-- data/
|   |-- level1.txt
|   |-- level2.txt
|   |-- level3.txt
|   |-- level4.txt
|   |-- custom.txt
|-- README_SOKOBAN.md      # tài liệu hiện tại
```

## Yêu cầu môi trường

- Python 3.x
- Pygame
- `g++` (hoặc GCC) để biên dịch solver C++

Cài Pygame:

```powershell
python -m pip install pygame
```

Kiểm tra `g++`:

```powershell
g++ --version
```

## Cách biên dịch solver C++

Từ thư mục `solver/`:

```powershell
cd "e:/NMTTNT - IT3160/IT3160_Sokoban/solver"
# Nếu bạn có make
make
```

Hoặc dùng g++ trực tiếp:

```powershell
g++ -O3 -std=c++17 -Wall -Wextra -Iinclude src\*.cpp -o sokoban.exe
```

Sau khi xây dựng thành công, file chạy sẽ là `solver/sokoban.exe`.

## Cách chạy solver C++

Solver hiện tại dùng đầu vào chuẩn là board đã có dòng `rows cols` ở đầu. Bạn có thể chạy như sau:

```powershell
Get-Content ..\data\level1.txt | .\sokoban.exe astar
```

Tùy chọn thuật toán:

- `astar`
- `bfs`
- `dfs`
- `ucs`

Ví dụ:

```powershell
Get-Content ..\data\level1.txt | .\sokoban.exe bfs
Get-Content ..\data\level1.txt | .\sokoban.exe dfs
Get-Content ..\data\level1.txt | .\sokoban.exe ucs
```

## Cấu trúc solver hiện tại

`solver/include/`

- `board.h`: định nghĩa board và trạng thái.
- `distance.h`: tìm đường và khoảng cách di chuyển của người chơi.
- `deadlock.h`: phát hiện bế tắc đơn giản và freeze deadlock.
- `heuristic.h`: hàm heuristic A*.
- `search.h`: khai báo các hàm giải thuật.
- `bfs.h`, `dfs.h`, `ucs.h`: header helper cho từng thuật toán.

`solver/src/`

- `main.cpp`: chương trình chính, chọn thuật toán bằng tham số dòng lệnh.
- `search_astar.cpp`: cài đặt A*.
- `search_bfs.cpp`: cài đặt BFS.
- `search_dfs.cpp`: cài đặt DFS.
- `search_ucs.cpp`: cài đặt UCS.
- `board.cpp`, `distance.cpp`, `deadlock.cpp`, `heuristic.cpp`: module hỗ trợ chung.

## Cách chạy GUI

Từ thư mục gốc dự án:

```powershell
python sokoban_gui\main.py
```

GUI hỗ trợ:

- di chuyển bằng `WASD` hoặc phím mũi tên
- `R`: chơi lại level
- `P`: tạm dừng / tiếp tục
- nút `SOLVE`: chọn thuật toán và giải tự động
- nút `STEP`: chạy từng bước của lời giải
- `RETRY`, `NEXT`, các chức năng điều khiển level

## Định dạng level

Các file level trong `data/` sử dụng ký hiệu chuẩn Sokoban:

```text
#  : tường
   : ô trống
$  : thùng
.  : đích
@  : người chơi
*  : thùng trên đích
+  : người chơi trên đích
```

Lưu ý: GUI có thể đọc cả level có hoặc không có dòng `rows cols` ở đầu. Solver C++ hiện tại cũng có thể được điều chỉnh để nhận đầu vào do GUI tạo.

## Thuật toán

Solver hỗ trợ 4 thuật toán:

- `A*`: tìm kiếm theo chi phí `g + h` với heuristic Manhattan matching và deadlock detection.
- `BFS`: tìm kiếm theo chiều rộng, bảo đảm tìm đường ngắn nhất theo số lần đẩy.
- `DFS`: tìm kiếm theo chiều sâu.
- `UCS`: Uniform Cost Search theo chi phí `g` (số lần đẩy).

## Ghi chú

- Luồng giải chung: mã trạng thái chứa vị trí người chơi và danh sách vị trí thùng.
- Mỗi bước successor là một lần đẩy thùng hợp lệ, kèm theo đường đi của người chơi tới vị trí đẩy.
- Deadlock detection giúp loại bỏ các trạng thái đẩy vào bế tắc.
- Chuỗi giải trả về gồm:
  - ký tự thường `l/u/r/d` cho từng bước đi của người chơi
  - ký tự hoa `L/U/R/D` cho bước đẩy thùng

## Ví dụ chạy nhanh

```powershell
# build solver
cd "e:/NMTTNT - IT3160/IT3160_Sokoban/solver"
g++ -O3 -std=c++17 -Wall -Wextra -Iinclude src\*.cpp -o sokoban.exe

# chạy A*
Get-Content ..\data\level1.txt | .\sokoban.exe astar

# chạy BFS
Get-Content ..\data\level1.txt | .\sokoban.exe bfs
```
