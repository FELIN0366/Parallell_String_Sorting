# Parallell_String_Sorting
using Pthread/OpenMP/MPI to sort string in parallell procedure
- 直接点击exe容易造成闪退，请使用cmd进行操作。

- mpi_exe:

  1)使用cmd将地址转到 `"..\202164700523_hw3_mpi\mpi_exe\Debug"`

  2)在cmd中输入命令` mpiexec -n 8 MPI_HW.exe`

  3)在cmd中输入数据集路径` 15611.txt\ 559649.txt\ 4998904.txt `

  4)在cmd中得到运行时间，在mpi_exe中得到结果文件夹


- openmp_mpi_exe:

  1)使用cmd将地址转到 `"..\202164700523_hw3_mpi\openmp_mpi_exe\Debug"`

  2)在cmd中输入命令` mpiexec -n 4 OEPNMP_MPI_HW.exe`

  3)在cmd中输入数据集路径` 15611.txt\ 559649.txt\ 4998904.txt `

  4)在cmd中得到运行时间，在openmp_mpi_exe中得到结果文件夹

  5)注意！！

  - 本程序已设定4个进程与8个线程，故命令只能为`mpiexec -n 4 OEPNMP_MPI_HW.exe`

