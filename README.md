# Sisop-4-2024-MH-IT03


## ***KELOMPOK IT 03***
| Nama      | NRP         |
  |-----------|-------------|
  | Haidar Rafi Aqyla | 5027231029   |
  | Hasan | 5027231073  |  
  | Muhammad Kenas Galeno Putra  | 5027231069  |
  

# Praktikum Sistem Operasi Modul 4


# Soal 1


**soal :**
Adfi merupakan seorang CEO agency creative bernama Ini Karya Kita. Ia sedang melakukan inovasi pada manajemen project photography Ini Karya Kita. Salah satu ide yang dia kembangkan adalah tentang pengelolaan foto project dalam sistem arsip Ini Karya Kita. Dalam membangun sistem ini, Adfi tidak bisa melakukannya sendirian, dia perlu bantuan mahasiswa Departemen Teknologi Informasi angkatan 2023 untuk membahas konsep baru yang akan mengubah project fotografinya lebih menarik untuk dilihat. Adfi telah menyiapkan portofolio hasil project fotonya yang bisa didownload dan diakses di www.inikaryakita.id . Silahkan eksplorasi web Ini Karya Kita dan temukan halaman untuk bisa mendownload projectnya. Setelah kalian download terdapat folder gallery dan bahaya.

Pada folder “gallery”:
Membuat folder dengan prefix "wm." Dalam folder ini, setiap gambar yang dipindahkan ke dalamnya akan diberikan watermark bertuliskan inikaryakita.id. 

Pada folder "bahaya," terdapat file bernama "script.sh." Adfi menyadari pentingnya menjaga keamanan dan integritas data dalam folder ini. 
Mereka harus mengubah permission pada file "script.sh" agar bisa dijalankan, karena jika dijalankan maka dapat menghapus semua dan isi dari  "gallery"
Adfi dan timnya juga ingin menambahkan fitur baru dengan membuat file dengan prefix "test" yang ketika disimpan akan mengalami pembalikan (reverse) isi dari file tersebut.  

**STEP**
1. Membuat directory untuk mount.
2. Membuat program dengan 'nano ini-karya-kita.c'
3. Compile dengan 'gcc -Wall `pkg-config fuse --cflags` inikaryakita.c -o inikaryakita `pkg-config fuse --libs` `pkg-config --cflags --libs MagickWand` -lcurl -lzip'
4. Mengakses mount directory.

**DOKUMENTASI**
1. Hasil watermark di foto yang dipindah ke directory dengan prefix wm

![image](https://github.com/haidarRA/Sisop-4-2024-MH-IT03/assets/149871906/07e02d3c-299d-45a3-80ac-1e63f1e9a048)

2. Permission script.sh yang sudah diganti sehingga script.sh dapat dijalankan

![image](https://github.com/haidarRA/Sisop-4-2024-MH-IT03/assets/149871906/67b31d89-d1db-4bcf-ba33-ecab0f0e5138)

3. Isi dari file dengan prefix test yang di-reverse setelah disimpan

![image](https://github.com/haidarRA/Sisop-4-2024-MH-IT03/assets/149871906/8efe89fa-6600-4198-b001-b3cb8b64db73)

**KENDALA YANG DIHADAPI SAAT PENGERJAAN**
1. Program tidak bisa mendownload dan/atau mengekstrak file dari drive secara tepat.

**REVISI**
1. Tidak ada

# Soal 2


**soal :**
Masih dengan Ini Karya Kita, sang CEO ingin melakukan tes keamanan pada folder sensitif Ini Karya Kita. Karena Teknologi Informasi merupakan departemen dengan salah satu fokus di Cyber Security, maka dia kembali meminta bantuan mahasiswa Teknologi Informasi angkatan 2023 untuk menguji dan mengatur keamanan pada folder sensitif tersebut. Untuk mendapatkan folder sensitif itu, mahasiswa IT 23 harus kembali mengunjungi website Ini Karya Kita pada www.inikaryakita.id/schedule . Silahkan isi semua formnya, tapi pada form subject isi dengan nama kelompok_SISOP24 , ex: IT01_SISOP24 . Lalu untuk form Masukkan Pesanmu, ketik “Mau Foldernya” . Tunggu hingga 1x24 jam, maka folder sensitif tersebut akan dikirimkan melalui email kalian. Apabila folder tidak dikirimkan ke email kalian, maka hubungi sang CEO untuk meminta bantuan.   
Pada folder "pesan" Adfi ingin meningkatkan kemampuan sistemnya dalam mengelola berkas-berkas teks dengan menggunakan fuse.
Jika sebuah file memiliki prefix "base64," maka sistem akan langsung mendekode isi file tersebut dengan algoritma Base64.
Jika sebuah file memiliki prefix "rot13," maka isi file tersebut akan langsung di-decode dengan algoritma ROT13.
Jika sebuah file memiliki prefix "hex," maka isi file tersebut akan langsung di-decode dari representasi heksadesimalnya.
Jika sebuah file memiliki prefix "rev," maka isi file tersebut akan langsung di-decode dengan cara membalikkan teksnya.
Contoh:
File yang belum didecode/ dienkripsi rot_13


File yang sudah didecode/ dienkripsi rot_13


Pada folder “rahasia-berkas”, Adfi dan timnya memutuskan untuk menerapkan kebijakan khusus. Mereka ingin memastikan bahwa folder dengan prefix "rahasia" tidak dapat diakses tanpa izin khusus. 
Jika seseorang ingin mengakses folder dan file pada “rahasia”, mereka harus memasukkan sebuah password terlebih dahulu (password bebas). 
Setiap proses yang dilakukan akan tercatat pada logs-fuse.log dengan format :
[SUCCESS/FAILED]::dd/mm/yyyy-hh:mm:ss::[tag]::[information]
Ex:
[SUCCESS]::01/11/2023-10:43:43::[moveFile]::[File moved successfully]


**STEP**
1. mkdir soal_2
2. cd soal_2
3. nano pastibisa.c
4. gcc -o pasti_bisa pasti_bisa.c -lssl -lcrypto -lfuse -D_FILE_OFFSET_BITS=64
5. fusermount -u ~/soal_2/mnt_point
6. ./pasti_bisa -f ~/soal_2/mnt_point



**DOKUMENTASI**
![Screenshot 2024-05-25 213956](https://github.com/haidarRA/Sisop-4-2024-MH-IT03/assets/150374973/b920db31-487f-4f08-8961-06a341c5a54f)

**KENDALA**
1. Terdapat error
2. Tidak mau membaca enkripsi
3. tidak muncul password






# Soal 3


**soal :**

Seorang arkeolog menemukan sebuah gua yang didalamnya tersimpan banyak relik dari zaman praaksara, sayangnya semua barang yang ada pada gua tersebut memiliki bentuk yang terpecah belah akibat bencana yang tidak diketahui. Sang arkeolog ingin menemukan cara cepat agar ia bisa menggabungkan relik-relik yang terpecah itu, namun karena setiap pecahan relik itu masih memiliki nilai tersendiri, ia memutuskan untuk membuat sebuah file system yang mana saat ia mengakses file system tersebut ia dapat melihat semua relik dalam keadaan utuh, sementara relik yang asli tidak berubah sama sekali.
Ketentuan :
Buatlah sebuah direktori dengan ketentuan seperti pada tree berikut

```
.
├── [nama_bebas]
├── relics
│   ├── relic_1.png.000
│   ├── relic_1.png.001
│   ├── dst dst…
│   └── relic_9.png.010
└── report

```


Direktori [nama_bebas] adalah direktori FUSE dengan direktori asalnya adalah direktori relics. Ketentuan Direktori [nama_bebas] adalah sebagai berikut :
Ketika dilakukan listing, isi dari direktori [nama_bebas] adalah semua relic dari relics yang telah tergabung.

Ketika dilakukan copy (dari direktori [nama_bebas] ke tujuan manapun), file yang disalin adalah file dari direktori relics yang sudah tergabung.

Ketika ada file dibuat, maka pada direktori asal (direktori relics) file tersebut akan dipecah menjadi sejumlah pecahan dengan ukuran maksimum tiap pecahan adalah 10kb.

File yang dipecah akan memiliki nama [namafile].000 dan seterusnya sesuai dengan jumlah pecahannya.
Ketika dilakukan penghapusan, maka semua pecahannya juga ikut terhapus.

Direktori report adalah direktori yang akan dibagikan menggunakan Samba File Server. Setelah kalian berhasil membuat direktori [nama_bebas], jalankan FUSE dan salin semua isi direktori [nama_bebas] pada direktori report.


Catatan:
pada contoh terdapat 20 relic, namun pada zip file hanya akan ada 10 relic
[nama_bebas] berarti namanya dibebaskan
pada soal 3c, cukup salin secara manual. File Server hanya untuk membuktikan bahwa semua file pada direktori [nama_bebas] dapat dibuka dengan baik.





**STEP**
1. buat direktori
```
.
├── [nama_bebas]
├── relics
│   ├── relic_1.png.000
│   ├── relic_1.png.001
│   ├── dst dst…
│   └── relic_9.png.010
└── report
```
2. install relics dari zip yang diberi
3. buat archeology.c
4. run archeology.c
5. coba copy file
6. coba hapus file
7. sesuaikan
   
  



**DOKUMENTASI**



![image](https://github.com/haidarRA/Sisop-4-2024-MH-IT03/assets/151866048/451c5ee0-e12b-4962-94ce-b8c105f3b0d5)


![image](https://github.com/haidarRA/Sisop-4-2024-MH-IT03/assets/151866048/40be2bf0-e9f2-406a-bca4-71eaa16cdf1b)


![image](https://github.com/haidarRA/Sisop-4-2024-MH-IT03/assets/151866048/0a9a6655-4238-4cac-9ad2-e9aef11c10c1)



**problem**

disini saya terdapat banyak problem yang dimana mulai dari error dll, dan untuk kode saya ini berjalan dengan lancar namun hanya tertinggal 1 fitur yaitu menggabungkan file relicnya.

