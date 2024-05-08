#include "analyse_http.h"
void sendHttpResponse(int client_socket, const char* response) {
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    // Get the current time.

    // Prepare and send an HTTP response
    char http_response[2048];
    // Create a buffer for the HTTP response.

    // Format the HTTP response, including the content length, server, date, and the actual response
    sprintf(http_response, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nServer: Sioux\r\nDate: %s\r\n\r\n%s",
        strlen(response), asctime(timeinfo), response);

    // Send the HTTP response to the client socket.
    send(client_socket, http_response, strlen(http_response), 0);
}

/*------------- Programmation de la methode pour lire le fichier HTML et traiter la request GET /HTTP/1.1*/
void sendHtmlFileResponse(int client_socket, const char* filename) {
    // Attempt to open the specified HTML file for reading.
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        perror("Failed to open HTML file");
        // If the file couldn't be opened, send a "Not Found" response to the client.
        const char* notFoundResponse = "<html><body><h1>404 Not Found</h1></body></html>";
        sendHttpResponse(client_socket, notFoundResponse);
        return;
    }

    // Determine the size of the file.
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Allocate memory to store the file content.
    char* buffer = (char*)malloc(file_size);

    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        // If memory allocation failed, send a "Internal Server Error" response to the client.
        const char* internalErrorResponse = "<html><body><h1>500 Internal Server Error</h1></body></html>";
        sendHttpResponse(client_socket, internalErrorResponse);
        return;
    }

    // Read the file content into the buffer.
    size_t read_size = fread(buffer, 1, file_size, file);
    fclose(file);

    if (read_size != file_size) {
        perror("Failed to read HTML file");
        free(buffer);
        // If the file couldn't be read completely, send a "Internal Server Error" response to the client.
        const char* internalErrorResponse = "<html><body><h1>500 Internal Server Error</h1></body></html>";
        sendHttpResponse(client_socket, internalErrorResponse);
        return;
    }

    // Send the HTML content as an HTTP response to the client.
    sendHttpResponse(client_socket, buffer);
    free(buffer);
}

/*------------- Programmation de la methode pour lire le fichier IMages et traiter la request GET /Images HTTP/1.1*/
void SendImageReponse(int client_socket, const char* filename){
    // Get the current time in seconds since the epoch
    time_t now;
    struct tm *timeinfo;
    time(&now);
    // Convert the time to a structured format for date and time information
    timeinfo = gmtime(&now);

    // Create a buffer to store the HTTP response headers and the current date
    char ReponseHTTP[1024];
    char timeBuffer[80];

    // Format the current time as a string in GMT format
    strftime(timeBuffer, sizeof(timeBuffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);

    // Construct the HTTP response, including headers, using snprintf
    snprintf(ReponseHTTP, 1024,"HTTP/1.1 200 OK\r\n"
        "Content-type: image/apng\r\n"
        "Server: Sioux\r\n"
        "Date: %s\r\n"
        "\r\n", timeBuffer
    );

    // Attempt to open the image file for reading in binary mode
    FILE *imageFile = fopen(filename, "rb");

    // Send the HTTP response headers to the client
    send(client_socket, ReponseHTTP, strlen(ReponseHTTP),0);
    if (imageFile) {
        char buffer[1024];
        size_t bytesRead;

        // Read and send the image data in chunks
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), imageFile)) > 0) {write(client_socket, buffer, bytesRead);}

        // Close the image file after reading
        fclose(imageFile);
    }else {
        // Image file not found. Send a 404 Not Found response.
        const char *notFoundResponse = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
        write(client_socket, notFoundResponse, strlen(notFoundResponse));
    } 
}

/*------------- programmation de la méthode pour traiter les diffirent request.-----------------------------------*/
int GesDeSocket_HTTP(int client_socket) {
    // Store the HTTP method (e.g., GET)
    char method[16]; 
    // Store the requested URL
    char url[MAX_LIGNE]; 
    // Store the HTTP version (e.g., HTTP/1.1)
    char version[16]; 

    // Read the HTTP request line from the client
    if(fscanf(fdopen(client_socket, "r"), "%15s %1023s %15s", method, url, version) != 3){
        perror("Erreur lors de la lecture de la requête");
        close(client_socket);
        return 1;
    }

    // Check if it's a GET request for the root path
    if (strcmp(method, "GET") == 0 && strcmp(url, "/") == 0 && strcmp(version, "HTTP/1.1") == 0) {
        // Send an HTML file as the response.
        sendHtmlFileResponse(client_socket, "../www/html/vote.html");
    }else if (strncmp(url, "/Imgs/Chat3.jpeg", 16) == 0){
        // Handle requests for Chat3 image.
        SendImageReponse(client_socket,"../www/Imgs/Chat3.jpeg");
    }else if (strncmp(url, "/Imgs/Chat1.jpeg", 16) == 0){
        // Handle requests for Chat1 image
        SendImageReponse(client_socket,"../www/Imgs/Chat1.jpeg");
    }else if (strncmp(url, "/Imgs/Chat2.jpeg", 16) == 0){
        // Handle requests for Chat2 image
        SendImageReponse(client_socket,"../www/Imgs/Chat2.jpeg");
        // Send an HTML file as the response.
    }else if (strncmp(url, "/Ablette.html", 13) == 0){
        sendHtmlFileResponse(client_socket,"Ablette.html");

    }else if (strncmp(url, "/vote-reponse.html?reponse=1", 36) == 0){
        // Handle requests for voting response 1 and log it to a CSV file
        FILE *csvFile = fopen("Reponses.csv", "a");
        if (csvFile) { 
            fprintf(csvFile, "%s\n", "Reponse = 1");
        }
        fclose(csvFile);
        // Reload the root html page.
        sendHtmlFileResponse(client_socket, "../www/html/vote.html");
    }else if(strncmp(url, "/vote-reponse.html?reponse=2", 36) == 0){
        // Handle requests for voting response 2 and log it to a CSV file
        FILE *csvFile = fopen("Reponses.csv", "a");
        if (csvFile) { 
            fprintf(csvFile, "%s\n", "Reponse = 2");
        }
        fclose(csvFile);
        // Reload the root html page.
        sendHtmlFileResponse(client_socket, "../www/html/vote.html");
    }else if(strncmp(url, "/vote-reponse.html?reponse=3", 36) == 0){
         // Handle requests for voting response 3 and log it to a CSV file
        FILE *csvFile = fopen("Reponses.csv", "a");
        if (csvFile) { 
            fprintf(csvFile, "%s\n", "Reponse = 3");
        }
        fclose(csvFile);
        // Reload the root html page.
        sendHtmlFileResponse(client_socket, "../www/html/vote.html");
    }else if(strncmp(url, "/css/styles.css", 10) == 0){
        // Handle requests for CSS styles file.
        sendHtmlFileResponse(client_socket,"../www/css/styles.css");
    }else{
        // Handle other types of requests or send a 404 Not Found response.
        const char *notFoundResponse = "HTTP/1.1 403 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
        write(client_socket, notFoundResponse, strlen(notFoundResponse));
    }
    // Close the client socket
    close(client_socket);
    return 0;
}