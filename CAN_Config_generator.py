import cantools
import sys
from typing import List
import json

def parse_dbc(filename: str):
    with open(filename, "r") as f:
        db = cantools.database.load(f)
    return db

def generate_header(db: cantools.database.can.database.Database):
    header =  """#ifndef __CAN_CONFIG__
    #define __CAN_CONFIG__
    #include \"WFR_Lib/CAN.hpp\"
    #include \"etl/map.h\"
    #include \"etl/set.h\"
    """
    
    for message in db.messages:
        message_name = message.name
        message_id = message.frame_id
        
        header += f"class {message_name} : public CAN::Message\n{{\n"
        header += f"private:\n// Singleton device class structure\nstatic {message_name} *instancePtr;\nstatic SemaphoreHandle_t mutex;\n{message_name}();"
        header += f"public:\n// Deleting the copy constructor and copy reference constructor to prevent copies\nM192_Command_Message(const M192_Command_Message &) = delete;\n"
        header += f"M192_Command_Message &operator=(const M192_Command_Message &) = delete;\n"
        header += f"M192_Command_Message(M192_Command_Message &&) = delete;\n"
        header += f"M192_Command_Message &operator=(M192_Command_Message &&) = delete;\n"
        header += f"static M192_Command_Message *Get();\n"
        header += f"// overridden functions\n"
        header += f"MESSAGE_ERROR parse(uint8_t buffer[]) override;\n"
        header += f"MESSAGE_ERROR serialize(uint8_t buffer[]) override;\n"
        header += f"\n//signals:\n"
        header += f"#pragma pack(1)"
        header += f"struct\n{{"
        # populate the struct
        for signal in message.signals:
            assert signal.byte_order == "little_endian", f"Error in {message_name}, big endian signals not support"
            bit_length = signal.length
            signed = signal.is_signed
            
            assert bit_length <= 64 and bit_length > 0, f"Error in {message_name}, how tf do you have a signal with length greater than 64 or less than 0"
            
            struct_type = ""
            if signed:
                struct_type += "int"
            else:
                struct_type += "uint"
            if bit_length <= 8:
                struct_type += "8"
            elif bit_length <= 16:
                struct_type += "16"
            elif bit_length <= 32:
                struct_type += "32"
            elif bit_length <= 64:
                struct_type += "64"
            
            
    
    
    
    return header

def generate_source(db: cantools.database.can.database.Database):
    source = """"""
    return source

def validate_config(config: dict):
    print(f"config:{config}")
    assert "Rx_messages" in config, "Config JSON Missing Rx_messages list"
    assert "Tx_10ms" in config, "Config JSON Missing Tx_10ms list"
    assert "Tx_100ms" in config, "Config JSON Missing Tx_100ms list"
    assert "Tx_1000ms" in config, "Config JSON Missing Tx_1000ms list"
    pass


def main(dbc_file: str, config_json: str):
    db = parse_dbc(dbc_file)
    with open(config_json) as config_file:
        config = json.load(config_file)
    
    validate_config(config)
    
    
    header = generate_header(db)
    with open(r"include\WFR_Lib\CAN_Config.hpp", "w+") as f:
        f.write(header)
    # source = generate_source(db)
    # with open(r"src\CAN_Config.cpp", "w") as f:
    #     f.write(source)
    
    
    print(f"Generated files!")



if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py input.dbc can_config.json")
        sys.exit(1)
    
    main(sys.argv[1], sys.argv[2])