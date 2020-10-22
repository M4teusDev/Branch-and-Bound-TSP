#include <stdio.h>
#include <stdlib.h>
#include <string.h>


    typedef struct cidade
    {
        float cod, cord_x, cord_y;
    }cidade;


//Receber dados
int   recebe_dados(char *path, cidade **p_dados);
int   tamanho_arquivo(char *path, char *line, FILE *p);
int   tamanho_cabecalho( char *line, FILE *p);
void  aloca(cidade **p, int tam);

//Monta matriz
void  monta_matriz(cidade *p, int lin, int col, float m[lin][col]);

int main ( void )
{
    cidade *p_cidade = NULL;

    char file[100] = "teste01.txt";
    int n = recebe_dados(file, &p_cidade);

    float m[n][n];
    monta_matriz(p_cidade, n, n, m);
}


int  recebe_dados(char *path, cidade **p_dados)
{
    char line[1000];
    FILE *p;
    float x = 0, cod  = 0, y = 0; //Variaveis auxiliares

    int tam = tamanho_arquivo(path, line, p) - 1, i = 0, head = 0, cont = 0;

    if( (p = fopen(path,"r")) == NULL)  return 0; 
    head = tamanho_cabecalho(line, p);

    for(i = 0; i < (tam - head) ; i++)
    {
        fscanf(p,"%f %f %f",&cod, &x, &y);
        aloca(p_dados, 3);

        (*p_dados + cont)->cod = cod;
        (*p_dados + cont)->cord_x = x;
        (*p_dados + cont++)->cord_y = y;
    }
    return cont;        
}

int tamanho_arquivo(char *path, char *line, FILE *p)
{
    int tam = 0;

    if((p = fopen(path,"r")) == NULL)
        return 0; 

    while( !feof(p) ) //For até final do arq
    {
        fgets(line, 1000, p); 
        tam++;
    }

    fclose(p);
    return tam;
}

int tamanho_cabecalho(char *line, FILE *p)
{
    int k = 0;

    do{
        fgets(line, 1000, p); 
        k++;
        if(strstr(line, "NODE_COORD_SECTION") != NULL) break;
    }while( !feof(p) );

    return k;
}

void aloca(cidade **p, int tam){

  if( (*p = (cidade *)realloc( *p ,tam * sizeof(cidade))) == NULL) {
        printf("Estamos sem memoria...");
        exit(1);
    }
}

void  monta_matriz(cidade *p, int lin, int col, float m[lin][col])
{
    int i = 0, j = 0;

    for(i = 0; i < lin; i++)
    {
         printf("%f\n", (p + i)->cod);    
    }
}