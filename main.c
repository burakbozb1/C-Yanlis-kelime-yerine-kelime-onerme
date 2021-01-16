/*
Burak Boz
Dinamik Programlama Algoritmasý | Levenshtein Edit Distance
Sorgulanan bir cümlede yanlýþ yazýlmýþ kelimeler varsa bu kelimelerin yerine doðru kelimeler öneren sistem.
Sözlük olarak "smallDictionary.txt" dosyasýndaki kelimeler kullanýlmýþtýr. Eðer girilen cümledeki kelimeler bu sözlükte bulunmuyorsa
kelime yanlýþ olarak kabul edilir. Yanlýþ olan kelime yerine Levenshtein Edit Distance algoritmasý kullanýlarak mesafesi 2 ve 2den az olan
kelimeler önerilir. Önerilecek kelime yoksa yeniden girmesi istenir. Bu yanlýþ kelime ve yerine girilen kelime kaydedilir. Tekrar yanlýþ kelime
ile karþýlaþýlýrsa kaydedilen listeden tekrar öneri yapýlýr.

Kelime hashlemek için horner methodu kullanýlmýþtýr. Hash tablosunda çakýþmalara karþý double hashing methodu kullanýlmýþtýr.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M 1009
#define MM 1008


typedef struct {//Hash tipinde bir dizi tanýmlanacak ve burada kelimeler saklanacaktýr.
	char kelime[1000];
}HASH;

typedef struct {//Hatalýhash tipinde bir dizi tanýmlanacak ve Hatalý girilen kelimeler burada tutulacaktýr.
	char kelime[1000];
	struct Node* head;
}HATALIHASH;


struct Node{ //Bu düðümlerde hatalý girilen kelime yerine önerilecek kelimeler linkli liste tipinde tutulacaktýr.
	char onerilen[1000];
	struct Node* next; 
};


//Fonksiyon prototipleri - Tüm fonksiyonlara açýklamalarý yazýlmýþtýr.
struct Node * kelimeAra(char []);
void yeniEkle(char * hatali,char * guncel);
void sMerge(char * );
int errHash(char * ,int);
int distance(char *,char *);
void hashTableYazdir();
void hataliHashTableYazdir();
int hash(char *,int);
int h1(int);
int h2(int);
int horner (char *);
int getPower(int,int);
void baslangic(char *);



HASH *hashTable;	//Sözlük kelimeleri bu dizide tutulacaktýr
HATALIHASH *hashErrorTable;		//Hatalý girilen kelimeler bu dizide tutulacaktýr
int loadFactor;		//Sözlük dizisinin doluluk oraný bu deðiþkende saklanacaktýr
int loadFactorError;		//Hatalý kelimeler dizisinin doluluk oraný bu deðiþkende saklanacaktýr.
char birlesmisCumle[5000];		//En son merge edilen cümle bu dizide saklanacaktýr.
int kelimeDurum=0;		//Kelimenin sözlükteki durumunu kontrol etmek için kullanýlacaktýr.

int main()
{
	char cumle[1000];	//Kullanýcýdan alýnan cümle
	int i=0;		//Döngüler için kullanýlacatýr
	int cumleSayac=0;	//Kullanýcýdan alýnan cümlenin uzunluðunu tutacaktýr.
	int kelimeSayac=0;	//Cümledeki iþlenen kelimenin uzunluðunu tutacaktýr.
	hashTable = (HASH*)calloc(M, sizeof(HASH));
	hashErrorTable = (HATALIHASH*)calloc(M, sizeof(HATALIHASH));
	loadFactor=0;
	loadFactorError=0;
	birlesmisCumle[0]='\0';//Cümle boþialtýldý
	int secim;	//Menü için kullanýlacak.
	
	
	for(i=0;i<M;i++)//Hazýrlanan tablolar temizleniyor.
	{
		memset(hashTable[i].kelime,0,sizeof(hashTable[i].kelime));
		memset(hashErrorTable[i].kelime,0,sizeof(hashErrorTable[i].kelime));
		hashErrorTable[i].head=NULL;
	}
	
	
	printf("sozluk okunuyor...");//Sözlük okunuyor ve ekrana yazdýrýlýyor
	baslangic("smallDictionary.txt");
	//hashTableYazdir();
	
	do
	{
		printf("\n\n==============================================================\n\n");
		printf("**MENU\n");
		printf("1-Cumle girmek icin\n");
		printf("2-Sozlugu yazdirmak icin\n");
		printf("3-Hatali kelime listesini yazdirmak icin\n");
		printf("0-Cikmak icin\n");
		printf("\n\n==============================================================\n\n");
		printf("Secim:");
		scanf("%d",&secim);
		getchar();
		if(secim==0)
		{
			free(hashErrorTable);
			free(hashTable);
			printf("\nBellek temizlendi.\n");
			return 0;
		}
		else if(secim==1)
		{
			cumle[0]='\0';
			printf("lutfen cumlenizi girin:");//Kullanýcýdan cümle alýnýyor.
			gets(cumle);
			int init_size = strlen(cumle);//Cümle kelimelere bölünüyor ve iþlenmeye baþlanýyor
			char delim[] = " ";
			char *ptr = strtok(cumle, delim);
			while(ptr != NULL)
			{
				char yeniKelime[100];
				struct Node* gelenKelime = kelimeAra(ptr);	//Kelime sözlükte aranmaya baþlanýyor.
				if(kelimeDurum==-1)// Kelime doðru (Eðer kelime sözlükte mevcutsa kelimeDurum deðiþkeni -1 olarak güncellenir.)
				{
					printf("%s (kelime dogru)\n",ptr);
					sMerge(ptr);			
				}
				else if(kelimeDurum==1)//kelimeDurum Kelime yanlýþ fakat önerilecek kelime yok. Bu durumda kullanýcýdan alýnan kelime hatalý kelimenin yanýna önerilen olarak eklenir
				{
					printf("%s - kelime yanlis fakat onerilecek kelime yok. Lutfen dogru halini tekrar yazin\n(hatali kelime yerine cumleye eklenecektir.):",ptr);
					gets(yeniKelime);
					yeniEkle(ptr,yeniKelime);
					sMerge(yeniKelime);
				}
				else//Diðer iki durum gerçekleþmemiþse kelime yanlýþtýr ve listede önerilecek kelime vardýr. Bu kelimeler kullanýcýya önerilir
				{
					int sayac=0;
					printf("%s - bunu mu demek istediniz?\n",ptr);
					while(gelenKelime!=NULL)
					{
						sayac++;
						printf("\t%s\n",gelenKelime->onerilen);
						gelenKelime=gelenKelime->next;
					}
					printf("\tYeni kelimeyi girin (hatali kelime yerine cumleye eklenecektir.):");
					gets(yeniKelime);
					sMerge(yeniKelime);
				}
				ptr = strtok(NULL, delim);
			}
			printf("\n\nCumlenin son hali : %s\n\n",birlesmisCumle);//Cümlenin son hali ekrana yazdýrýlýr
			memset(birlesmisCumle, '\0', sizeof birlesmisCumle);
		}
		else if(secim==2)//Sözlüðü yazdýr
		{
			hashTableYazdir();
		}
		else if(secim==3)//Hatalý kelimeleri yazdýr
		{
			hataliHashTableYazdir();
		}
		else
		{
			printf("Gecersiz secim\n");
		}
	}while(secim!=0);
	
}

void yeniEkle(char * hatali,char * guncel)//(hatalý kelime, yerine girilecek güncel kelime) -> void
{
	int kontrol=0;
	int hornerKey = horner(hatali);
	int i= 0,hash = 0;
	do
	{
		i++;
		hash = (h1(hornerKey) + i*h2(hornerKey)) % M;
		if(strcmp(hashErrorTable[hash].kelime,hatali) == 0)//Hashlenen kelime doðrumu
		{
			kontrol=1;
			i==1010;
		}
	}while(((kontrol!=1) && (strlen(hashErrorTable[hash].kelime) != 0)) && i<1009);
	
	struct Node *yeniKelime = (struct Node *) calloc(1,sizeof(struct Node));
	strcpy(yeniKelime->onerilen,guncel);
	yeniKelime->next=NULL;
	hashErrorTable[hash].head=yeniKelime;
}

void sMerge(char * ptr)//(kelime) -> void | birleþmiþ cümle deðiþkeninde uygun kelimeler birleþtirilir.
{
	int kelimeSayac=strlen(ptr);
	int cumleSayac=strlen(birlesmisCumle);
	int i=0;
	for(i=0;i<kelimeSayac;i++)
	{
		birlesmisCumle[cumleSayac+i]=ptr[i];
	}
	birlesmisCumle[strlen(birlesmisCumle)]=' ';
	birlesmisCumle[strlen(birlesmisCumle)+1]='\0';
}

struct Node * kelimeAra(char kelime[])//(AranacakKelime) return -> önerilecek kelimeler | önerilecek kelime yoksa kelimeDurum deðiþkeni güncellenir.
{
	struct Node *gelenKelimeler;
	int kontrol=0;
	int hornerKey = horner(kelime);
	int i= 0,hash = 0;
	do
	{
		i++;
		hash = (h1(hornerKey) + i*h2(hornerKey)) % M;
		if(strcmp(hashTable[hash].kelime,kelime) == 0)//Hashlenen kelime doðrumu
		{
			kontrol=1;
			i==1010;
		}
	}while(((kontrol!=1) && (strlen(hashTable[hash].kelime) != 0)) && i<1009);
	
	
	
	if(kontrol==1)//Kelime tamam
	{
		gelenKelimeler=NULL;
		kelimeDurum=-1;
		return gelenKelimeler;
	}
	else//Hatalý kelime tablosu iþlemleri
	{
		gelenKelimeler = (struct Node *)calloc(1,sizeof(struct Node));
		gelenKelimeler = hashErrorTable[errHash(kelime,horner(kelime))].head;
		if(gelenKelimeler==0)//Önerilecek kelime bulunamadý
		{
			kelimeDurum=1;
			return gelenKelimeler;
		}
		else
		{
			kelimeDurum=0;
			return gelenKelimeler;
			
		}
		
		
	}
	return gelenKelimeler;
}

int errHash(char kelime[1000],int hornerKey){//(hashlenecek Kelime, kelimenin horner methoduyla çevrilmiþ sayýsal hali) -> return hesaplanan hash deðeri döndürülür.
	/*
		Bu fonksiyonda hatalý kelimeler hashlenir. Kelimenin durumu kontrol edildikten sonra
		tüm kelimelerle mesafe hesaplamasý yapýlýr ve uygun bulunan kelimeler linkli liste
		þeklide eklenir. Eðer önerilebilecek kelime yoksa head kýsmý Null olarak kalýr.
		hesaplanan hash deðerini döndürür.
	*/
	int i= 0,hash = 0;
	do{
		int kelimeKontrol=0;
		i++;
		hash = (h1(hornerKey) + i*h2(hornerKey)) % M;
		if(strlen(hashErrorTable[hash].kelime)==0)//Yanlýþ gelen kelime error tablosuna hashlenMEmiþtir
		{
			for(i=0;i<M;i++)//Tüm kelimelerle mesafeleri kontrol edilecek
			{
				if(strlen(hashTable[i].kelime)>0)//mesafesi kontrol edilecek kelime
				{
					int mesafe = distance(kelime,hashTable[i].kelime);
					if(mesafe<=2)
					{
						kelimeKontrol++;
						
						if(hashErrorTable[hash].head==NULL)
						{
							struct Node *yeniDugum = (struct Node *)calloc(1,sizeof(struct Node));
							hashErrorTable[hash].head=yeniDugum;
							yeniDugum->next=NULL;
							strcpy(yeniDugum->onerilen,hashTable[i].kelime);
						}
						else
						{
							struct Node *nodeTmp = hashErrorTable[hash].head;
							while(nodeTmp->next != NULL)
							{
								nodeTmp=nodeTmp->next;
							}
							struct Node *yeniDugum = (struct Node *)calloc(1,sizeof(struct Node));
							nodeTmp->next=yeniDugum;
							yeniDugum->next=NULL;
							strcpy(yeniDugum->onerilen,hashTable[i].kelime);
						}
					}
				}
			}
			strcpy(hashErrorTable[hash].kelime,kelime);
			loadFactorError++;
			
		}
	}while(strcmp(hashErrorTable[hash].kelime,kelime) != 0 && i<1009);
	
	return hash;
	
}

int distance(char * kelime1,char * kelime2)//(kelime1, kelime2) -> return kelimeler arasýndaki mesafe
{
	int len1=0;
	int len2=0;
	len1=strlen(kelime1);
	len2=strlen(kelime2);
	int j;
	int i;
    int matris[len1 + 1][len2 + 1];
    for (i = 0; i <= len1; i++) {
        matris[i][0] = i;
    }
    for (i = 0; i <= len2; i++) {
        matris[0][i] = i;
    }
    for (i = 1; i <= len1; i++) {
        
        char c1;

        c1 = kelime1[i-1];
        for (j = 1; j <= len2; j++) {
            char c2;

            c2 = kelime2[j-1];
            if (c1 == c2) {
                matris[i][j] = matris[i-1][j-1];
            }
            else {
                int sil;
                int ekle;
                int substitute;
                int minimum;
				sil = matris[i-1][j] + 1;
                ekle = matris[i][j-1] + 1;
                substitute = matris[i-1][j-1] + 1;
                minimum = sil;
                if (ekle < minimum) {
                    minimum = ekle;
                }
                if (substitute < minimum) {
                    minimum = substitute;
                }
                matris[i][j] = minimum;
            }
        }
    }
    return matris[len1][len2];
}

void hashTableYazdir()	//Sözlüðün kaydedildiði hash tablosunu yazdýrýr.
{
	int i=0;
	printf("hash tablosu:\n");
	for(i=0;i<M;i++)
	{
		if(strlen(hashTable[i].kelime)>0)
		{
			printf("%d - %s\n",i,hashTable[i].kelime);	
		}
	}
}

void hataliHashTableYazdir()	//Hatalý kelimelerin ve bu kelimeler için bulunan önerileri ekrana yazdýrýr.
{
	int i=0;
	printf("hatali hash tablosu:\n");
	for(i=0;i<M;i++)
	{
		if(hashErrorTable[i].head!=NULL)
		{
			printf("%s => ",hashErrorTable[i].kelime);
			struct Node *nodeTmp = hashErrorTable[i].head;
			while(nodeTmp != NULL)
			{
				printf("%s ",nodeTmp->onerilen);
				nodeTmp=nodeTmp->next;
			}
			printf("\n");
		}
	}
}

int hash(char kelime[1000],int hornerKey){	//Kelimeleri hashlemek için kullanýlýr.
	int i= 0,hash = 0;
	do{
		i++;
		hash = (h1(hornerKey) + i*h2(hornerKey)) % M;
		if(strlen(hashTable[hash].kelime)==0)
		{
			strcpy(hashTable[hash].kelime,kelime);
			//printf("LoadFactor: %d \n",loadFactor);
			loadFactor++;	
		}
		else
		{
			if(strcmp(hashTable[hash].kelime,kelime) == 0)
			{
				//printf("ortak kelime bulundu. bulunan kelime : %s",kelime);
			}
		}
	}while(strcmp(hashTable[hash].kelime,kelime) != 0 && i<1009);
	
	
	return hash;
}


int h1(int key){	//Hashlemek için kullanýlan yardýmcý fonksiyon
	return key % M;
}

int h2(int key){	//Hashlemek için kullanýlan yardýmcý fonksiyon
	return 1 + ( key % MM);
}

int horner (char kelime[1000])	//Kelimelerin horner methoduyla sayýsal deðerlere çevrilmesi
{
	int i=0,len=0, x =2;
	int sonuc=0;
	len = strlen(kelime);
	while(kelime[i]!='\0')
	{
		sonuc += kelime[i] * getPower(x,i);
		i++;
	}
	return sonuc;
}

int getPower(int number, int power){	//Hashlemek için kullanýlan yardýmcý fonksiyon. Üs almak için kullanýlýr.
	int i,sum = 1;
	for(i = 0; i<power; i++){
		sum *= number;
	}
	return sum;
}

void baslangic(char hashDosyaAdi[])		//Baþlangýçta sözlüðün okunup hashTable isimli diziye iþlenmesi için kullanýlan fonksiyon
{
	FILE *fp=fopen(hashDosyaAdi,"r");
	if(fp=fopen(hashDosyaAdi,"r")){
		char bilgi[1000];
		while(!feof(fp)){
			fscanf(fp,"%s",bilgi);
			strcpy(bilgi,tolower(bilgi));
			int hashParameter = hash(bilgi,horner(bilgi));
			
		}
	}else{
		printf("Aradiginiz dosya yok.");
	}
	
}


