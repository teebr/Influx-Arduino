#include <WiFi101.h>
#include "InfluxArduino.hpp"

InfluxArduino::InfluxArduino()
{
}

InfluxArduino::~InfluxArduino()
{
}

void InfluxArduino::configure(const char database[],const char host[],const uint16_t port)
{   
    //copy these strings to private class pointers for future use

    _database = new char[strlen(database)+1];
    strcpy(_database,database); //strncpy fails for some reason
    _host = new char[strlen(host)+1];
    strcpy(_host,host);
    _port = port;
}

void InfluxArduino::useTLS(const bool tf)
{
    _isSecure = tf;
}

void InfluxArduino::authorize(const char userPassb64[])
{
    //copy these strings to private class pointers for future use

    _authToken = new char[strlen(userPassb64)+1];
    strcpy(_authToken,userPassb64);
    _isAuthorised = true;

}

bool InfluxArduino::write(const char *measurement,const char *fieldString)
{
    write(measurement,"",fieldString);
}

bool InfluxArduino::write(const char *measurement,const char *tagString,const char *fieldString)
{   
    WiFiClient client;

    bool isConnected = false;
    if(_isSecure)
    {
        isConnected = client.connectSSL(_host,_port);
    }
    else
    {
        isConnected = client.connect(_host,_port);
    }

    if(!isConnected)
    {
        //TODO cause of no connection?
        _latestResponse = -1;
        return false;
    }
    
    char writeBuf[512]; // ¯\_(ツ)_/¯ 
    if(strlen(tagString) > 0)
    {
        sprintf(writeBuf,"%s,%s %s",measurement,tagString,fieldString); //no comma between tags and fields
    }

    else
    {
        //no tags
        sprintf(writeBuf,"%s %s",measurement,fieldString); //no comma between tags and fields
    }

    //manually assemble request.
    client.print("POST /write?db=");
    client.print(_database);
    client.println(" HTTP/1.1");

    client.println("Connection: close");
    
    client.print("Host: ");
    client.print(_host);
    client.print(":");
    client.println(_port);

    // client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Type: text/plain");
    client.println("User-Agent: ATWINC1500 with InfluxArduino");
    if(_isAuthorised)
    {
        client.print("Authorization: Basic ");
        client.println(_authToken);
    }
    client.print("Content-Length: ");
    client.println(strlen(writeBuf));

    client.println();
    client.println(writeBuf);

    /*
     * Now, wait until we receive a response from influx.
     */
    unsigned long sendTime = micros(); //to test for timeout
    while (client.available() < 12) //the HTTP code (hopefully 204) is in bytes 10-12
    {
        if(!client.connected())
        {
            _latestResponse = -4; //use same as ESP32 httpclient for timeout
            return false;
        }
        
        if ((micros() - sendTime) > _RESPONSE_TIMEOUT_US)
        {   
            client.stop();
            _latestResponse = -11; //use same as ESP32 httpclient for timeout
            return false;
        }               
    }

    //we've got enough bytes, now parse for response code
    unsigned int readCount = 0;
    int responseCode = -1;
    while (client.available())
    {
        int c = (int)client.read() -48; //-48: from ASCII to number
        readCount++;
        
        //get HTTP return code (hopefully 204) from bytes 10-12. Ugly but avoids using string conversions
        if (readCount == 10)
        {
            responseCode = c * 100;
        }
        else if (readCount == 11)
        {
            responseCode += c * 10;
        }
        else if (readCount == 12)
        {
            responseCode += c;
            break;
        }
    }
     client.stop(); //I see no reason to give a shit about the rest of the response
    _latestResponse = responseCode;
    return responseCode == 204;
}

int InfluxArduino::getResponse()
{
    return _latestResponse;
}

bool InfluxArduino::isSecure()
{
    return _isSecure;
}