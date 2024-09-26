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

typedef struct SecondaryName
{
    char nome[50];
    int address;
} SecondaryName;

typedef struct SecondaryIDList
{
    char id_aluno[4];
    char sigla_disc[4];
    int prox_address;

} SecondaryIDList;

typedef struct SecondarySearch
{
    char nome_aluno[50];
} SecondarySearch;

FILE *startsLogFile()
{
    FILE *file = fopen("log_file.bin", "r+b");

    if (file == NULL)
    {

        file = fopen("log_file.bin", "w+b");

        if (file != NULL)
        {
            int index1 = -1, index2 = 0;

            fwrite(&index1, sizeof(int), 1, file);
            fwrite(&index1, sizeof(int), 1, file);
            fwrite(&index1, sizeof(int), 1, file);
            fwrite(&index1, sizeof(int), 1, file);
            fwrite(&index2, sizeof(int), 1, file);
            fwrite(&index1, sizeof(int), 1, file);

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

FILE *startsSecondaryNameIndexFIle()
{

    FILE *file = fopen("name_secondary.bin", "r+b");

    if (file == NULL)
    {

        file = fopen("name_secondary.bin", "w+b");

        if (file != NULL)
        {
            return file;
        }
        else
        {
            printf("Falha na abertura do arquivo name_secondary.\n");
            return NULL;
        }
    }

    return file;
}

FILE *startsSecondaryKeyIndexFile()
{

    FILE *file = fopen("key_secondary.bin", "r+b");

    if (file == NULL)
    {

        file = fopen("key_secondary.bin", "w+b");

        if (file != NULL)
        {
            return file;
        }
        else
        {
            printf("Falha na abertura do arquivo key_secondary.\n");
            return NULL;
        }
    }

    return file;
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

int isNameInSecondaryFile(char *nome, int numbOfNames, SecondaryName *nomes)
{
    for (int i = 0; i < numbOfNames; i++)
    {
        if (strcmp(nome, nomes[i].nome) == 0)
        {
            return -1;
        }
    }

    return 0;
}

PrimaryKeyOnFile *readFilePrimaryKey()
{
    FILE *log_file = startsLogFile();

    int correctEnd, numKeys, valor = 1;
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

            fread(&keys[i].id_aluno, 3, 1, data_file);
            keys[i].id_aluno[3] = '\0';

            fseek(data_file, 1, SEEK_CUR);

            fread(&keys[i].sigla_disc, 3, 1, data_file);
            keys[i].sigla_disc[3] = '\0';

            keys[i].address_number = address;

            int prox = size - 7;
            fseek(data_file, prox, SEEK_CUR);

            address = ftell(data_file);
            i++;
        }

        fclose(data_file);
        numKeys = i;
    }

    fseek(log_file, 8, SEEK_SET);
    fwrite(&numKeys, sizeof(int), 1, log_file);

    /*for (int i = 0; i < numKeys; i++)
    {
        printf("Chave %d: ID Aluno: %s, Sigla Disciplina: %s, Endereço Número: %d\n",
               i + 1, keys[i].id_aluno, keys[i].sigla_disc, keys[i].address_number);
    }*/

    fclose(log_file);
    return keys;
}

char *imprimeChave(int address)
{
    FILE *data_file = iniciaArquivo();

    fseek(data_file, address, SEEK_SET);

    int tamanhoRegistro = 0;
    fread(&tamanhoRegistro, sizeof(int), 1, data_file);

    char *buffer = (char *)malloc((tamanhoRegistro - 9 + 1) * sizeof(char));
    fread(buffer, sizeof(char), tamanhoRegistro - 9, data_file);

    buffer[tamanhoRegistro - 9] = '\0';

    fclose(data_file);
    return buffer;
}

int isAlunoInSecondaryFile(char *nome, SecondaryIDList *list, int numbOfId)
{

    for (int i = 0; i < numbOfId; i++)
    {
        if (strcmp(list[i].id_aluno, nome) == 0 && list[i].prox_address == -1)
        {
            return i;
        }
    }

    return -1;
}

SecondarySearch *readSearch()
{
    FILE *file = fopen("busca_s.bin", "r+b");

    SecondarySearch *searchName = (SecondarySearch *)malloc(100 * sizeof(SecondarySearch));

    if (searchName != NULL)
    {

        fread(searchName, sizeof(SecondarySearch), 100, file);
        fclose(file);

        return searchName;
    }

    printf("Falha na alocação de memória para as chaves de busca secundária\n");

    fclose(file);
    return NULL;
}

SecondaryName *createVectorOfSecondaryName()
{
    FILE *log_file = startsLogFile();

    int correctEnd, numbOfNames;
    fread(&correctEnd, sizeof(int), 1, log_file);
    fseek(log_file, 16, SEEK_SET);
    fread(&numbOfNames, sizeof(int), 1, log_file);

    SecondaryName *vectorOfSecondaryName = (SecondaryName *)malloc(sizeof(SecondaryName) * 100);

    if (correctEnd == 1 && numbOfNames != 0)
    {
        FILE *secondaryIndexName = startsSecondaryNameIndexFIle();

        char buffer[55], a;
        int address;

        for (int j = 0; j < numbOfNames; j++)
        {
            memset(buffer, 0, sizeof(buffer));

            for (int i = 0; i < 55; i++)
            {
                fread(&a, sizeof(char), 1, secondaryIndexName);
                if (a != '#')
                {
                    buffer[i] = a;
                }
                else
                {
                    buffer[i] = '\0';
                    break;
                }
            }

            fread(&address, sizeof(int), 1, secondaryIndexName);
            strcpy(vectorOfSecondaryName[j].nome, buffer);
            vectorOfSecondaryName[j].address = address;
        }

        fclose(secondaryIndexName);
    }
    else
    {
        PrimaryKeyOnFile *keys = readFilePrimaryKey();
        int numbPrimaryKeys, contador = 0;

        fseek(log_file, 8, SEEK_SET);
        fread(&numbPrimaryKeys, sizeof(int), 1, log_file);

        numbOfNames = 0;

        for (int i = 0; i < numbPrimaryKeys; i++)
        {
            char *buffer = imprimeChave(keys[i].address_number);

            strtok(buffer, "#");
            strtok(NULL, "#");

            char *nome_aluno = strtok(NULL, "#");

            int valor = isNameInSecondaryFile(nome_aluno, numbOfNames, vectorOfSecondaryName);

            if (valor == 0)
            {
                strcpy(vectorOfSecondaryName[numbOfNames].nome, nome_aluno);
                vectorOfSecondaryName[numbOfNames].address = numbOfNames + contador;
                numbOfNames++;
            }
            else
            {
                contador++;
            }
        }

        fseek(log_file, 16, SEEK_SET);
        fwrite(&numbOfNames, sizeof(int), 1, log_file);
    }

    fclose(log_file);
    return vectorOfSecondaryName;
}

SecondaryIDList *createVectorOfIDList()
{

    FILE *log_file = startsLogFile();

    int correctEnd, numbOfId;

    fread(&correctEnd, sizeof(int), 1, log_file);
    fseek(log_file, 8, SEEK_SET);
    fread(&numbOfId, sizeof(int), 1, log_file);

    SecondaryIDList *list = (SecondaryIDList *)malloc(sizeof(SecondaryIDList) * numbOfId);

    if (correctEnd == 1)
    {
        FILE *secondaryIndexID = startsSecondaryKeyIndexFile();

        for (int i = 0; i < numbOfId; i++)
        {

            char id_aluno[4];
            char id_disciplina[4];
            int address;

            fread(id_aluno, sizeof(char), 3, secondaryIndexID);
            fread(id_disciplina, sizeof(char), 3, secondaryIndexID);
            fread(&address, sizeof(int), 1, secondaryIndexID);

            id_aluno[3] = '\0';
            id_disciplina[3] = '\0';

            if (address != -1)
            {
                address = address / 10;
            }

            strcpy(list[i].id_aluno, id_aluno);
            strcpy(list[i].sigla_disc, id_disciplina);
            list[i].prox_address = address;
        }

        fclose(secondaryIndexID);
    }
    else
    {
        PrimaryKeyOnFile *keys = readFilePrimaryKey();

        for (int i = 0; i < numbOfId; i++)
        {
            strcpy(list[i].id_aluno, keys[i].id_aluno);
            strcpy(list[i].sigla_disc, keys[i].sigla_disc);
            list[i].prox_address = -1;

            int valor = isAlunoInSecondaryFile(list[i].id_aluno, list, i);

            if (valor != -1)
            {
                list[valor].prox_address = i;
            }
        }
    }

    /*printf("\nNumbOfId: %d\n", numbOfId);

    for (int i = 0; i < numbOfId; i++)
    {
        printf("ID Aluno: %s\n", list[i].id_aluno);
        printf("Sigla Disciplina: %s\n", list[i].sigla_disc);
        printf("Próximo Endereço: %d\n", list[i].prox_address);
        printf("-------------------------\n");
    }
    */

    fclose(log_file);
    return list;
}

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
    int result = strncmp(a->id_aluno, b->id_aluno, 3);
    if (result == 0)
    {
        result = strncmp(a->sigla_disc, b->sigla_disc, 3);
    }
    return result;
}

int compareKeysWrapper(const void *a, const void *b)
{
    return compareKeys((PrimaryKeyOnFile *)a, (PrimaryKeyOnFile *)b);
}

void ordenaPrimaryIndex(PrimaryKeyOnFile *keys, int lastKeyInserted)
{
    qsort(keys, lastKeyInserted, sizeof(PrimaryKeyOnFile), compareKeysWrapper);
}

int compareByName(const void *a, const void *b)
{
    SecondaryName *nameA = (SecondaryName *)a;
    SecondaryName *nameB = (SecondaryName *)b;

    return strcmp(nameA->nome, nameB->nome);
}

void ordenaSecondaryNameIndex(SecondaryName *names, int numbOfNames)
{
    qsort(names, numbOfNames, sizeof(SecondaryName), compareByName);
}

void writePrimaryIndexInFile(PrimaryKeyOnFile *keys)
{
    if (keys == NULL)
    {
        return;
    }

    FILE *primary_key = startsPrimaryIndexFile();
    FILE *log_file = startsLogFile();

    int lastKeyInserted;

    fseek(log_file, 8, SEEK_SET);
    fread(&lastKeyInserted, sizeof(int), 1, log_file);

    ordenaPrimaryIndex(keys, lastKeyInserted);
    fwrite(keys, sizeof(PrimaryKeyOnFile), lastKeyInserted, primary_key);

    fclose(primary_key);
    fclose(log_file);
}

void writeSecondaryIndexInFile(SecondaryName *names)
{
    FILE *secondaryNameFIle = startsSecondaryNameIndexFIle();
    FILE *log_file = startsLogFile();

    int numbOfNames;
    char delimitador = '#';

    fseek(log_file, 16, SEEK_SET);
    fread(&numbOfNames, sizeof(int), 1, log_file);

    ordenaSecondaryNameIndex(names, numbOfNames);

    for (int i = 0; i < numbOfNames; i++)
    {
        fwrite(names[i].nome, sizeof(char), strlen(names[i].nome), secondaryNameFIle);
        fwrite(&delimitador, sizeof(char), 1, secondaryNameFIle);
        fwrite(&names[i].address, sizeof(int), 1, secondaryNameFIle);
    }

    fclose(secondaryNameFIle);
    fclose(log_file);
}

void writeSecondaryIDIndexInFile(SecondaryIDList *list)
{

    FILE *secondaryIDFile = startsSecondaryKeyIndexFile();
    FILE *log_file = startsLogFile();

    int numbOfKeys;

    fseek(log_file, 8, SEEK_SET);
    fread(&numbOfKeys, sizeof(int), 1, log_file);
    int valor = 1;

    for (int i = 0; i < numbOfKeys; i++)
    {

        fwrite(list[i].id_aluno, sizeof(char), 3, secondaryIDFile);
        fwrite(list[i].sigla_disc, sizeof(char), 3, secondaryIDFile);

        int valor = list[i].prox_address;

        if (valor != -1)
        {
            valor = valor * 10;
        }

        fwrite(&valor, sizeof(int), 1, secondaryIDFile);
    }

    fseek(log_file, 0, SEEK_SET);
    fwrite(&valor, sizeof(int), 1, log_file);

    fclose(secondaryIDFile);
    fclose(log_file);
}

void insertRegister(Register *registro, PrimaryKeyOnFile *keys, int numb)
{
    FILE *log_file = startsLogFile();
    FILE *dados_file = iniciaArquivo();

    int numbKeys, valor = -1, numbName;

    fseek(log_file, 8, SEEK_SET);
    fread(&numbKeys, sizeof(int), 1, log_file);

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

        // Chave primária

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

        fseek(log_file, 4, SEEK_SET);
        fwrite(&j, sizeof(int), 1, log_file);
    }

    fseek(log_file, 0, SEEK_SET);
    fwrite(&valor, sizeof(int), 1, log_file);
    fseek(log_file, 8, SEEK_SET);
    fwrite(&numbKeys, sizeof(int), 1, log_file);

    fclose(log_file);
    fclose(dados_file);
}

void buscaChave(PrimaryKeyOnSearch *searchKeys, PrimaryKeyOnFile *keys)
{
    FILE *log_file = startsLogFile();

    int lastKeyInserted;

    fseek(log_file, 8, SEEK_SET);
    fread(&lastKeyInserted, sizeof(int), 1, log_file);

    for (int i = 0; i < lastKeyInserted; i++)
    {

        if (strcmp(keys[i].id_aluno, searchKeys->id_aluno) == 0)
        {
            if (strcmp(keys[i].sigla_disc, searchKeys->sigla_disc) == 0)
            {
                fclose(log_file);
                char *buffer = imprimeChave(keys[i].address_number);

                printf("Registro Encontrado: %s\n", buffer);

                return;
            }
        }
    }

    fclose(log_file);
    printf("Registro não encontrada!\n");
}

void buscaChavePrimaria(PrimaryKeyOnSearch *searchKeys, PrimaryKeyOnFile *keys)
{
    FILE *log_file = startsLogFile();

    int lastKeySearched;

    fseek(log_file, 12, SEEK_SET);
    fread(&lastKeySearched, sizeof(int), 1, log_file);
    lastKeySearched++;

    fseek(log_file, 12, SEEK_SET);
    fwrite(&lastKeySearched, sizeof(int), 1, log_file);

    fclose(log_file);

    buscaChave(&searchKeys[lastKeySearched], keys);
}

void *buscaChaveSecundaria(SecondarySearch *searchNames, SecondaryName *names, SecondaryIDList *list, PrimaryKeyOnFile *keys)
{

    FILE *log_file = startsLogFile();

    int lastKeySearched, numbNames;

    fseek(log_file, 16, SEEK_SET);

    fread(&numbNames, sizeof(int), 1, log_file);
    fread(&lastKeySearched, sizeof(int), 1, log_file);

    lastKeySearched++;

    fseek(log_file, 20, SEEK_SET);
    fwrite(&lastKeySearched, sizeof(int), 1, log_file);

    fclose(log_file);

    for (int i = 0; i < numbNames; i++)
    {

        if (strcmp(searchNames[lastKeySearched].nome_aluno, names[i].nome) == 0)
        {

            int j = names[i].address;

            while (j != -1)
            {

                PrimaryKeyOnSearch *novaBusca = (PrimaryKeyOnSearch *)malloc(sizeof(PrimaryKeyOnSearch));

                strcpy(novaBusca->id_aluno, list[j].id_aluno);
                strcpy(novaBusca->sigla_disc, list[j].sigla_disc);

                buscaChave(novaBusca, keys);

                j = list[j].prox_address;
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

    SecondaryIDList *list = createVectorOfIDList();
    SecondaryName *names = createVectorOfSecondaryName();
    SecondarySearch *nameSearch = readSearch();

    if (registros == NULL || searchKeys == NULL || keys == NULL)
    {
        printf("Erro ao alocar memória para Registros ou searchKeys\n");
        return 1;
    }

    printf("Seja bem-vindo ao gerenciador de registros escolares\n");

    while (flag)
    {
        printf("\n(1)Inserção\n(2)Busca por chave primária\n(3)Encerrar programa\n(4)Excluir Arquivos\n(5)Busca por chave secundária\nO que deseja fazer:");
        scanf("%d", &r);

        if (r == 1)
        {
            insertRegister(registros, keys, 1);
            list = createVectorOfIDList();
            names = createVectorOfSecondaryName();
        }
        else if (r == 2)
        {
            FILE *log_file = startsLogFile();
            int lastKeyInserted;

            fseek(log_file, 8, SEEK_SET);
            fread(&lastKeyInserted, sizeof(int), 1, log_file);
            fclose(log_file);

            ordenaPrimaryIndex(keys, lastKeyInserted);
            buscaChavePrimaria(searchKeys, keys);
        }
        else if (r == 3)
        {
            writePrimaryIndexInFile(keys);
            writeSecondaryIndexInFile(names);
            writeSecondaryIDIndexInFile(list);
            flag = false;
        }
        else if (r == 4)
        {
            remove("dados.bin");
            remove("log_file.bin");
            remove("primary_index_file.bin");
            remove("name_secondary.bin");
            remove("key_secondary.bin");

            PrimaryKeyOnFile *keys = readFilePrimaryKey();
            PrimaryKeyOnSearch *searchKeys = readSearchPrimaryKey();
            SecondaryIDList *list = createVectorOfIDList();
            SecondaryName *names = createVectorOfSecondaryName();
        }
        else if (r == 5)
        {
            buscaChaveSecundaria(nameSearch, names, list, keys);
        }
    }

    free(registros);
    free(keys);
    free(searchKeys);
    free(list);
    free(names);

    return 0;
}