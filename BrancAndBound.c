#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct cidade
{
    float cod, cord_x, cord_y;
}cidade;


typedef struct caminho
{
    int cod;
    float distancia;
}trajeto;

//Receber dados
int    recebe_dados(char *path, cidade **p_dados);
int    tamanho_arquivo(char *path, char *line, FILE *p);
int    tamanho_cabecalho( char *line, FILE *p);
void   aloca(cidade **p, int tam);

//Monta matriz
float  monta_matriz(cidade *p, int lin, int col, float m[lin][col]);
float  minimiza_matriz(int lin, int col, float m[lin][col]);
float  menor_valor_linha(int lin, int col, float m[lin][col]);
float  menor_valor_coluna(int lin, int col, int aux, float m[lin][col]);

void   exibe_matriz(int lin, int col, float m[lin][col]);

//Branch and bound
void  branch_and_bound(int lin, int col, float m[lin][col], float valor_minimo);
void  copia_matriz(int lin, int col, float m[lin][col], float m_aux[lin][col]);
float traca_caminho(int inicio_de_partida, int prox_parada, int lin, int col, float m[lin][col]);
void  zera_coluna_linha_e_encontro(int inicio_de_partida, int prox_parada,int lin, int col, float m[lin][col]);

int main ()
{
    cidade *p_cidade = NULL;

    char file[100] = "teste01.txt";
    int n = recebe_dados(file, &p_cidade);

    float m[n][n];
    float min = monta_matriz(p_cidade, n, n, m);
//Ok

    branch_and_bound(n,n,m,min);
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
        aloca(p_dados, 100);

        (*p_dados + cont)->cod = cod;
        (*p_dados + cont)->cord_x = x;
        (*p_dados + cont++)->cord_y = y;
    }

    return cont;
}

int   tamanho_arquivo(char *path, char *line, FILE *p)
{
    int tam = 0;

    if((p = fopen(path,"r")) == NULL)
        return 0;

    while( !feof(p) ) //For at� final do arq
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

float  monta_matriz(cidade *p, int lin, int col, float m[lin][col])
{
    int i = 0, j = 0;
    float min;

    for(i = 0; i < lin; i++)
    {
        for(j = 0; j < col; j++)
        {
            if(j==i) m[i][j] = -1;
            else m[i][j] = sqrt(pow( ((p + i)->cord_x - (p + j)->cord_x), 2) + pow(((p + i)->cord_y - (p + j)->cord_y), 2));
        }
    }
 
    exibe_matriz(lin,col,m);

    min = minimiza_matriz(lin, col, m);
    printf("Primeira solucao factivel %.2f\n\n",min);
    return min;
}

float minimiza_matriz(int lin, int col, float m[lin][col])
{
    int i = 0, j = 0;
    float min = 0, sum_min = 0.0;

    for( i = 0; i < lin; i++) {
        sum_min += (min = menor_valor_linha(i, col, m));
        for( j = 0; j < col; j++)
            if(i  != j && m[i][j] >= 0 ) m[i][j] -= min;
    }

    printf("\n");
    for( j = 0; j < col; j++){
        sum_min += (min = menor_valor_coluna(lin, col, j, m));
        for( i = 0; i < lin; i++)
            if(i  != j && m[i][j] >= 0) m[i][j] -= min;
    }

    return sum_min; //Retorna 1� solu��o
}

float menor_valor_linha(int lin, int col, float m[lin][col])
{
    int j = 0, flag = 0;
    float min = 0;

    for(j = 0; j < col; j++) {
        if((min > m[lin][j] && lin != j && m[lin][j] >= 0) || (flag == 0 && lin != j && m[lin][j] >= 0)) {
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
        if((min > m[i][aux] && i != aux && m[i][aux] >= 0) || (flag == 0 && aux != i && m[i][aux] >= 0)) {
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

void  branch_and_bound(int lin, int col, float m[lin][col], float valor_minimo)
{
    float m_aux[lin][col], min_parcial = 0.0, resultado_parcial = 0.0;
    int i = 0, inicio_de_partida = 0;

    copia_matriz(lin,col, m, m_aux);
    exibe_matriz(lin,col,m);

    min_parcial = traca_caminho(inicio_de_partida, 1, lin, col, m_aux);
    resultado_parcial = m[inicio_de_partida][1] + valor_minimo + min_parcial;


    exibe_matriz(lin,col,m_aux);
    printf("%f + %f + %f = %f\n\n", m[inicio_de_partida][1] , valor_minimo , min_parcial, resultado_parcial );
}

void  copia_matriz(int lin, int col, float m[lin][col], float m_aux[lin][col])
{
    int i = 0, j = 0;

    for(i = 0; i < lin; i++)
        for(j = 0; j < col; j++)
            m_aux[i][j] = m[i][j];
}

float traca_caminho(int inicio_de_partida, int prox_parada, int lin, int col, float m[lin][col])
{
    zera_coluna_linha_e_encontro(inicio_de_partida, prox_parada,lin,col,m);
    //NOK
    return  minimiza_matriz(lin,  col, m);
}

void  zera_coluna_linha_e_encontro(int inicio_de_partida, int prox_parada,int lin, int col, float m[lin][col])
{
    int i = 0, j = 0;
    for(i = 0; i < lin; i++)
    {
        for(j = 0; j < col; j++)
        {
            if(i == inicio_de_partida) m[i][j] = -1;
            if(j == prox_parada) m[i][j] = -1;
            if(inicio_de_partida == j && prox_parada == i) m[i][j] = -1;
        }
    }
}