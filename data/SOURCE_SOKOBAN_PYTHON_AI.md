# Nguồn level benchmark

Các level sau được chuyển đổi từ bộ warehouse trong dự án
`hikariakio/sokoban-python-ai`:

- `level6.txt`: `warehouses/warehouse_109.txt`
- `level7.txt`: `warehouses/warehouse_115.txt`
- `level8.txt`: `warehouses/warehouse_137.txt`
- `level9.txt`: `warehouses/warehouse_35.txt`
- `level10.txt`: `warehouses/warehouse_83.txt`
- `level11.txt`: `warehouses/warehouse_87.txt`
- `level12.txt`: `warehouses/warehouse_127.txt`

Thay đổi định dạng:

- bỏ dòng trọng số của biến thể weighted Sokoban nếu có;
- thêm dòng `rows cols` để solver C++ đọc trực tiếp;
- đổi `!` thành `+` nếu map có người chơi đứng trên goal;
- giữ nguyên bố cục tường, thùng, goal và vị trí người chơi.

Dự án nguồn được phát hành theo giấy phép MIT.

Copyright (c) 2022 Ye Gaung Kyaw

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
