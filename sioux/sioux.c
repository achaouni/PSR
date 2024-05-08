/*
    1- RECEVOIR LES TOP 5 PAQUETS DE LA MEMOIRE PARTAGÉE
    2- HOST SERVER
*/
#include "sioux.h"
// ----------------------------
short int port = 80;
static struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}};
//-----------------------------------------------------------------
void print_syntax()
{
    // Print usage information
    printf("Usage: sioux [OPTIONS]\n");
    printf("Options:\n");
    printf("  -p, --port PORT      Spécifie le numéro de port\n");
    printf("  -h, --help           Affiche l'aide\n");
}
void analyse_arguments(short int *character)
{
    switch (*character)
    {
    case 'p':
        // If the character is 'p', extract the port number from the argument and store it
        port = atoi(optarg); // Convertir l'argument en entier
        break;
    case 'h':
        // If the character is 'h', print the usage information and exit successfully
        print_syntax();
        exit(EXIT_SUCCESS);
    default:
        // For any other character, print the usage information and exit with failure
        print_syntax();
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    int option_index = 0;
    short int c;
    int socket;

    // Parse command-line arguments using getopt_long
    while ((c = getopt_long(argc, argv, "p:h", long_options, &option_index)) != -1)
    {
        // Analyze and process each argument
        analyse_arguments(&c);
    }

    /* Lecture des arguments de la commande */
    /* Initialize the server with the specified port and maximum connections */
    socket = Initialisation_serveur(&port, MAX_CONNEXIONS);
    if (socket < 0)
    {
        fprintf(stderr, "Port non utilisable\n");
        exit(EXIT_FAILURE);
    }
    /* Start the server loop, listening for incoming connections and handling HTTP requests */
    boucle_Serveur(socket, Gestion_HTTP_THREAD);
}
