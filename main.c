/*
Burak Boz
Dinamik Programlama Algoritmas� | Levenshtein Edit Distance
Sorgulanan bir c�mlede yanl�� yaz�lm�� kelimeler varsa bu kelimelerin yerine do�ru kelimeler �neren sistem.
S�zl�k olarak "smallDictionary.txt" dosyas�ndaki kelimeler kullan�lm��t�r. E�er girilen c�mledeki kelimeler bu s�zl�kte bulunmuyorsa
kelime yanl�� olarak kabul edilir. Yanl�� olan kelime yerine Levenshtein Edit Distance algoritmas� kullan�larak mesafesi 2 ve 2den az olan
kelimeler �nerilir. �nerilecek kelime yoksa yeniden girmesi istenir. Bu yanl�� kelime ve yerine girilen kelime kaydedilir. Tekrar yanl�� kelime
ile kar��la��l�rsa kaydedilen listeden tekrar �neri yap�l�r.

Kelime hashlemek i�in horner methodu kullan�lm��t�r. Hash tablosunda �ak��malara kar�� double hashing methodu kullan�lm��t�r.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M 1009
#define MM 1008


typedef struct {//Hash tipinde bir dizi tan�mlanacak ve burada kelimeler saklanacakt�r.
	char kelime[1000];
}HASH;

typedef struct {//Hatal�hash tipinde bir dizi tan�mlanacak ve Hatal� girilen kelimeler burada tutulacakt�r.
	char kelime[1000];
	struct Node* head;
}HATALIHASH;


struct Node{ //Bu d���mlerde hatal� girilen kelime yerine �nerilecek kelimeler linkli liste tipinde tutulacakt�r.
	char onerilen[1000];
	struct Node* next; 
};


//Fonksiyon prototipleri - T�m fonksiyonlara a��klamalar� yaz�lm��t�r.
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



HASH *hashTable;	//S�zl�k kelimeleri bu dizide tutulacakt�r
HATALIHASH *hashErrorTable;		//Hatal� girilen kelimeler bu dizide tutulacakt�r
int loadFactor;		//S�zl�k dizisinin doluluk oran� bu de�i�kende saklanacakt�r
int loadFactorError;		//Hatal� kelimeler dizisinin doluluk oran� bu de�i�kende saklanacakt�r.
char birlesmisCumle[5000];		//En son merge edilen c�mle bu dizide saklanacakt�r.
int kelimeDurum=0;		//Kelimenin s�zl�kteki durumunu kontrol etmek i�in kullan�lacakt�r.

int main()
{
	char cumle[1000];	//Kullan�c�dan al�nan c�mle
	int i=0;		//D�ng�ler i�in kullan�lacat�r
	int cumleSayac=0;	//Kullan�c�dan al�nan c�mlenin uzunlu�unu tutacakt�r.
	int kelimeSayac=0;	//C�mledeki i�lenen kelimenin uzunlu�unu tutacakt�r.
	hashTable = (HASH*)calloc(M, sizeof(HASH));
	hashErrorTable = (HATALIHASH*)calloc(M, sizeof(HATALIHASH));
	loadFactor=0;
	loadFactorError=0;
	birlesmisCumle[0]='\0';//C�mle bo�ialt�ld�
	int secim;	//Men� i�in kullan�lacak.
	
	
	for(i=0;i<M;i++)//Haz�rlanan tablolar temizleniyor.
	{
		memset(hashTable[i].kelime,0,sizeof(hashTable[i].kelime));
		memset(hashErrorTable[i].kelime,0,sizeof(hashErrorTable[i].kelime));
		hashErrorTable[i].head=NULL;
	}
	
	
	printf("sozluk okunuyor...");//S�zl�k okunuyor ve ekrana yazd�r�l�yor
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
			printf("lutfen cumlenizi girin:");//Kullan�c�dan c�mle al�n�yor.
			gets(cumle);
			int init_size = strlen(cumle);//C�mle kelimelere b�l�n�yor ve i�lenmeye ba�lan�yor
			char delim[] = " ";
			char *ptr = strtok(cumle, delim);
			while(ptr != NULL)
			{
				char yeniKelime[100];
				struct Node* gelenKelime = kelimeAra(ptr);	//Kelime s�zl�kte aranmaya ba�lan�yor.
				if(kelimeDurum==-1)// Kelime do�ru (E�er kelime s�zl�kte mevcutsa kelimeDurum de�i�keni -1 olarak g�ncellenir.)
				{
					printf("%s (kelime dogru)\n",ptr);
					sMerge(ptr);			
				}
				else if(kelimeDurum==1)//kelimeDurum Kelime yanl�� fakat �nerilecek kelime yok. Bu durumda kullan�c�dan al�nan kelime hatal� kelimenin yan�na �nerilen olarak eklenir
				{
					printf("%s - kelime yanlis fakat onerilecek kelime yok. Lutfen dogru halini tekrar yazin\n(hatali kelime yerine cumleye eklenecektir.):",ptr);
					gets(yeniKelime);
					yeniEkle(ptr,yeniKelime);
					sMerge(yeniKelime);
				}
				else//Di�er iki durum ger�ekle�memi�se kelime yanl��t�r ve listede �nerilecek kelime vard�r. Bu kelimeler kullan�c�ya �nerilir
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
			printf("\n\nCumlenin son hali : %s\n\n",birlesmisCumle);//C�mlenin son hali ekrana yazd�r�l�r
			memset(birlesmisCumle, '\0', sizeof birlesmisCumle);
		}
		else if(secim==2)//S�zl��� yazd�r
		{
			hashTableYazdir();
		}
		else if(secim==3)//Hatal� kelimeleri yazd�r
		{
			hataliHashTableYazdir();
		}
		else
		{
			printf("Gecersiz secim\n");
		}
	}while(secim!=0);
	
}

void yeniEkle(char * hatali,char * guncel)//(hatal� kelime, yerine girilecek g�ncel kelime) -> void
{
	int kontrol=0;
	int hornerKey = horner(hatali);
	int i= 0,hash = 0;
	do
	{
		i++;
		hash = (h1(hornerKey) + i*h2(hornerKey)) % M;
		if(strcmp(hashErrorTable[hash].kelime,hatali) == 0)//Hashlenen kelime do�rumu
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

void sMerge(char * ptr)//(kelime) -> void | birle�mi� c�mle de�i�keninde uygun kelimeler birle�tirilir.
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

struct Node * kelimeAra(char kelime[])//(AranacakKelime) return -> �nerilecek kelimeler | �nerilecek kelime yoksa kelimeDurum de�i�keni g�ncellenir.
{
	struct Node *gelenKelimeler;
	int kontrol=0;
	int hornerKey = horner(kelime);
	int i= 0,hash = 0;
	do
	{
		i++;
		hash = (h1(hornerKey) + i*h2(hornerKey)) % M;
		if(strcmp(hashTable[hash].kelime,kelime) == 0)//Hashlenen kelime do�rumu
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
	else//Hatal� kelime tablosu i�lemleri
	{
		gelenKelimeler = (struct Node *)calloc(1,sizeof(struct Node));
		gelenKelimeler = hashErrorTable[errHash(kelime,horner(kelime))].head;
		if(gelenKelimeler==0)//�nerilecek kelime bulunamad�
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

int errHash(char kelime[1000],int hornerKey){//(hashlenecek Kelime, kelimenin horner methoduyla �evrilmi� say�sal hali) -> return hesaplanan hash de�eri d�nd�r�l�r.
	/*
		Bu fonksiyonda hatal� kelimeler hashlenir. Kelimenin durumu kontrol edildikten sonra
		t�m kelimelerle mesafe hesaplamas� yap�l�r ve uygun bulunan kelimeler linkli liste
		�eklide eklenir. E�er �nerilebilecek kelime yoksa head k�sm� Null olarak kal�r.
		hesaplanan hash de�erini d�nd�r�r.
	*/
	int i= 0,hash = 0;
	do{
		int kelimeKontrol=0;
		i++;
		hash = (h1(hornerKey) + i*h2(hornerKey)) % M;
		if(strlen(hashErrorTable[hash].kelime)==0)//Yanl�� gelen kelime error tablosuna hashlenMEmi�tir
		{
			for(i=0;i<M;i++)//T�m kelimelerle mesafeleri kontrol edilecek
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

int distance(char * kelime1,char * kelime2)//(kelime1, kelime2) -> return kelimeler aras�ndaki mesafe
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

void hashTableYazdir()	//S�zl���n kaydedildi�i hash tablosunu yazd�r�r.
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

void hataliHashTableYazdir()	//Hatal� kelimelerin ve bu kelimeler i�in bulunan �nerileri ekrana yazd�r�r.
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

int hash(char kelime[1000],int hornerKey){	//Kelimeleri hashlemek i�in kullan�l�r.
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


int h1(int key){	//Hashlemek i�in kullan�lan yard�mc� fonksiyon
	return key % M;
}

int h2(int key){	//Hashlemek i�in kullan�lan yard�mc� fonksiyon
	return 1 + ( key % MM);
}

int horner (char kelime[1000])	//Kelimelerin horner methoduyla say�sal de�erlere �evrilmesi
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

int getPower(int number, int power){	//Hashlemek i�in kullan�lan yard�mc� fonksiyon. �s almak i�in kullan�l�r.
	int i,sum = 1;
	for(i = 0; i<power; i++){
		sum *= number;
	}
	return sum;
}

void baslangic(char hashDosyaAdi[])		//Ba�lang��ta s�zl���n okunup hashTable isimli diziye i�lenmesi i�in kullan�lan fonksiyon
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


