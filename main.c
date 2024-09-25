#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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

FILE *iniciaArquivo()
{

    FILE *file = fopen("dados.bin", "r+b");

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

PrimaryKeyOnFile *readFilePrimaryKey()
{
    FILE *log_file = startsLogFile();

    int correctEnd, numKeys;
    fread(&correctEnd, sizeof(int), 1, log_file);

    PrimaryKeyOnFile *keys = (PrimaryKeyOnFile *)malloc(100 * sizeof(PrimaryKeyOnFile));

    if (keys == NULL)
    {
        printf("Erro na alocação (1)\n");
        fclose(log_file);
        return NULL;
    }

    if (correctEnd == 1)
    {
        FILE *file = startsPrimaryIndexFile();

        numKeys = fread(keys, sizeof(struct PrimaryKeyOnFile), 100, file);
        fclose(file);
    }
    else
    {

        FILE *data_file = iniciaArquivo();
        fseek(data_file, sizeof(bool), SEEK_SET);

        int size, address, i = 0;
        address = ftell(data_file);

        while (fread(&size, sizeof(int), 1, data_file) == 1)
        {

            fread(&keys[i], 6, 1, data_file);
            keys[i].address_number = address;

            int prox = size - 6;
            fseek(data_file, prox, SEEK_CUR);

            address = ftell(data_file);
            i++;
        }

        fclose(data_file);
        numKeys = i;
    }

    fseek(log_file, 8, SEEK_SET);
    fwrite(&numKeys, sizeof(int), 1, log_file);

    fclose(log_file);
    return keys;
}

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

int compareKeys(PrimaryKeyOnFile *a, PrimaryKeyOnFile *b)
{
    // Compara os id_aluno
    int result = strncmp(a->id_aluno, b->id_aluno, 3);
    if (result == 0)
    {
        // Se id_aluno for igual, compara id_disciplina
        result = strncmp(a->sigla_disc, b->sigla_disc, 3);
    }
    return result;
}

int compareKeysWrapper(const void *a, const void *b)
{
    return compareKeys((PrimaryKeyOnFile *)a, (PrimaryKeyOnFile *)b);
}

void ordenaIndex(PrimaryKeyOnFile *keys, int lastKeyInserted)
{
    qsort(keys, lastKeyInserted, sizeof(PrimaryKeyOnFile), compareKeysWrapper);
}

void writeIndexInFile(PrimaryKeyOnFile *keys)
{
    if (keys == NULL)
    {
        printf("eita bixo");
        return;
    }

    FILE *primary_key = startsPrimaryIndexFile();
    FILE *log_file = startsLogFile();

    int lastKeyInserted, valor = 1;

    fseek(log_file, 8, SEEK_SET);
    fread(&lastKeyInserted, sizeof(int), 1, log_file);

    ordenaIndex(keys, lastKeyInserted);
    fwrite(keys, sizeof(PrimaryKeyOnFile), lastKeyInserted + 1, primary_key);

    fseek(log_file, 0, SEEK_SET);
    fwrite(&valor, sizeof(int), 1, log_file);

    fclose(primary_key);
    fclose(log_file);
}

void insertRegister(Register *registro, PrimaryKeyOnFile *keys, int numb)
{
    FILE *log_file = startsLogFile();
    FILE *dados_file = iniciaArquivo();

    int numbKeys, valor = -1;

    fseek(log_file, 8, SEEK_SET);
    fread(&numbKeys, sizeof(int), 1, log_file);

    printf("%d", numbKeys);

    int i = 0, tam_reg = 0;
    char delimitador = '#';

    bool operation_flag, flag = true;
    fread(&operation_flag, sizeof(bool), 1, dados_file);

    if (operation_flag)
    {
        fseek(log_file, 4, SEEK_SET);
        fread(&i, sizeof(int), 1, log_file);
        i++;
    }
    else
    {
        fseek(dados_file, 0, SEEK_SET);
        fwrite(&flag, sizeof(bool), 1, dados_file);
    }

    for (int j = i; j < i + numb; j++, numbKeys++)
    {
        fseek(dados_file, 0, SEEK_END);
        int address_number = ftell(dados_file);

        strcpy(keys[numbKeys + 1].id_aluno, registro[j].id_aluno);
        strcpy(keys[numbKeys + 1].sigla_disc, registro[j].sigla_disc);
        keys[numbKeys + 1].address_number = address_number;

        tam_reg = calcula_tamanho(&registro[j]);
        fwrite(&tam_reg, sizeof(int), 1, dados_file);
        fwrite(registro[j].id_aluno, sizeof(char), 3, dados_file);
        fwrite(&delimitador, sizeof(char), 1, dados_file);
        fwrite(registro[j].sigla_disc, sizeof(char), 3, dados_file);
        fwrite(&delimitador, sizeof(char), 1, dados_file);
        fwrite(registro[j].nome_aluno, strlen(registro[j].nome_aluno) + 1, 1, dados_file);
        fwrite(&delimitador, sizeof(char), 1, dados_file);
        fwrite(registro[j].nome_disc, strlen(registro[j].nome_disc) + 1, 1, dados_file);
        fwrite(&delimitador, sizeof(char), 1, dados_file);
        fwrite(&registro[j].media, sizeof(float), 1, dados_file);
        fwrite(&delimitador, sizeof(char), 1, dados_file);
        fwrite(&registro[j].freq, sizeof(float), 1, dados_file);

        fseek(log_file, 4, SEEK_SET);
        fwrite(&j, sizeof(int), 1, log_file);
    }

    fseek(log_file, 0, SEEK_SET);
    fwrite(&valor, sizeof(int), 1, log_file);
    fseek(log_file, 4, SEEK_CUR);
    fwrite(&numbKeys, sizeof(int), 1, log_file);

    fclose(log_file);
    fclose(dados_file);
}

void imprimeChave(int address)
{
    FILE *data_file = iniciaArquivo();

    fseek(data_file, address, SEEK_SET);

    int tamanhoRegistro = 0;
    fread(&tamanhoRegistro, sizeof(int), 1, data_file);

    char buffer[tamanhoRegistro];
    fread(buffer, sizeof(buffer), 1, data_file);

    printf("%s\n", buffer);

    fclose(data_file);
}

void buscaChave(PrimaryKeyOnSearch *searchKeys, PrimaryKeyOnFile *keys)
{
    FILE *log_file = startsLogFile();

    int lastKeySearched, lastKeyInserted;

    fseek(log_file, 8, SEEK_SET);
    fread(&lastKeyInserted, sizeof(int), 1, log_file);
    fread(&lastKeySearched, sizeof(int), 1, log_file);

    lastKeySearched++;

    for (int i = 0; i < lastKeyInserted; i++)
    {
        if (strcmp(keys[i].id_aluno, searchKeys[lastKeySearched].id_aluno) == 0)
        {
            if (strcmp(keys[i].sigla_disc, searchKeys[lastKeySearched].sigla_disc) == 0)
            {
                imprimeChave(keys[i].address_number);

                fseek(log_file, 12, SEEK_SET);
                fwrite(&lastKeySearched, sizeof(int), 1, log_file);

                return;
            }
        }
    }
}

int main()
{
    int r;
    bool flag = true;

    Register *registros = readInsertRegister();
    PrimaryKeyOnFile *keys = readFilePrimaryKey();
    PrimaryKeyOnSearch *searchKeys = readSearchPrimaryKey();

    if (registros == NULL || searchKeys == NULL || keys == NULL)
    {
        printf("Erro ao alocar memória para Registros ou searchKeys\n");
        return 1;
    }

    printf("Seja bem-vindo ao gerenciador de registros escolares\n");

    while (flag)
    {
        printf("\n(1)Inserção\n(2)Busca por chave primária\n(3)Encerrar programa\n(4)Excluir Arquivos\nO que deseja fazer:");
        scanf("%d", &r);

        if (r == 1)
        {
            insertRegister(registros, keys, 1);
        }
        else if (r == 2)
        {
            FILE *log_file = startsLogFile();
            int lastKeyInserted;

            fseek(log_file, 8, SEEK_SET);
            fread(&lastKeyInserted, sizeof(int), 1, log_file);
            fclose(log_file);

            ordenaIndex(keys, lastKeyInserted);
            buscaChave(searchKeys, keys);
        }
        else if (r == 3)
        {
            writeIndexInFile(keys);
            flag = false;
        }
        else if (r == 4)
        {
            remove("dados.bin");
            remove("log_file.bin");
            remove("primary_index_file.bin");
        }
        else
        {
            printf("Escolha desconhecida, tente novamente!\n");
        }
    }
    return 0;
}