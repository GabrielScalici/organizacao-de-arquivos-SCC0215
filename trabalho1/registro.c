/*
 *  TRABALHO DE ORGANIZAÇÃO DE ARQUIVOS
 *
 *  NOMES:                      NUMERO USP
 *  GABRIEL SCALICI             9292970
 *  RODRIGO NOVENTA JR          9791243
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "registro.h"


/*
    DISPOSICAO DOS CAMPOS NO ARQUIVO
    PRESTADORA, DATA, CODIGO, NOME_ESCOLA, MUNICIPIO ,UF
    DISPOSICAO QUE DEVE SER EXIBIDA AO USUARIO
    CODIGO, DATA, UF, (TAMANHO_NOME_ESCOLA), ESCOLA, (TAMANHO MUNICIPIO), MUNICIPIO, (TAMANHO PRESTADORA), PRESTADORA
    REGISTRO REMOVIDO: USAR SEEK_CUR
    RECUPERAR TODOS OS DADOS SEEK_SET
*/


/*
 *  Criar arquivo (caso não exista) e inserir o RRN do ultimo
 *  registro removido (PILHA).
*/
FILE* cria_arquivo(char *arquivo){
    FILE *f;
    Registro *reg;

    f = fopen(arquivo, "rb");       //Abre o arquivo em modo de leitura binária
    verifica_arquivo(f);            //Verifica se o arquivo abriu corretamente

    printf("Arquivo carregado.\n");

    return f;
}

//Funcao auxiliar para verificar se o srquivo foi aberto corretamente
void verifica_arquivo(FILE *f){
    if(!f){
        printf("Falha no carregamento do arquivo.\n");
        exit(1);
    }
}

//funcao para ler todos os registros e alocar na RAM
Registro *recuperar_registros(FILE *f, int qtdRegs){
    int tam;
    int alternando = 0, qtd = 0, i=0;
    char c;
    Registro *reg;

    //Aloca a quantidade de memória necessária
    reg = (Registro *) calloc(qtdRegs, sizeof(Registro));

    while(1){
        char* carac= (char*)calloc(80, sizeof(char));
        do{
            c= fgetc(f);
            if(c != ';' && c != '\n' && c != EOF){
                carac[i] = c ;//string temporaria para pegar os registros
                i++;
            }
        } while (c != ';' && c != '\n' && c != EOF );
        carac[i] = '\0';

        if(i > 0){    //caso o campo nao esteja preenchido
            if(alternando == 0){//colocando no campos campos prestadora e tam_prestadora
                reg[qtd].tam_prestadora = i;
                reg[qtd].prestadora = (char*) malloc(sizeof(char)*(strlen(carac)));
                strncpy(reg[qtd].prestadora, carac,strlen(carac));
                //printf("\n\n%d\nPrestadora: %s\n",qtd, reg[qtd].prestadora);
                alternando=1;
            }
            else if(alternando == 1){//colocando no campo de dataAtiv
                reg[qtd].dataAtiv = (char*) malloc(sizeof(char)*10);
                strncpy(reg[qtd].dataAtiv, carac,10);
                // printf("Data:%s\n", reg[qtd].dataAtiv);
                alternando=2;
            }
            else if(alternando == 2){//colocando no campo de codINEP
                reg[qtd].codINEP = atoi(carac);
                // printf("codINEP:%d\n", reg[qtd].codINEP);
                alternando=3;
            }
            else if(alternando == 3){//colocando nos campos nomEscola e tam_nomEscola
                reg[qtd].tamEscola = i;
                reg[qtd].nomEscola = (char*) malloc(sizeof(char)*(strlen(carac)));
                strncpy(reg[qtd].nomEscola, carac, strlen(carac));
               // printf("tamEscola: %d\n", reg[qtd].tam_nomEscola);
                alternando=4;
            }
            else if(alternando == 4){//colocando nos campos municipio e tam_municipio
                reg[qtd].municipio= (char*) malloc(sizeof(char)*(strlen(carac)));
                strncpy(reg[qtd].municipio, carac,strlen(carac));
                reg[qtd].tam_municipio = strlen(carac);
               // printf("Municipio:%s\n", reg[qtd].municipio);
                alternando=5;
            }
            else if(alternando == 5){//colocando no campo de uf
                reg[qtd].uf = (char*) malloc(sizeof(char)*2);
                strcpy(reg[qtd].uf, carac);
               // printf("UF:%s\n", reg[qtd].uf);
                qtd++;
                alternando=0;
            }
        }
        else alternando++;  //se houver um campo sem informacao pula para o proximo campo

        //criar uma nova string temp, para pegar o valor do proximo campo
            free(carac);
            i = 0;
           // printf("-----------------------\n");

        if(feof(f)) break; //ou c == EOF (?)
    }

    fclose(f);  //Tira o arquivo da memória
    return reg;
}

//funcao pra tranferir os registros para o arquivo de saida ja com o cabecalho
void transfere_arquivo(Registro* reg, int qtdRegs){
    FILE* f;
    int tamAtual;
    char c = '0';
    f = fopen("teste.bin", "r+b");
    verifica_arquivo(f);

    //Cria um cabecalho auxiliar
    Cabecalho cab;
    cab.status = '1';          //consistente
    cab.topoPilha = -1;      //pilha vazia por enquanto

    //Escreve o cabecalho no arquivo
    fwrite(&cab.status, sizeof(char), 1, f);
    fwrite(&cab.topoPilha,sizeof(int),1,f);
    printf("%c \n",cab.status);
    printf("%d \n",cab.topoPilha);


    //Escreve todos os registros no arquivo na ordem correta
    for (int i = 0; i < qtdRegs; ++i){
        fwrite(&reg[i].codINEP,sizeof(int),1,f);
        fwrite(&reg[i].dataAtiv,(10*sizeof(char)),1,f);
        fwrite(&reg[i].uf,(2*sizeof(char)),1,f);
        fwrite(&reg[i].tamEscola,sizeof(int),1,f);
        fwrite(&reg[i].nomEscola,reg[i].tamEscola,1,f);
        fwrite(&reg[i].tam_municipio,sizeof(int),1,f);
        fwrite(&reg[i].municipio,reg[i].tam_municipio,1,f);
        fwrite(&reg[i].tam_prestadora,sizeof(int),1,f);
        fwrite(&reg[i].prestadora,reg[i].tam_prestadora,1,f);

       /*Testando no terminal as variaves
        printf("%d ", reg[i].codINEP);
        printf("%s ", reg[i].dataAtiv);
        printf("%s ", reg[i].uf);
        printf("%d-", reg[i].tamEscola);
        printf("%s ", reg[i].nomEscola);
        printf("%d-", reg[i].tam_municipio);
        printf("%s ", reg[i].municipio);
        printf("%d-", reg[i].tam_prestadora);
        printf("%s \n", reg[i].prestadora);

       */
        tamAtual = (((28 + reg[i].tamEscola) + reg[i].tam_municipio) + reg[i].tam_prestadora);
        if(tamAtual < TAM_REG){
        	for(int i =0; i< (TAM_REG - tamAtual);i++)fwrite(&c,sizeof(char),1,f);
        }
       // fwrite(&reg[i],TAM_REG, 1, f);
    }
}

void busca_rrn(int RRN){
    //Abrindo o arquivo e verificando sua condição
    FILE *f = fopen("teste.txt", "rb");
    verifica_arquivo(f);

    Registro reg;
    char c;

    fseek(f, 5*sizeof(char), SEEK_SET);///Pula o topo da Pilha
    fseek(f, RRN * sizeof(Registro), SEEK_CUR);///Procura o registro pelo RRN


    //Lendo o primeiro caracter e analisando
    fread(&c, sizeof(char), 1, f);

    if((c == '*') || (feof(f))){
        ///Arquivo removido ou RRN não existe
        printf("\nRegistro inexistente.\n");
        fclose(f);
        return;
    }
    else{
        fseek(f, -sizeof(char), SEEK_CUR);///Retorna o ponteiro uma posicao pois foi checado o REMOVIDO
        fread(&reg, sizeof(reg), 1, f);

        //fscanf(f, "%d" , &reg.codINEP);
        printf("code = %d  ", reg.codINEP);
        printf("data = %s  ", reg.dataAtiv);
        printf("uf = %s  ", reg.uf);
        printf("tamesc = %d ", reg.tamEscola);
        printf("nomescola= %s  ", reg.nomEscola);
        printf("tam muni= %d ", reg.tam_municipio);
        printf("muni = %s  ", reg.municipio);
        printf("tamprest= %d ", reg.tam_prestadora);
        printf("prest= %s\n", reg.prestadora);

    }

    fclose(f);

}

void remover_registro_rrn(int RRN){

    FILE *f = fopen("teste.txt", "r+b");
    verifica_arquivo(f);
    //Variaveis auxiliares
    int aux_pilha;
    char c;

    //Pular o status do cabecalho
    fseek(f, sizeof(char), SEEK_SET);

    //Le o RRN no topo da pilha.
    fread(&aux_pilha, sizeof(int), 1, f);
    //printf("%d ", &aux_pilha);

    //Vai ate o registro que se deseja remover.
    fseek(f, RRN * sizeof(Registro), SEEK_CUR);
    //Le o primeiro caracter para saber se ja foi removido.
    fread(&c, sizeof(char), 1, f);
    if((c == '*') || (feof(f)))///Caso tenha sido removido ou tenha chegado no fim do arquivo, para a funcao.
    {
        printf("Registro inexistente.\n");
        fclose(f);
        return;
    }else{
        //Caso ainda nao tenha sido removido
        fseek(f, -sizeof(char), SEEK_CUR); //Volta uma posição para a primeira do registro
        //Armazenando na variavel auxiliar o valor para indicar remoção logica
        c = '*';
        fwrite(&c, sizeof(char), 1, f);

        //Atualizando os valores da pilha com o removido
        fwrite(&aux_topo, sizeof(int), 1, f);

        //Atualizar o topo da pilha com o valor desse RRN
        fseek(f, 0, SEEK_SET);

        //Pular o status do cabecalho e atualiza o topo da pilha
        fseek(f, sizeof(char), SEEK_SET);
        fwrite(&RRN, sizeof(int), 1, f);

        
        //Indicando para o usuário que foi removido com sucesso
        printf("Registro removido com sucesso.\n");

        fclose(f);
    }

    fclose(f);

}
