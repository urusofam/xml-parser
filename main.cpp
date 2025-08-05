#include <iostream>
#include <string>
#include <stdexcept>

// Функция извлечения значения поля из строки тега
std::string get_field(const std::string& tag_content, const std::string& field_name) {
    // Ищем начало
    std::string search_str = field_name + "=\"";
    size_t start_pos = tag_content.find(search_str);
    if (start_pos == std::string::npos) {
        throw std::invalid_argument("Поле " + field_name + " не найдено");
    }

    // Ищем конец
    start_pos += search_str.length();
    size_t end_pos = tag_content.find('"', start_pos);
    if (end_pos == std::string::npos) {
        throw std::invalid_argument("У поля " + field_name + " не хватает закрывающей кавычки");
    }

    // Извлекаем значение и проверяем на пустоту
    std::string field_content = tag_content.substr(start_pos, end_pos - start_pos);
    if (field_content.empty()) {
        throw std::invalid_argument("Поле " + field_name + " пустое");
    }
    return field_content;
}

// Функция конвертации hexchar в int
unsigned char hexchar_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;

    throw std::invalid_argument(std::string("Некорректный hex-символ: ") + c);
}

// Функция конвертации hex в ascii строку
std::string hex_to_ascii(const std::string& hex) {
    std::string ascii;
    if (hex.length() % 2 != 0) {
        throw std::invalid_argument("Нечетная длина hex-строки: " + hex);
    }

    for (size_t i = 0; i < hex.length(); i += 2) {
        unsigned char byte = hexchar_to_int(hex[i]) * 16 + hexchar_to_int(hex[i + 1]);
        ascii += byte;
    }
    return ascii;
}

// Функция конвертации hex в десятичное
long long hex_to_decimal(const std::string& hex) {
    try {
        long long dec = std::stoll(hex, nullptr, 16);
        return dec;
    } catch (std::exception& e) {
        throw std::runtime_error("Не удалось перевести " + hex + " в десятичное: " + e.what());
    }
}

// Основная функция парсинга XML
void parse_xml(const std::string& xml_data) {
    int command_counter = 0;
    size_t search_pos = 0;

    // Итерация по всем тегам TcontextCMD
    while ((search_pos = xml_data.find("<TcontextCMD", search_pos)) != std::string::npos) {
        // Находим конец открывающего тега TcontextCMD
        size_t tag_end_pos = xml_data.find('>', search_pos);
        if (tag_end_pos == std::string::npos) {
            throw std::invalid_argument("Не найден конец открывающего тега TcontextCMD");
        }

        // Находим закрывающий тег </TcontextCMD>
        size_t block_end_pos = xml_data.find("</TcontextCMD>", tag_end_pos);
        if (block_end_pos == std::string::npos) {
            throw std::invalid_argument("Не найден закрывающийся тег </TcontextCMD>");
        }

        // Извлекаем содержимое всего тега TcontextCMD
        std::string cmd_tag_content = xml_data.substr(search_pos, tag_end_pos - search_pos + 1);
        // Извлекаем содержимое блока между <TcontextCMD> и </TcontextCMD>
        std::string cmd_block_content = xml_data.substr(tag_end_pos + 1, block_end_pos - tag_end_pos - 1);

        // Ответ или запрос
        command_counter++;
        std::cout << (command_counter % 2 != 0 ? "Request:" : "Response:") << std::endl;

        // Получаем полную команду из поля Data
        std::string full_command_hex = get_field(cmd_tag_content, "Data");
        std::cout << "Command: " << full_command_hex << std::endl;

        // Смещение для чтения из строки Data
        size_t data_now = 0;
        size_t cont_search_pos = 0;

        // Итерация по вложенным тегам Tcont внутри блока
        while ((cont_search_pos = cmd_block_content.find("<Tcont", cont_search_pos)) != std::string::npos) {

            // Находим конец открывающего тега Tcont
            size_t cont_tag_end_pos = cmd_block_content.find('>', cont_search_pos);
            if (cont_tag_end_pos == std::string::npos) {
                throw std::invalid_argument("Не найден конец открывающего тега Tcont");
            }

            // Получаем Name, Type и StorageLen
            std::string cont_tag_content = cmd_block_content.substr(cont_search_pos, cont_tag_end_pos - cont_search_pos + 1);
            std::string name = get_field(cont_tag_content, "Name");
            std::string type = get_field(cont_tag_content, "Type");
            int storage_len = std::stoi(get_field(cont_tag_content, "StorageLen"));
            if (storage_len <= 0) {
                throw std::invalid_argument("StorageLen <= 0");
            }

            // Проверяем на переполнение
            size_t chars_to_read = storage_len * 2;
            if (data_now + chars_to_read > full_command_hex.length()) {
                throw std::invalid_argument("Недостаточно данных в Data для поля " + name);
            }

            // Получаем hex для нашего Tcont и смещаем указатель
            std::string field_hex = full_command_hex.substr(data_now, chars_to_read);
            data_now += chars_to_read;

            std::cout << name << ": ";

            // Разбираем тип данных
            if (type == "A") {
                std::cout << hex_to_ascii(field_hex);
            } else if (type == "H" || type == "B") {
                std::cout << field_hex;
            } else if (type == "N") {
                std::cout << hex_to_decimal(field_hex);
            } else {
                throw std::invalid_argument("Неизвестный тип: " + type);
            }
            std::cout << std::endl;

            // Переходим к следующему Tcont
            cont_search_pos = cont_tag_end_pos + 1;
        }

        // Переходим к следующему TcontextCMD
        search_pos = block_end_pos + 1;
        std::cout << std::endl;
    }
}

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
