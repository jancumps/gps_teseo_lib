#include "teseo.h"
namespace teseo {

nmea_rr teseo::gll("$PSTMNMEAREQUEST,100000,0\r\n", "GLL,");
nmea_rr teseo::gsv("$PSTMNMEAREQUEST,80000,0\r\n", "GSV,");
nmea_rr teseo::gsa("$PSTMNMEAREQUEST,4,0\r\n", "GSA,");
nmea_rr teseo::gga("$PSTMNMEAREQUEST,2,0\r\n", "GGA,");
nmea_rr teseo::rmc("$PSTMNMEAREQUEST,40,0\r\n", "RMC,");
nmea_rr teseo::vtg("$PSTMNMEAREQUEST,10,0\r\n", "VTG,");

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
    write("$PSTMGPSSUSPEND\r\n");
    // do {
    //     read(s);            
    // }
    // while((s.find("$PSTMGPSSUSPENDED*") == std::string::npos)); // command successful

    // reset the UART message list
    write("$PSTMCFGMSGL,0,1,0,0\r\n");
    // do {
    //     read(s);            
    // }
    // while((s.find("$PSTMCFGMSGLOK*") == std::string::npos)); // command successful

    // reset the I2C message list
    write("$PSTMCFGMSGL,3,1,0,0\r\n");
    // do {
    //     read(s);            
    // }
    // while((s.find("$PSTMCFGMSGLOK*") == std::string::npos)); // command successful

    // disable the eco-ing message
    write("$PSTMSETPAR,1227,1,2\r\n");
    // do {
    //     read(s);            
    // }
    // while((s.find("$PSTMSETPAROK") == std::string::npos)); // command successful 

    write("$PSTMGPSRESTART\r\n");
    do {
        read(s);            
    }
    // TODO validate if I2C is OK with checking for empty
    while(((s.length()) && s.find("$PSTMGPSRESTART") == std::string::npos)); // command successful
}

bool teseo::parse_multiline_reply(std::span<std::string> strings, const std::string s, unsigned int& count, const nmea_rr& command) {
        std::size_t maxelements = strings.size(); // at this moment, don't support growing the array (embedded)
        std::size_t string_index = 0;
        std::size_t new_string_index; // intentionally uninitialised
        std::size_t vector_index; // intentionally uninitialised
        bool valid = false;

        // TODO: current implementation will reply false if there are more answers than strings.size()
        // it stores all valid replies up to that point. the remaining ones are discarded.
        // In the future, I may add a parameter with the max count (default = 0: use strings.size()) 
        // and rely on the user to provide a container that's big enough for that max count (can assert that)
    
        for(vector_index = 0; vector_index < maxelements; vector_index++) {
            new_string_index = s.find("\r\n", string_index);
            if (new_string_index == s.length() - 2) {// exhausted. This should be the status string
#ifdef __GNUC__ // this requires a recent version of GCC.
#if __GNUC_PREREQ(10,0)
                valid = s.substr(string_index, s.length() - string_index).starts_with(command.first.substr(0, command.first.length()-2));
#else
                valid = (s.substr(string_index, s.length() - string_index).find((command.first.substr(0, command.first.length()-2)))) != std::string::npos;
#endif
#endif
                break;
            }
            assert(vector_index < maxelements);
            strings[vector_index] = s.substr(string_index, (new_string_index + 2) - string_index); // include the separator
            valid = strings[vector_index].length() >= 7 && strings[vector_index].substr(3, 4).starts_with(command.second);
            if (!valid) {
                vector_index = 0;
                break;
            }
            string_index = new_string_index + 2; // skip the separator
        }
        count = vector_index; // report the number of retrieved data lines.
        std::for_each(strings.begin() + count, strings.end(), [](auto &discard) { 
            discard = std::string(); });
        return valid;
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

bool teseo::ask_nmea_multiple(const nmea_rr& command, std::span<std::string> strings, unsigned int& count) {
    unsigned int retval; // intentionally not initialised
    std::string s;
    write(command.first);
    read(s);
    retval = parse_multiline_reply(strings, s, count, command);
    return retval;
}

bool teseo::ask_gll(std::string& s) {
    return ask_nmea(gll, s);
}

bool teseo::ask_gsv(std::span<std::string> strings, unsigned int& count) {
    return ask_nmea_multiple(gsv, strings, count);
}

bool teseo::ask_gsa(std::span<std::string> strings, unsigned int& count) {
    return ask_nmea_multiple(gsa, strings, count);
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