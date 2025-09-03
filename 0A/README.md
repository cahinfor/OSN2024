# Riwayat Bebek

## Deskripsi

Pak Dengklek mengingat kembali masa-masa ia mulai memelihara bebek.

Pada awalnya, Pak Dengklek tidak memiliki bebek peliharaan. Setiap harinya selama _N_ hari berturut-turut, ia menambah seekor bebek peliharaan baru. Lebih detailnya, pada akhir hari ke-i, Pak Dengklek menambah seekor bebek peliharaan baru yang berumur A<sub>i</sub> hari. Perhatikan bahwa umur bebek yang sudah dipelihara Pak Dengklek akan bertambah setiap harinya.

Pak Dengklek penasaran, pada akhir setiap harinya, berapakah umur bebek termuda, umur bebek tertua, dan juga umur rata-rata bebek peliharaannya?

## Batasan

- 1 ≤ N ≤ 100 000
- 1 ≤ A<sub>i</sub> ≤ 200 000

## Subsoal

1. (10 poin) Hanya berisi kasus uji berikut:
   ```
   5
   4 2 3 9 4
   ```
2. (16 poin) A<sub>i</sub> = i
3. (12 poin) A<sub>1</sub> = A<sub>2</sub> = ... = A<sub>N</sub>
4. (32 poin) N <= 1000
5. (30 poin) Tidak ada batasan tambahan.

Pada setiap subsoal, Anda juga bisa memperoleh 50% dari poin subsoal tersebut jika Anda hanya bisa menjawab umur termuda dan umur tertua dengan tepat untuk setiap harinya (dan tidak menjawab umur rata-rata dengan tepat).

## Masukan

Masukan diberikan dalam format berikut:
```text
N
A1 A2 … AN
```

## Keluaran

Keluarkan N baris, masing-masing berisi dua buah bilangan bulat dan sebuah bilangan riil yang menyatakan umur bebek termuda, umur bebek tertua, dan umur rata-rata bebek. Umur rata-rata dinyatakan benar apabila selisih mutlak terhadap keluaran juri kurang dari 10<sup>−2</sup>. Perhatikan bahwa Anda harus selalu mengeluarkan tiga buah bilangan setiap barisnya, meskipun Anda bermaksud untuk memperoleh 50% poin.

## Contoh Masukan

```text
4
3 1 3 7
```

## Contoh Keluaran

```text
3 3 3.0000
1 4 2.5000
2 5 3.3333
3 7 5.0000
```

## Penjelasan Contoh

- Pada hari ke-1, bebek peliharaan Pak Dengklek berumur 3 hari.
- Pada hari ke-2, bebek-bebek peliharaan Pak Dengklek berumur 4 dan 1 hari.
- Pada hari ke-3, bebek-bebek peliharaan Pak Dengklek berumur 5, 2, dan 3 hari.
- Pada hari ke-4, bebek-bebek peliharaan Pak Dengklek berumur 6, 3, 4, dan 7 hari.
