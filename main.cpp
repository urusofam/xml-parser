#include <iostream>
#include <string>
#include <stdexcept>

#include "xml_parser.h"

int main() {
    std::string xml_data = R"(
<TcontextCMD Data="3132333442413031323334353637383930313246333234333835343438373033">
    <Tcont Name="MsgHdr" Type="A" StorageLen="4"/>
    <Tcont Name="Cmd" Type="A" StorageLen="2"/>
    <Tcont Name="PIN" Type="A" StorageLen="14"/>
    <Tcont Name="PAN" Type="A" StorageLen="12"/>
</TcontextCMD>
<TcontextCMD Data="31323334424230303038363333313732353331323434">
    <Tcont Name="MsgHdr" Type="A" StorageLen="4"/>
    <Tcont Name="Cmd" Type="A" StorageLen="2"/>
    <Tcont Name="ErrCode" Type="A" StorageLen="2"/>
    <Tcont Name="PIN_ENCR" Type="A" StorageLen="14"/>
</TcontextCMD>
    )";

    try {
        parse_xml(xml_data);
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
