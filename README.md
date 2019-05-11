# FP_SISOP19_A07

Final Project - MP3 Player

Cara kerja play.c :
1. Proses akan menampilkan pesan mengenai lagu yang sedang, akan, dan belum dimainkan.
2. Proses dapat melakukan pause.
3. Proses dapat memainkan lagu mp3.
4. Proses dapat meembaca keyboard sebagai input, 1. Previous, 2. Next, 3. Pause
5. Pause dilakukan dengan system pause.
6. Proses terhubung pada folder file system ketika di run.

Cara kerja fuse mp3.c :
1. file system akan memindahkan semua file reguler berekstensi ".mp3" ke root.
2. file system tidak akan menampilkan file selain file berekstensi ".mp3"
3. file system menyediakan fitur read, atribut, list.

Kelemahan : 
1. file system memindahkan file berekstensi .mp3 ke root
Alasan : File system mungkin menyediakan buffer untuk alamat asli dari file, sehingga tidak perlu dipindahkan, tetapi tim kami mengalami kesulitan dalam fuse.
