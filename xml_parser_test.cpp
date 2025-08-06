#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <string>
#include <stdexcept>

#include "xml_parser.h"

// Тесты для функции get_field

// Валидная ситуация
TEST(get_field_test, valid_field) {
    std::string tag = R"(<Tag name="value" type="A" len="10"/>)";
    EXPECT_EQ(get_field(tag, "name"), "value");
    EXPECT_EQ(get_field(tag, "type"), "A");
    EXPECT_EQ(get_field(tag, "len"), "10");
}

// Нет поля
TEST(get_field_test, field_not_found) {
    std::string tag = R"(<Tag name="value"/>)";
    EXPECT_THROW(get_field(tag, "nonexistent"), std::invalid_argument);
}

// Пустое поле
TEST(get_field_test, empty_field_value) {
    std::string tag = R"(<Tag name=""/>)";
    EXPECT_THROW(get_field(tag, "name"), std::invalid_argument);
}

// Нет конца тега
TEST(get_field_test, no_end_of_tag) {
    std::string tag = R"(<Tag name="value)";
    EXPECT_THROW(get_field(tag, "name"), std::invalid_argument);
}

// Тесты для функции hexchar_to_int

// Валидные символы
TEST(hexchar_to_int_test, valid_digits) {
    EXPECT_EQ(hexchar_to_int('0'), 0);
    EXPECT_EQ(hexchar_to_int('5'), 5);
    EXPECT_EQ(hexchar_to_int('9'), 9);
}

TEST(hexchar_to_int_test, valid_lower) {
    EXPECT_EQ(hexchar_to_int('a'), 10);
    EXPECT_EQ(hexchar_to_int('c'), 12);
    EXPECT_EQ(hexchar_to_int('f'), 15);
}

TEST(hexchar_to_int_test, valid_upper) {
    EXPECT_EQ(hexchar_to_int('A'), 10);
    EXPECT_EQ(hexchar_to_int('D'), 13);
    EXPECT_EQ(hexchar_to_int('F'), 15);
}

// Невалидные символы
TEST(hexchar_to_int_test, invalid_chars) {
    EXPECT_THROW(hexchar_to_int('g'), std::invalid_argument);
    EXPECT_THROW(hexchar_to_int('G'), std::invalid_argument);
    EXPECT_THROW(hexchar_to_int('!'), std::invalid_argument);
    EXPECT_THROW(hexchar_to_int(' '), std::invalid_argument);
}

// Тесты для функции hex_to_ascii

// Валидная ситуация
TEST(hex_to_ascii_test, valid_conversion) {
    EXPECT_EQ(hex_to_ascii("48656C6C6F"), "Hello");
    EXPECT_EQ(hex_to_ascii("313233"), "123");
    EXPECT_EQ(hex_to_ascii("41"), "A");
    EXPECT_EQ(hex_to_ascii(""), "");
}

// Нечетная длина
TEST(hex_to_ascii_test, odd_length) {
    EXPECT_THROW(hex_to_ascii("123"), std::invalid_argument);
    EXPECT_THROW(hex_to_ascii("A"), std::invalid_argument);
}

// Невалидные символы
TEST(hex_to_ascii_test, invalid_hex_chars) {
    EXPECT_THROW(hex_to_ascii("XY"), std::invalid_argument);
    EXPECT_THROW(hex_to_ascii("12GH"), std::invalid_argument);
}

// Тесты для функции hex_to_decimal

// Валидная ситуация
TEST(hex_to_decimal_test, valid_conversion) {
    EXPECT_EQ(hex_to_decimal("0"), 0);
    EXPECT_EQ(hex_to_decimal("A"), 10);
    EXPECT_EQ(hex_to_decimal("FF"), 255);
    EXPECT_EQ(hex_to_decimal("100"), 256);
    EXPECT_EQ(hex_to_decimal("FFFF"), 65535);
}

// Невалидная строка
TEST(hex_to_decimal_test, invalid_hex_string) {
    EXPECT_THROW(hex_to_decimal("XYZ"), std::runtime_error);
}

// Тесты для функции parse_xml

// Один тег TcontextCMD
TEST(parse_xml_test, valid_xml_parsing) {
    std::string xml = R"(
<TcontextCMD Data="313233344241">
    <Tcont Name="Field1" Type="A" StorageLen="6"/>
</TcontextCMD>
    )";

    // Перенаправляем cout для проверки вывода
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_NO_THROW(parse_xml(xml));

    std::string output = buffer.str();
    EXPECT_TRUE(output.find("Request:") != std::string::npos);
    EXPECT_TRUE(output.find("Command: 313233344241") != std::string::npos);
    EXPECT_TRUE(output.find("Field1: 1234BA") != std::string::npos);

    std::cout.rdbuf(old);
}

// Несколько тегов TcontextCMD
TEST(parse_xml_test, multiple_commands) {
    std::string xml = R"(
<TcontextCMD Data="3132">
    <Tcont Name="F1" Type="A" StorageLen="1"/>
</TcontextCMD>
<TcontextCMD Data="3334">
    <Tcont Name="F2" Type="A" StorageLen="1"/>
</TcontextCMD>
    )";

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_NO_THROW(parse_xml(xml));

    std::string output = buffer.str();
    EXPECT_TRUE(output.find("Request:") != std::string::npos);
    EXPECT_TRUE(output.find("Response:") != std::string::npos);

    std::cout.rdbuf(old);
}

// Парсинг hex данных
TEST(parse_xml_test, type_h) {
    std::string xml = R"(
<TcontextCMD Data="ABCD">
    <Tcont Name="HexField" Type="H" StorageLen="2"/>
</TcontextCMD>
    )";

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_NO_THROW(parse_xml(xml));

    std::string output = buffer.str();
    EXPECT_TRUE(output.find("HexField: ABCD") != std::string::npos);

    std::cout.rdbuf(old);
}

// Парсинг десятичных данных
TEST(parse_xml_test, type_n) {
    std::string xml = R"(
<TcontextCMD Data="FF">
    <Tcont Name="NumField" Type="N" StorageLen="1"/>
</TcontextCMD>
    )";

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_NO_THROW(parse_xml(xml));

    std::string output = buffer.str();
    EXPECT_TRUE(output.find("NumField: 255") != std::string::npos);

    std::cout.rdbuf(old);
}

// Недостаточное количество байт в Data
TEST(parse_xml_test, not_enough_data) {
    std::string xml = R"(
<TcontextCMD Data="31">
    <Tcont Name="Field" Type="A" StorageLen="2"/>
</TcontextCMD>
    )";

    EXPECT_THROW(parse_xml(xml), std::invalid_argument);
}

// Невалидный StorageLen
TEST(parse_xml_test, invalid_storage_length) {
    std::string xml = R"(
<TcontextCMD Data="3132">
    <Tcont Name="Field" Type="A" StorageLen="0"/>
</TcontextCMD>
    )";

    EXPECT_THROW(parse_xml(xml), std::invalid_argument);
}

// Неизвестный тип данных
TEST(parse_xml_test, unknown_type) {
    std::string xml = R"(
<TcontextCMD Data="3132">
    <Tcont Name="Field" Type="X" StorageLen="1"/>
</TcontextCMD>
    )";

    EXPECT_THROW(parse_xml(xml), std::invalid_argument);
}

// Нет закрывающего тега TcontextCMD
TEST(parse_xml_test, missing_clossing_tag) {
    std::string xml = R"(
<TcontextCMD Data="3132">
    <Tcont Name="Field" Type="A" StorageLen="1"/>
    )";

    EXPECT_THROW(parse_xml(xml), std::invalid_argument);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}