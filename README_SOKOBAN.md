# IT3160 Sokoban

Du an Sokoban gom 2 phan chinh:

- `A_star/`: thuat toan giai Sokoban bang C++.
- `sokoban_gui/`: giao dien choi game bang Pygame, co the goi C++ solver.

## Cau Truc Thu Muc

```text
IT3160_Sokoban/
|-- A_star/
|   |-- include/
|   |   |-- board.h
|   |   |-- deadlock.h
|   |   |-- distance.h
|   |   |-- heuristic.h
|   |   `-- search.h
|   |-- src/
|   |   |-- main.cpp
|   |   |-- board.cpp
|   |   |-- deadlock.cpp
|   |   |-- distance.cpp
|   |   |-- heuristic.cpp
|   |   `-- search.cpp
|   `-- solver
|
|-- data/
|   |-- level2.txt
|   |-- level3.txt
|   |-- level4.txt
|   |-- mylevel.txt
|   `-- web_levels/
|
`-- sokoban_gui/
    |-- main.py
    |-- config.py
    |-- bin/
    |   `-- cpp_solver.exe
    |-- assets/
    |   |-- fonts/
    |   `-- images/
    `-- src/
        |-- assets.py
        |-- button.py
        |-- game.py
        |-- level.py
        |-- renderer.py
        `-- solver_client.py
```

## Yeu Cau Moi Truong

Can co:

- Python 3
- Pygame
- `g++` neu muon GUI tu build lai C++ solver

Cai Pygame:

```powershell
python -m pip install pygame
```

Kiem tra `g++`:

```powershell
g++ --version
```

## Chay GUI

Tu thu muc `Project_AI`, chay:

```powershell
python IT3160_Sokoban\sokoban_gui\main.py
```

Dieu khien:

- `WASD` hoac phim mui ten: di chuyen nguoi choi
- `R`: choi lai level hien tai
- `P`: pause/resume
- `SOLVE`: mo menu chon thuat toan
- `STEP`: chay tung buoc trong loi giai da tim duoc
- `RETRY`: choi lai level hien tai
- `NEXT`: chuyen sang level tiep theo

## Goi Solver Trong GUI

Khi bam `SOLVE`, GUI se hien menu chon thuat toan:

- `A STAR`: goi C++ A* solver trong `A_star`
- `DFS`: nut da co trong GUI, chua noi thuat toan
- `BFS`: nut da co trong GUI, chua noi thuat toan
- `UCS`: nut da co trong GUI, chua noi thuat toan

Luong chay cua `A STAR`:

```text
Pygame GUI
-> lay map hien tai
-> tao input dang rows cols
-> goi cpp_solver.exe bang subprocess
-> nhan chuoi loi giai tu C++ solver
-> luu loi giai vao Game
-> bam STEP de chay tung buoc
```

Neu chua co `sokoban_gui/bin/cpp_solver.exe`, GUI se tu build tu source trong `A_star`.
Viec build dung ban copy tam trong `sokoban_gui/bin`, khong sua source goc trong `A_star`.

## Format Level

GUI doc cac file:

```text
IT3160_Sokoban/data/level*.txt
```

Ho tro ky hieu Sokoban chuan:

```text
#  : tuong
   : nen trong
$  : thung
.  : o dich
@  : nguoi choi
*  : thung tren o dich
+  : nguoi choi tren o dich
```

GUI co the doc level co hoac khong co dong dau `rows cols`.

C++ solver goc trong `A_star` yeu cau input co dong dau:

```text
rows cols
```

Khi GUI goi solver, phan nay duoc tao tu dong.

## Chay C++ Solver Rieng

Neu muon chay solver C++ doc lap, input can co dong dau `rows cols`.

Vi du:

```powershell
cd IT3160_Sokoban\A_star
g++ -std=c++14 -O2 src\main.cpp src\board.cpp src\search.cpp src\heuristic.cpp src\distance.cpp src\deadlock.cpp -o solver.exe
.\solver.exe < ..\data\mylevel.txt
```

Luu y: mot so level trong `data/` khong co dong `rows cols`, nen chay truc tiep bang C++ solver co the doc sai. GUI thi tu xu ly phan nay khi goi solver.

## Mo Ta Thuat Toan A*

Phan C++ solver dung A* theo trang thai day thung:

- State gom vi tri nguoi choi va danh sach vi tri cac thung.
- BFS dung de kiem tra nguoi choi co di toi vi tri can dung de day thung duoc khong.
- Successor chi sinh ra khi co mot lan day thung hop le.
- Heuristic dua tren khoang cach Manhattan ghep thung voi o dich, co them penalty cho thung bi ket.
- Deadlock detection kiem tra cac dang ket don gian nhu goc chet, canh chet va freeze deadlock.

Chuoi loi giai gom:

- Chu thuong `l/u/r/d`: nguoi choi di bo
- Chu hoa `L/U/R/D`: nguoi choi day thung

Trong GUI, ca chu thuong va chu hoa deu duoc dung de di chuyen tung buoc khi bam `STEP`.
