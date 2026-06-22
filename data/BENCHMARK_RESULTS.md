# Kết quả benchmark solver

## Nhóm cả A*, BFS và UCS đều giải được

Kết quả tham khảo trên máy phát triển:

| Level | Thuật toán | Trạng thái | Thời gian | Tổng step | Push |
|---|---|---:|---:|---:|---:|
| 9 | A* | Solved | ~0.17 s | 77 | 31 |
| 9 | BFS | Solved | ~2.80 s | 77 | 31 |
| 9 | UCS | Solved | ~3.16 s | 77 | 31 |
| 9 | DFS | Solved | ~0.85 s | 175 | 49 |
| 10 | A* | Solved | ~0.25 s | 164 | 47 |
| 10 | BFS | Solved | ~3.90 s | 164 | 47 |
| 10 | UCS | Solved | ~4.50 s | 164 | 47 |
| 10 | DFS | Solved | ~3.64 s | 9756 | 1993 |
| 11 | A* | Solved | ~0.21 s | 149 | 53 |
| 11 | BFS | Solved | ~5.52 s | 149 | 53 |
| 11 | UCS | Solved | ~6.26 s | 149 | 53 |
| 11 | DFS | Solved | ~4.92 s | 2239 | 483 |
| 12 | A* | Solved | ~0.03 s | 106 | 32 |
| 12 | BFS | Solved | ~3.60 s | 106 | 32 |
| 12 | UCS | Solved | ~4.25 s | 106 | 32 |
| 12 | DFS | Solved | ~4.61 s | 1372 | 244 |

A*, BFS và UCS đều tối ưu tổng step nên cho cùng độ dài lời giải. A* tốt hơn
ở tốc độ tìm ra chính lời giải tối ưu đó: nhanh hơn khoảng 14–100 lần trên
nhóm map này. DFS không đảm bảo tối ưu nên có thể trả về lời giải rất dài.

## Nhóm map khó

Ở nhóm này A* vẫn trả lời trong vài giây, trong khi BFS/UCS thường vượt cửa
sổ benchmark do số trạng thái theo từng bước tăng quá lớn.

Thời gian cụ thể có thể thay đổi theo CPU và trạng thái máy.
