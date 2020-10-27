#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
void  pos_branch_and_bound(int lin, int col, float m[lin][col], float valor_minimo, trajeto **p_trajeto, float* melhor_resultado);
float *branch_and_bound(int lin, int col, float m[lin][col], float valor_minimo, trajeto **p_trajeto);
void  copia_matriz(int lin, int col, float m[lin][col], float m_aux[lin][col]);
float traca_caminho(int inicio_de_partida, int prox_parada, int lin, int col, float m[lin][col]);
void  zera_coluna_linha_e_encontro(int inicio_de_partida, int prox_parada,int lin, int col, float m[lin][col]);
int   verifica_visita(bool *p, int tam);

void  inicializa_vetor_cidades_visitadas(bool *p, int tam);
void  copia_caminho_ja_caminhado(bool *p, bool *q, int tam);
void  aloca_trajeto(trajeto **p, int tam);

//Verifica soluÃ§Ã£o
bool verifica_solucao(int lin, int col, float m[lin][col], trajeto *p, float valor_minimo);

int main ()
{
    cidade  *p_cidade  = NULL;
    trajeto *p_trajeto = NULL, *pos_p_trajeto = NULL;
    int flag = 0, j = 0, i = 0;

    char file[100] = "teste01.txt";
    int n = recebe_dados(file, &p_cidade);

    float m[n][n];
    float min = monta_matriz(p_cidade, n, n, m);
    float *menores_resultados_linhas;

    if(flag == 0)
    {
        menores_resultados_linhas = branch_and_bound(n,n,m,min, &p_trajeto);
        flag++;
    }

    printf("AAQQQQQQQQQQQQQQQQQQQQQQUUUUUUUUUUUUUUUUUUUIiiiiiiiiiiiiiiiiiiiiiiiiiiii");
 

    float valor = (p_trajeto +  n - 1)->distancia; //Melhor resultado 

    printf("Trajeto: ");
    for(i = 0; i < n; i++)
        printf("%i\t", (p_trajeto + i)->cod + 1);

    printf("\nDistancoa: ");
    for(i = 0; i < n; i++)
        printf("%f\t", (p_trajeto + i)->distancia);
    
    printf("\nMenores valores da linha: ");
    for(i = 0; i < n; i++)
        printf("%f\t", *(menores_resultados_linhas + i));
        
    //Ok   

    printf("\n");    
    while (j != 3) //Potencia do resultado 
    {
    	pos_branch_and_bound(n,n,m,min, &pos_p_trajeto, menores_resultados_linhas);
    	
	    printf("\n\nTrajeto %i: ", (j + 1));
	    for(i = 0; i < n; i++)
	        printf("%i\t", (pos_p_trajeto + i)->cod + 1);
	    
	    printf("\nDistancoa %i: ", j + 1);
	    for(i = 0; i < n; i++)
	        printf("%f\t", (pos_p_trajeto + i)->distancia);
	    j++;
	    printf("\n\n\nJ = %i\n\n", j);
	}
	
	return 0;
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
        aloca(p_dados, 1000);

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

    while( !feof(p) ) //For atï¿½ final do arq
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

    for( j = 0; j < col; j++){
        sum_min += (min = menor_valor_coluna(lin, col, j, m));
        for( i = 0; i < lin; i++)
            if(i  != j && m[i][j] >= 0) m[i][j] -= min;
    }

    return sum_min; //Retorna 1ï¿½ soluï¿½ï¿½o
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

void pos_branch_and_bound(int lin, int col, float m[lin][col], float valor_minimo, trajeto **p_trajeto, float *melhor_resultado)
{
    float m_aux[lin][col], m_aux_aux[lin][col], min_parcial = 0.0, resultado_parcial = 0.0, min_parcial_aux = 0, x = 0, aux  = 0;
    int i = 0, j = 0, inicio_de_partida = 0, profundidade = lin, tamanho = lin, cidade = 0, pos = 0, flag = 0, flag2 = -1, pos_aux = 0, y = 0, cont  = 0;
    bool cidades_visitadas[lin], cidades_trajeto[lin];

    inicializa_vetor_cidades_visitadas(cidades_visitadas, lin);
    inicializa_vetor_cidades_visitadas(cidades_trajeto, lin);
	
    aloca_trajeto(p_trajeto, cont + 1);
    (*p_trajeto + cont)->cod = 0;
    (*p_trajeto + cont)->distancia = valor_minimo;
    cont++;

    cidades_visitadas[0] = true;   //Cidade que observamos para ir
    cidades_trajeto[0] = true; //Cidades que ja passamos

    copia_matriz(lin,col, m, m_aux); //Guarda matriz inicial

    for(i = 0; i < profundidade; i++) 
    {
        for(j = 1; j < tamanho - i; j++) 
        {
            if((cidade = verifica_visita(cidades_visitadas, lin)) != -1)
            {
                copia_matriz(lin,col, m_aux, m_aux_aux);
                min_parcial = traca_caminho( inicio_de_partida, cidade, lin, col, m_aux_aux);
                x  = m[inicio_de_partida][cidade] + valor_minimo + min_parcial;
 
                if(flag == 0)
                { 
                    printf("\n\t1.  --- %f < %f|| %f > %f\n",x,resultado_parcial,x,*(melhor_resultado + i - 1));  //Primeira interação da profundidade da arvores
                    resultado_parcial = m[inicio_de_partida][cidade] + valor_minimo + min_parcial;
                    pos_aux = cidade;
                    flag++;
                }
                else if ( x < resultado_parcial && *(melhor_resultado + i ) < x && flag2 == -1 ||  x < resultado_parcial && *(melhor_resultado + i ) < x && x < aux )
                {
                    printf("\n\t2.  --- %f < %f|| %f > %f\t\t\t Valor de i: %i\n",x,resultado_parcial,x,*(melhor_resultado + i ), i ); 
                	resultado_parcial = m[inicio_de_partida][cidade] + valor_minimo + min_parcial;
                    aux = x;
                    pos_aux = cidade;
                    flag2++;
				}
                else if( x < resultado_parcial && flag != 0)
                {
                    printf("\n\t3.  --- %f < %f|| %f > %f\n",x,resultado_parcial,x,*(melhor_resultado + i )); 
                    resultado_parcial = m[inicio_de_partida][cidade] + valor_minimo + min_parcial;
                    pos_aux = cidade;
                }
            }
        }
        
        

        //printf("\t\t\nAs is: %f",*(melhor_resultado + i));
	  	*(melhor_resultado + i) = resultado_parcial;
        //printf("\t\t\nTo be: %f\n", *(melhor_resultado + i));
	  	
        min_parcial = traca_caminho(inicio_de_partida,pos_aux, lin, col, m_aux);
        inicio_de_partida = pos_aux;
        valor_minimo = resultado_parcial;
        cidades_trajeto[inicio_de_partida] = true;

        
        aloca_trajeto(p_trajeto, cont + 1);
        (*p_trajeto + cont)->cod = inicio_de_partida;
        (*p_trajeto + cont)->distancia = resultado_parcial;
        cont++;

        copia_caminho_ja_caminhado(cidades_trajeto, cidades_visitadas, lin);
        flag = 0;
    }
}

float *branch_and_bound(int lin, int col, float m[lin][col], float valor_minimo, trajeto **p_trajeto)
{
    float m_aux[lin][col], m_aux_aux[lin][col], min_parcial = 0.0, resultado_parcial = 0.0;
    int i = 0, j = 0, inicio_de_partida = 0, profundidade = lin, tamanho = lin, cidade = 0, pos = 0, flag = 0, pos_aux = 0, y = 0, cont  = 0;
    bool cidades_visitadas[lin], cidades_trajeto[lin];
    float *menores_resultados_linhas = malloc (sizeof (float) * lin);

    //printf("\n\nendereco da puta q pariu: %i\n\n", menores_resultados_linhas);

    inicializa_vetor_cidades_visitadas(cidades_visitadas, lin);
    inicializa_vetor_cidades_visitadas(cidades_trajeto, lin);

    aloca_trajeto(p_trajeto, cont + 1);
    (*p_trajeto + cont)->cod = 0;
    (*p_trajeto + cont)->distancia = valor_minimo;
    cont++;

    cidades_visitadas[0] = true;   //Cidade que observamos para ir
    cidades_trajeto[0] = true; //Cidades que ja passamos

    copia_matriz(lin,col, m, m_aux); //Guarda matriz inicial

    for(i = 0; i < profundidade; i++) //Profundidade da arvore
    {
        for(j = 1; j < tamanho - i; j++) //Tamanho da arvore
        {
            if((cidade = verifica_visita(cidades_visitadas, lin)) != -1)
            {
                copia_matriz(lin,col, m_aux, m_aux_aux);
                min_parcial = traca_caminho(inicio_de_partida,cidade, lin, col, m_aux_aux);

                if(flag == 0)
                {
                    resultado_parcial = m[inicio_de_partida][cidade] + valor_minimo + min_parcial;
                    pos_aux = cidade;
                    flag++;
                }
                else if( (min_parcial + valor_minimo +  m[inicio_de_partida][cidade]) < resultado_parcial)
                {
                    resultado_parcial = m[inicio_de_partida][cidade] + valor_minimo + min_parcial;
                    pos_aux = cidade;
                }
            }
        }

        *(menores_resultados_linhas + i) = resultado_parcial;

        min_parcial = traca_caminho(inicio_de_partida,pos_aux, lin, col, m_aux);
        inicio_de_partida = pos_aux;
        valor_minimo = resultado_parcial;
        cidades_trajeto[inicio_de_partida] = true;

        aloca_trajeto(p_trajeto, cont + 1);
        (*p_trajeto + cont)->cod = inicio_de_partida;
        (*p_trajeto + cont)->distancia = resultado_parcial;
        cont++;

        copia_caminho_ja_caminhado(cidades_trajeto, cidades_visitadas, lin);
        flag = 0;
    }

    return menores_resultados_linhas;
}

void  inicializa_vetor_cidades_visitadas(bool *p, int tam)
{
    int i = 0;
    for( i = 0; i < tam; i++)
        *(p + i) = false;
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

int   verifica_visita(bool *p, int tam)
{
    int i = 0;
    for( i = 0; i < tam; i++)
    {
        if(*(p + i) == false) //Busca cidade ainda nÃ£o visita
        {
            *(p + i) = true; //cidade visita;
            return i;
        }
    }

    return -1;
}

void  copia_caminho_ja_caminhado(bool *p, bool *q, int tam)
{
    int i = 0;
    for(i = 0; i < tam; i++)
        *(q + i) = *(p + i);
}

void  aloca_trajeto(trajeto **p, int tam){
  if( (*p = (trajeto *)realloc( *p ,tam * sizeof(trajeto))) == NULL) {
        printf("Estamos sem memoria...");
        exit(1);
    }
}

bool verifica_solucao(int lin, int col, float m[lin][col], trajeto *p, float valor_minimo)
{
    float melhor_solucao_atual = (p + lin - 1)->distancia, m_aux[lin][col], m_aux_aux[lin][col], min_parcial;
    int cidade_incial  = (p + 1)->cod,  i = 0;

    copia_matriz(lin,col,m,m_aux);

    for(i = 1; i < lin ;i++ )
    {
        if(i != cidade_incial)
        {
            copia_matriz(lin,col, m_aux, m_aux_aux);
            min_parcial = traca_caminho(0,i, lin, col, m_aux_aux);

            if((min_parcial + valor_minimo +  m[0][i]) < melhor_solucao_atual)
                return true;
        }
    }

    return false;
}