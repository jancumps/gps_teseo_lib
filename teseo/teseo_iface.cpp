module;

#include <string>
// std::pair
#include <utility> 
#include <span>

export module teseo;

import callbackmanager;

export namespace teseo {

//! Struct holds combinations of NMEA commands and their reply signature validation string
struct nmea_rr {
  const std::string command;
  const std::string signature;
  nmea_rr(const std::string& command, const std::string& signature) : 
    command(command), signature(signature) {}
};

//! Driver class for ST Teseo IC.
/*!
  Understands the Teseo command set and replies. 
  For the communication, it relies on I2C or UART functions that the user has to provide.  
  Callbacks are required for:  
  - writing comms protocol  
  - reading comms protocol  
  - resetting the Teseo (optional, see init() documentation)  
 */
class teseo {
public:

    //! constructor.
    teseo() : single_line_parser_() {}

    //! expose the callback manager for writing to Teseo.
    /*!
      The developer has to register the logic for writing to the device.  
      Callback parameter: const std::string reference with data to be written to Teseo.  
      This can be a C style function, an object method, a static class object method, or lambda code.  

      Example code:
      @code
      // device specific I2C writer function. Classic C style.
      void write(const std::string& s) {
        i2c_write_blocking(i2c_default, I2C_ADDR, reinterpret_cast<const uint8_t*>(s.c_str()), s.length() +1, false);
        return;
      }

      teseo::teseo gps;
      // register that write() function as the handler for writing to the Teseo.
      gps.writer().set([](const std::string& s) -> void {
        write(s);
      });
      @endcode
    */
    inline callbackmanager::Callback<void, const std::string&>& writer() {
        return writer_;
    }

    //! expose the callback manager for reading from Teseo
    /*!
      The developer has to register the logic for reading from the device.  
      Callback parameter: std::string reference where the data returned by the Teseo will be stored.  
      For instructions on how to register your handler, check the documentation of writer().
    */
    inline callbackmanager::Callback<void, std::string&>& reader() {
        return reader_;
    }

    //! expose the callback manager for resetting the Teseo
    /*!
      The developer has to register the logic that resets the device. It is optional. See the init() documentation.  
      The handler has to pull reset low then high. Then it needs to wait 4 seconds to allow the Teseo to boot.  
      Callback parameter: none.  
      For instructions on how to register your handler, check the documentation of writer().
    */
    inline callbackmanager::Callback<void>& resetter() {
        return resetter_;
    }

    //! configure the Teseo for use as a position sensor (optional).
    /*!
    init() is used for dynamic configuration of the Teseo.  
    Precondition (asserted): the handlers have to be set by the developer before calling init().  
    Optional. When the Teseo is preset for i2c according to AN5203,
    init is not required, and developer can cut the 4s 10ms from the startup sequence,
    that are consumed during the reset sequence.  
    https://www.st.com/resource/en/application_note/an5203-teseoliv3f--i2c-positioning-sensor--stmicroelectronics.pdf  
    In that case, the developer doesn't need to provide a resetter callback handler.
    */
    void initialize();

    //! utility to parse a multiline Teseo reply into separate strings
    /*!
      \param strings std::span<std::string> will get the individual strings.  
      \param s constant std::string reference string to be parsed.  
      \param count unsigned int reference gets count of strings parsed.  
      \param command nmea_rr const reference used to validate the status line.  
      \returns  bool true if valid reply 

      split a big Teseo reply in its individual strings. The separator is "\r\n"
    */
    static bool parse_multiline_reply(std::span<std::string> strings, const std::string s, unsigned int& count, const nmea_rr& command);

    //! write command to the Teseo
    /*!
      \param s constant std::string reference.  

      Write command to the Teseo by invoking the provided callback handler.  
      Precondition (asserted): the handler has to be set by the developer before first use.     
    */
    void write(const std::string& s);
    
    //! read data from the Teseo
    /*!
      \param s std::string reference. 

      Read replies from the Teseo by invoking the provided callback handler.  
      Precondition (asserted): the handler has to be set by the developer before first use.     
    */    
    void read(std::string& s);

    //! send NMEA request to the Teseo and return reply
    /*!
      \param command const nmea_rr reference holds the NMEA command.   
      \param s std::string reference gets the reply.  
      \returns bool true if valid reply

      Send NMEA request to the Teseo. Validate and Return the repy.
    */    
    bool ask_nmea(const nmea_rr& command, std::string& s);

    //! send NMEA request to the Teseo and return multi line reply
    /*!
      \param command const nmea_rr reference holds the NMEA command.   
      \param strings astd::span<std::string> gets the replies. 
      \param count unsigned int reference count of strings parsed.  
      \returns  bool true if valid reply 

      Send NMEA request that expects more than 1 reply to the Teseo. Validate and Return the repies.
    */    
    bool ask_nmea_multiple(const nmea_rr& command, std::span<std::string> strings, unsigned int& count);

    //! get GLL request to the Teseo and read reply
    /*!
      \param s std::string reference gets the reply.  
      \returns bool true if valid reply  

      Send request for GLL data to the Teseo. Retrieve the repy.
    */    
    bool ask_gll(std::string& s);

    //! get GSV request to the Teseo and read reply
    /*!
      \param strings std::span<std::string> gets the reply. 
      \param count unsigned int reference gets count of replies. 
      \returns boold true if validated.

      Send request for GSV data to the Teseo. Retrieve the replies.
    */    
    bool ask_gsv(std::span<std::string> strings, unsigned int& count);

    //! get GSA request to the Teseo and read reply
    /*!
      \param strings std::span<std::string> gets the reply. 
      \param count unsigned int reference gets count of replies. 
      \returns boold true if validated.

      Send request for GSA data to the Teseo. Retrieve the replies.
    */    
    bool ask_gsa(std::span<std::string> strings, unsigned int& count);

    //! get RMC request to the Teseo and read reply
    /*!
      \param s std::string reference gets the reply.  
      \returns bool true if valid reply  

      Send request for RMC data to the Teseo. Retrieve the repy.
    */    
    bool ask_rmc(std::string& s);

    //! get GGA request to the Teseo and read reply
    /*!
      \param s std::string reference gets the reply.  
      \returns bool true if valid reply  

      Send request for GGA data to the Teseo. Retrieve the repy.
    */    
    bool ask_gga(std::string& s);

    //! get VTG request to the Teseo and read reply
    /*!
      \param s std::string reference gets the reply.  
      \returns bool true if valid reply  

      Send request for VTG data to the Teseo. Retrieve the repy.
    */    
    bool ask_vtg(std::string& s);

private:

    //! command to retrieve GLL data
    static nmea_rr gll_;
    //! command to retrieve GSV data
    static nmea_rr gsv_;
    //! command to retrieve GSA data
    static nmea_rr gsa_;
    //! command to retrieve GGA data
    static nmea_rr gga_;
    //! command to retrieve RMC data
    static nmea_rr rmc_;
    //! command to retrieve VTG data
    static nmea_rr vtg_;
    //! callback manager for writing to the Teseo
    callbackmanager::Callback<void, const std::string&> writer_;
    //! callback manager for reading from the Teseo
    callbackmanager::Callback<void, std::string&> reader_;
    //! callback manager for resetting the Teseo
    callbackmanager::Callback<void> resetter_;
    //! every single line NMEA command has two lines. reply and status
    std::array<std::string,2> single_line_parser_;

};

} // namespace teseo

