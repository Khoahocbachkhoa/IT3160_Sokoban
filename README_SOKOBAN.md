# Sokoban Solver (A* Algorithm)

Chương trình giải Sokoban bằng thuật toán A*.

## Cấu trúc thư mục

```
A_star/
├── src/           # Các file source code (.cpp)
│   ├── main.cpp
│   ├── board.cpp
│   ├── search.cpp
│   ├── heuristic.cpp
│   ├── distance.cpp
│   └── deadlock.cpp
├── include/       # Các file header (.h)
│   ├── board.h
│   ├── search.h
│   ├── heuristic.h
│   ├── distance.h
│   └── deadlock.h
└── Makefile       # Build script

data/
├── level1.txt     # Bản đồ test 1
├── level2.txt     # Bản đồ test 2
└── level3.txt     # Bản đồ test 3
```

## Cách chạy

### Compile chương trình:
```bash
cd A_star
make clean
make
```

### Chạy với file đầu vào:
```bash
./sokoban_solver < ../data/level1.txt
```

### Chạy test tất cả:
```bash
make test
```

## Định dạng input

File input bao gồm:
- Dòng đầu: `rows cols` (số hàng và cột)
- Các dòng tiếp theo: Bản đồ theo biểu tượng:
  - `#` - Tường
  - `.` - Đích (mục tiêu)
  - `$` - Thùng
  - `@` - Người chơi
  - `*` - Thùng đã trên đích
  - `+` - Người chơi trên đích
  - Ký tự khác hoặc khoảng trắng - Ô trống

Ví dụ:
```
6 8
 #######
##@##..#
# $$ $.#
#   $  #
####  .#
   #####
```

## Output

Chương trình sẽ in ra:
- Trạng thái (Tìm thấy / Không tìm thấy lời giải)
- Thời gian chạy (ms)
- Số lần đẩy thùng
- Số bước di chuyển tổng cộng
- Số node đã duyệt
- **Chuỗi di chuyển**: L (Trái), R (Phải), U (Trên), D (Dưới)

## Ví dụ output

```
Tìm ra lời giải!
Thời gian chạy    : 8 ms
Số lần đẩy thùng  : 12
Số bước di chuyển : 36
Số node duyệt     : 105 trạng thái
Chuỗi di chuyển   : RURRDLRDDDLDRUUUULLLRDRDRDDLLDLLUUDR
```

## Thuật toán

- **Tìm kiếm**: A* (với heuristic tối thiểu và phạt di động)
- **Heuristic**: Hungarian Lower Bound (khoảng cách Manhattan ghép cặp) + Penalty
- **Deadlock Detection**: Phát hiện góc chết, cạnh chết, và freeze deadlock
- **Tối ưu**: Chỉ sinh ra các trạng thái đẩy thùng, không xét di chuyển rỗng
