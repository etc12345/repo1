#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define BUFFER_SIZE 1024

const char *interFile = "SIC_Intermed.txt";
const char *opTab     = "SIC_Optab.txt";
const char *symFile   = "SIC_Symbol.txt";
const char *objFile   = "SIC_Object.txt";

struct SIC
{
    char PC[6];
    char label[10];
    char opcode[10];
    char operand[10];
    char objcode[10];
}store[100];

struct opTab
{
    char opcode[10];
    char code[10];
}opStore[100];

struct symTab
{
    char label[10];
    char PC[10];
}symStore[100];

char opSTR[20];
int line=0;

void store_symTab()
{
    char *last_token;
    FILE *fileptr1 = fopen(symFile,"r");
    const char *delimiter_characters = " \t\0";
    char buffer[BUFFER_SIZE];
    int index = -1,col=0,i=0;

    while(fgets(buffer, BUFFER_SIZE, fileptr1) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = 0;
        index++;

        last_token = strtok(buffer, delimiter_characters);
       
        while (last_token != NULL)
        {
            if(col==0) strcpy(symStore[index].label,last_token);
            else       strcpy(symStore[index].PC,last_token);  
            last_token = strtok(NULL, delimiter_characters);
            col++;
        }
        col=0;
    }
    strcpy(symStore[index+1].label,"END");
    printf("\nSYMBOL TABLE : \n\n");
    while(strcmp(symStore[i].label,"END"))
    {
        printf("%s\t%s\n",symStore[i].label,symStore[i].PC);
        i++;
    }
}

void store_opTab()
{
    char *last_token;
    FILE *fileptr1 = fopen(opTab,"r");
    const char *delimiter_characters = " \t\0";
    char buffer[BUFFER_SIZE];
    int index = -1,col=0,i=0;

    while(fgets(buffer, BUFFER_SIZE, fileptr1) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = 0;
        index++;

        last_token = strtok(buffer, delimiter_characters);
       
        while (last_token != NULL)
        {
            if(col==0) strcpy(opStore[index].opcode,last_token);
            else       strcpy(opStore[index].code,last_token);  
            last_token = strtok(NULL, delimiter_characters);
            col++;
        }
        col=0;
    }
    strcpy(opStore[index+1].opcode,"END");
    printf("\nOPTAB : \n\n");
    while(strcmp(opStore[i].opcode,"END"))
    {
        printf("%s\t%s\n",opStore[i].opcode,opStore[i].code);
        i++;
    }
    fclose(fileptr1);
}

void read_and_store()
{
    char *last_token;
    FILE *fileptr1 = fopen(interFile,"r");
    const char *delimiter_characters = " \t\0";
    char buffer[BUFFER_SIZE];
    int index = -1,col=0;

    while(fgets(buffer, BUFFER_SIZE, fileptr1) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = 0;
        index++;

        last_token = strtok(buffer, delimiter_characters);
       
        while (last_token != NULL)
        {
                 if(col==0) strcpy(store[index].PC,last_token);
            else if(col==1) strcpy(store[index].label,last_token);
            else if(col==2) strcpy(store[index].opcode,last_token);
            else            strcpy(store[index].operand,last_token);  
            last_token = strtok(NULL, delimiter_characters);
            col++;
        }
        if(col==3)
        {
            strcpy(store[index].operand,store[index].opcode);
            strcpy(store[index].opcode,store[index].label);
            store[index].label[0]='\0';
        }
        else if(col==2)
        {
            strcpy(store[index].opcode,store[index].label);
            store[index].label[0]='\0';
        }
        col=0;
    }
}

void returnCode(char to_search[])
{
    int i=0;
    while(strcmp(opStore[i-1].opcode,"END"))
    {
        if(!(strcmp(opStore[i].opcode,to_search)))
        {
            strcpy(opSTR,opStore[i].code);
            return;
        }
        i++;
    }
    strcpy(opSTR,"");
}

void store_opTabCode()
{
    int i=0;
    while(strcmp(store[i-1].opcode,"END"))
    {
        returnCode(store[i].opcode);
        strcpy(store[i].objcode,opSTR);
        i++;
    }
}

void returnAdd(char to_search[])
{
    int i=0;
    while(strcmp(symStore[i-1].label,"END"))
    {
        if(!(strcmp(symStore[i].label,to_search)))
        {
            strcpy(opSTR,symStore[i].PC);
            return;
        }
        i++;
    }
    strcpy(opSTR,"");
}

void storeBYTE(char byte_oprand[])
{
    strcpy(opSTR,"");
    char temp_opSTR[20];
    char temp2_opSTR[20];
    int temp = strlen(byte_oprand);
    temp=temp-3;
    int ASI;
    if(byte_oprand[0] == 'C')
    {
        strcpy(temp_opSTR,"");
        for(int i=2,j=0;i<2+temp;i++,j++)
        {
            ASI = byte_oprand[i];
            sprintf(temp2_opSTR,"%x",ASI);
            strcat(temp_opSTR,temp2_opSTR);
        }
    }
    else
    {
        for(int i=2,j=0;i<2+temp;i++,j++)
            temp_opSTR[j]=byte_oprand[i];

        temp_opSTR[temp]='\0';
    }
    strcpy(opSTR,temp_opSTR);
}

void storeWORD(char byte_oprand[])
{
    strcpy(opSTR,"");
    for(int i=0;i<6-strlen(byte_oprand);i++)
        strcat(opSTR,"0");
    strcat(opSTR,byte_oprand);
}

void concat_address()
{
    int i=0;
    char tempSTR[10];
    while(strcmp(store[i].opcode,"END"))
    {
        if( strlen(store[i].label) == 0 && strlen(store[i].operand) == 0)
            strcat(store[i].objcode,"0000");
        else if(store[i].operand[strlen(store[i].operand)-2] == ',')
        {
            strcpy(tempSTR,store[i].operand);
            tempSTR[strcspn(tempSTR, ",")] = 0;
            tempSTR[strcspn(tempSTR, "X")] = 0;
            returnAdd(tempSTR);
            strcat(store[i].objcode,opSTR);
        }
        else if(!strcmp(store[i].opcode,"BYTE"))
        {
            storeBYTE(store[i].operand);
            strcat(store[i].objcode,opSTR);
        }
        else if(!strcmp(store[i].opcode,"WORD"))
        {
            storeWORD(store[i].operand);
            strcat(store[i].objcode,opSTR);
        }
        else
        {
            returnAdd(store[i].operand);
            strcat(store[i].objcode,opSTR);
        }
        i++;
    }
}

void genObjectCode()
{
    store_opTab();
    store_symTab();
    store_opTabCode();
    concat_address();
}

void print_structure()
{
    int i=0;
    line = 0;
    printf("\n\n\nCurrent Structure : \n\n");
    while(strcmp(store[i-1].opcode,"END"))
    {
        if(i+1<=9)
            printf("0");
        printf("%d\t%s\t%s\t%s\t%s\t%s\n",i+1,store[i].PC,store[i].label,store[i].opcode,store[i].operand,store[i].objcode);
        i++;
        line++;
    }
    line--;
}

int prog_length(int a,int b)
{
    char te_init[8]="0x";
    char te_end[8]="0x";
    strcat(te_init,store[a].PC);
    strcat(te_end,store[b].PC);
    int init = (int)strtol(te_init,NULL,16);
    int end  = (int)strtol(te_end,NULL,16);

    return end-init;
}

void generate_header(int PL)
{
    FILE *fileptr2 = fopen(objFile,"w");
    fprintf(fileptr2,"H^");

    fprintf(fileptr2,"%s",store[0].label);
    for(int i=0;i<6-strlen(store[0].label);i++)
        fprintf(fileptr2,"_");
   
    int Pcount = (int)strtol(store[0].PC,NULL,16);
    fprintf(fileptr2,"^%.6X",Pcount);
   
    fprintf(fileptr2,"^%.6X\n",PL);

    fclose(fileptr2);
}

void generate_textRec()
{
    int flag=1,i=1,lim=0,leno,temz;
    char inter[61]="";
    FILE *fileptr2 = fopen(objFile,"a");
    fprintf(fileptr2,"T^");

    while(strcmp(store[i].opcode,"END"))
    {
        if(lim == 10)
        {
            leno = strlen(inter)/2;
            fprintf(fileptr2,"%X^%s\n",leno,inter);
            lim=0;
            fprintf(fileptr2,"T^");
        }
        else if( strlen(store[i].objcode)==0 )
        {
            leno = strlen(inter)/2;
            fprintf(fileptr2,"2000^%X^%s\n",leno,inter);
            lim=0;
            fprintf(fileptr2,"T^");
            for(int j=i;j<line;j++)
            {
                if(strlen(store[j].objcode)!=0)
                {
                    temz = j;
                    break;
                }
            }
            //printf("xx%dxx",temz);
            i = temz-1;
            strcpy(inter,"");
        }
        strcat(inter,store[i].objcode);
        //printf("%d-Current String : %s\n",i+1,inter);
        i++;

        if(i==line)
        {
            leno = strlen(inter)/2;
            fprintf(fileptr2,"2012^%.2X^%s\n",leno,inter);
        }
        lim++;
    }
    fclose(fileptr2);
}

void generate_end()
{
    FILE *fileptr2 = fopen(objFile,"a");
    fprintf(fileptr2,"E^");

    int Sadd = (int)strtol(store[0].PC,NULL,16);
    fprintf(fileptr2,"%.6X",Sadd);
    fclose(fileptr2);
}

int main()
{
    read_and_store();
    genObjectCode();
    print_structure();
    int PL = prog_length(0,line);
    generate_header(PL);
    generate_textRec();
    generate_end();
    return 0;
}