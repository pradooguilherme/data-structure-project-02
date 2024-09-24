#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct Register
{
    char id_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float freq;

} Register;

typedef struct PrimaryKeyOnFile
{
    char id_aluno[4];
    char sigla_disc[4];
    int address_number;

} PrimaryKeyOnFile;

typedef struct PrimaryKeyOnSearch
{
    char id_aluno[4];
    char sigla_disc[4];

} PrimaryKeyOnSearch;

// Function: readInsertRegister - Read all the register that'll be inserted;

Register *readInsertRegister()
{
    FILE *file = fopen("insere.bin", "r+b");

    Register *registros = (Register *)malloc(100 * sizeof(Register));

    if (registros != NULL)
    {
        fread(registros, sizeof(struct Register), 100, file);
        fclose(file);

        return registros;
    }

    printf("Falha na alocação de memória para o registro de inserção\n");

    fclose(file);
    return NULL;
}

// Function: readSearchPrimaryKey - Read all the keys that'll be searched;

PrimaryKeyOnSearch *readSearchPrimaryKey()
{
    FILE *file = fopen("busca_p.bin", "r+b");

    PrimaryKeyOnSearch *keys = (PrimaryKeyOnSearch *)malloc(100 * sizeof(PrimaryKeyOnSearch));

    if (keys != NULL)
    {
        fread(keys, sizeof(struct PrimaryKeyOnSearch), 100, file);
        fclose(file);

        return keys;
    }

    printf("Falha na alocação de memória para o registro de inserção\n");

    fclose(file);
    return NULL;
}

// Function: readFilePrimaryKey - Read all the keys in memory

PrimaryKeyOnFile *readFilePrimaryKey(int *numKeys)
{
    FILE *file = fopen("primary_index.bin", "r+b");

    PrimaryKeyOnFile *keys = (PrimaryKeyOnFile *)malloc(100 * sizeof(PrimaryKeyOnFile));

    if (keys != NULL)
    {
        *numKeys = (keys, sizeof(struct PrimaryKeyOnFile), 100, file);
        fclose(file);

        return keys;
    }

    printf("Falha na alocação de memória para o registro de inserção\n");

    fclose(file);
    return NULL;
}

// Function: startsPrimaryIndexFile - Open or Create the primary_index_file;

FILE *startsPrimaryIndexFile()
{
    FILE *file = fopen("primary_index_file.bin", "r+b");

    if (file == NULL)
    {

        file = fopen("primary_index_file.bin", "w+b");

        if (file != NULL)
        {
            return file;
        }
        else
        {
            printf("Falha na abertura do arquivo log.\n");
            return NULL;
        }
    }

    return file;
}

// Function: startsLogFile - Open or Create the log_file;

FILE *startsLogFile()
{
    FILE *file = fopen("log_file.bin", "r+b");

    if (file == NULL)
    {

        file = fopen("log_file.bin", "w+b");

        if (file != NULL)
        {
            int index = -1;

            fwrite(&index, sizeof(int), 1, file);
            fwrite(&index, sizeof(int), 1, file);
            fwrite(&index, sizeof(int), 1, file);

            fseek(file, 0, SEEK_SET);

            return file;
        }
        else
        {
            printf("Falha na abertura do arquivo log.\n");
            return NULL;
        }
    }

    return file;
}

// Function: iniciaArquivo - Open or Create the dados_file

FILE *iniciaArquivo()
{

    FILE *file = fopen("dados_file.bin", "r+b");

    if (file == NULL)
    {

        file = fopen("dados.bin", "w+b");

        if (file != NULL)
        {

            bool operacao_flag = false;

            fwrite(&operacao_flag, sizeof(bool), 1, file);
            fseek(file, 0, SEEK_SET);

            return file;
        }
        else
        {
            printf("Falha na abertura do arquivo dados.\n");
            return NULL;
        }
    }
    return file;
}

// Function: calcula_tamanho - Returns the register size

int calcula_tamanho(Register *registro)
{

    int tam = 0;

    tam += strlen(registro->id_aluno);
    tam += strlen(registro->sigla_disc);
    tam += strlen(registro->nome_aluno);
    tam += strlen(registro->nome_disc);
    tam += sizeof(registro->media);
    tam += sizeof(registro->freq);
    tam = tam + 5;

    return tam;
}

/*
    Funtion InsertRegister

    Parameter: Vector of structs; Number of register that'll be inserted;
    Return: Nothing;

    Should:

    - Open both Log and Data file;
    - Check the flag on header;
    - If flag equals true, read the index of the last register inserted;
    - Write the true flag on data file header;

*/

void insertRegister(Register *registro, int numb)
{
    FILE *log_file = startsLogFile();
    FILE *dados_file = iniciaArquivo();

    int numbKeys;

    PrimaryKeyOnFile *keys = readFilePrimaryKey(&numbKeys);

    int i = 0, tam_reg = 0;
    char delimitador = '#';
    bool operation_flag, flag = true;

    fread(&operation_flag, sizeof(bool), 1, dados_file);

    if (operation_flag)
    {
        fread(&i, sizeof(int), 1, log_file);
        i++;
    }
    else
    {
        fseek(dados_file, 0, SEEK_SET);
        fwrite(&flag, sizeof(bool), 1, dados_file);
    }

    for (int j = i; j < i + numb; j++)
    {

        fseek(dados_file, 0, SEEK_END);
        int address_number = ftell(dados_file);

        strcpy(keys[numbKeys].id_aluno, registro[j].id_aluno);
        strcpy(keys[numbKeys].sigla_disc, registro[j].sigla_disc);
        keys[numbKeys].address_number = address_number;

        tam_reg = calcula_tamanho(&registro[j]);

        fwrite(&tam_reg, sizeof(int), 1, dados_file);
        fwrite(registro[j].id_aluno, sizeof(char), 3, dados_file);
        fwrite(&delimitador, sizeof(char), 1, dados_file);
        fwrite(registro[j].sigla_disc, sizeof(char), 3, dados_file);
        fwrite(&delimitador, sizeof(char), 1, dados_file);
        fwrite(registro[j].nome_aluno, strlen(registro[j].nome_aluno), 1, dados_file);
        fwrite(&delimitador, sizeof(char), 1, dados_file);
        fwrite(registro[j].nome_disc, strlen(registro[j].nome_disc), 1, dados_file);
        fwrite(&delimitador, sizeof(char), 1, dados_file);
        fwrite(&registro[j].media, sizeof(float), 1, dados_file);
        fwrite(&delimitador, sizeof(char), 1, dados_file);
        fwrite(&registro[j].freq, sizeof(float), 1, dados_file);

        fseek(log_file, 0, SEEK_SET);
        fwrite(&j, sizeof(int), 1, log_file);
    }

    fclose(log_file);
    fclose(dados_file);

    return;
}
