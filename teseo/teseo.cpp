module;

#include<algorithm>
#include <cassert>
#include <string>
#include <utility> 
#include <span>

module teseo;

namespace teseo {

nmea_rr teseo::gll_("$PSTMNMEAREQUEST,100000,0\r\n", "GLL,");
nmea_rr teseo::gsv_("$PSTMNMEAREQUEST,80000,0\r\n", "GSV,");
nmea_rr teseo::gsa_("$PSTMNMEAREQUEST,4,0\r\n", "GSA,");
nmea_rr teseo::gga_("$PSTMNMEAREQUEST,2,0\r\n", "GGA,");
nmea_rr teseo::rmc_("$PSTMNMEAREQUEST,40,0\r\n", "RMC,");
nmea_rr teseo::vtg_("$PSTMNMEAREQUEST,10,0\r\n", "VTG,");

/*
when the teseo is preset for i2c according to AN5203,
init is not required, and you can cut 4s 10ms from the startup sequence
https://www.st.com/resource/en/application_note/an5203-teseoliv3f--i2c-positioning-sensor--stmicroelectronics.pdf
*/
 void teseo::initialize() {
    assert(writer_.is_set());
    assert(reader_.is_set());
    assert(resetter_.is_set());

    std::string s;
    resetter_();

    // stop the engine
    write("$PSTMGPSSUSPEND\r\n");

    // reset the UART message list
    write("$PSTMCFGMSGL,0,1,0,0\r\n");
    // reset the I2C message list
    write("$PSTMCFGMSGL,3,1,0,0\r\n");
    // disable the eco-ing message
    write("$PSTMSETPAR,1227,1,2\r\n");

    // restart the engine
    write("$PSTMGPSRESTART\r\n");
    do {
        read(s);            
    }
    while(((s.length()) && s.find("$PSTMGPSRESTART") == std::string::npos)); // command successful
}

bool teseo::parse_multiline_reply(std::span<std::string> strings, const std::string s, unsigned int& count, const nmea_rr& command) {
    std::size_t message_count = strings.size();
    std::size_t string_index = 0;
    std::size_t new_string_index; // intentionally uninitialised
    std::size_t vector_index; // intentionally uninitialised
    bool valid = false;

    for(vector_index = 0; vector_index < message_count; vector_index++) {
        new_string_index = s.find("\r\n", string_index);
        if (new_string_index == s.length() - 2) {  // exhausted. This should be the status string
            valid = s.substr(string_index, s.length() - string_index).starts_with(command.command.substr(0, command.command.length()-2));
            break;
        }
        assert(vector_index < message_count);
        strings[vector_index] = s.substr(string_index, (new_string_index + 2) - string_index); // include the separator
        valid = strings[vector_index].length() >= 7 && strings[vector_index].substr(3, 4).starts_with(command.signature);
        if (!valid) {
            vector_index = 0;
            break;
        }
        string_index = new_string_index + 2; // skip the separator
    }
    count = vector_index; // report the number of retrieved data lines.
    std::for_each(strings.begin() + count, strings.end(),
        [](auto &discard) { discard = std::string(); }); // clean out unused positions
    return valid;
}

void teseo::write(const std::string& s) {
    assert(writer_.is_set());
    writer_(s);
}

void teseo::read(std::string& s) {
    assert(reader_.is_set());
    reader_(s);
}

bool teseo::ask_nmea(const nmea_rr& command, std::string& s) {
    bool retval; // intentionally not initialised
    unsigned int count;
    write(command.command);
    read(s);
    retval = parse_multiline_reply(single_line_parser_, s, count, command);
    s = single_line_parser_[0];    
    return retval;
}

bool teseo::ask_nmea_multiple(const nmea_rr& command, std::span<std::string> strings, unsigned int& count) {
    unsigned int retval; // intentionally not initialised
    std::string s;
    write(command.command);
    read(s);
    retval = parse_multiline_reply(strings, s, count, command);
    return retval;
}

bool teseo::ask_gll(std::string& s) {
    return ask_nmea(gll_, s);
}

bool teseo::ask_gsv(std::span<std::string> strings, unsigned int& count) {
    return ask_nmea_multiple(gsv_, strings, count);
}

bool teseo::ask_gsa(std::span<std::string> strings, unsigned int& count) {
    return ask_nmea_multiple(gsa_, strings, count);
}
bool teseo::ask_gga(std::string& s) {
    return ask_nmea(gga_, s);
}

bool teseo::ask_rmc(std::string& s) {
    return ask_nmea(rmc_, s);
}

bool teseo::ask_vtg(std::string& s) {
    return ask_nmea(vtg_, s);
}

} // namespace teseo