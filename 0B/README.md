# Harta Karun Nasional

## Deskripsi

Terdapat N petak tanah yang dinomori dari 1 hingga N yang tersusun secara melingkar. Petak i berada tepat di sebelah kiri petak i+1 untuk setiap 1 ≤ i ≤ N−1, dan petak N berada tepat di sebelah kiri petak 1.

Bu Dengklek mengetahui bahwa tepat satu dari N petak itu berisi harta karun dan dia ingin mencari posisi petak tersebut. Untuk membantu pencariannya, di setiap petak i, Bu Dengklek memasang sebuah mesin bernomor i.

Dalam satu operasi, Bu Dengklek bisa memilih suatu bilangan bulat k dan mengaktifkan k buah mesin berbeda. Setelah itu, Bu Dengklek akan mendapatkan informasi berupa jarak terdekat antara mesin aktif terdekat dengan harta karunnya. Operasi ini membutuhkan $\left \lceil \frac{N}{k}\right \rceil$ Kwakkoin. Setelah operasi, semua mesin yang diaktifkan menjadi tidak aktif kembali.

Bu Dengklek hanya mempunyai 512 Kwakkoin. Bantulah Bu Dengklek untuk mencari tahu posisi petak yang berisi harta karun!

Catatan:
- Secara formal, jarak terdekat antara petak x dan petak y (x ≤ y) adalah nilai minimum antara y − x dan N − (y − x).
- Notasi ⌈w⌉ untuk suatu bilangan riil w menyatakan pembulatan ke atas dari w.


## Batasan

- N= 256

## Subsoal

1. (9 poin) Harta karun berada di petak 1.
2. (15 poin) Harta karun berada di salah satu petak antara 1 hingga 3 inklusif.
3. (33 poin) Harta karun berada di salah satu petak antara 1 hingga 128 inklusif.
4. (43 poin) Tidak ada batasan tambahan.

## Informasi Tipe Soal

Soal ini bertipe "interaktif". Pada soal ini, Anda akan berinteraksi dengan program penguji melalui masukan standar (stdin) dan keluaran standar (stdout). Perhatikan interaksi di bawah ini dengan saksama.

## Interaksi

Untuk melakukan operasi yang mengaktifkan k buah mesin a<sub>1</sub>, a<sub>2</sub>, ..., a<sub>k</sub> (1 ≤ a<sub>1</sub> < a<sub>2</sub> < … < a<sub>k</sub> ≤ N), Anda bisa menghasilkan keluaran dalam format berikut:

```text
TANYA k
a1 a2 … ak
```

Perhatikan bahwa nomor-nomor mesin harus **terurut menaik**.

Untuk setiap operasi, Anda akan mendapatkan jawabannya dalam format berikut:

```text
d
```

Ini berarti, jarak terdekat antara mesin aktif terdekat dengan harta karun adalah d.

Jika Anda sudah menemukan posisi harta karun, katakanlah pada petak z, Anda dapat menjawab dengan format berikut:

```text
JAWAB z
```

## Penilaian

Setiap operasi yang mengaktifkan k buah mesin membutuhkan $\left \lceil \frac{N}{k}\right \rceil$ Kwakkoin. Anggap  C menyatakan total Kwakkoin dari semua operasi. Nilai PERFORMA dihitung berdasarkan tabel berikut:

| **Kondisi** | PERFORMA                            |
|-------------|-------------------------------------|
| 512 < C     | 0%                                  |
| 6 < C ≤ 512 | (100 - 5 * log<sub>2</sub>(C - 5))% |
| C ≤ 6       | 100%                                |

Untuk setiap kasus uji, program akan dinilai PERFORMA-nya. Setiap subsoal terdiri dari beberapa kasus uji. Untuk setiap subsoal, akan diambil nilai PERFORMA yang paling kecil. Poin yang didapat untuk suatu subsoal adalah nilai PERFORMA terkecil tersebut dikalikan dengan poin subsoal. Nilai akhir adalah jumlah nilai semua subsoal, yang kemudian dibulatkan ke bawah.

Program Anda akan mendapatkan "Wrong Answer" jika:
- menggunakan lebih dari 512 Kwakkoin, atau
- menghasilkan keluaran dengan format yang salah atau di luar batasan.

Jika suatu kasus uji berhasil dijalankan, Anda bisa melihat penilaiannya secara detail di halaman pengumpulan. Anda bisa melihat total Kwakkoin yang digunakan untuk kasus uji tersebut.

Grader **tidak adaptif**. Dengan kata lain, posisi petak yang berisi harta karun sudah ditentukan sebelum interaksi dimulai.

## Contoh Interaksi

Berikut adalah sebuah contoh interaksi yang mungkin. Pada contoh ini, harta karun terletak di petak 42.

| **Masukan** | **Keluaran**             | Penjelasan                                                                                                                           |
|-------------|--------------------------|--------------------------------------------------------------------------------------------------------------------------------------|
|             | TANYA 3<br/>2 5 6        | Operasi ini mengaktifkan mesin 2, 5, dan 6. Operasi ini membutuhkan $\left \lceil \frac{256}{3}\right \rceil$ = 86 Kwakkoin.         |
| 36          |                          | Mesin aktif terdekat dengan harta karun adalah mesin 6 dengan jarak 36.                                                              |
|             | TANYA 2<br/>196 234      | Operasi ini mengaktifkan mesin 196 dan 234. Operasi ini membutuhkan $\left \lceil \frac{256}{2}\right \rceil$ = 128 Kwakkoin.        |
| 64          |                          | Mesin aktif terdekat dengan harta karun adalah mesin 234 dengan jarak 64.                                                            |
|             | TANYA 4<br/>26 52 78 104 | Operasi ini mengaktifkan mesin 26, 52, 78, dan 104. Operasi ini membutuhkan $\left \lceil \frac{256}{4}\right \rceil$ = 64 Kwakkoin. |
| 10          |                          | Mesin aktif terdekat dengan harta karun adalah mesin 52 dengan jarak 10.                                                             |
|             | JAWAB 42                 | Menjawab bahwa harta karun berada di petak 42                                                                                        |
| (selesai)   | (selesai)                |                                                                                                                                      |

Pada contoh di atas, total Kwakkoin yang digunakan adalah 86 + 128 + 64 = 278. Karena 6 < 278 ≤ 512 berlaku, maka nilai PERFORMA-nya adalah (100 − 5 × log₂(278 − 5))% ≈ 59.536%.

Peringatan
Selalu lakukan flush setiap kali setelah program Anda mengeluarkan keluaran.
- Untuk library stdio.h (cstdio):
```c
fflush(stdout);
```
- Untuk library iostream:
```c++
std::cout << std::flush;
```