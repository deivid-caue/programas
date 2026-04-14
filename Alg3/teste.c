#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "arvoreB.h"

int main ()
{
  struct arvoreB *arv;
  int32_t t, k, *idxEncontrado;

  printf("digite o t da arvoreB: ");
  scanf("%d", &t);
  printf("testando funcoes\n\ncriando arvore\n");
  
  arv = criarArvoreB(t);
  if(!arv)
    return 1;
  
  imprimirArvoreB(arv);
  imprimirEmOrdem(arv);
  printf("arvore criada!\n");

  do{
    
    printf("\no que fazer agora?\n\t1. inserir em arvoreB\n\t2. buscar em arvoreB\n\t3. deletar arvoreB e encerrar programa\nobs: ao escolher inserir ou buscar, a acao vai se repetir, -1 para sair\n");
    
    do{
      scanf("%d", &k);
    }while(k < 1 || k > 3);

    switch (k){
      
      case 1: 
        while(k != -1)
        {
          printf("\ninserir: ");
          scanf("%d", &k);
          inserirArvoreB(arv, k);
          imprimirArvoreB(arv);
          imprimirEmOrdem(arv);
          printf("%d inserido!\n", k);
        }
        break;
      
      case 2: 
        while(k != -1)
        {
          printf("\nbuscar: ");
          scanf("%d", &k);
          buscarArvoreB(arv, k, idxEncontrado);
          imprimirArvoreB(arv);
          imprimirEmOrdem(arv);
          printf("%d esta em %d!\n", k, *idxEncontrado);
        }
        break;
      
      default:
        break; 
    }
    
  }while(k == -1);

  destruirArvoreB(arv);
  printf("\nprograma encerrado!\n");
  
  return 0;
}
