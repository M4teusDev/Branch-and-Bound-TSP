#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct city
{
    float cod;
    float cord_x;
    float cord_y;
}CITY;

int *recebe_matriz(char *path, CITY *p_city);
int  tamanho_arquivo(char *path, char *line, FILE *p);

int main ( void )
{
    CITY *p_city;

    char file[100] = "teste01.txt";
    recebe_matriz(file, p_city);
}


int *recebe_matriz(char *path, CITY *p_city)
{
    char line[1000];
    FILE *p = NULL;
    float x,y,z; //Variaveis auxiliares

    int tam = tamanho_arquivo(path, line, p) - 1, i = 0, head = 0;

    if( (p = fopen(path,"r")) == NULL)  return 0; 
    head = tamanho_cabecalho(path, line, p);

    for(i = 0; i < (tam - head) ; i++)
    [
        fscanf(p,"%f %f %f",&x, &y, &z);
    ]
        
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

int tamanho_cabecalho(char *path, char *line, FILE *p)
{
    int k = 0;

    do{
        fgets(line, 1000, p); 
        k++;
        if(strstr(line, "NODE_COORD_SECTION") != NULL) break;
    }while( !feof(p) );

    return k;
}