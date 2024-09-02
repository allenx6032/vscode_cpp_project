#include <stdio.h>
#include <stdlib.h>


void assign1(int n,int a[3][3])
{
    switch(n)
    {
        case 1: a[0][0]='X';
                break;
        case 2: a[0][1]='X';
                break;
        case 3: a[0][2]='X';
                 break;
        case 4: a[1][0]='X';
                 break;
        case 5: a[1][1]='X';
                 break;
        case 6: a[1][2]='X';
                 break;
        case 7: a[2][0]='X';
                 break;
        case 8: a[2][1]='X';
                 break;
        case 9: a[2][2]='X';
                 break;
    }
}
void assign2(int n,int a[3][3])
{
    switch(n)
    {
        case 1: a[0][0]='O';
                break;
        case 2: a[0][1]='O';
                break;
        case 3: a[0][2]='O';
                 break;
        case 4: a[1][0]='O';
                 break;
        case 5: a[1][1]='O';
                 break;
        case 6: a[1][2]='O';
                 break;
        case 7: a[2][0]='O';
                 break;
        case 8: a[2][1]='O';
                 break;
        case 9: a[2][2]='O';
                 break;
    }



}
void checkwinner(int a[3][3])
{int i,j;

    for(i=0;i<3;i++)
    {
        if(a[i][0]=='X'&&a[i][1]=='X'&&a[i][2]=='X')
        {
            printf("Player 1 is winner!");
            exit(0);
        }
    }
    for(j=0;j<3;j++)
    {
        if(a[0][j]=='X'&&a[1][j]=='X'&&a[2][j]=='X')
        {
            printf("Player 1 is winner!");
            exit(0);
        }
    }
     for(i=0;i<3;i++)
    {
        if(a[i][0]=='O'&&a[i][1]=='O'&&a[i][2]=='O')
        {
            printf("Player 2 is winner!");
            exit(0);
        }
    }
    for(j=0;j<3;j++)
    {
        if(a[0][j]=='O'&&a[1][j]=='O'&&a[2][j]=='O')
        {
            printf("Player 2 is winner!");
            exit(0);
        }
    }
    if(a[0][0]=='X'&&a[1][1]=='X'&&a[2][2]=='X')
    {
        printf("Player 1 is winner!");
        exit(0);
    }
    if(a[0][0]=='O'&&a[1][1]=='O'&&a[2][2]=='O')
    {
        printf("Player 2 is winner!");
        exit(0);
    }
    if(a[0][2]=='X'&&a[1][1]=='X'&&a[2][0]=='X')
    {
        printf("Player 1 is winner!");
        exit(0);
    }
    if(a[0][2]=='O'&&a[1][1]=='O'&&a[2][0]=='O')
    {
        printf("Player 2 is winner!");
        exit(0);
    }



}
void display(int a[3][3])
{int i,j;
  for(i=0;i<3;i++)
  {
      for(j=0;j<3;j++)
      {
          printf("%c|",a[i][j]);
      }
      printf("\n------\n");
  }
}
int main()
{
  char a[3][3];
  int i,j;
  int b1[3][3]={1,2,3,4,5,6,7,8,9};
  int a1,b,c,d,e,f,g,h;
  printf("\n                                         ********Tic-Tac Toe********                         \n");

  for(i=0;i<3;i++)
  {
      for(j=0;j<3;j++)
        printf("%d|",b1[i][j]);
      printf("\n------\n");
  }
  printf("Player 1=X\n");
  printf("Player 2=O\n");
  printf("Choose the position (Enter values from 1 to 9)\n   Alert!Do not repeat the values\n ");
  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
    a[i][j]='\0';

  printf("\n                                         Tic-Tac Toe                         \n");

  for(i=0;i<3;i++)
  {
      for(j=0;j<3;j++)
      {
          printf("%c|",a[i][j]);
      }
      printf("\n------\n");
  }
  printf("Player 1: ");
  scanf("%d",&a1);
  assign1(a1,a);
  display(a);
  printf("Player 2: ");
  scanf("%d",&b);
  if(a==b)
  {
      printf("Value already taken");
      exit(0);
  }
 assign2(b,a);
 display(a);
 printf("Player 1:");
 scanf("%d",&c);
 assign1(c,a);
 display(a);
 printf("Player 2: ");


 scanf("%d",&d);
 assign2(d,a);
 display(a);
 printf("Player 1: ");
 scanf("%d",&e);
 assign1(e,a);
 display(a);
 checkwinner(a);
 printf("Player 2: ");
 scanf("%d",&f);
 assign2(f,a);
 display(a);
 checkwinner(a);
 printf("Player 1: ");
 scanf("%d",&g);
 assign1(g,a);
 display(a);
 checkwinner(a);
 printf("Player 2: ");
 scanf("%d",&h);
 assign2(h,a);
 display(a);
 checkwinner(a);
 printf("Player 1: ");
 scanf("%d",&i);
 assign2(i,a);
 display(a);
 checkwinner(a);
 printf("Match drawn!");




  return 0;

}