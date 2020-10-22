#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>

typedef struct cidade
{
    float cod, cord_x, cord_y;
}cidade;


//Receber dados
int    recebe_dados(char *path, cidade **p_dados);
int    tamanho_arquivo(char *path, char *line, FILE *p);
int    tamanho_cabecalho( char *line, FILE *p);
void   aloca(cidade **p, int tam);

//Monta matriz
void   monta_matriz(cidade *p, int lin, int col, float m[lin][col]);
float   minimiza_matriz(int lin, int col, float m[lin][col]);
float  menor_valor_linha(int lin, int col, float m[lin][col]);
float  menor_valor_coluna(int lin, int col, int aux, float m[lin][col]);

void   exibe_matriz(int lin, int col, float m[lin][col]);

int main ( void )
{
    setlocale(LC_ALL, "Portuguese");

    cidade *p_cidade = NULL;

    char file[100] = "teste01.txt";
    int n = recebe_dados(file, &p_cidade);

    float m[n][n];
    monta_matriz(p_cidade, n, n, m);
}


int   recebe_dados(char *path, cidade **p_dados)
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
    return cont - 1;        
}

int   tamanho_arquivo(char *path, char *line, FILE *p)
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

int   tamanho_cabecalho(char *line, FILE *p)
{
    int k = 0;

    do{
        fgets(line, 1000, p); 
        k++;
        if(strstr(line, "NODE_COORD_SECTION") != NULL) break;
    }while( !feof(p) );

    return k;
}

void  aloca(cidade **p, int tam){

  if( (*p = (cidade *)realloc( *p ,tam * sizeof(cidade))) == NULL) {
        printf("Estamos sem memoria...");
        exit(1);
    }
}

void  monta_matriz(cidade *p, int lin, int col, float m[lin][col])
{
    int i = 0, j = 0;
    float min;

    for(i = 0; i < lin; i++)
        for(j = 0; j < col; j++)
            m[i][j] = sqrt(pow( ((p + i)->cord_x - (p + j)->cord_x), 2) + pow(((p + i)->cord_y - (p + j)->cord_y), 2));
    
    min = minimiza_matriz(lin, col, m);

    printf("Primeira solucao factivel %.2f\n\n",min);
}

float  minimiza_matriz(int lin, int col, float m[lin][col])
{
    int i = 0, j = 0;
    float min = 0, sum_min = 0.0;
   
    for( i = 0; i < lin; i++) {
        sum_min += (min = menor_valor_linha(i, col, m));
        //printf("Valor minimo: %.2f\n", min);
        for( j = 0; j < col; j++)
            if(i  != j) m[i][j] -= min;
    }

    for( j = 0; j < col; j++){
        sum_min += (min = menor_valor_coluna(lin, col, j, m));
        for( i = 0; i < lin; i++)
            if(i  != j)
                m[i][j] -= min;
    }

    return sum_min; //Retorna 1° solução
}

float menor_valor_linha(int lin, int col, float m[lin][col])
{
    int j = 0, flag = 0;
    float min = 0;

    for(j = 0; j < col; j++) {
        if((min > m[lin][j] && lin != j) || (flag == 0 && lin != j)) {
            min =  m[lin][j];
            flag++;
        }
    }
    return min;
}

float menor_valor_coluna(int lin, int col, int aux, float m[lin][col])
{
    int i = 0, j = 0, flag = 0;
    float min  = 0.0;

    for( i = 0; i < lin; i++)
    {
        if((min > m[i][aux] && i != aux) || (flag == 0 && aux != i)) {
            min =  m[i][aux];
            flag++;
        }
    }
    return min;
}

void  exibe_matriz(int lin, int col, float m[lin][col])
{
    int i = 0, j = 0;

    printf("\t");
    for(i = 0; i < lin; i++)
        printf("x%i\t", i + 1);
    printf("\n\n");

    for(i = 0; i < lin; i++)
    {
        printf("x%i\t", i + 1);
        for(j = 0; j < col; j++)
        {
            printf("%.1f\t", m[i][j]);
        }
        printf("\n");
    } 
}
