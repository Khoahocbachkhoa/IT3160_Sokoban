# IT3160 Sokoban

Xây dựng AI chơi Sokoban

## Yêu cầu môi trường

- Python 3
- Pygame
- g++

Cài Pygame:

```
python -m pip install pygame
```

## Chạy GUI


```
python gui/main.py
```

Điều khiển:

- `WASD` hoặc phím mũi tên: di chuyển;
- `R`: chơi lại level;
- `P`: pause/resume;
- `SOLVE`: chọn A*, BFS, DFS hoặc UCS;
- `STEP`: thực hiện một bước trong lời giải;
- `RETRY`: tải lại level;
- `NEXT`: chuyển level.

## Chạy CLI

Từ thư mục `engine`:

```
make
```

Chạy chương trình với tùy chọn thuật toán và một level:

```powershell
cat ../data/level.txt | ./sokoban astar
cat ../data/level.txt | ./sokoban bfs
cat ../data/level.txt | ./sokoban dfs
cat ../data/level.txt | ./sokoban ucs
```

Nếu không truyền đối số, solver mặc định chạy A*.

## Định dạng level

Ký hiệu :

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