#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define HASH_TABLE_LOAD_FACTOR 0.75

#define CSV_MAX_LINE_SIZE 1024
#define CSV_SEPARATOR ','

/* ======================================================================
 * Provided utility functions
 * ======================================================================
 */

/** ----------------------------------------------------------------------
 ** Ask for a filename
 **/
void ask_filename(const char* question, char filename[])
{
    size_t len = 0;
    do {
        fputs(question, stdout);
        fflush(stdout);
        fgets(filename, FILENAME_MAX + 1, stdin);
        filename[strcspn(filename, "\r\n")] = '\0';
        len = strlen(filename) - 1;
    } while (len < 1 && !feof(stdin) && !ferror(stdin));
}

/** ----------------------------------------------------------------------
 ** Ask for a filename to open in a given mode
 **/
FILE* ask_filename_and_open(const char* question, const char* mode)
{
    char filename[FILENAME_MAX + 1] = "";

    ask_filename(question, filename);
    FILE* retour = fopen(filename, mode);

    if (retour == NULL) {
        fprintf(stderr, "Oops, je n'ai pas réussi à ouvrir le fichier \"%s\" en mode \"%s\"!\n",
                filename, mode);
    }
    return retour;
}

/** ----------------------------------------------------------------------
 ** Ask for a size
 **/
size_t ask_size_t(const char* question)
{
    size_t v = 0;
    int res = 0;
    do {
        fputs(question, stdout);
        fflush(stdout);
        res = scanf("%zu", & v);
        while (!feof(stdin) && !ferror(stdin) && getc(stdin) != '\n');
    } while (!feof(stdin) && !ferror(stdin) && res != 1 && v == 0);
    return v;
}

/* ======================================================================
 * Part I -- Hashtable
 * ======================================================================
 */

/** ----------------------------------------------------------------------
 ** Hash a string for a given hashtable size.
 ** See http://en.wikipedia.org/wiki/Jenkins_hash_function
 **/
size_t hash_function(const char* key, size_t size)
{
    size_t hash = 0;
    size_t key_len = strlen(key);
    for (size_t i = 0; i < key_len; ++i) {
        hash += (unsigned char) key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % size;
}

/* ****************************************
 * TODO : add your own code here.
 * **************************************** */

/**
 * @brief Datastructure for type bucket with his key, his value,
 * 		  and the pointer to his next element in the linked list
 */
struct bucket_struct {
	
	const char* key;
	const void* value;
	struct bucket_struct* next_elem;
	
};

typedef struct bucket_struct bucket;

/**
 * @brief Datastructure for type Htable with his size
 * 		  and the double pointer to his double dimension content
 */
struct hashtable_struct {

	size_t size;
	struct bucket_struct** content;

};

typedef struct hashtable_struct Htable;

/**
 * @brief Constructs a hash table with a given size
 * 
 * @param size The size of the hash table which is also the
 * 		  maximum number of linked lists it can contains
 */
Htable* construct_Htable(size_t size) {
	
	Htable* new_hash_table = NULL;
	
	if (size < 1) return NULL;
	
	new_hash_table = malloc(sizeof(Htable));
	
	if (new_hash_table == NULL) return NULL;
	
	new_hash_table->size = size;
	
	new_hash_table->content = calloc(size, sizeof(bucket*));
	
	if (new_hash_table->content == NULL) {
		free(new_hash_table);
		new_hash_table = NULL;
		return NULL;
	}
	
	int i;
	
	for (i = 0; i < size; i++) {
		new_hash_table->content[i] = NULL;
	}
	
	return new_hash_table;
	
}

/**
 * @brief Destructs a given hash table
 * 
 * @param hash_table The hash table
 */
void delete_Htable_and_content(Htable** hash_table) {
	
	if (*hash_table != NULL) {
	
		int i;
	
		bucket* elem_to_delete = NULL;
	
		for(i = 0; i < (*hash_table)->size; i++) {
			while((*hash_table)->content[i] != NULL) {
				elem_to_delete = (*hash_table)->content[i];
				(*hash_table)->content[i] = (*hash_table)->content[i]->next_elem;
				free((void*) elem_to_delete->value);
				elem_to_delete->value = NULL;
				free((char*) elem_to_delete->key);
				elem_to_delete->key = NULL;
				free(elem_to_delete);
				elem_to_delete = NULL;
			}
		}

		free((*hash_table)->content);
		(*hash_table)->content = NULL;
	
		free(*hash_table);
		*hash_table = NULL;
	
	}
	
}

/**
 * @brief Adds an element to the hash table which will create a linked
 * 		  list, or will be put at the end of a linked list, except 
 * 		  if the key already exists in the hash table, then the 
 * 		  value will be overwritten
 * 
 * @param hash_table The given hash table
 * 		  key The given key of the element
 * 		  value The given value of the element
 */
void add_Htable_value(Htable* hash_table, const char* key, const void* value) {
	
	if (hash_table != NULL && key != NULL && value != NULL) {
	
		int hash_key_value = hash_function(key, hash_table->size);
	
		bucket* current_elem = NULL;
		bucket* previous_elem = NULL;
		bucket* added_pair = NULL;
	
		current_elem = hash_table->content[hash_key_value];
	
		while (current_elem != NULL && strcmp(key, current_elem->key) != 0) {
			previous_elem = current_elem;
			current_elem = current_elem->next_elem;
		}
	
		if (current_elem != NULL) {
			current_elem->value = value;
		} else {
			added_pair = malloc(sizeof(bucket));
			if (added_pair != NULL) {
				added_pair->key = key;
				added_pair->value = value;
				added_pair->next_elem = NULL;
				if (previous_elem == NULL) hash_table->content[hash_key_value] = added_pair;
				else previous_elem->next_elem = added_pair;
			}
		}
	
	}
	
}

/**
 * @brief Returns the element which has the given key 
 * 		  or NULL if it doesn't exist in the hash table
 * 
 * @param hash_table The given hash table
 * 		  key The key of the element we want to find
 */
const void* get_Htable_value(Htable* hash_table, const char* key) {
	
	if (hash_table != NULL && key != NULL) {
	
		int hash_key_value = hash_function(key, hash_table->size);
	
		bucket* current_elem = NULL;
	
		current_elem = hash_table->content[hash_key_value];
	
		while (current_elem != NULL && strcmp(key, current_elem->key) != 0) {
			current_elem = current_elem->next_elem;
		}
	
		if (current_elem != NULL) {
			return current_elem->value;
		}
	
	}
	
	return NULL;
	
}

/* ======================================================================
 * Provided: CSV file parser
 * ======================================================================
 */

typedef char* csv_row;
typedef const char* csv_const_row;

/** ----------------------------------------------------------------------
 ** Allocate and read a CSV row from a file
 **/
csv_row read_row(FILE* f)
{
    char line[CSV_MAX_LINE_SIZE + 1] = "";
    fgets(line, CSV_MAX_LINE_SIZE, f);
    line[strcspn(line, "\r\n")] = '\0'; // remove trailing '\n'
    size_t len = strlen(line);

    assert(len < CSV_MAX_LINE_SIZE); /* We should not have lines that long...
                                      * Not handled in this homework anyway!
                                      * Should be properly handled in real-life appli.
                                      */

    csv_row row;
    if ((row = calloc(len + 1, sizeof(char))) == NULL) {
        return NULL;
    }
    strncpy(row, line, len);

    return row;
}

/** ----------------------------------------------------------------------
 ** Write a CSV row to a file
 **/
void write_row(FILE* out, const csv_const_row row, size_t ignore_index)
{
    size_t len = strlen(row);
    size_t current_element = 0;
    for (size_t i = 0; i < len; ++i) {
        if (row[i] == CSV_SEPARATOR) {
            ++current_element;
        }
        if (current_element != ignore_index && (current_element != ignore_index + 1 || row[i] != CSV_SEPARATOR)) {
            fprintf(out, "%c", row[i]);
        }
    }
}

/** ----------------------------------------------------------------------
 ** Write 2 CSV rows side-by-side to a file
 **/
void write_rows(FILE* out, const csv_const_row row1, const csv_const_row row2, size_t ignore_index)
{
    write_row(out, row1, (size_t) -1);
    fprintf(out, "%c", CSV_SEPARATOR);
    write_row(out, row2, ignore_index);
    fprintf(out, "\n");
}

/** ----------------------------------------------------------------------
 ** Copy and return the i'th element in the row
 **/
char* row_element(const csv_const_row row, size_t index)
{
    size_t len = strlen(row);
    size_t start = 0, end = 0;
    size_t current_element = 0;
    for (size_t i = 0; i < len; ++i) {
        if (row[i] == CSV_SEPARATOR || i == len - 1) {
            ++current_element;
            if (current_element == index) {
                start = i + 1;
            } else if (current_element == index + 1) {
                end = (i == len - 1) ? len : i;
                break;
            }
        }
    }

    if (end > 0) { // success
        size_t elem_len = end - start;
        char* element;
        if ((element = calloc(elem_len + 1, sizeof(char))) == NULL) {
            return NULL;
        }
        element[elem_len] = '\0';
        strncpy(element, &row[start], elem_len);
        return element;
    } else {
        return NULL;
    }
}


/* ======================================================================
 * Part II -- Hashjoin
 * ======================================================================
 */

/* ****************************************
 * TODO : add your own code here.
 * **************************************** */

/**
 * @brief Join two files thanks to their column of the same 
 * 		  type of content
 * 
 * @param first_file The file that has unique key values
 * 		  second_file The file that can have several same key values
 * 		  output_file The file where the result is written
 * 		  column_first_file The index of the first file's join column
 * 		  column_second_file The index of the second file's join column
 * 		  max_memory The maximum authorized memory in the hash table
 */
int hash_join(FILE* first_file, FILE* second_file, FILE* output_file, size_t column_first_file, size_t column_second_file, size_t max_memory) {
	
	if (first_file == NULL || second_file == NULL || output_file == NULL) {
		fprintf(stderr, "Un ou plusieurs fichiers déstinés à être écrits ou lus sont invalides");
		return 9;
	}
	
	Htable* hash_table = NULL;
	const void* found_elem = NULL;
	
	csv_row row1 = NULL;
	csv_row row2 = NULL;
	char* id1 = NULL;
	char* id2 = NULL;
	
	int count = 0;
	size_t size_given = max_memory/sizeof(bucket);
	
	if (size_given == 0) {
		fprintf(stderr, "Mémoire maximum autorisée insuffisante pour la hash table\n");
		return 8;
	}
	
	row1 = read_row(first_file);
	if (row1 == NULL) {
		fprintf(stderr, "Erreur dans l'allocation de mémoire pour une ligne du premier fichier\n");
		return 3;
	}
	if (ferror(first_file) != 0) {
		fprintf(stderr, "Erreur dans la lecture du premier fichier\n");
		free(row1);
		row1 = NULL;
		return 4;
	}
	if (feof(first_file) != 0) {
		fprintf(stderr, "Absence des données dans le premier fichier\n");
		free(row1);
		row1 = NULL;
		return 5;
	}
	
	row2 = read_row(second_file);
	if (row2 == NULL) {
		fprintf(stderr, "Erreur dans l'allocation de mémoire pour une ligne du deuxième fichier\n");
		free(row1);
		row1 = NULL;
		return 3;
	}
	if (ferror(second_file) != 0) { 
		fprintf(stderr, "Erreur dans la lecture du deuxième fichier\n");
		free(row1);
		row1 = NULL;
		free(row2);
		row2 = NULL;
		return 4;
	}
	if (feof(second_file) != 0) {
		fprintf(stderr, "Absence des données dans le deuxième fichier\n");
		free(row1);
		row1 = NULL;
		free(row2);
		row2 = NULL;
		return 5;
	}
	
	id1 = row_element(row1, column_first_file);
	id2 = row_element(row2, column_second_file);
	if (id1 == NULL || id2 == NULL) {
		fprintf(stderr, "Indexes entrés invalides\n");
		free(id1);
		id1 = NULL;
		free(row1);
		row1 = NULL;
		free(id2);
		id2 = NULL;
		free(row2);
		row2 = NULL;
		return 6;
	}
	if (strcmp(id1, id2) != 0) {
		fprintf(stderr, "Colonnes aux types de contenu différents\n");
		free(id1);
		id1 = NULL;
		free(row1);
		row1 = NULL;
		free(id2);
		id2 = NULL;
		free(row2);
		row2 = NULL;
		return 7;
	}
	
	write_rows(output_file, row1, row2, column_second_file);
	
	free(id1);
	id1 = NULL;
	free(row1);
	row1 = NULL;
	free(id2);
	id2 = NULL;
	free(row2);
	row2 = NULL;
	
	while (feof(first_file) == 0) {
		
		row1 = read_row(first_file);
		id1 = row_element(row1, column_first_file);
		
		if (ferror(first_file) != 0 || (strcmp(row1, "\0") != 0 && (row1 == NULL || id1 == NULL))) {
			if (hash_table != NULL) delete_Htable_and_content(&hash_table);
			if (id1 == NULL) { 
				fprintf(stderr, "Erreur dans l'allocation de mémoire pour une clé du premier fichier\n"); 
				if (row1 != NULL) {
					free(row1);
					row1 = NULL;
				}
				return 2;
			}
			if (row1 == NULL) { 
				fprintf(stderr, "Erreur dans l'allocation de mémoire pour une ligne du premier fichier\n"); 
				free(id1);
				id1 = NULL;
				return 3;
			}
			if (ferror(first_file) != 0) {
				fprintf(stderr, "Erreur dans la lecture du premier fichier\n");
				free(id1);
				id1 = NULL;
				free(row1);
				row1 = NULL;
				return 4;
			}
		}
		
		if (strcmp(row1, "\0") != 0) {
			if (hash_table == NULL) hash_table = construct_Htable(size_given);
			add_Htable_value(hash_table, row_element(row1, column_first_file), row1);
			count++;
		} else {
			if (hash_table == NULL) break;
		}
		
		if (count >= hash_table->size * HASH_TABLE_LOAD_FACTOR || (feof(first_file) != 0)) {

			while (feof(second_file) == 0) {
				
				row2 = read_row(second_file);
				id2 = row_element(row2, column_second_file);
				
				if (ferror(second_file) != 0 || (strcmp(row2, "\0") != 0 && (row2 == NULL || id2 == NULL))) {
					delete_Htable_and_content(&hash_table);
					if (id2 == NULL) { 
						fprintf(stderr, "Erreur dans l'allocation de mémoire pour une clé du deuxième fichier\n");
						free(id1);
						id1 = NULL;
						free(row1);
						row1 = NULL;
						if (row2 != NULL) {
							free(row2);
							row2 = NULL;
						}
						return 2;
					}
					if (row2 == NULL) {
						fprintf(stderr, "Erreur dans l'allocation de mémoire pour une ligne du deuxième fichier\n");
						free(id1);
						id1 = NULL;
						free(row1);
						row1 = NULL;
						free(id2);
						id2 = NULL;
						return 3;
					}
					if (ferror(second_file) != 0) {
						fprintf(stderr, "Erreur dans la lecture du deuxième fichier\n"); 
						free(id1);
						id1 = NULL;
						free(row1);
						row1 = NULL;
						free(id2);
						id2 = NULL;
						free(row2);
						row2 = NULL;
						return 4;
					}
				}
				
				if (strcmp(row2, "\0") != 0) {		
					found_elem = get_Htable_value(hash_table, row_element(row2, column_second_file));			
					if (found_elem != NULL) write_rows(output_file, found_elem, row2, column_second_file);
				}
				
				free(id2);
				id2 = NULL;
				free(row2);
				row2 = NULL;
				
			}
			
			fseek(second_file, 0, SEEK_SET);
			delete_Htable_and_content(&hash_table);
			count = 0;
			
		}
		
	}
	
	free(id1);
	id1 = NULL;
	free(row1);
	row1 = NULL;
	
	return 0;
	
}

/* ======================================================================
 * Provided: main()
 * ======================================================================
 */

int main(void)
{
    FILE* in1 = ask_filename_and_open("Entrez le nom du premier fichier : ", "r");
    if (in1 == NULL) {
        return EXIT_FAILURE;
    }

    FILE* in2 = ask_filename_and_open("Entrez le nom du second  fichier : ", "r");
    if (in2 == NULL) {
        fclose(in1);
        return EXIT_FAILURE;
    }

    FILE* out = ask_filename_and_open("Entrez le nom du fichier où écrire le résultat : ", "w");
    if (out == NULL) {
        fclose(in1);
        fclose(in2);
        return EXIT_FAILURE;
    }

    size_t col1 = ask_size_t("Entrez l'index de la colonne à joindre dans le premier fichier : "),
           col2 = ask_size_t("Entrez l'index de la colonne à joindre dans le second  fichier : ");

    int success = hash_join(in1, in2, out, col1, col2,
                            ask_size_t("Entrez le budget mémoire autorisé (en octets) : ")
                            );

    fclose(in1);
    fclose(in2);
    fclose(out);

    return success;
}
