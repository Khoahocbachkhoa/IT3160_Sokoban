# IT3160 Sokoban

Project gồm một game Sokoban bằng Pygame và một solver C++ hỗ trợ bốn thuật
toán: A*, BFS, DFS và UCS.

Điểm quan trọng khi so sánh:

- **A*** là solver được tối ưu riêng cho Sokoban, dùng heuristic, phát hiện
  deadlock và tìm kiếm theo các lần đẩy hợp lệ.
- **BFS, DFS và UCS** được giữ ở dạng graph-search cổ điển theo từng bước di
  chuyển. Ba baseline này không dùng heuristic hoặc tối ưu chuyên biệt
  Sokoban.

## Cấu trúc project

```text
IT3160_Sokoban/
|-- solver/
|   |-- include/                 # Khai báo cấu trúc và thuật toán
|   |-- src/                     # Mã nguồn C++
|   |-- Makefile
|-- sokoban_gui/
|   |-- assets/                  # Ảnh và font
|   |-- bin/                     # Solver do GUI tự build
|   |-- src/                     # Game, renderer, level loader, solver client
|   |-- config.py
|   |-- main.py
|-- data/
|   |-- level1.txt ... level12.txt
|   |-- custom.txt
|   |-- BENCHMARK_RESULTS.md
|   |-- SOURCE_SOKOBAN_PYTHON_AI.md
|-- README_SOKOBAN.md
```

GUI tự nhận các file có tên `level<number>.txt`, sắp xếp theo số level và cho
nút `NEXT` chuyển tuần tự rồi quay lại level đầu. `custom.txt` không nằm trong
vòng level tự động.

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

Khi cần, GUI tự biên dịch tất cả file `solver/src/*.cpp` thành
`sokoban_gui/bin/cpp_solver.exe`. Dấu vân tay SHA-256 của source, header và cờ
biên dịch được lưu lại, nên solver chỉ được build lại khi nội dung liên quan
thay đổi.

Cờ build do GUI sử dụng:

```text
-std=c++14 -O3 -Wall -Wextra -static-libgcc -static-libstdc++
```

GUI gọi executable với đối số `astar`, `bfs`, `dfs` hoặc `ucs`, truyền level
qua standard input và đọc chuỗi lời giải từ standard output.

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

Solver C++ yêu cầu dòng đầu chứa:

```text
rows cols
```

Sau đó là `rows` dòng bàn chơi. GUI có thể đọc cả file có hoặc không có dòng
kích thước.

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
goal.

## Biểu diễn trạng thái

`Board` lưu tường, goal và bảng push-distance trên mảng một chiều. Một `State`
lưu:

- vị trí người chơi;
- danh sách vị trí thùng đã được sắp xếp;
- trường `canonical_player` dùng trong so sánh và hashing.

Ở phiên bản hiện tại, cả A* và ba baseline đều đặt `canonical_player` bằng vị
trí thật của người chơi. Điều này cần thiết vì project đang tối ưu **tổng số
thao tác**: vị trí người chơi ảnh hưởng trực tiếp đến chi phí tới cú đẩy tiếp
theo.

Mỗi thao tác đi bộ hoặc đẩy thùng đều có chi phí 1.

## A* — solver được cải tiến cho Sokoban

A* dùng priority queue theo:

```text
f(n) = g(n) + h(n)
```

Trong đó:

- `g(n)` là tổng số thao tác thực tế từ trạng thái đầu;
- `h(n)` là cận dưới cho số thao tác còn lại.

### Tìm kiếm theo lần đẩy

A* không tạo một node mới cho từng bước đi bộ. Tại mỗi trạng thái, solver:

1. chạy BFS để tìm toàn bộ ô người chơi có thể tới mà không di chuyển thùng;
2. xét từng thùng và bốn hướng đẩy;
3. chỉ sinh successor nếu người chơi tới được phía sau thùng và ô đích của
   thùng đang trống;
4. tính chi phí cạnh bằng đường đi bộ ngắn nhất tới vị trí đẩy cộng một thao
   tác đẩy.

Nhờ vậy A* vẫn tối ưu tổng step giống BFS/UCS, nhưng không phải đưa mọi bước đi
bộ trung gian vào frontier.

### Heuristic push-distance

Trước khi tìm kiếm, solver chạy reverse BFS từ từng goal để tính
`push_dist[goal][cell]`: cận dưới số lần đẩy cần thiết để đưa một thùng từ
`cell` tới `goal`, có xét tường và vị trí cần thiết để người chơi đứng đẩy.

Push-distance chính xác hơn Manhattan thông thường vì nó phản ánh hình dạng
của map. Nó vẫn bỏ qua tương tác giữa các thùng, do đó phù hợp làm cận dưới.

### Ghép thùng–goal bằng Hungarian

Solver tạo ma trận chi phí push-distance giữa mọi thùng và mọi goal, sau đó
dùng thuật toán Hungarian để tìm phép ghép có tổng chi phí nhỏ nhất.

Cách này tốt hơn ghép tham lam: mỗi goal chỉ được gán cho một thùng và solver
xét chi phí toàn cục của tất cả cặp thùng–goal.

Nếu một thùng không thể tới bất kỳ goal nào trên bản đồ tĩnh, trạng thái được
loại ngay bằng assignment pruning.

### Cận dưới vị trí người chơi

Ngoài chi phí thùng–goal, heuristic cộng thêm cận dưới Manhattan từ người chơi
tới vị trí kề một thùng gần nhất. Thành phần này ước lượng tối thiểu số bước đi
bộ trước cú đẩy tiếp theo.

### Deadlock pruning

A* loại các trạng thái chắc chắn không thể giải:

- **dead square**: ô mà thùng không thể được đẩy tới bất kỳ goal nào, xác định
  bằng tìm kiếm ngược từ các goal;
- **corner deadlock**: thùng không nằm trên goal bị kẹt ở góc tường;
- **freeze deadlock 2×2**: cụm tường/thùng khóa cứng và chứa thùng chưa ở goal.

Các kiểm tra này chỉ được dùng trong A*, không được dùng trong BFS, DFS hoặc
UCS.

### Các tối ưu triển khai khác

A* còn sử dụng:

- `best_g` để lưu chi phí tốt nhất đã biết của mỗi trạng thái và bỏ các entry
  cũ trong priority queue;
- cache heuristic theo cấu hình thùng, tránh chạy Hungarian lại khi chỉ vị trí
  người chơi thay đổi;
- tie-break theo `h` khi hai node có cùng `f`;
- chỉ lưu cú đẩy và vị trí người chơi cần đứng trong parent map;
- chỉ dựng đường đi bộ chi tiết bằng BFS sau khi đã tìm thấy goal.

Với heuristic cận dưới và cơ chế cập nhật `best_g`, A* hướng tới lời giải tối
ưu theo tổng số step, cùng tiêu chí với BFS và UCS.

## BFS — baseline cổ điển

BFS dùng `std::queue` FIFO. Mỗi successor là đúng một thao tác đi bộ hoặc đẩy
thùng.

BFS chỉ có các thành phần chuẩn của graph search:

- trạng thái đầy đủ gồm vị trí người chơi và các thùng;
- `visited` để không đưa lại trạng thái đã gặp;
- parent map để truy vết lời giải.

BFS **không dùng**:

- heuristic;
- deadlock detection;
- push-distance;
- Hungarian matching;
- tìm kiếm theo lần đẩy;
- cache heuristic hoặc pruning chuyên biệt Sokoban.

Vì mỗi thao tác có chi phí 1, BFS đảm bảo lời giải có tổng step nhỏ nhất nếu
hoàn thành trong giới hạn tài nguyên.

## DFS — baseline cổ điển

DFS dùng `std::stack` LIFO và cùng bộ successor từng bước với BFS.

DFS chỉ dùng `visited` và parent map của graph search thông thường. Nó không
dùng heuristic hay bất kỳ tối ưu Sokoban nào. DFS không đảm bảo lời giải ngắn
nhất và có thể trả về chuỗi di chuyển rất dài, phụ thuộc mạnh vào thứ tự duyệt.

## UCS — baseline cổ điển

UCS dùng priority queue theo chi phí tích lũy `g`. Mỗi thao tác có chi phí 1,
vì vậy UCS hiện có cùng mục tiêu tối ưu và gần như cùng thứ tự mở rộng theo độ
sâu chi phí với BFS.

UCS sử dụng `best_g` và bỏ entry cũ trong priority queue — đây là cơ chế chuẩn
của Uniform Cost Search, không phải heuristic hoặc cải tiến riêng cho Sokoban.

UCS không dùng heuristic, deadlock pruning, push-distance, Hungarian matching
hay successor theo lần đẩy.

## So sánh đúng cách

A*, BFS và UCS đều tối ưu tổng step. Nếu cả ba hoàn thành, độ dài lời giải phải
tương ứng với cùng chi phí tối ưu; lợi thế chính của A* là tìm ra lời giải đó
nhanh hơn và với ít trạng thái hơn.

DFS không tối ưu, nên có thể nhanh hơn tình cờ trên một map nhưng trả về lời
giải dài hơn nhiều.

Các level benchmark:

- level 6–8: map khó, dùng để thể hiện khả năng mở rộng;
- level 9–12: A*, BFS và UCS đều giải được và cho cùng tổng step, nhưng A*
  thường nhanh hơn rõ rệt;
- nguồn map và giấy phép: `data/SOURCE_SOKOBAN_PYTHON_AI.md`;
- số liệu tham khảo: `data/BENCHMARK_RESULTS.md`.

Thời gian benchmark phụ thuộc CPU, compiler và trạng thái máy.

## Giới hạn thời gian

- Solver C++: **120.000 ms (120 giây)** cho mỗi lần tìm kiếm.
- GUI chờ solver: **130 giây**.
- GUI chờ quá trình build C++: **60 giây**.

Khoảng chênh 10 giây giữa solver và GUI giúp C++ có thời gian tự dừng và trả về
lý do timeout trước khi Python kết thúc process.

## Chuỗi lời giải

Solver trả về:

- `l`, `u`, `r`, `d`: bước đi bộ;
- `L`, `U`, `R`, `D`: bước đẩy thùng.

GUI có thể phát lại chuỗi này từng bước bằng nút `STEP`.

## Các file chính

Phần C++:

- `board.cpp`: đọc map, biểu diễn board/state và tính push-distance;
- `classic_search.cpp`: sinh successor từng bước cho BFS/DFS/UCS;
- `search_astar.cpp`: A* chuyên biệt Sokoban;
- `search_bfs.cpp`: BFS cổ điển;
- `search_dfs.cpp`: DFS cổ điển;
- `search_ucs.cpp`: UCS cổ điển;
- `distance.cpp`: BFS vùng reachable và dựng đường đi;
- `deadlock.cpp`: dead-square, corner và freeze deadlock;
- `heuristic.cpp`: push-distance matching và Hungarian.

Phần GUI:

- `main.py`: event loop và menu chọn solver;
- `game.py`: luật di chuyển và phát lại lời giải;
- `level.py`: đọc và chuyển level;
- `solver_client.py`: build, gọi và parse kết quả C++;
- `renderer.py`: vẽ board, sidebar và menu.
