#include<stdio.h>

int main(){

    int i, j, temp, valores[30];

    for (i=0; i<30; i++){
        scanf("%d", &valores[i]);
    }

    for (i=0; i<30; i++){
        for(j=0; j<29; j++){
            if (valores[j] > valores[j+1]){
                temp = valores[j];
                valores[j] = valores[j+1];
                valores[j+1] = temp;
            }
        }
    }

    for (i=0; i<30; i++)
        printf("%d ", valores[i]);
    printf("\n\n");
    return;
}