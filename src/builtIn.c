#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtIn.h"
#include "environ.h" 

/**
 * @brief Vérifie si une commande est une commande intégrée (built-in) du shell.
 *
 * Cette fonction prend une chaîne de caractères représentant une commande et vérifie
 * si cette commande est une commande intégrée supportée par le shell. Les commandes 
 * intégrées prises en charge sont : "cd", "pwd", "echo", "exit", "env", "export" et "unset".
 *
 * @param cmd La commande à vérifier. Ce paramètre ne doit pas être NULL.
 * @return 1 si la commande est une commande intégrée, 0 sinon.
 *
 * @note Si `cmd` est NULL, la fonction retourne 0.
 *
 * @example
 * int result = isBuiltIn("cd"); // result sera 1 car "cd" est une commande intégrée.
 * result = isBuiltIn("ls"); // result sera 0 car "ls" n'est pas une commande intégrée.
 */
int 
isBuiltIn(const char* cmd){
    const char* buildIn[] = {"cd", "pwd", "echo", "exit", "env", "export", "unset", NULL};

    if (cmd == NULL) {
        return 0;
    }

    for (size_t i = 0; buildIn[i]; i++) {
        if (!strcmp(buildIn[i], cmd)){
            return 1;
		} 
    }
    return 0;
}

/**
 * @brief Change le répertoire de travail courant.
 *
 * Cette fonction change le répertoire de travail courant en utilisant le chemin spécifié.
 * Si le chemin est NULL ou si l'opération échoue, un message d'erreur est affiché.
 *
 * @param nextPath Le chemin vers le nouveau répertoire de travail. Ce paramètre ne doit pas être NULL.
 *
 * @note Si "nextPath" est NULL, un message d'erreur est affiché sur la sortie d'erreur standard (stderr).
 *       Si l'appel à "chdir" échoue, un message d'erreur est affiché sur la sortie d'erreur standard (stderr)
 *       en utilisant "perror" pour fournir des informations supplémentaires sur l'erreur.
 *
 * @example
 * builtInCd("/home/user/Documents"); // Change le répertoire de travail courant vers /home/user/Documents
 */
void 
builtInCd(const char* nextPath){
    if (nextPath == NULL) {
        fprintf(stderr, "Error: Null path provided to built_in_cd.\n");
        return;
    }
    int result = chdir(nextPath);
    if(result == -1) {
        perror("chdir");
    }
}

/**
 * @brief Affiche le répertoire de travail courant.
 *
 * Cette fonction utilise la fonction "getcwd" pour obtenir le chemin absolu
 * du répertoire de travail courant et l'affiche sur la sortie standard. Si 
 * l'opération échoue, un message d'erreur est affiché sur la sortie d'erreur 
 * standard (stderr).
 *
 * @note La constante "MAX_PATH_LEN" doit être définie pour spécifier la taille maximale 
 *       du buffer "cwd" utilisé pour stocker le chemin du répertoire courant.
 *
 * @example
 * builtInPwd(); // Affiche le chemin du répertoire de travail courant, par exemple: /home/user
 */
void 
builtInPwd(){
    char cwd[MAX_PATH_LEN];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd");
    }
}

/**
 * @brief Affiche une chaîne de caractères.
 *
 * Cette fonction prend un tableau de chaînes de caractères en entrée et les affiche
 * sur la sortie standard, séparées par des espaces.
 *
 * @param str Tableau de chaînes de caractères. Le premier élément du tableau est 
 *            ignoré, et les éléments suivants sont affichés. La dernière chaîne doit 
 *            être suivie d'un pointeur NULL pour marquer la fin du tableau.
 *
 * @note Si le tableau de chaînes de caractères est NULL, un message d'erreur est 
 *       affiché sur la sortie d'erreur standard (stderr).
 *
 * @example
 * char *args[] = {"echo", "Hello", "World", NULL};
 * builtInEcho(args); // Affiche: Hello World
 */
void 
builtInEcho(char** cmd){
    if (cmd == NULL) {
        fprintf(stderr, "Error: Null string provided to built_in_echo.\n");
        return;
    }
    for(size_t i = 1; cmd[i] != NULL; i++){
    	printf("%s ", cmd[i]);
    }
    printf("\n");	
}

/**
 * @brief Quitte le programme avec un code de sortie spécifié.
 *
 * Cette fonction termine le programme en cours d'exécution avec un code de sortie 
 * fourni en tant que chaîne de caractères. Si le code de sortie est NULL ou invalide, 
 * elle quitte avec le code de sortie 0.
 *
 * @param statusStr Une chaîne de caractères représentant le code de sortie. Si NULL, 
 *                  le code de sortie par défaut est 0.
 *
 * @note Si "statusStr" est une chaîne invalide (c'est-à-dire ne représentant pas un 
 *       entier valide), un message d'erreur est affiché et le programme se termine 
 *       avec un code de sortie de 0.
 *
 * @example
 * builtInExit("0");  // Quitte le programme avec le code de sortie 0.
 * builtInExit("1");  // Quitte le programme avec le code de sortie 1.
 * builtInExit(NULL); // Quitte le programme avec le code de sortie 0.
 * builtInExit("abc"); // Affiche une erreur et quitte avec le code de sortie 0.
 */
void 
builtInExit(const char* statusStr){
    int exitStatus = 0;

    if (statusStr != NULL) {
        char *endptr;
        exitStatus = strtol(statusStr, &endptr, 10);

        if (*endptr != '\0') {
            fprintf(stderr, "Error: Invalid exit status '%s'. Exiting with status 0.\n", statusStr);
            exitStatus = 0;
        }
    }
    exit(exitStatus);
}

/**
 * @brief Affiche toutes les variables d'environnement.
 *
 * Cette fonction parcourt et affiche toutes les variables d'environnement actuelles.
 * Chaque variable est imprimée sur une ligne distincte.
 *
 * @note La variable externe "environ" doit être disponible. Cette variable contient un
 *       tableau de pointeurs vers des chaînes de caractères représentant les variables
 *       d'environnement.
 *
 * @example
 * builtInEnv(); // Affiche toutes les variables d'environnement
 */
void 
builtInEnv(){
	for (size_t i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
}

/**
 * @brief Gère l'exportation des variables d'environnement.
 *
 * Cette fonction permet de définir ou d'afficher des variables d'environnement. 
 * Si aucun argument n'est fourni, elle affiche toutes les variables d'environnement 
 * sous la forme "declare -x". Si un argument est fourni, il doit être sous la forme 
 * "NOM=VALEUR" pour définir une nouvelle variable d'environnement.
 *
 * @param cmd Tableau de chaînes de caractères représentant les arguments passés à 
 *            la commande export. Le premier élément (cmd[0]) est la commande elle-même 
 *            ("export"), et le second élément (cmd[1]) est l'argument à traiter.
 *
 * @note Si cmd[1] est NULL, toutes les variables d'environnement sont affichées.
 *       Si cmd[1] ne contient pas un argument valide (i.e., une chaîne sous la forme 
 *       "NOM=VALEUR"), un message d'erreur est affiché.
 *
 * @example
 * char *args1[] = {"export", "MYVAR=myvalue", NULL};
 * builtInExport(args1); // Définit la variable d'environnement MYVAR avec la valeur myvalue.
 * 
 * char *args2[] = {"export", NULL};
 * builtInExport(args2); // Affiche toutes les variables d'environnement.
 */
void builtInExport(char** cmd) {
    char* arg = cmd[1];
    
    if (arg == NULL) {
        for (size_t i = 0; environ[i] != NULL; i++) {
            printf("declare -x %s\n", environ[i]);
        }
        return;
    } else if (arg[0] == '=' && arg[1] == ' ') {
        fprintf(stderr, "export: `=': not a valid identifier\n");
        return;  
    } else {
        char* delim = "=";
        char* envVariableName = strtok(arg, delim);
        char* envVariableValue = strtok(NULL, delim);
        
        if (envVariableName == NULL || envVariableValue == NULL) {
            fprintf(stderr, "export: Invalid argument format\n");
            return;
        } else {
        	int result = setenv(envVariableName, envVariableValue, 1);
            if (result != 0) {
                perror("setenv");
            }
        }
    }
}

/**
 * @brief Supprime une variable d'environnement.
 *
 * Cette fonction supprime la variable d'environnement spécifiée par `varName`.
 * Si une erreur se produit lors de la suppression, un message d'erreur est affiché.
 *
 * @param varName Le nom de la variable d'environnement à supprimer.
 *
 * @example
 * builtInUnset("MYVAR"); // Supprime la variable d'environnement MYVAR.
 */
void builtInUnset(const char* varName) {
    if (varName == NULL || varName[0] == '\0') {
        fprintf(stderr, "builtInUnset: Invalid variable name\n");
        return;
    }

    if (unsetenv(varName) != 0) {
        perror("unsetenv");
    }
}

/**
 * @brief Exécute une commande intégrée spécifiée.
 *
 * Cette fonction prend en charge l'exécution des commandes intégrées suivantes :
 * - pwd : Affiche le répertoire de travail courant.
 * - cd : Change le répertoire de travail courant.
 * - echo : Affiche les arguments passés à la commande.
 * - exit : Quitte le programme avec un code de sortie spécifié.
 * - env : Affiche toutes les variables d'environnement.
 * - export : Définit ou affiche des variables d'environnement.
 * - unset : Supprime une variable d'environnement.
 *
 * @param builtIn Un tableau de chaînes de caractères représentant la commande intégrée
 *                et ses éventuels arguments.
 *
 * @note Si `builtIn` est NULL ou si `builtIn[0]` est NULL, un message d'erreur est affiché.
 *       Si la commande intégrée spécifiée n'est pas reconnue, un message d'erreur est également affiché.
 *
 * @example
 * char *args1[] = {"pwd", NULL};
 * execBuiltIn(args1); // Exécute la commande intégrée "pwd".
 *
 * char *args2[] = {"cd", "/home/user", NULL};
 * execBuiltIn(args2); // Exécute la commande intégrée "cd" avec l'argument "/home/user".
 */
void 
execBuiltIn(char** builtIn) {
    if (builtIn == NULL || builtIn[0] == NULL) {
        fprintf(stderr, "Error: Null command provided to exec_built_in.\n");
        return;
    }

    if (strcmp(builtIn[0], "pwd") == 0) {
        builtInPwd();
    } else if (!strcmp(builtIn[0], "cd")) {
        builtInCd(builtIn[1]);
    } else if (!strcmp(builtIn[0], "echo")) {
        builtInEcho(builtIn);
    } else if (!strcmp(builtIn[0], "exit")) {
        builtInExit(builtIn[1]);
    } else if (!strcmp(builtIn[0], "env")) {
    	builtInEnv();
    } else if (!strcmp(builtIn[0], "export")) {
    	builtInExport(builtIn);
    } else if(!strcmp(builtIn[0], "unset")){
    	builtInUnset(builtIn[1]);	
    } else {
        fprintf(stderr, "Error: Unknown built-in command '%s'.\n", builtIn[0]);
    }
}
