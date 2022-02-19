const uint8_t THERMOSTAT=4;
const uint8_t BLOWER    =7;
#ifndef DEBUG
    #define DEBUG 1
#endif

/******************************************************************************/

class Device {
    protected:
        // time[0] is latest, [1] is previous
        unsigned long _time[2];
        bool _val;
        
    public:
        Device(){
            _time[0]=0;
            _time[1]=0;
            _val=0;
        }
        
        void set(bool newval, bool force=false){
            if (newval!=_val || force){
                _time[1]=_time[0];
                _time[0]=millis();
                _val=newval;
            }
        }
        
        bool val(){
            return _val;
        }
        
        unsigned long time(){
            return _time[0];
        }
        
        unsigned long duration(){
            return _time[0]-_time[1];
        }
};

class Thermostat : public Device {
    private:
        const static uint8_t _pin=THERMOSTAT;
        const static unsigned long _timeout=50000;
    
    public:
        Thermostat(){
            pinMode(_pin, INPUT_PULLUP);
            Device();
        }
        
        void poll(){
            auto newval=pulseIn(_pin, HIGH, _timeout) > 0 ? false : true;
            set(newval);
        }
        
        #if DEBUG
            void set(bool newval, bool force=false){
                if (newval!=_val || force){
                    Serial.print(F("THERMOSTAT: "));
                    Serial.println(newval);
                }
                Device::set(newval, force);
            }
        #endif
};

class Blower : public Device {
    private:
        const static uint8_t _pin=BLOWER;
    
    public:
        Blower(){
            pinMode(_pin, OUTPUT);
            Device();
        }
        
        void set(bool newval, bool force=false){
            #if DEBUG
                if (newval!=_val || force){
                    Serial.print(F("BLOWER: "));
                    Serial.println(newval);
                }
            #endif
            digitalWrite(_pin, newval);
            Device::set(newval, force);
        }
};

/******************************************************************************/

static Thermostat thermostat;
static Blower blower;

void setup(){
    #if DEBUG
        Serial.begin(9600);
        Serial.println(F("Ready"));
    #endif
    
    // Turn on blower if thermostat is turned on in case of power glitch
    thermostat.poll();
    blower.set(thermostat.val(), true);
}

void loop(){
    /* Update thermostat status */
    thermostat.poll();
    
    /* Turn on blower 45s after heater turned on
     * Turn off blower # after heater turned off, where # is:
     * 1) 240s if heater was on for >=600s
     * 2) 120s if heater was on for <600s
     */
    auto now=millis();
    if ((thermostat.val() && now-thermostat.time() > 45000) || (!thermostat.val() && now-thermostat.time() > (thermostat.duration() >= 600000 ? 240000 : 120000))){
        blower.set(thermostat.val());
    }
}
