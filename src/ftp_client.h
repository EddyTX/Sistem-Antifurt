#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include <WiFi.h>
#include <WiFiClient.h>

//FTP Server Credentials
extern const char *ftpServer;
extern const int ftpPort;
extern const char *ftpUser;
extern const char *ftpPassword;

//Initialize FTP connection
void initFTPConnection();

//Upload or modify a file on the server
void writeToFile(const char *fileName, const char *content);

#endif