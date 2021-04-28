#include <stdio.h>
#include <string.h>

#define MAX_IMAGE_HEIGHT 100
#define MAX_IMAGE_WIDTH 100

/**
 * @brief Definition of type Pixel which is similar to a double value
 */
typedef double Pixel;

/**
 * @brief Datastructure for type Image with the height, the width,
 * 		  and a double entry array for the pixels
 */
typedef struct MyImageStruct {
	Pixel pixelTab[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH];
	int height;
	int width;
} Image;

/* Here are the prototypes of the auxiliary functions */

Image diamond(Image img, int diag);

void display(Image img, FILE* flot);

int write_to_file(char* nom_fichier, Image img);

Image read_from_file(char* nom_fichier);

Image filter(Image img, Image mask);

/* Here is our Main function */

int main (void) {
	
	Image img;
	img.height = 0;
	img.width = 0;		
	int diag = -1;
	Image mask;
	
	int k;
	
	/* We ask the user for the name of the file to be used */
	
	char nom_fichier[FILENAME_MAX + 1] = "";
	int len;
	do {
		printf("Dans quel fichier voulez vous écrire ?\n");
		fgets(nom_fichier, FILENAME_MAX+1, stdin);
		len = strlen(nom_fichier) - 1;
		if ((len >= 0) && (nom_fichier[len] == '\n'))
			nom_fichier[len] = '\0';
	} while ((len < 1) && !feof(stdin) && !ferror(stdin));
	
	/* We ask the user for the height, the width, and the length of the diagonal */
	
	do {
		printf("Donnez la hauteur de l'image (100 > entier > 0) : ");
		fflush(stdout);
		k = scanf("%d", &img.height);
		if (k != 1) {
			printf("Je vous ai demandé un entier positif !\n");
			while (!feof(stdin) && !ferror(stdin) && getc(stdin) != '\n');
		}
	} while ((img.height <= 0 || img.height >= MAX_IMAGE_HEIGHT) && !feof(stdin) && !ferror(stdin));
	
	do {
		printf("Donnez la largeur de l'image (100 > entier > 0) : ");
		fflush(stdout);
		k = scanf("%d", &img.width);
		if (k != 1) {
			printf("Je vous ai demandé un entier positif !\n");
			while (!feof(stdin) && !ferror(stdin) && getc(stdin) != '\n');
		}
	} while ((img.width <= 0 || img.width >= MAX_IMAGE_WIDTH) && !feof(stdin) && !ferror(stdin));
	
	do {
		printf("Donnez la diagonale du losange (entier >= 0 et doit être < hauteur de l'image et < largeur de l'image) : "); 
		fflush(stdout);
		k = scanf("%d", &diag);
		if (k != 1) {
			printf("Je vous ai demandé un entier positif ou nul inférieur à la largeur et inférieur à la hauteur !\n");
			while (!feof(stdin) && !ferror(stdin) && getc(stdin) != '\n');
		}
	} while ((diag < 0 || diag > img.height - 1 || diag > img.width - 1) && !feof(stdin) && !ferror(stdin));
	
	/* Part 1 */
	
	printf("\nPartie 1 :\n\n");
	printf("Création de l'image via la méthode diamond\n");
	printf("(si la valeur entrée pour la hauteur, ou la largeur, est paire, elle sera augmentée de 1)\n");
	printf("...\n");
	img = diamond(img, diag);
	printf("Création de l'image terminée !\n");
	
	/* Part 2 */
	
	printf("\nPartie 2 :\n\n");
	printf("Affichage de l'image précédemment créée sur la sortie standard via la méthode display :\n\n");
	display(img, stdout);
	
	/* Part 3 */
	
	printf("\nPartie 3 :\n\n");
	printf("Ecriture dans le fichier de la largeur, de la hauteur, et du dessin de l'image, via la méthode write_to_file\n");
	printf("...\n");
	if (write_to_file(nom_fichier, img) == 0) {
		printf("Ecriture réussie !\n\n");
		printf("Lecture du fichier, et affichage du dessin qui y est contenu, via la méthode read_to_file :\n");
		printf("(si une erreur est détéctée, une image de hauteur 0 et de largeur 0 sera retournée)\n\n");
		display(read_from_file(nom_fichier), stdout);
	}
	
	/* Part 4 */
	
	printf("\nPartie 4 :\n\n");
	printf("Affichage de l'image filtrée, avec le masque prédéfini, via la méthode filter :\n\n");
	display(filter(img, mask), stdout);
	
	return 0;
	
}

/**
 * @brief Gives a value to each pixel of the image 
 * 		  to design a diamond form
 * 
 * @param img The image
 * 		  diag The diagonal
 */
Image diamond(Image img, int diag) {
	
	int i;
	int j;
	
	/* We make sure that the width and the height are odd */
	
	img.height |= 1;
	img.width |= 1;
	
	/* We first set all the pixels to 0.0 to get like a blank page */
	
	for (i = 0; i < img.height; i++) {
		for (j = 0; j < img.width; j++) {
			img.pixelTab[i][j] = 0.0;
		}
	}
	
	/* We then set to 1.0 the pixels of the diamond symmetrically to reduce the number of iterations */
	
	for (i = (img.width/2) - (diag/2); i <= (img.width/2) + (diag/2); i++) {
		img.pixelTab[img.height/2][i] = 1.0;
	}
	for (i = (img.height/2) + 1; i <= (img.height/2) + (diag/2); i++) {
		for (j = (img.width/2) - (diag/2) + (i - (img.height/2)); j <= (img.width/2) + (diag/2) - (i - (img.height/2)); j++) {
			img.pixelTab[i][j] = 1.0;
			img.pixelTab[(img.height - 1) - i][j] = 1.0;
		}
	}
	
	return img;
	
}

/**
 * @brief Displays the image on the given flot
 * 
 * @param img The image
 * 		  flot The flot
 */
void display(Image img, FILE* flot) {
	
	int i;
	int j;
	
	/* We put in the flot the associated symbol of the analyzed pixel */
	
	for (i = 0; i < img.height; i++) {
		for (j = 0; j < img.width; j++) {
			if (img.pixelTab[i][j] == 0.0) {
				fputs(".", flot);
			} else if (img.pixelTab[i][j] == 1.0) {
				fputs("+", flot);
			} else {
				fputs("*", flot);
			}	
		}
		fputs("\n", flot);
	}
	
}

/**
 * @brief Writes the dimensions and the drawing of the image 
 * 		  in the given file
 * 
 * @param nom_fichier The file
 * 		  img The image
 */
int write_to_file(char* nom_fichier, Image img) {
	
	FILE* myFile = NULL;
	
	/* Common writing procedure in a file */
	
	if (&nom_fichier[0] != '\0') {
		myFile = fopen(nom_fichier, "w");
		if (myFile == NULL) {
			fprintf(stderr, "Erreur : impossible d'écrire dans le fichier %s\n", nom_fichier);
			return 1;
		} else {
			fprintf(myFile, "Largeur : %d\nLongueur : %d\n", img.width, img.height);
			display(img, myFile);
			fclose(myFile);
		}
	}
	
	return 0;
		
}

/**
 * @brief Reads the image and its dimensions from the given 
 * 		  file and returns it
 * 
 * @param nom_fichier The file
 */
Image read_from_file(char* nom_fichier) {
	
	Image img;
	FILE* myFile = NULL;
	char c;
	
	/* We open the file, check for errors, and build the image */
	
	myFile = fopen(nom_fichier, "r");
	
	if (myFile == NULL) {
		
		fprintf(stderr, "Erreur : impossible de lire le fichier %s\n", nom_fichier);
		img.width = 0;
		img.height = 0;
		return img;
		
	} else {
		
		if (fscanf(myFile, "Largeur : %d\n", &img.width) != 1) {
			fprintf(stderr, "Erreur : impossible de lire la valeur de la largeur de l'image\n");
			img.width = 0;
			img.height = 0;
			return img;
		}
		if (img.width > MAX_IMAGE_WIDTH) {
			fprintf(stderr, "Erreur : valeur de la largeur trop élevée\n");
			img.width = 0;
			img.height = 0;
			return img;
		}
		if (fscanf(myFile, "Longueur : %d\n", &img.height) != 1) {
			fprintf(stderr, "Erreur : impossible de lire la valeur de la hauteur de l'image\n");
			img.width = 0;
			img.height = 0;
			return img;
		}
		if (img.height > MAX_IMAGE_HEIGHT) {
			fprintf(stderr, "Erreur : valeur de la hauteur trop élevée\n");
			img.width = 0;
			img.height = 0;
			return img;
		}
		
		/* We decided to associate the value 2.0 to '*' symbols */
		
		int i = 0;
		int j = 0;
		while((c = fgetc(myFile)) != EOF){
			if (c == '.') {
				img.pixelTab[i][j] = 0.0;
				j += 1;
			} else if (c == '+') {
				img.pixelTab[i][j] = 1.0;
				j += 1;
			} else if (c == '*') {
				img.pixelTab[i][j] = 2.0;
				j += 1;
			} else if (c == '\n') {
				i += 1;
				j = 0;
			} else {
				fprintf(stderr, "Erreur : impossible de lire toute l'image car le symbole %c ne correspond pas à un pixel\n", c);
				img.width = 0;
				img.height = 0;
				return img;
			}
		}
		
		fclose(myFile);
		return img;
		
	}
	
}

/**
 * @brief Returns a filtered image from a given image
 * 		  on which we apply a given mask
 * 
 * @param img The image
 * 		  mask The mask
 */
Image filter(Image img, Image mask) {
	
	Image filteredImg;
	
	int i;
	int j;
	int k;
	int l;
	
	/* We build the mask */
	
	mask.height = 3;
	mask.width = 3;
	for (i = 0; i < mask.height; i++) {
		for (j = 0; j < mask.width; j++) {
			mask.pixelTab[i][j] = -2.0 + 2.0*i;
		}
	}
	
	/* We set the height and the width of the filtered image */
	
	filteredImg.height = img.height;
	filteredImg.width = img.width;
	
	/* We do the computation with the given formula */
	
	for (i = 0; i < img.height; i++) {
		for (j = 0; j < img.width; j++) {
			for (k = 0; k < mask.height; k++) {
				for (l = 0; l < mask.width; l++) {
					
					/* For the negative index values, because applying on them the operator % in C doesn't give the wanted result, our computation will be a bit different */
					
					if ((i + (3/2) - k) < 0 && (j + (3/2) - l) >= 0) {
						filteredImg.pixelTab[i][j] += img.pixelTab[(i + (3/2) - k + img.height)%img.height][(j + (3/2) - l)%img.width] * mask.pixelTab[k][l];
					} else if ((i + (3/2) - k) >= 0 && (j + (3/2) - l) < 0) {
						filteredImg.pixelTab[i][j] += img.pixelTab[(i + (3/2) - k)%img.height][(j + (3/2) - l + img.width)%img.width] * mask.pixelTab[k][l];
					} else if ((i + (3/2) - k) < 0 && (j + (3/2) - l) < 0) {
						filteredImg.pixelTab[i][j] += img.pixelTab[(i + (3/2) - k + img.height)%img.height][(j + (3/2) - l + img.width)%img.width] * mask.pixelTab[k][l];
					} else {
						filteredImg.pixelTab[i][j] += img.pixelTab[(i + (3/2) - k)%img.height][(j + (3/2) - l)%img.width] * mask.pixelTab[k][l];
					}
					
				}
			}
		}
	}
	
	return filteredImg;
	
}
