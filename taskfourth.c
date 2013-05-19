#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>


typedef enum {
    false,
    true
} bool;

#define VECTOR_INIT_SIZE 8
#define VECTOR_TYPE uint

typedef struct {
    int  _capacity;
    int  size;
    VECTOR_TYPE *data;
} Vector;


Vector* vector_init() {
    Vector *vector;
    vector = (Vector*) malloc( sizeof(Vector) );
    vector->_capacity = VECTOR_INIT_SIZE;
    vector->size = 0;
    vector->data = (VECTOR_TYPE*) malloc( vector->_capacity * sizeof(VECTOR_TYPE) );
    return vector;
}


void vector_destruct(Vector *vector) {
    free( vector->data );
    free( vector );
}


void vector_push(Vector *vector, VECTOR_TYPE x) {
    if ( vector->size == vector->_capacity) {
        vector->_capacity += vector->_capacity / 2;
        vector->data = (VECTOR_TYPE*) realloc(vector->data, vector->_capacity * sizeof(VECTOR_TYPE));
    }
    vector->data[vector->size] = x;
    vector->size++;
}


static void help() {
    printf("Кодровщик-декодировщик. Author: alexander lavrukov\n");
    printf("\n");
    printf("Использование: fourth [-e|-d] [FILE] [-o FILE]\n");
    printf("\n");
    printf("Без параметров исполнит кодирование stdin.\n");
    printf("Если не указать -o FILE то будет производиться вывод на stdout.\n");
    printf("\n");
    printf("Ключи:\n");
    printf("    -h            показать эту справку\n");
    printf("    -e            кодировать входной файл\n");
    printf("    -d            декодировать входной файл\n");
    printf("    -o <файл>     записать результат в FILE\n");
    printf("\n");

}


static void open_file(FILE **file, char *file_name, const char *flag) {
    if ( *file_name ) {
        *file = fopen(file_name, flag);
        if ( !(*file) ) {
            fprintf(stderr, "Ошибка при открытии файла '%s'\n", file_name);
            exit(1);
        }
    }
}


static void read_argv(int argc, char **argv, FILE **input_file, FILE **output_file, bool *is_encode) {
    char *input_file_name  = "";
    char *output_file_name = "";
    int  noargs;
    char c;
    bool is_decode = false;

    *is_encode = false;

    while ( (c = getopt(argc, argv, "hedo:")) != -1 ) {
        switch (c) {
            case 'e':
                *is_encode = true;
                break;
            case 'd':
                is_decode = true;
                break;
            case 'o':
                output_file_name = optarg;
                break;
            case 'h':
                help();
                exit(0);
            case '?':
                exit(1);
            default:
                abort();
        }
    }
    
    noargs = argc - optind;

    if ( *is_encode && is_decode ) {
        fprintf(stderr, "Определитесь: кодировать или декодировать!\n");
        exit(1);
    }
    else if ( !(*is_encode) && !is_decode ) {
        *is_encode = true;
    }

    if ( noargs == 1 ) {
        input_file_name = argv[optind];
    }
    else if ( noargs > 1 ) {
        fprintf(stderr, "Многовато имен файлов.\n");
        exit(1);
    }

    open_file(&*input_file,  input_file_name,  "r");
    open_file(&*output_file, output_file_name, "w");
}


static void read_picture(FILE *input_file, int *ret_size, char ***picture) {
    int  i, j;
    int  size;
    char **data;

    if (fscanf(input_file, "%d", &size) ==EOF ) {
        fprintf(stderr, "Неожиданный конец входа.\n");
        exit(1);
    }

    if ( (size & (size-1)) || !(size-1) ) {
        fprintf(stderr, "Некорректный размер изображения: длина стороны " \
                        "должнa быть степенью двойки большей нуля\n");
        exit(1);
    }

    data = (char**) malloc( size * sizeof(char*) );
    for ( i=0; i < size; i++ ) {
        data[i] = (char*) malloc( size * sizeof(char) );
    }

    for ( i=0; i < size; i++) {
        for ( j=0; j < size; j++) {
            int num;
            if ( fscanf(input_file, "%d", &num) == EOF ) {
                fprintf(stderr, "Неожиданный конец входа.\n");
                exit(1);
            }
            data[i][j] = (char) (num & 1);
        }
    }

    *ret_size = size;
    *picture  = data;
}


static void read_code(FILE *input_file, Vector **vector, int *ret_size) {
    int size;
    int n;
    uint num;
    Vector *data;

    data = vector_init();
    
    if (fscanf(input_file, "%d", &size) == EOF ) {
        fprintf(stderr, "Неожиданный конец входа.\n");
        exit(1);
    }

    n = size * size;

    while ( n-- ) {
        if (fscanf(input_file, "%u", &num) == EOF ) {
            break;
        }
        vector_push(data, num);
    }

    *ret_size = size;
    *vector   = data;
}


static void write_picture(FILE *output_file, char **picture, int size) {
    int i, j;

    fprintf(output_file, "%d", size);
    for (i=0; i<size; i++) {
        fprintf(output_file, "\n");
        for (j=0; j<size; j++) {
            fprintf(output_file, "%d ", picture[i][j]);
        }
    }
}


static void write_code(FILE *output_file, Vector *vector, int size) {
    int i;

    fprintf(output_file, "%d", size);
    for (i=0; i<vector->size; i++) {
        fprintf(output_file, " %d", vector->data[i]);
    }
}


static bool is_zero_square(char **picture, int size, int x, int y) {
    int i, j;

    for ( i=0; i < size; i++) {
        for ( j=0; j < size; j++ ) {
            if ( picture[x+i][y+j] ) {
                return false;
            }
        }
    }

    return true;
}


static void _encode(Vector *vector, uint code, uint order, char **picture, int size, int x, int y) {
    int  i;
    int  x1, y1;

    if ( size == 1 ) {
        if ( picture[x][y] ) {
            vector_push(vector, code);
        }
        return;
    }
    
    for (i=0; i < 4; i++ ) { 
        x1 = ( (i & 2) >> 1 ) * (size / 2);
        y1 = (i & 1) * (size / 2);
        if ( ! is_zero_square(picture, size/2, x+x1, y+y1) ) {
            _encode(vector, code + i*order, order*4,  picture, size/2, x+x1, y+y1);
        }
    }
}


static Vector* encode(char **picture, int size) {
    Vector *vector; 
    vector = vector_init();
    _encode(vector, 0, 1, picture, size, 0, 0);
    return vector;
}


static void decode_number(uint num, int size, int *x, int *y) {
    int p;
    *x = 0;
    *y = 0;
    while ( size > 1 ) {
        size /= 2;
        p = num % 4;
        num /= 4;
        *x += ( (p & 2) >> 1 ) * size;
        *y += (p & 1) * size;
    }
}


static char** decode(Vector *vector, int size) {
    int i;
    int x, y;
    char **picture;

    picture = (char**) malloc( size * sizeof(char*) );
    for ( i=0; i < size; i++ ) {
        picture[i] = (char*) malloc( size * sizeof(char) );
    }
    
    for ( i=0; i < vector->size; i++ ) {
        decode_number(vector->data[i], size, &x, &y);
        picture[x][y] = 1;
    }

    return picture;
}


int main(int argc, char **argv) {
    FILE *input_file  = stdin;
    FILE *output_file = stdout;
    bool is_encode;
    int  size;
    char **picture;
    Vector *vector;
    
    read_argv(argc, argv, &input_file, &output_file, &is_encode);

    if ( is_encode ) {
        read_picture(input_file, &size, &picture);
        vector = encode(picture, size);
        write_code(output_file, vector, size);
    }
    else {
        read_code(input_file, &vector, &size);
        picture = decode(vector, size);
        write_picture(output_file, picture, size);
    }

    return 0;
}

