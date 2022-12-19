#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/** Constantes para as strings a serem lidas */
#define MAX_LINE 100
#define MAX_ACTION 10
#define MAX_ID_SIZE 10

typedef struct CARTA
//  struct para separar as cartas em seu valor (carta) e seu naipe (naipe)
{
  char carta[3];
  char naipe[4];
}CARTA;

char naipe_frequente[4];  // variavel global para representar naipe mais frequente da mao
int mao_quant = 7;  //  tamanho de mao (7 sendo o inicial) que será alterado durante partida

void debug(char *message) {
//  funcao para debug no terminal
  fprintf(stderr, "%s\n", message);
}
CARTA trata_carta(char *carta_nao_tratada){
//  recebe uma string (carta_nao_tratada) e retorna uma carta tratada no formato CARTA
  CARTA carta;
  carta.carta[0] = carta_nao_tratada[0];
  if (carta_nao_tratada[1] == '0'){ //  verifica se é um "10" para tratamento especial
    carta.carta[1] = carta_nao_tratada[1];
    carta.carta[2] = '\0';
    carta.naipe[0] = carta_nao_tratada[2];
    carta.naipe[1] = carta_nao_tratada[3];
    carta.naipe[2] = carta_nao_tratada[4];
    carta.naipe[3] = '\0';
  }else{
    carta.carta[1] = '\0';
    carta.naipe[0] = carta_nao_tratada[1];
    carta.naipe[1] = carta_nao_tratada[2];
    carta.naipe[2] = carta_nao_tratada[3];
    carta.naipe[3] = '\0';
  }
  return carta;
}

char *naipe_mais_frequente(CARTA *mao){
//  recebe uma array de CARTA (mao) e retorna o naipe que mais aparece nela em formato de string
  typedef struct{
    int quant;
    char naipe[4];
  }NAIPES;

  NAIPES mais_frequente;
  mais_frequente.quant = 0;
  NAIPES naipes[4];
  for (int i = 0; i < 4; i++){
    naipes[i].quant = 0;
  }
  //  assinala a cada elemento de naipes um dos 4 naipes
  sprintf(naipes[0].naipe, "♥");
  sprintf(naipes[1].naipe, "♦");
  sprintf(naipes[2].naipe, "♣");
  sprintf(naipes[3].naipe, "♠");

  for (int i = 0; i < mao_quant; i++){      //  conta quantas cartas de cada naipe tem
    for (int j = 0; j < 4; j++){
      if ((strcmp(mao[i].naipe, naipes[j].naipe) == 0 &&
          mao[i].carta[0] != 'C' && mao[i].carta[0] != 'A')){
        naipes[j].quant++;
      }
    }
  }
  for (int i = 0; i < 4; i++){      //  vê qual naipe é mais frequente
    if (naipes[i].quant > mais_frequente.quant){
      mais_frequente.quant = naipes[i].quant;
      strcpy(mais_frequente.naipe, naipes[i].naipe);
    }
  }
  strcpy(naipe_frequente, mais_frequente.naipe);    //   copia a string do naipe mais frequente para retornar me uma variavel global
  return naipe_frequente;
}

void add_carta(char *carta, CARTA *mao){    
//  recebe uma string de carta (carta) e a adiciona numa array (mao) no formato CARTA
  CARTA carta_tratada = trata_carta(carta); 
  strcpy(mao[mao_quant].carta, carta_tratada.carta);
  strcpy(mao[mao_quant].naipe, carta_tratada.naipe);
  mao_quant++;
}

void joga_carta(CARTA carta, CARTA *mao){
//  recebe uma carta no formato CARTA (carta) e a discarta no formato 
//  "DISCARD <carta> [naipe_mais_frequente]" e a remove de uma array de CARTA (mao)
  for (int i = 0; i < mao_quant; i++){
    if (strcmp(mao[i].carta, carta.carta) == 0 && strcmp(mao[i].naipe, carta.naipe) == 0){
      mao_quant--;
      for (int j = i; j < mao_quant; j++){
        strcpy(mao[j].carta, mao[j+1].carta);
        strcpy(mao[j].naipe, mao[j+1].naipe);
      }
      break;
    }
  }
  if (carta.carta[0] == 'C' || carta.carta[0] == 'A'){
    printf("DISCARD %s%s %s\n", carta.carta, carta.naipe, naipe_mais_frequente(mao));
  }else{
    printf("DISCARD %s%s\n", carta.carta, carta.naipe);
  }
}

void printa_cartas(char *titulo, CARTA *cartas){
//  printa uma array de CARTA (cartas) com um título (titulo)
  char *debugstring = malloc(200 * sizeof(char));
  sprintf(debugstring, "%s", titulo);
  for (int i = 0; i < mao_quant; i++){
    sprintf(debugstring + strlen(debugstring),"%s%s ", cartas[i].carta, cartas[i].naipe);
  }
  debug(debugstring);
  free(debugstring);
}

CARTA att_mesa(CARTA *mao, int i){
//  recebe uma array de CARTA (mao) e indice (i) do elemento a ser retornado de forma
//  que atualize a mesa
  CARTA mesa;
  strcpy(mesa.carta, mao[i].carta);
  strcpy(mesa.naipe, mao[i].naipe);
  if (mao[i].carta[0] == 'A' || mao[i].carta[0] == 'C'){
    strcpy(mesa.naipe, naipe_mais_frequente(mao));
  }
  return mesa;
}

int main() {
  char temp[MAX_LINE];   // string para leitura temporária de dados
  char my_id[MAX_ID_SIZE];  // identificador do seu bot

  setbuf(stdin, NULL);   // stdin, stdout e stderr não terão buffers
  setbuf(stdout, NULL);  // assim, nada é "guardado temporariamente"
  setbuf(stderr, NULL);

  // === INÍCIO DA PARTIDA ===
  scanf("PLAYERS %[^\n]\n", temp);

  scanf("YOU %s\n", my_id);  // Lê o identificador do próprio bot.

  scanf("HAND %[^\n]\n", temp);
  char *mao_inicial = malloc(100 * sizeof(char)); // variavel que salva a mao inicial
  strcpy(mao_inicial, temp);
  
  CARTA *mao = malloc(100 * sizeof(CARTA)); //  array que manterá atualizada a mao do meu bot
  CARTA mesa; //  variavel para guardar a carta da mesa
  
  int count_mao = 0;

  mao_inicial[strlen(mao_inicial) - 1] = '\0';  //  trata os colchetes da mao inicial
  mao_inicial[0] = ' ';

  char* pedaco;                                 // trata a mao inicial
  pedaco = strtok(mao_inicial, " ");
  while(pedaco != NULL){
    mao[count_mao] = trata_carta(pedaco);
    count_mao++;
    pedaco = strtok(NULL, " ");
  }

  char *debugstring = malloc(200 * sizeof(char));
  printa_cartas("Mão inicial: ", mao); // printa no terminal a mao inicial
  debug("\n");

  // Lê a carta aberta sobre a mesa. Ex: TABLE 8♣
  scanf("TABLE %s\n", temp);
  mesa = trata_carta(temp);

  // === PARTIDA ===
  char id[MAX_ID_SIZE];
  char action[MAX_ACTION];
  char previous_action[MAX_ACTION]; //  variavel para gravar a ultima acao de jogadores
  char complement[MAX_LINE];
  char previous_complement2[MAX_LINE];  //  variavel para gravar o segundo complemento
  char carta_comprada[MAX_LINE];  //  variavel para ler as cartas compradas
  int playing_condition = 0;  //  variavel para checar a condicao de jogo, 0 para nao jogou, 1 para jogou

  while(1) {
    do {
      // espera vez do bot
      scanf("%s %s", action, complement);
      if (strcmp(action, "DISCARD") == 0){ //  salva a ultima carta jogada
        mesa = trata_carta(complement);
        if (mesa.carta[0] == 'A' || mesa.carta[0] == 'C'){  // verifica por mudanca de naipe e grava quando preciso
          scanf(" %s", previous_complement2);   
          strcpy(mesa.naipe, previous_complement2);
        }    
      }

      if (strcmp(action, "TURN") != 0){ //  salva a ultima ação feita
        strcpy(previous_action, action);
      }
    } while (strcmp(action, "TURN") || strcmp(complement, my_id));
    
    // agora é a vez do meu bot jogar
    debug("----- MINHA VEZ -----");

    sprintf(debugstring,"Mesa: %s%s", mesa.carta, mesa.naipe);  //  printa a mesa no terminal
    debug(debugstring);

    printa_cartas("Mão: ", mao);

    playing_condition = 0;  //  livre para jogar

    if (playing_condition == 0){
      //  compra 4 quando recebe o coringa
      if(mesa.carta[0] == 'C' && strcmp(previous_action, "DISCARD") == 0){ 
        printf("SAY AGUARDE O PROCESSO!\n");
        printf("BUY 4\n");

        sprintf(debugstring, "Comprei: ");
        for(int i = 0; i < 4; i++){
          scanf("%s\n", carta_comprada);
          add_carta(carta_comprada, mao);
          sprintf(debugstring + strlen(debugstring),"%s ", carta_comprada);
        }
        debug(debugstring);

        printa_cartas("Mão: ", mao);
        playing_condition = 1;
      }
    }
    if (playing_condition == 0){
      //  compra 2 quando recebe o coringa
      if(mesa.carta[0] == 'V' && strcmp(previous_action, "DISCARD") == 0){
        printf("SAY Pra que isso rapaz ):\n");
        printf("BUY 2\n");

        sprintf(debugstring, "Comprei: ");
        for(int i = 0; i < 2; i++){
          scanf("%s\n", carta_comprada);
          add_carta(carta_comprada, mao);
          sprintf(debugstring + strlen(debugstring),"%s ", carta_comprada);
        }
        debug(debugstring);

        printa_cartas("Mão: ", mao);
        playing_condition = 1;
      }
    }
    if (playing_condition == 0){
      //  jogar prioritariamente cartas de pular a vez de outro jogador
      for (int i = 0; i < mao_quant; i++){
        if (mao[i].carta[0] == 'R' && strcmp(mesa.naipe, mao[i].naipe) == 0){
          mesa = att_mesa(mao, i);
          joga_carta(mao[i], mao);
          playing_condition = 1;
          break;
        }
      }
    }

    if (playing_condition == 0){
      //  em segundo grau de prioridade jogar carta de trocar sentido do jogo
      for (int i = 0; i < mao_quant; i++){
        if (mao[i].carta[0] == 'D' && strcmp(mesa.naipe, mao[i].naipe) == 0){
          mesa = att_mesa(mao, i);
          joga_carta(mao[i], mao);
          playing_condition = 1;
          break;
        }
      }
    }    

    if (playing_condition == 0){
      //  em terceiro grau de prioridade jogar carta que faz o proximo jogador comprar +2
      for (int i = 0; i < mao_quant; i++){
        if (mao[i].carta[0] == 'V' && strcmp(mesa.naipe, mao[i].naipe) == 0){
          mesa = att_mesa(mao, i);
          joga_carta(mao[i], mao);
          playing_condition = 1;
          break;
        }
      }
    }
    
    if (playing_condition == 0){
      //  em quarto grau de prioridade jogar carta de mesmo naipe
      for (int i = 0; i < mao_quant; i++){
        if (strcmp(mao[i].naipe, mesa.naipe) == 0 && mao[i].carta[0] != 'A' && mao[i].carta[0] != 'C'){
          mesa = att_mesa(mao, i);
          joga_carta(mao[i], mao);
          playing_condition = 1;
          break;
        }
      }
    }
    if (playing_condition == 0){
      //  em quinto grau de prioridade jogar carta de mesmo valor
      for (int i = 0; i < mao_quant; i++){
        if (mao[i].carta[0] == mesa.carta[0] && mao[i].carta[0] != 'A' && mao[i].carta[0] != 'C'){
          mesa = att_mesa(mao, i);
          joga_carta(mao[i], mao);
          playing_condition = 1;
          break;
        }
      }
    }
    if (playing_condition == 0){
      //  em sexto grau de prioridade jogar carta coringa que faz o proximo jogador comprar +4
      for (int i = 0; i < mao_quant; i++){
        if (mao[i].carta[0] == 'C'){
          mesa = att_mesa(mao, i);
          joga_carta(mao[i], mao);
          playing_condition = 1;
          break;
        }
      }
    }
    if (playing_condition == 0){
      //  em ultimo grau de prioridade jogar carta coringa
      for (int i = 0; i < mao_quant; i++){
        if (mao[i].carta[0] == 'A'){
          mesa = att_mesa(mao, i);
          joga_carta(mao[i], mao);
          playing_condition = 1;
          break;
        }
      }
    }
    if (playing_condition == 0){
      // caso não possua nenhuma carta jogável compra 1 carta
      printf("BUY 1\n");
      scanf("%s\n", carta_comprada);
      add_carta(carta_comprada, mao);

      sprintf(debugstring, "Comprei: ");
      sprintf(debugstring + strlen(debugstring),"%s ", carta_comprada);
      debug(debugstring);

      printa_cartas("Mão: ", mao);
      playing_condition = 1;
    }  
  }
  free(mao);
  free(debugstring);
  free(mao_inicial);
  return 0;
}
