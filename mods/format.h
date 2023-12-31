/**
 * @file format.h
 * @author Krisna Pranav
 * @brief format
 * @version 6.0
 * @date 2023-07-01
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once

#include "array.h"
#include "genericlexer.h"
#include "string_view.h"

#ifndef KERNEL
#    include <stdio.h>
#endif

namespace Mods {

    class TypeErasedFormatParams;
    class FormatParser;
    class FormatBuilder;

    template<typename T, typename = void>
    struct Formatter {
        using __no_formatter_defined = void;
    };

    constexpr size_t max_format_arguments = 256;

    struct TypeErasedParameter {
        enum class Type {
            UInt8,
            UInt16,
            UInt32,
            UInt64,
            Int8,
            Int16,
            Int32,
            Int64,
            Custom
        };

        /**
         * @tparam T 
         * @return Type 
         */
        template<typename T>
        static Type get_type() {
            if (IsSame<T, u8>::value)
                return Type::UInt8;
            if (IsSame<T, u16>::value)
                return Type::UInt16;
            if (IsSame<T, u32>::value)
                return Type::UInt32;
            if (IsSame<T, u64>::value)
                return Type::UInt64;
            if (IsSame<T, i8>::value)
                return Type::Int8;
            if (IsSame<T, i16>::value)
                return Type::Int16;
            if (IsSame<T, i32>::value)
                return Type::Int32;
            if (IsSame<T, i64>::value)
                return Type::Int64;

            return Type::Custom;
        }

        const void* value;
        Type type;
        void (*formatter)(TypeErasedFormatParams&, FormatBuilder&, FormatParser&, const void* value);
    };

    class FormatParser : public GenericLexer {
    public:
        struct FormatSpecifier {
            StringView flags;
            size_t index;
        };

        /**
         * @param input 
         */
        explicit FormatParser(StringView input);

        /**
         * @return StringView 
         */
        StringView consume_literal();

        /**
         * @param value 
         * @return true 
         * @return false 
         */
        bool consume_number(size_t& value);

        /**
         * @param specifier 
         * @return true 
         * @return false 
         */
        bool consume_specifier(FormatSpecifier& specifier);

        /**
         * @param index 
         * @return true 
         * @return false 
         */
        bool consume_replacement_field(size_t& index);
    };

    class FormatBuilder {
    public:

        enum class Align {
            Default,
            Left,
            Center,
            Right,
        };

        enum class SignMode {
            OnlyIfNeeded,
            Always,
            Reserved,
            Default = OnlyIfNeeded,
        };

        /**
         * @param builder 
         */
        explicit FormatBuilder(StringBuilder& builder)
            : m_builder(builder)
        {
        }

        /**
         * @param fill 
         * @param amount 
         */
        void put_padding(char fill, size_t amount);

        /**
         * @param value 
         */
        void put_literal(StringView value);

        /**
         * @param value 
         * @param align 
         * @param min_width 
         * @param max_width 
         * @param fill 
         */
        void put_string(
            StringView value,
            Align align = Align::Left,
            size_t min_width = 0,
            size_t max_width = NumericLimits<size_t>::max(),
            char fill = ' ');

        /**
         * @param value 
         * @param base 
         * @param prefix 
         * @param upper_case 
         * @param zero_pad 
         * @param align 
         * @param min_width 
         * @param fill 
         * @param sign_mode 
         * @param is_negative 
         */
        void put_u64(
            u64 value,
            u8 base = 10,
            bool prefix = false,
            bool upper_case = false,
            bool zero_pad = false,
            Align align = Align::Right,
            size_t min_width = 0,
            char fill = ' ',
            SignMode sign_mode = SignMode::OnlyIfNeeded,
            bool is_negative = false);

        /**
         * @param value 
         * @param base 
         * @param prefix 
         * @param upper_case 
         * @param zero_pad 
         * @param align 
         * @param min_width 
         * @param fill 
         * @param sign_mode 
         */
        void put_i64(
            i64 value,
            u8 base = 10,
            bool prefix = false,
            bool upper_case = false,
            bool zero_pad = false,
            Align align = Align::Right,
            size_t min_width = 0,
            char fill = ' ',
            SignMode sign_mode = SignMode::OnlyIfNeeded);

    #ifndef KERNEL
        /**
         * @param value 
         * @param base 
         * @param upper_case 
         * @param align 
         * @param min_width 
         * @param precision 
         * @param fill 
         * @param sign_mode 
         */
        void put_f64(
            double value,
            u8 base = 10,
            bool upper_case = false,
            Align align = Align::Right,
            size_t min_width = 0,
            size_t precision = 6,
            char fill = ' ',
            SignMode sign_mode = SignMode::OnlyIfNeeded);
    #endif

        /**
         * @return const StringBuilder& 
         */
        const StringBuilder& builder() const {
            return m_builder;
        }

        /**
         * @return StringBuilder& 
         */
        StringBuilder& builder() { 
            return m_builder; 
        }

    private:
        StringBuilder& m_builder;
    };

    class TypeErasedFormatParams {
    public:

        /**
         * @return Span<const TypeErasedParameter> 
         */
        Span<const TypeErasedParameter> parameters() const { 
            return m_parameters; 
        }

        /**
         * @param parameters 
         */
        void set_parameters(Span<const TypeErasedParameter> parameters) {
            m_parameters = parameters; 
        }

        /**
         * @return size_t 
         */
        size_t take_next_index() { 
            return m_next_index++;
        }

        /**
         * @param value 
         * @param default_value 
         * @return size_t 
         */
        size_t decode(size_t value, size_t default_value = 0);

    private:
        Span<const TypeErasedParameter> m_parameters;
        size_t m_next_index { 0 };
    };

    /**
     * @tparam T 
     * @param params 
     * @param builder 
     * @param parser 
     * @param value 
     */
    template<typename T>
    void __format_value(TypeErasedFormatParams& params, FormatBuilder& builder, FormatParser& parser, const void* value) {
        Formatter<T> formatter;

        formatter.parse(params, parser);
        formatter.format(params, builder, *static_cast<const T*>(value));
    }

    /**
     * @tparam Parameters 
     */
    template<typename... Parameters>
    class VariadicFormatParams : public TypeErasedFormatParams {
    public:
        static_assert(sizeof...(Parameters) <= max_format_arguments);

        explicit VariadicFormatParams(const Parameters&... parameters)
            : m_data({ TypeErasedParameter { &parameters, TypeErasedParameter::get_type<Parameters>(), __format_value<Parameters> }... })
        {
            this->set_parameters(m_data);
        }

    private:
        Array<TypeErasedParameter, sizeof...(Parameters)> m_data;
    };

    struct StandardFormatter {
        enum class Mode {
            Default,
            Binary,
            BinaryUppercase,
            Decimal,
            Octal,
            Hexadecimal,
            HexadecimalUppercase,
            Character,
            String,
            Pointer,
            Float,
            Hexfloat,
            HexfloatUppercase,
        };

        static constexpr size_t value_not_set = NumericLimits<size_t>::max();
        static constexpr size_t value_from_next_arg = NumericLimits<size_t>::max() - 1;
        static constexpr size_t value_from_arg = NumericLimits<size_t>::max() - max_format_arguments - 2;

        FormatBuilder::Align m_align = FormatBuilder::Align::Default;
        FormatBuilder::SignMode m_sign_mode = FormatBuilder::SignMode::OnlyIfNeeded;
        Mode m_mode = Mode::Default;
        bool m_alternative_form = false;
        char m_fill = ' ';
        bool m_zero_pad = false;
        size_t m_width = value_not_set;
        size_t m_precision = value_not_set;

        void parse(TypeErasedFormatParams&, FormatParser&);
    };

    /**
     * @tparam T 
     */
    template<typename T>
    struct Formatter<T, typename EnableIf<IsIntegral<T>::value>::Type> : StandardFormatter {
        Formatter() { }
        explicit Formatter(StandardFormatter formatter)
            : StandardFormatter(formatter)
        {
        }

        /**
         * @param value 
         */
        void format(TypeErasedFormatParams&, FormatBuilder&, T value);
    };

    template<>
    struct Formatter<StringView> : StandardFormatter {
        Formatter() { }
        explicit Formatter(StandardFormatter formatter)
            : StandardFormatter(formatter)
        {
        }

        /**
         * @param value 
         */
        void format(TypeErasedFormatParams&, FormatBuilder&, StringView value);
    };

    template<>
    struct Formatter<const char*> : Formatter<StringView> {
        void format(TypeErasedFormatParams& params, FormatBuilder& builder, const char* value) {
            if (m_mode == Mode::Pointer) {
                Formatter<FlatPtr> formatter { *this };
                formatter.format(params, builder, reinterpret_cast<FlatPtr>(value));
            } else {
                Formatter<StringView>::format(params, builder, value);
            }
        }
    };
    template<>
    struct Formatter<char*> : Formatter<const char*> {
    };
    template<size_t Size>
    struct Formatter<char[Size]> : Formatter<const char*> {
    };
    template<>
    struct Formatter<String> : Formatter<StringView> {
    };
    template<>
    struct Formatter<FlyString> : Formatter<StringView> {
    };


    template<typename T>
    struct Formatter<T*> : StandardFormatter {
        /**
         * @param params 
         * @param builder 
         * @param value 
         */
        void format(TypeErasedFormatParams& params, FormatBuilder& builder, T* value) {
            if (m_mode == Mode::Default)
                m_mode = Mode::Pointer;

            Formatter<FlatPtr> formatter { *this };
            formatter.format(params, builder, reinterpret_cast<FlatPtr>(value));
        }
    };

    template<>
    struct Formatter<char> : StandardFormatter {
        void format(TypeErasedFormatParams&, FormatBuilder&, char value);
    };

    template<>
    struct Formatter<bool> : StandardFormatter {
        void format(TypeErasedFormatParams&, FormatBuilder&, bool value);
    };

    #ifndef KERNEL
    template<>
    struct Formatter<float> : StandardFormatter {
        void format(TypeErasedFormatParams&, FormatBuilder&, float value);
    };

    template<>
    struct Formatter<double> : StandardFormatter {
        Formatter() { }
        explicit Formatter(StandardFormatter formatter)
            : StandardFormatter(formatter)
        {
        }

        /**
         * @param value 
         */
        void format(TypeErasedFormatParams&, FormatBuilder&, double value);
    };
    #endif

    /**
     * @param builder 
     * @param fmtstr 
     */
    void vformat(StringBuilder& builder, StringView fmtstr, TypeErasedFormatParams);

    /**
     * @param stream 
     * @param fmtstr 
     */
    void vformat(const LogStream& stream, StringView fmtstr, TypeErasedFormatParams);

    #ifndef KERNEL

    /**
     * @param fmtstr 
     * @param newline 
     */
    void vout(FILE*, StringView fmtstr, TypeErasedFormatParams, bool newline = false);

    /**
     * @tparam Parameters 
     * @param file 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void out(FILE* file, StringView fmtstr, const Parameters&... parameters) { vout(file, fmtstr, VariadicFormatParams { parameters... }); }

    /**
     * @tparam Parameters 
     * @param file 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void outln(FILE* file, StringView fmtstr, const Parameters&... parameters) { vout(file, fmtstr, VariadicFormatParams { parameters... }, true); }

    /**
     * @tparam Parameters 
     * @param file 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void outln(FILE* file, const char* fmtstr, const Parameters&... parameters) { vout(file, fmtstr, VariadicFormatParams { parameters... }, true); }

    inline void outln(FILE* file) { 
        fputc('\n', file); 
    }

    /**
     * @tparam Parameters 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void out(StringView fmtstr, const Parameters&... parameters) { out(stdout, fmtstr, parameters...); }

    /**
     * @tparam Parameters 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void outln(StringView fmtstr, const Parameters&... parameters) { outln(stdout, fmtstr, parameters...); }

    /**
     * @tparam Parameters 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void outln(const char* fmtstr, const Parameters&... parameters) { outln(stdout, fmtstr, parameters...); }

    
    inline void outln() { 
        outln(stdout); 
    }

    /**
     * @tparam Parameters 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void warn(StringView fmtstr, const Parameters&... parameters) { out(stderr, fmtstr, parameters...); }

    /**
     * @tparam Parameters 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void warnln(StringView fmtstr, const Parameters&... parameters) { outln(stderr, fmtstr, parameters...); }

    /**
     * @tparam Parameters 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void warnln(const char* fmtstr, const Parameters&... parameters) { outln(stderr, fmtstr, parameters...); }

    inline void warnln() { 
        outln(stderr); 
    }
    #endif

    void vdbgln(StringView fmtstr, TypeErasedFormatParams);

    /**
     * @tparam Parameters 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void dbgln(StringView fmtstr, const Parameters&... parameters) { 
        vdbgln(fmtstr, VariadicFormatParams { parameters... }); 
    }

    /**
     * @tparam Parameters 
     * @param fmtstr 
     * @param parameters 
     */
    template<typename... Parameters>
    void dbgln(const char* fmtstr, const Parameters&... parameters) { 
        dbgln(StringView { fmtstr }, parameters...); 
    }

    template<typename T, typename = void>
    struct HasFormatter : TrueType {
    };
    template<typename T>
    struct HasFormatter<T, typename Formatter<T>::__no_formatter_defined> : FalseType {
    };

    template<typename T>
    class FormatIfSupported {
    public:
        explicit FormatIfSupported(const T& value)
            : m_value(value)
        {
        }

        /**
         * @return const T& 
         */
        const T& value() const { 
            return m_value; 
        }

    private:
        const T& m_value;
    };

    template<typename T, bool Supported = false>
    struct __FormatIfSupported : Formatter<StringView> {
        void format(TypeErasedFormatParams& params, FormatBuilder& builder, const FormatIfSupported<T>&)
        {
            Formatter<StringView>::format(params, builder, "?");
        }
    };

    template<typename T>
    struct __FormatIfSupported<T, true> : Formatter<T> {
        void format(TypeErasedFormatParams& params, FormatBuilder& builder, const FormatIfSupported<T>& value)
        {
            Formatter<T>::format(params, builder, value.value());
        }
    };  

    template<typename T>
    struct Formatter<FormatIfSupported<T>> : __FormatIfSupported<T, HasFormatter<T>::value> {
    };

} 

#ifndef KERNEL
using Mods::out;
using Mods::outln;
using Mods::warn;
using Mods::warnln;
#endif

using Mods::dbgln;

using Mods::FormatIfSupported;
