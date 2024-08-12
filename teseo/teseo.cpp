#include "teseo.h"
namespace teseo {

nmea_rr teseo::gll("$PSTMNMEAREQUEST,100000,0\n\r", "GLL,");
nmea_rr teseo::gsv("$PSTMNMEAREQUEST,80000,0\n\r", "GSV,");
nmea_rr teseo::gsa("$PSTMNMEAREQUEST,4,0\n\r", "GSA,");
nmea_rr teseo::gga("$PSTMNMEAREQUEST,2,0\n\r", "GGA,");
nmea_rr teseo::rmc("$PSTMNMEAREQUEST,40,0\n\r", "RMC,");
nmea_rr teseo::vtg("$PSTMNMEAREQUEST,10,0\n\r", "VTG,");

/*
when the teseo is preset for i2c according to AN5203,
init is not required, and you can cut 4s 10ms from the startup sequence
https://www.st.com/resource/en/application_note/an5203-teseoliv3f--i2c-positioning-sensor--stmicroelectronics.pdf
*/
 void teseo::init() {
    assert(writer.armed());
    assert(reader.armed());
    assert(resetter.armed());

    std::string s;

    resetter.call();

    // stop the engine
    write("$PSTMGPSSUSPEND\n\r");
    // do {
    //     read(s);            
    // }
    // while((s.find("$PSTMGPSSUSPENDED*") == std::string::npos)); // command successful

    // reset the UART message list
    write("$PSTMCFGMSGL,0,1,0,0\n\r");
    // do {
    //     read(s);            
    // }
    // while((s.find("$PSTMCFGMSGLOK*") == std::string::npos)); // command successful

    // reset the I2C message list
    write("$PSTMCFGMSGL,3,1,0,0\n\r");
    // do {
    //     read(s);            
    // }
    // while((s.find("$PSTMCFGMSGLOK*") == std::string::npos)); // command successful

    // disable the eco-ing message
    write("$PSTMSETPAR,1227,1,2\n\r");
    // do {
    //     read(s);            
    // }
    // while((s.find("$PSTMSETPAROK") == std::string::npos)); // command successful 

    write("$PSTMGPSRESTART\n\r");
    do {
        read(s);            
    }
    // TODO validate if I2C is OK with checking for empty
    while(((s.length()) && s.find("$PSTMGPSRESTART") == std::string::npos)); // command successful
}

void teseo::write(const std::string& s) {
    assert(writer.armed());
    writer.call(s);
}

void teseo::read(std::string& s) {
    assert(reader.armed());
    reader.call(s);
}

bool teseo::ask_nmea(const nmea_rr& command, std::string& s) {
    bool retval; // intentionally not initialised
    unsigned int count;
    write(command.first);
    read(s);
    retval = parse_multiline_reply(single_line_parser, s, count, command);
    s = single_line_parser[0];    
    return retval;
}

bool teseo::ask_gll(std::string& s) {
    return ask_nmea(gll, s);
}

bool teseo::ask_gga(std::string& s) {
    return ask_nmea(gga, s);
}

bool teseo::ask_rmc(std::string& s) {
    return ask_nmea(rmc, s);
}

bool teseo::ask_vtg(std::string& s) {
    return ask_nmea(vtg, s);
}

} // namespace teseo