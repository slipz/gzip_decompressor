/*Author: Rui Pedro Paiva
Teoria da Informação, LEI, 2007/2008*/

#include <cstdlib>
#include <math.h>

#include "gzip.h"
#include "huffman.h"

#define maxhclen 19

HuffmanTree* cria_arvore( int num , int comp[]) ;

//função principal, a qual gere todo o processo de descompactação
int main(int argc, char** argv)
{
	//--- Gzip file management variables
	FILE *gzFile;  //ponteiro para o ficheiro a abrir
	FILE *newFile;
	long fileSize;
	long origFileSize;
	int numBlocks = 0;
	gzipHeader gzh;
	unsigned char byte;  //variável temporária para armazenar um byte lido directamente do ficheiro
	int rb = 0;  //último byte lido (poderá ter mais que 8 bits, se tiverem sobrado alguns de leituras anteriores)
	int needBits = 0, availBits = 0;


	//--- obter ficheiro a descompactar
	char fileName[] = "big.txt.gz";

	/*if (argc != 2)
	{
		printf("Linha de comando inválida!!!");
		return -1;
	}*/
	//char * fileName = argv[1];

	//--- processar ficheiro
	gzFile = fopen(fileName, "rb");
	fseek(gzFile, 0L, SEEK_END);
	fileSize = ftell(gzFile);
	fseek(gzFile, 0L, SEEK_SET);

	//ler tamanho do ficheiro original (acrescentar: e definir Vector com símbolos
	origFileSize = getOrigFileSize(gzFile);


	//--- ler cabeçalho
	int erro = getHeader(gzFile, &gzh);
	if (erro != 0)
	{
		printf ("Formato inválido!!!");
		return -1;
	}

	//--- Para todos os blocos encontrados
	char BFINAL;
	int HLIT ;
    int HDIST ;
    int HCLEN ;

    newFile = fopen(gzh.fName, "wb");
    printf("%d\n", origFileSize);

    printf("oi");
    if(newFile == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

	do
	{
	    printf("\n\n%s", "Outro Bloco !!!!!");
		//--- ler o block header: primeiro byte depois do cabeçalho do ficheiro
		BFINAL = le_bits(1, &availBits, &rb , gzFile) ;

		//analisar block header e ver se é huffman dinâmico
		int BTYPE = le_bits(2, &availBits, &rb , gzFile) ;
		if (!isDynamicHuffman2(BTYPE))  //ignorar bloco se não for Huffman dinâmico
			continue;

		//--- Se chegou aqui --> compactado com Huffman dinâmico --> descompactar
        ///exercicio 1
        /// calcula HLIT = 19
        HLIT = le_bits(5, &availBits, &rb , gzFile) ;
        printf("HLIT = %d\n",HLIT);

		/// calcula HDIST = 22
		HDIST = le_bits(5, &availBits, &rb , gzFile) ;
        printf("HDIST = %d\n",HDIST);

        /// CALCULA HCLEN = 10
        HCLEN = le_bits(4, &availBits, &rb , gzFile) ;
        printf("HCLEN = %d\n",HCLEN);

        /// exercicio 2
        int i ;
        int j = 0 ;
        int vetor[maxhclen] ;
         for(i = 0 ; i < maxhclen;i++){
            vetor[i] = 0 ;
         }
        int d = 16 ;
        for(i = 0 ; i < (HCLEN + 4);i++){
            if(i < 3){
                vetor[d] = le_bits(3, &availBits, &rb , gzFile) ;
                printf("%d = %d\n", d ,vetor[d]) ;
                d++;
            }
            else if(i == 3){
                vetor[0] = le_bits(3, &availBits, &rb , gzFile) ;
                printf("0 = %d\n",vetor[0]) ;
                d = 8 ;
            }
            else{
                if(i % 2 == 0){
                    vetor[d + j] = le_bits(3, &availBits, &rb , gzFile) ;
                    printf("%d = %d\n",d + j,vetor[d + j]) ;
                    j++;
                }
                else {
                    vetor[d - j] = le_bits(3, &availBits, &rb , gzFile) ;
                    printf("%d = %d\n", d - j ,vetor[d - j]) ;
                }
            }


        }
        /*for(i = 0 ; i < 19;i++)
            printf("%d\n",vetor[i]) ;*/
        ///exercicio 3
        int menor = vetor[0] ;
        int limite = vetor[0] ;
        for(i = 0;i<maxhclen;i++){
            if(vetor[i] != 0){
                if(menor>vetor[i])
                    menor = vetor[i] ;
            }
            if(vetor[i] != 0){
                if(limite<vetor[i])
                    limite = vetor[i] ;
            }
        }
        printf("menor = %d\n",menor) ;
        printf("limite = %d\n",limite) ;
        int decimal = 0 ;
        int vdec[maxhclen] ;
        for(i = 0;i<(HCLEN + 4);i++){
            vdec[i] = 0 ;
        }
        while(limite+1>menor){
            decimal = decimal*2;
            for(i = 0;i<maxhclen;i++){
                if(vetor[i] == menor){
                    vdec[i] = decimal;
                    decimal++ ;
                }
            }
            menor++;
        }
        printf("inicio\n") ;

        int erro,pos;
        short verbose = 1;
        char code[100];
        HuffmanTree *hft = createHFTree();

        char **vbin = (char**)malloc((maxhclen)*sizeof(char*));
        for(i = 0;i<maxhclen;i++){
            resetCurNode(hft) ;
            vbin[i] = (char*)malloc(sizeof(char) * vetor[i]+1);
            bits2String( vbin[i], vdec[i] ,vetor[i]);
            if(strcmp("",vbin[i]) != 0){
                printf("\n%d vet[%d] = %d\t",vdec[i],i,i);
                printf("huff = %s ",vbin[i]);
                strcpy(code, vbin[i]);
                addNode(hft, vbin[i], i, verbose);
            }
        }
        ///exercicio 4
        int compHLIT[HLIT + 257] ;
        int valor = 0;
        int anterior ;
        for(int i = 0;i< HLIT + 257;){
            resetCurNode(hft) ;

            valor = nextNode(hft , le_bits(1, &availBits, &rb , gzFile) + '0') ;
            while(valor < 0){
                valor = nextNode(hft , le_bits(1, &availBits, &rb , gzFile) + '0' ) ;
            }
            if(valor < 16){
                anterior = valor ;
                compHLIT[i] = valor ;
                printf("compHLIT[%d] = %d\n",i,compHLIT[i]);
                i++ ;
            }
            else if (valor == 16){
                limite = le_bits(2, &availBits, &rb , gzFile) ;
                for(int j = 0 ; j < limite + 3 ; j++){
                    compHLIT[i] = anterior ;
                    printf("compHLIT[%d] = %d\n",i,compHLIT[i]);
                    i++;
                }
            }
            else if (valor == 17){
                limite = le_bits(3, &availBits, &rb , gzFile) ;
                for(int j = 0 ; j < limite + 3 ; j++){
                    compHLIT[i] = 0 ;
                    printf("compHLIT[%d] = %d\n",i,compHLIT[i],limite);
                    i++;
                }
            }
            else if (valor == 18){
                limite = le_bits(7, &availBits, &rb , gzFile) ;
                for(int j = 0 ; j < limite + 11 ; j ++){
                    compHLIT[i] = 0 ;
                    printf("compHLIT[%d] = %d\n",i,compHLIT[i]);
                    i++;
                }
            }
        }
        /// exercicio 6 parte a

        HuffmanTree *literais = cria_arvore( HLIT + 257 , compHLIT );


        /// exercicio 5
        int compHDIST[HDIST + 1] ;
        for(int i = 0;i< HDIST + 1;){
            resetCurNode(hft) ;

            valor = nextNode(hft , le_bits(1, &availBits, &rb , gzFile) + '0') ;
            while(valor < 0){
                valor = nextNode(hft , le_bits(1, &availBits, &rb , gzFile) + '0' ) ;
            }
            if(valor < 16){
                anterior = valor ;
                compHDIST[i] = valor ;
                printf("compHDIST[%d] = %d\n",i,compHDIST[i]);
                i++ ;
            }
            else if (valor == 16){
                limite = le_bits(2, &availBits, &rb , gzFile) ;
                for(int j = 0 ; j < limite + 3 ; j++){
                    compHDIST[i] = anterior ;
                    printf("compHDIST[%d] = %d\n",i,compHDIST[i]);
                    i++;
                }
            }
            else if (valor == 17){
                limite = le_bits(3, &availBits, &rb , gzFile) ;
                for(int j = 0 ; j < limite + 3 ; j++){
                    compHDIST[i] = 0 ;
                    printf("compHDIST[%d] = %d\n",i,compHDIST[i],limite);
                    i++;
                }
            }
            else if (valor == 18){
                limite = le_bits(7, &availBits, &rb , gzFile) ;
                for(int j = 0 ; j < limite + 11 ; j ++){
                    compHDIST[i] = 0 ;
                    printf("compHDIST[%d] = %d\n",i,compHDIST[i]);
                    i++;
                }
            }
        }

        /// exercicio 6 parte b
        HuffmanTree *distancias = cria_arvore( HDIST + 1 , compHDIST ) ;

        unsigned char descomprimido[1000000];
        int index = 0;

        for(;;){
            //printf(" %d ", index);
            resetCurNode(literais) ;
            int x = le_bits(1, &availBits, &rb , gzFile);
            valor = nextNode(literais , x + '0') ;
            ///printf("%d",x) ;
            while(valor < 0){
                x = le_bits(1, &availBits, &rb , gzFile);
                valor = nextNode(literais , x + '0' ) ;
                ///printf("%d",x) ;
            }

            if(valor == 256){
                printf("Chegou ao fim do bloco\n");
                break;
            }else if(valor > 256 && valor < 286){
                int comp;
                if(valor < 265){
                    comp = valor - 257 + 3;
                }
                /*else if (valor <= 284) {
                    int numExtraBits = (valor - 261) / 4;
                    comp = (((valor - 265) % 4 + 4) << numExtraBits) + 3 + le_bits(numExtraBits, &availBits, &rb , gzFile);
                }*/
                else if(valor < 269){
                    comp = valor - 257 + 3 + (2*(valor-265)) - (valor-265)+ le_bits(1, &availBits, &rb , gzFile);
                }else if(valor < 273){
                    comp = valor - 250 + (4*(valor-269)) - (valor-269)+ le_bits(2, &availBits, &rb , gzFile);
                }else if(valor < 277){
                    comp = valor - 238 + (8*(valor-273)) - (valor-273)+ le_bits(3, &availBits, &rb , gzFile);
                }else if(valor < 281){
                    comp = valor - 210 + (16*(valor-277)) - (valor-277)+ le_bits(4, &availBits, &rb , gzFile);
                }else if(valor < 285){
                    comp = valor - 150 + (32*(valor-281)) - (valor-281)+ le_bits(5, &availBits, &rb , gzFile);
                }else if(valor == 285){
                    comp = 258;
                }

                resetCurNode(distancias) ;
                valor = nextNode(distancias , le_bits(1, &availBits, &rb , gzFile) + '0') ;
                while(valor < 0){
                    valor = nextNode(distancias , le_bits(1, &availBits, &rb , gzFile) + '0' ) ;
                }

                int dist;
                if(valor < 4){
                    dist = valor + 1;
                }

                /*else if (valor <= 29) {
                    int numExtraBits = valor / 2 - 1;
                    dist = ((valor % 2 + 2) << numExtraBits) + 1 + le_bits(numExtraBits, &availBits, &rb , gzFile);
                }*/
                else if(valor < 6){
                    dist = valor + 1 + (2*(valor-4)) - (valor-4) + le_bits(1, &availBits, &rb , gzFile);
                }else if(valor < 8){
                    dist = valor + 3 + (4*(valor-6)) - (valor-6) + le_bits(2, &availBits, &rb , gzFile);
                }else if(valor < 10){
                    dist = valor + 9 + (8*(valor-8)) - (valor-8) + le_bits(3, &availBits, &rb , gzFile);
                }else if(valor <12){
                    dist = valor + 23 + (16*(valor-10)) - (valor-10) + le_bits(4, &availBits, &rb , gzFile);
                }else if(valor < 14){
                    dist = valor + 53 + (32*(valor-12)) - (valor-12) + le_bits(5, &availBits, &rb , gzFile);
                }else if(valor < 16){
                    dist = valor + 115 + (64*(valor-14)) - (valor-14) + le_bits(6, &availBits, &rb , gzFile);
                }else if(valor < 18){
                    dist = valor + 241 + (128*(valor-16)) - (valor-16) + le_bits(7, &availBits, &rb , gzFile);
                }else if(valor < 20){
                    dist = valor + 495 + (256*(valor-18)) - (valor-18) + le_bits(8, &availBits, &rb , gzFile);
                }else if(valor < 22){
                    dist = valor + 1005 + (512*(valor-20)) - (valor-20) + le_bits(8, &availBits, &rb , gzFile) + (le_bits(1, &availBits, &rb , gzFile) << 8);
                }else if(valor < 24){
                    dist = valor + 2027 + (1024*(valor-22)) - (valor-22) + le_bits(8, &availBits, &rb , gzFile) + (le_bits(2, &availBits, &rb , gzFile) << 8);
                }else if(valor < 26){
                    dist = valor + 4073 + (2048*(valor-24)) - (valor-24) + le_bits(8, &availBits, &rb , gzFile) + (le_bits(3, &availBits, &rb , gzFile) << 8);
                }else if(valor < 28){
                    dist = valor + 8167 + (4096*(valor-26)) - (valor-26) + le_bits(8, &availBits, &rb , gzFile) + (le_bits(4, &availBits, &rb , gzFile) << 8);
                }else if(valor < 30){
                    dist = valor + 16357 + (8192*(valor-28)) - (valor-28) + le_bits(8, &availBits, &rb , gzFile) + (le_bits(5, &availBits, &rb , gzFile) << 8);
                }
                //printf("comp = %d, dist = %d ",comp, dist);
                int primeiro = index-dist ;
                for(int j = 0;j < comp;j++){
                        //printf(" %d ", j);
                     descomprimido[index] = descomprimido[primeiro + j];
                     ///printf("%c",descomprimido[index]) ;
                     index++;
                }


            }else if(valor < 256){
                descomprimido[index] = valor;
                ///printf("valor = %d %c\n",valor ,descomprimido[index]) ;
                ///printf("%c",descomprimido[index]) ;
                index++;
            }
        }

        //printf("'%c'  ia= %d , is = %d\n",descomprimido[index],index ,index-dist+j) ;
        fwrite(descomprimido, sizeof(unsigned char)*index, 1, newFile);

		//actualizar número de blocos analisados
		numBlocks++;
		printf("fim de bloco");
		delete descomprimido;
		delete distancias;
		delete hft;
		delete literais;
	}while(BFINAL == 0);

	//terminações
	fclose(gzFile);
	printf("End: %d bloco(s) analisado(s).\n", numBlocks);


    //teste da função bits2String: RETIRAR antes de criar o executável final
	char str[9];
	bits2String2(str, 0x03);
	printf("%s\n", str);


    //RETIRAR antes de criar o executável final
    system("PAUSE");
    return EXIT_SUCCESS;
}
/*int comprimentos(){
    int comp[HLIT + 257] ;
        int valor = 0;
        int anterior ;
        for(int i = 0;i< HLIT + 257;){
            resetCurNode(hft) ;
            anterior = valor ;
            valor = nextNode(hft , le_bits(1, &availBits, &rb , gzFile) + '0') ;
            while(valor < 0){
                valor = nextNode(hft , le_bits(1, &availBits, &rb , gzFile) + '0' ) ;
            }
            if(valor < 16){
                compHLIT[i] = valor ;
                printf("compHLIT[%d] = %d\n",i,compHLIT[i]);
                i++ ;
            }
            else if (valor == 16){
                limite = le_bits(2, &availBits, &rb , gzFile) ;
                for(int j = 0 ; j < limite + 3 ; j++){
                    compHLIT[i] = anterior ;
                    printf("compHLIT[%d] = %d\n",i,compHLIT[i]);
                    i++;
                }
            }
            else if (valor == 17){
                limite = le_bits(3, &availBits, &rb , gzFile) ;
                for(int j = 0 ; j < limite + 3 ; j++){
                    compHLIT[i] = 0 ;
                    printf("compHLIT[%d] = %d\n",i,compHLIT[i],limite);
                    i++;
                }
            }
            else if (valor == 18){
                limite = le_bits(7, &availBits, &rb , gzFile) ;
                for(int j = 0 ; j < limite + 11 ; j ++){
                    compHLIT[i] = 0 ;
                    printf("compHLIT[%d] = %d\n",i,compHLIT[i]);
                    i++;
                }
            }
        }
    return comp ;
}*/

HuffmanTree* cria_arvore( int num , int comp[]){
     int menor = 100 ;
     int limite = comp[0] ;
     int verbose = 1 ;
     int i ;
        for(i = 0;i<num;i++){
            if(comp[i] != 0){
                if(menor>comp[i])
                    menor = comp[i] ;
            }
            if(comp[i] != 0){
                if(limite<comp[i])
                    limite = comp[i] ;
            }
        }
        printf("menor = %d\n",menor) ;
        printf("limite = %d\n",limite) ;
        int decimal = 0 ;
        int vHLIT[num] ;
        for(i = 0;i<num;i++){
            vHLIT[i] = 0 ;
        }
        decimal = 0 ;
        while(limite+1>menor){
            decimal = decimal*2;
            for(i = 0;i<num;i++){
                if(comp[i] == menor){
                    vHLIT[i] = decimal;
                    decimal++ ;
                }
            }
            menor++;
        }

        HuffmanTree *tree = createHFTree();

        char **vhHLIT = (char**)malloc(num*sizeof(char*));
        for(i = 0;i<num;i++){
            if(comp[i] != 0){
                vhHLIT[i] = (char*)malloc(sizeof(char) * comp[i]+1);
                if (comp[i] <= 8){
                    bits2String( vhHLIT[i], vHLIT[i] ,comp[i]);
                    printf("\n%d vHLIT[%d] = %d  comp = %d\t",vHLIT[i],i,i,comp[i]);
                    printf("huff = %s \t",vhHLIT[i]);
                }
                else{
                    int compatual = comp[i];
                    while(compatual>8){
                        char *temp2 = (char*)malloc(sizeof(char) * 8 + 1);
                        bits2String( temp2, vHLIT[i] , 8 );
                        bits2String( vhHLIT[i], vHLIT[i] >> 8 , comp[i] - 8 );
                        strcat(vhHLIT[i] , temp2) ;
                        printf("\n%d vHLIT[%d] = %d  comp = %d\t",vHLIT[i],i,i,comp[i]);
                        printf("huff = %s\t",vhHLIT[i]);
                        compatual -= 8;
                    }
                }
                if(strcmp(vhHLIT[i],"")!=0){
                    addNode(tree, vhHLIT[i], i, verbose);
                    printf("valor inserido = %d\n",findNode(tree, vhHLIT[i],verbose));
                }
            }
        }
        return tree ;
}


int le_byte(FILE *gzFile){
    unsigned char byte ;
    fread(&byte, 1, 1, gzFile);
    int rb = (byte << 0) | 0;
    return rb ;
}

int le_bits(int needBits, int* availBits, int* rb , FILE *gzFile){
    int result;
    unsigned char mask;
    unsigned char byte;
    if (*availBits < needBits){
        fread(&byte, 1, 1, gzFile);
        *rb = (byte << (*availBits)) | *rb;
        *availBits += 8;
    }

    mask = (1 << needBits) - 1;
    result = (*rb) & mask;
    *rb = *rb >> needBits;
    *availBits -=needBits;

    return result;
}

//---------------------------------------------------------------
//Lê o cabeçalho do ficheiro gzip: devolve erro (-1) se o formato for inválidodevolve, ou 0 se ok
int getHeader(FILE *gzFile, gzipHeader *gzh) //obtém cabeçalho
{
	unsigned char byte;

	//Identicação 1 e 2: valores fixos
	fread(&byte, 1, 1, gzFile);
	(*gzh).ID1 = byte;
	if ((*gzh).ID1 != 0x1f) return -1; //erro no cabeçalho

	fread(&byte, 1, 1, gzFile);
	(*gzh).ID2 = byte;
	if ((*gzh).ID2 != 0x8b) return -1; //erro no cabeçalho

	//Método de compressão (deve ser 8 para denotar o deflate)
	fread(&byte, 1, 1, gzFile);
	(*gzh).CM = byte;
	if ((*gzh).CM != 0x08) return -1; //erro no cabeçalho

	//Flags
	fread(&byte, 1, 1, gzFile);
	unsigned char FLG = byte;

	//MTIME
	char lenMTIME = 4;
	fread(&byte, 1, 1, gzFile);
	(*gzh).MTIME = byte;
	for (int i = 1; i <= lenMTIME - 1; i++)
	{
		fread(&byte, 1, 1, gzFile);
		(*gzh).MTIME = (byte << 8)*(i+1) + (*gzh).MTIME;		///*(i+1)
	}

	//XFL (not processed...)
	fread(&byte, 1, 1, gzFile);
	(*gzh).XFL = byte;

	//OS (not processed...)
	fread(&byte, 1, 1, gzFile);
	(*gzh).OS = byte;

	//--- Check Flags
	(*gzh).FLG_FTEXT = (char)(FLG & 0x01);
	(*gzh).FLG_FHCRC = (char)((FLG & 0x02) >> 1);
	(*gzh).FLG_FEXTRA = (char)((FLG & 0x04) >> 2);
	(*gzh).FLG_FNAME = (char)((FLG & 0x08) >> 3);
	(*gzh).FLG_FCOMMENT = (char)((FLG & 0x10) >> 4);

	//FLG_EXTRA
	if ((*gzh).FLG_FEXTRA == 1)
	{
		//ler 2 bytes XLEN + XLEN bytes de extra field
		//1º byte: LSB, 2º: MSB
		char lenXLEN = 2;

		fread(&byte, 1, 1, gzFile);
		(*gzh).xlen = byte;
		fread(&byte, 1, 1, gzFile);
		(*gzh).xlen = (byte << 8) + (*gzh).xlen;

		(*gzh).extraField = new unsigned char[(*gzh).xlen];

		//ler extra field (deixado como está, i.e., não processado...)
		for (int i = 0; i <= (*gzh).xlen - 1; i++)
		{
			fread(&byte, 1, 1, gzFile);
			(*gzh).extraField[i] = byte;
		}
	}
	else
	{
		(*gzh).xlen = 0;
		(*gzh).extraField = 0;
	}

	//FLG_FNAME: ler nome original
	if ((*gzh).FLG_FNAME == 1)
	{
		(*gzh).fName = new char[1024];
		unsigned int i = 0;
		do
		{
			fread(&byte, 1, 1, gzFile);
			if (i <= 1023)  //guarda no máximo 1024 caracteres no array
				(*gzh).fName[i] = byte;
			i++;
		}while(byte != 0);
		if (i > 1023)
			(*gzh).fName[1023] = 0;  //apesar de nome incompleto, garantir que o array termina em 0
	}
	else
		(*gzh).fName = 0;

	//FLG_FCOMMENT: ler comentário
	if ((*gzh).FLG_FCOMMENT == 1)
	{
		(*gzh).fComment = new char[1024];
		unsigned int i = 0;
		do
		{
			fread(&byte, 1, 1, gzFile);
			if (i <= 1023)  //guarda no máximo 1024 caracteres no array
				(*gzh).fComment[i] = byte;
			i++;
		}while(byte != 0);
		if (i > 1023)
			(*gzh).fComment[1023] = 0;  //apesar de comentário incompleto, garantir que o array termina em 0
	}
	else
		(*gzh).fComment = 0;


	//FLG_FHCRC (not processed...)
	if ((*gzh).FLG_FHCRC == 1)
	{
		(*gzh).HCRC = new unsigned char[2];
		fread(&byte, 1, 1, gzFile);
		(*gzh).HCRC[0] = byte;
		fread(&byte, 1, 1, gzFile);
		(*gzh).HCRC[1] = byte;
	}
	else
		(*gzh).HCRC = 0;

	return 0;
}


//---------------------------------------------------------------
//Analisa block header e vê se é huffman dinâmico
int isDynamicHuffman(unsigned char rb)
{
	unsigned char BTYPE = rb & 0x03;

	if (BTYPE == 0) //--> sem compressão
	{
		printf("Ignorando bloco: sem compactação!!!\n");
		return 0;
	}
	else if (BTYPE == 1)
	{
		printf("Ignorando bloco: compactado com Huffman fixo!!!\n");
		return 0;
	}
	else if (BTYPE == 3)
	{
		printf("Ignorando bloco: BTYPE = reservado!!!\n");
		return 0;
	}
	else
		return 1;
}

int isDynamicHuffman2(int BTYPE)
{
	if (BTYPE == 0) //--> sem compressão
	{
		printf("Ignorando bloco: sem compactação!!!\n");
		return 0;
	}
	else if (BTYPE == 1)
	{
		printf("Ignorando bloco: compactado com Huffman fixo!!!\n");
		return 0;
	}
	else if (BTYPE == 3)
	{
		printf("Ignorando bloco: BTYPE = reservado!!!\n");
		return 0;
	}
	else
		return 1;
}


//---------------------------------------------------------------
//Obtém tamanho do ficheiro original
long getOrigFileSize(FILE * gzFile)
{
	//salvaguarda posição actual do ficheiro
	long fp = ftell(gzFile);

	//últimos 4 bytes = ISIZE;
	fseek(gzFile, -4, SEEK_END);

	//determina ISIZE (só correcto se cabe em 32 bits)
	unsigned long sz = 0;
	unsigned char byte;
	fread(&byte, 1, 1, gzFile);
	sz = byte;
	for (int i = 0; i <= 2; i++)
	{
		fread(&byte, 1, 1, gzFile);
		sz = (byte << 8*(i+1)) + sz;
	}


	//restaura file pointer
	fseek(gzFile, fp, SEEK_SET);

	return sz;
}


//---------------------------------------------------------------
void bits2String(char *strBits, unsigned char byte,int comp)
{
	char mask = 0x01;  //get LSbit

	for (char i = comp-1; i >= 0; i--)
	{
		char bit = byte & mask;
		strBits[i] = bit +48; //converter valor numérico para o caracter alfanumérico correspondente
		byte = byte >> 1;

	}
	strBits[comp] = '\0'; //Insere \0 na ultima posição da string (seq escape)
}

void bits2String2(char *strBits, unsigned char byte)
{
	char mask = 0x01;  //get LSbit

	strBits[8] = 0;
	for (char bit, i = 7; i >= 0; i--)
	{
		bit = byte & mask;
		strBits[i] = bit +48; //converter valor numérico para o caracter alfanumérico correspondente
		byte = byte >> 1;
	}
}
