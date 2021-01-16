# C - Yanlış girilen kelime yerine yeni kelime önerme c kodu
C - Girilen cümledeki yanlış kelime yerine Levenshtein Edit Distance yöntemi ile kelime önerme, Hashing ile kelimelerin kaydedilmesi

Problem.pdf dosyasındaki 2. sorunun cevabıdır.

Kullanıcıdan bir cümle girmesi istenir. Girilen cümledeki kelimelerin, sözlük olarak kullanılan smallDictionary.txt dosyasındaki kelimeler ile
eşleşip eşleşmediği kontrol edilir. Burada kelimeler hashing methodu ile tutulmaktadır. Eğer cümledeki kelime hashing tablosundaki kelimeler ile eşleşemiyorsa
sözlükte yoktur ve yanlış yazıldığı kabul edilir. Bu durumda tüm kelimeler ile Levenshtein Edit Distance metoduyla kelimeler arasındaki mesafeler ölçülür.
Eğer kelimede mesafe 2 ve 2den düşükse kelime kullanıcıya önerilir. Eğer önerilecek kelime yoksa kullanıcının yeni kelime girmesi istenir. Burada önerilen kelimeler
ve kullanıcının yanlış girdiği kelime yerine girdiği yeni kelimeler kayıt edilir. Tekrar yanlış girilen kelime ile karşılaşılırsa kayıt edilen kelimeler tekrar önerilir.

Detaylı bilgi için Rapor.pdf dosyasını inceleyebilirsiniz.
Soru sormak için sosyal medya hesaplarıma @burakbozb1 kullanıcı adı ile ulaşabilirsiiz.
