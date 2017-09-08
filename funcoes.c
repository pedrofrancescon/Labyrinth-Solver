/* Autores: Pedro Francescon Cittolin
            Viviane Lima Bonfim
            Gabriel Alves da Costa
*/

#include "trabalho3.h"
#include <stdlib.h>

/*
    Inverte as cores da imagem
*/
void negativa(Imagem1C* img, int largura, int altura)
{
    int i, j;
    for(i = 0; i < altura; i++)
    {
        for(j = 0; j < largura; j++)
        {
            img->dados[i][j] = 255 - img->dados[i][j];
        }
    }
}

/*
    Aplica o filtro Sobel (acha os contornos da imagem)
*/
void sobel(Imagem1C** img2, int largura, int altura)
{
    int i, j;
    Imagem1C* img = *img2;
    Imagem1C* nova = criaImagem1C(largura,altura);
    for(i = 1; i < altura-1; i ++)
    {
        for(j = 1; j < largura-1; j++)
        {
            int x =  img->dados[i-1][j-1];
            x+=  img->dados[i-1][j+1]*-1;
            x+=  img->dados[i][j-1]*2;
            x+= img->dados[i][j+1]*-2;
            x+= img->dados[i+1][j-1];
            x+= img->dados[i+1][j+1]*-1;

            int y =  img->dados[i-1][j-1];
            y+=  img->dados[i-1][j]*2;
            y+=  img->dados[i-1][j+1]*1;
            y+= img->dados[i+1][j-1]*-1;
            y+= img->dados[i+1][j]*-2;
            y+= img->dados[i+1][j+1]*-1;

            nova->dados[i][j] = sqrt(x*x+y*y);
        }
    }

    //arruma as bordas da imagem
    for(i = 1; i < largura-1; i++)
    {
        nova->dados[0][i] = nova->dados[1][i];
    }
    for(i = 1; i < largura-1; i++)
    {
        nova->dados[altura-1][i] = nova->dados[altura-2][i];
    }
    for(i = 1; i < altura-1; i++)
    {
        nova->dados[i][0] = nova->dados[i][1];
    }
    for(i = 1; i < altura-1; i++)
    {
        nova->dados[i][largura-1] = nova->dados[i][largura-2];
    }

    nova->dados[0][0] = 0;
    nova->dados[0] [largura-1] =0;
    nova->dados[altura-1][0] =0;
    nova->dados[altura-1][ largura-1] =0;
    *img2 = nova;

}

/*
    Borra a imagem para a garantir continuidade do caminho
*/
void media(Imagem1C* img, int largura, int altura)
{
    int i, j;
    for(i = 1; i < altura-1; i ++)
    {
        for(j = 1; j < largura-1; j++)
        {
            int media =  img->dados[i-1][j-1] + img->dados[i-1][j] + img->dados[i-1][j+1] +
                         img->dados[i][j]     + img->dados[i][j-1] + img->dados[i][j+1] +
                         img->dados[i+1][j-1] + img->dados[i+1][j] + img->dados[i+1][j+1];
            media/= 9;
            img->dados[i][j] = media;
        }
    }
}

/*
    Normaliza a imagem
*/
void normaliza(Imagem1C* img, int maior,int menor)
{
    int largura = img->largura;
    int altura = img->altura;
    int j, i;
    int conta = maior - menor;
    int pixel;
    double novo; // pixel após a normalização, que será posteriormente truncado para int

    for(i = 0; i< altura; i++)
    {
        for(j=0; j < largura; j++)
        {
            pixel = img->dados[i][j];
            if(pixel < menor)
            {
                pixel = menor;
            }
            if(pixel > maior)
            {
                pixel = maior;
            }
            novo = (255*(pixel-menor))/(conta); // formula de mudança de escala.
            img->dados[i][j] = novo;
        }
    }

}

/*
    Cria uma pilha de coordenadas
*/
Coordenada* criaPilha (int largura, int altura) //
{
    Coordenada* pilha;
    pilha= (Coordenada*) malloc(sizeof(Coordenada)*altura*largura);
    return(pilha);
}

/*
    Usa o "principio da enchente/inundação" para atribuir um valor para cada posição do vetor binarizado da imagem
*/
void preencheCusto (int** imgInt, int altura, int largura, Coordenada* pilha, int tamanho)
{
    int i, j=0, count=1;

    //procura possiveis inicios de caminho
    for (i=0; i<altura; i++)
    {
        if (imgInt[i][largura-1]==-1)
        {
            imgInt[i][largura-1]=count;
            pilha[j].x=largura-1;
            pilha[j].y=i;
            j++;
        }
    }
    int x,y,maiorCount=0;
    for (i=0; j; i++)

    {
        j--;
        x=pilha[j].x;
        y=pilha[j].y;
        count=imgInt[y][x]+1;

        if (y+1 < altura && (imgInt[y+1][x]==-1 || imgInt[y+1][x]>count))
        {
            imgInt[y+1][x]=count;
            pilha[j].x=x;
            pilha[j].y=y+1;
            j++;
        }
        if ( y-1 >= 0 && (imgInt[y-1][x]==-1 || imgInt[y-1][x]>count) )
        {
            imgInt[y-1][x]=count;
            pilha[j].x=x;
            pilha[j].y=y-1;
            j++;

        }
        if (x+1 < largura && (imgInt[y][x+1]==-1 || imgInt[y][x+1]>count))
        {
            imgInt[y][x+1]=count;
            pilha[j].x=x+1;
            pilha[j].y=y;
            j++;
        }
        if (x-1 >= 0 && (imgInt[y][x-1]==-1 || imgInt[y][x-1]>count))
        {
            imgInt[y][x-1]=count;
            pilha[j].x=x-1;
            pilha[j].y=y;
            j++;
        }
    }
}

/*
    Encontra a ultima posição do menor caminho e vai "voltando" até achar a primeira, e coloca isso em um vetor de coordenadas
*/
Coordenada* percorreMenorCaminho(int** imgInt, int largura, int altura, int* tCaminho)
{
    int i, j;
    int  t=largura*altura,count;

    //procura o valor do comeco do menor caminho com base nos valores preenchidos na matriz inteira
    for (i=0; i<altura; i++)
    {
        //procura o menor valor positivo
        if (imgInt[i][0]>0 && imgInt[i][0]<t)
        {
            t = imgInt[i][0];
        }
    }

    //aloca um vetor de coordenadas do caminho a ser retornado com base no valor encontrado
    Coordenada* caminho = malloc(sizeof(Coordenada) * (t+1));
    count = t;
    *tCaminho = count;

    //procura o inicio do caminho e preenche no vetor
    for (i=0; i<altura; i++)
    {
        if (imgInt[i][0]==t)
        {
            caminho[0].x=0;
            caminho[0].y=i;
            break;
        }
    }
    t--;
    int x = caminho[0].x;
    int y = caminho[0].y;

    // procura próxima coordenada do caminho com base no valor da ultima coordenada preenchida, olhando coordenadas ao redor
    for(i = 1; t; i++)
    {
        if (y+1 < altura && (imgInt[y+1][x]==t)) //olha a de baixo
        {
            caminho[i].x = x;
            caminho[i].y = y+1;
            t--;
        }
        else if (x-1 > 0 && (imgInt[y][x-1]==t)) //olha a da esquerda
        {
            caminho[i].x = x-1;
            caminho[i].y = y;
            t--;
        }
        else if (x+1 < largura && (imgInt[y][x+1]==t)) //olha a da direita
        {
            caminho[i].x = x+1;
            caminho[i].y = y;
            t--;
        }
        else if ( y-1 > 0 && (imgInt[y-1][x]==t)) //olha a de cima
        {
            caminho[i].x = x;
            caminho[i].y = y-1;
            t--;
        }
        x=caminho[i].x;
        y=caminho[i].y;
    }
    return caminho;
}

/*
    Zera um vetor de tamanho 256
*/
void zeraVetor(int* v)
{
    int i;
    for(i=0; i < 256; i++)
    {
        v[i] = 0;
    }
}
/*
    De acordo com uma porcentagem, acha o maior e o menor valor dos pixels da imagem
*/
void preNormaliza(Imagem1C* img,int largura,int altura, int* menor, int* maior)
{
    int i, j;
    double percent = 0.03;
    int total = largura*altura*percent;
    int histograma[256];
    zeraVetor(histograma);

    //preenche o histograma da imagem
    for(i = 0; i< altura; i++)
    {
        for(j =0; j < largura; j++)
        {
            histograma[img->dados[i][j]]++;
        }
    }
    int count=0;
    for(i =0; count< total ; i++)
    {
        count+= histograma[i];
    }
    count = 0;
    *menor = i;

    for(i =255; count < total ; i--)
    {
        count+= histograma[i];
    }
    *maior = i;
}

/*
    Aloca uma matriz com todas as posiçoes = -2
*/
int** criaMatrizZerada(int n, int m)
{
    int i, j;
    int** ret = malloc(sizeof(int*) * n);
    for(i = 0; i < n; i++)
    {
        ret[i] = malloc(sizeof(int)*m);
        for(j=0; j < m; j++)
        {
            ret[i][j] = -2;
        }
    }
    return ret;
}

/*
    Salva uma versao binarizada da imagem em uma matriz
*/
int** binarizar(Imagem1C* img, int largura, int altura, int range, int maior, int menor, int* count )
{

    int i, j, g, h;
    (*count)=0;
    int** m = criaMatrizZerada(altura,largura);
    for(i = 0; i < altura; i++)
    {
        for(j = 0; j < largura; j++)
        {
            // se o pixel estiver no intervalo considerado caminho
            if(img->dados[i][j] >= menor && img->dados[i][j] < menor+range)
            {
                m[i][j] = -1;
                count++;
            }
        }
    }
    return m;
}
int encontraCaminho (Imagem1C* img, Coordenada** caminho)
{
    int count = 0;
    int range;
    double percent = .8;
    int largura = img->largura, altura = img->altura;
    int menor, maior;
    int tam = img->largura * img->altura;

    // trabalha a imagem para ficar mais facil de binarizar
    salvaImagem1C(img, "sample (1).bmp");
    media(img,largura,altura);
    salvaImagem1C(img, "sample (2) - Borramento.bmp");
    sobel(&img,largura,altura);
    salvaImagem1C(img, "sample (3) - Filtro Sobel.bmp");
    media(img,largura,altura);
    salvaImagem1C(img, "sample (4) - Borramento apos Filtro Sobel.bmp");
    negativa(img,largura,altura);
    salvaImagem1C(img, "sample (5) - Inversao de Cores (negativo).bmp");
    preNormaliza(img,largura,altura,&menor,&maior);
    normaliza(img,maior, menor);
    salvaImagem1C(img, "sample (6) - Aumento de Contraste.bmp");

    // estabelece um intervalo do que é caminho ou não
    range = 255 * percent;

    int ** matriz = binarizar(img,largura,altura,range,255,0,&count);
    Coordenada* pilha  = criaPilha(largura,altura);                     // guarda as posiçoes do caminho que ainda nao foram usadas
    preencheCusto(matriz,altura,largura,pilha,count);                   // colocoa um valor em cada posiçao da matriz
    *caminho = percorreMenorCaminho(matriz,largura, altura, &count);

    return count;
}
