#include <EtherCard.h>

byte Ethernet::buffer[700]; // configure buffer size to 700 octets
static byte mymac[] = { 0x74,0x42,0x42,0x42,0x42,0x42 }; // define (unique on LAN) hardware (MAC) address
static BufferFiller bfill;  // used as cursor while filling the buffer

int current_state = 0;

void setup(){
  Serial.begin(112500);
  ether.begin(sizeof Ethernet::buffer, mymac);
  const static byte ip[] = {10,0,0,2};
  const static byte gw[] = {192,168,1,193};
  ether.staticSetup(ip, gw);
  ether.hisport = 80;
  pinMode(2,OUTPUT);
}

static void homepage(BufferFiller& buf){
    buf.emit_p(PSTR(
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Pragma: no-cache\r\n" //Disables page caching for most browsers
        "\r\n"
        "<title>Arduino</title>\r\n"
        "<p>Now: $S</p><br>\r\n"
        "<a href='on'>Switch ON</a> / <a href='off'>Switch OFF</a>"), current_state?"ON":"OFF");
        
}
static void onpage(BufferFiller& buf){
    digitalWrite(2,HIGH);
    current_state = 1;
    buf.emit_p(PSTR(
        "HTTP/1.0 302 found\r\n"
        "Location: /\r\n" //Redirect to main page
        "\r\n"));
}
static void offpage(BufferFiller& buf){
    digitalWrite(2,LOW);
    current_state = 0;
    buf.emit_p(PSTR(
        "HTTP/1.0 302 found\r\n"
        "Location: /\r\n" //Redirect to main page
        "\r\n"));
}
void loop(){
  word len = ether.packetReceive(); // are there new packets?
  word pos = ether.packetLoop(len); // deal with them
 
  if (pos) { // there is data for us
        bfill = ether.tcpOffset();
        char* data = (char *) Ethernet::buffer + pos;
        Serial.println(data);
        // receive buf hasn't been clobbered by reply yet
        if (strncmp("GET / ", data, 6) == 0)
            homepage(bfill);
        else if (strncmp("GET /on", data, 7) == 0)
            onpage(bfill);
        else if (strncmp("GET /off", data, 8) == 0)
            offpage(bfill);
        else
            bfill.emit_p(PSTR(
                "HTTP/1.0 404 Page not found\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<h1>404 Page not found</h1>"));  
        ether.httpServerReply(bfill.position()); // send web page data
  }
}
