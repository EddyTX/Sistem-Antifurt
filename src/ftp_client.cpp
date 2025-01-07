#include "ftp_client.h"

WiFiClient ftpClient;

const char *ftpServer = "192.168.4.2";
const int ftpPort = 21;
const char *ftpUser = "esp_user";
const char *ftpPassword = "esp_pass";


//Initialize FTP connection
void initFTPConnection()
{
    Serial.println("Connecting to FTP server...");
    if(!ftpClient.connect(ftpServer, ftpPort))
    {
        Serial.println("Failed to connect to FTP server.");
        return;
    }
    Serial.println("Connected to FTP server.");

    //Log in to the FTP server
    ftpClient.print("USER ");
    ftpClient.println(ftpUser);
    delay(100);

    ftpClient.print("PASS ");
    ftpClient.println(ftpPassword);
    delay(100);

    if(ftpClient.available())
    {
        Serial.println("FTP login successful.");
    } else {
        Serial.println("FTP login failed.");
    }
}

//Write or overwrite content in a file
void writeToFile(const char *fileName, const char *content)
{
    Serial.println("Opening file on FTP server...");
    ftpClient.print("STOR ");
    ftpClient.println(fileName);
    delay(100);

    if(ftpClient.available())
    {
        ftpClient.println(content); //write content to file
        Serial.println("File written successfully.");
    } else {
        Serial.println("Failed to write to file.");
    }

    ftpClient.print("QUIT");
    delay(100);
    ftpClient.stop();
}