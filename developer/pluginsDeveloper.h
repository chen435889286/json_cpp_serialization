#ifndef __PLUGINS_DEVELOPER_H__
#define __PLUGINS_DEVELOPER_H__
#include <memory>
#include <iostream>
#include <functional>
#include <nlohmann/json.hpp>
#include <map>
#include "abstract.h"
using namespace nlohmann;
using std::string;
using std::map;
#ifndef _BASIC_JSON_TPL_PARAMS_

#    define _BASIC_JSON_TPL_PARAMS_                                                                \
        ObjectType, ArrayType, StringType, BooleanType, NumberIntegerType, NumberUnsignedType,     \
            NumberFloatType, AllocatorType, JSONSerializer, BinaryType

#endif // !_BASIC_JSON_TPL_PARAMS_

#ifndef _BASIC_JSON_TPL_PARAM_DECL_

#    define _BASIC_JSON_TPL_PARAM_DECL_                                                            \
        template <typename U, typename V, typename... Args>                                        \
        class ObjectType = std::map,                                                               \
              template <typename U, typename... Args>                                              \
              class ArrayType  = std::vector,                                                      \
              class StringType = std::string, class BooleanType = bool,                            \
              class NumberIntegerType = std::int64_t, class NumberUnsignedType = std::uint64_t,    \
              class NumberFloatType = double,                                                      \
              template <typename U>                                                                \
              class AllocatorType = std::allocator,                                                \
              template <typename T, typename SFINAE = void>                                        \
              class JSONSerializer = nlohmann::adl_serializer,                                     \
              class BinaryType     = std::vector<std::uint8_t>

#endif // !_BASIC_JSON_TPL_PARAM_DECL_


#ifndef _BASIC_JSON_TPL_DECLARATION_
#    define _BASIC_JSON_TPL_DECLARATION_ template <_BASIC_JSON_TPL_PARAM_DECL_>
#endif // !_BASIC_JSON_TPL_DECLARATION_

#ifndef _BASIC_JSON_TPL_
#    define _BASIC_JSON_TPL_                                                                       \
        nlohmann::basic_json<ObjectType, ArrayType, StringType, BooleanType, NumberIntegerType,    \
                             NumberUnsignedType, NumberFloatType, AllocatorType, JSONSerializer,   \
                             BinaryType>

#endif // ! _BASIC_JSON_TPL_
namespace nlohmann
{
// ?????????????????????perator= ????????????json
template <typename T>
struct adl_serializer<std::shared_ptr<T>>
{
    static void to_json(json &j, const std::shared_ptr<T> &opt)
    {
        if (opt) { j = *opt; }
        else
        {
            j = nullptr;
        }
    }
    static void from_json(const json &j, std::shared_ptr<T> &opt)
    {
        if (j.is_null()) { opt = nullptr; }
        else
        {
            opt.reset(new T(j.get<T>())); // NOLINT(cppcoreguidelines-owning-memory)
        }
    }
};

} // namespace nlohmann
namespace udt
{

template <typename BasicJsonType>
static void to_json(BasicJsonType &j, const udt::udtTest &f)
{
    j = json{ { "str", f.str } };
}
} // namespace udt

namespace nlohmann
{
template <>
/**
 * @brief ???????????????????????????
 *
 */
struct adl_serializer<udt::testPlugins>
{
    // ??????basic_json???????????????????????????
    _BASIC_JSON_TPL_DECLARATION_
    static udt::testPlugins from_json(const _BASIC_JSON_TPL_ &j)
    {
        return { j.template get<std::string>() };
    }
    /**
     * @brief ??????????????????????????????????????????????????????
     *
     * @param j
     * @param u
     * @return _BASIC_JSON_TPL_DECLARATION_
     */
    _BASIC_JSON_TPL_DECLARATION_
    static void to_json(_BASIC_JSON_TPL_ &j, const udt::testPlugins &u)
    {
        j["pluginsNametest"] = u.pluginsName;
    }
};
template <>
/**
 * @brief ???????????????????????????
 *
 */
struct adl_serializer<udt::pluginTest>
{
    // ??????basic_json???????????????????????????
    _BASIC_JSON_TPL_DECLARATION_
    static udt::pluginTest from_json(const _BASIC_JSON_TPL_ &j)
    {
        return { j.template get<std::string>() };
    }
    /**
     * @brief ??????????????????????????????????????????????????????
     *
     * @param j
     * @param u
     * @return _BASIC_JSON_TPL_DECLARATION_
     */
    _BASIC_JSON_TPL_DECLARATION_
    static void to_json(_BASIC_JSON_TPL_ &j, const udt::pluginTest &u)
    {
        j["udt::pluginTest"] = u.plugins;
    }
};
} // namespace nlohmann


namespace udt
{
/**
 * @brief ????????????????????????,??????json?????????????????????
 *
 */
template <typename S>
class pluginsDeveloper : public udt::abstract
{
public:
    template <typename T>
    static void to_jsons(json &j, const T &f)
    {
        j = json{ { "plugins", f.plugins } };
        std::cout << "j = " << j << std::endl;
    }
    pluginsDeveloper() : udt::abstract({}, {}) {}
    // ??????????????????
    template <typename T>
    using toJsonFucntion = std::function<void(json &j, T)>;
    // ????????????function,????????????????????????????????????
    pluginsDeveloper(testPlugins t, udt::udtTest utest) : udt::abstract(t, utest)
    {
        this->plugins = t;
        this->m_test  = utest;
    }
    /**
     * @brief Set the Function Json object ?????????????????????,?????????.h?????????
     *
     * @param func
     */
public:
    /**
     * @brief Set the Function Json object ??????????????????????????????????????????
     *
     * @param func
     */
    void setFunctionJson(toJsonFucntion<S> &func)
    {
        json            j;
        udt::pluginTest p{ "plugins" };
        m_function = func;
        m_function(j, p); // ??????????????????
    }
    /**
     * @brief Set the Function Json object
     *
     * @tparam DUMMY shared_ptr<udt::udtTest>???????????????
     * @param N
     */
    template <typename DUMMY = std::shared_ptr<udt::udtTest>>
    void setFunctionJson(toJsonFucntion<DUMMY> &N)
    {
        json                                          j;
        toJsonFucntion<std::shared_ptr<udt::udtTest>> func;
        func = N;
        std::shared_ptr<udt::udtTest> p;
        p.reset(new udt::udtTest{ "DUMMY" });
        func(j, p);
    }
    /**
     * @brief ?????????????????????tojson function ?????????????????????
     *
     * @tparam T1
     * @tparam T2
     */
    template <typename T1, typename T2>
    using toJsonFucntionMap = std::function<void(T1, T2)>;
    /**
     * @brief Set the Function Json object ????????????????????????????????????,std::function<T1,T2> ???????????????
     *
     * @tparam M ????????????
     * @tparam DUMMY ????????????
     * @param N
     */
    template <typename M = map<string, string>, typename DUMMY = std::shared_ptr<udt::udtTest>>
    void setFunctionJson(toJsonFucntionMap<M, DUMMY> &N)
    {
        toJsonFucntionMap<map<string, string>, std::shared_ptr<udt::udtTest>> func = N;

        map<string, string>           j;
        std::shared_ptr<udt::udtTest> p;

        p.reset(new udt::udtTest{ "DUMMY" });
        func(j, p);
    }
    json setFromJson(void *data) { this->set_from_json(data); }
    void setToJson() { this->set_to_json(); }
    void setToJson(void *p, TYPE_NAME name) { this->set_to_json(p, name); }

private:
    toJsonFucntion<S> m_function;
    // ????????????????????????,?????????????????????????????????,????????????????????????????????????
    virtual void set_to_json(void *p, TYPE_NAME name) override final
    {
        switch (name)
        {
            case TYPE_NAME::UDT_PLUGIN_TEST:
            {
                auto tmp1 = reinterpret_cast<udt::pluginTest *>(p);

                json j1;
                j1 = *tmp1;
                std::cout << "set to_json udt::pluginTest   " << j1 << std::endl;
            }
            break;
            case TYPE_NAME::TEST_PLUGINS:
            {
                auto tmp2 = reinterpret_cast<udt::testPlugins *>(p);
                json j2;
                j2 = *tmp2;

                std::cout << "set to_json udt::testPlugins" << j2 << std::endl;
            }
            default:
            {
                auto tmp3 = reinterpret_cast<udt::udtTest *>(p);
                json j3;
                j3 = *tmp3;
                std::cout << "set_to_json udt::udtTest" << j3 << std::endl;
                break;
            }
        }
    }
    virtual json set_from_json(void *) override final {}
    virtual void set_to_json() override final
    {
        json            j;
        udt::pluginTest t{ "??????1" };
        j = t;
        std::cout << "set_to_json = " << j << std::endl;

        // testJson();
    }
    // ?????????????????????????????????json??????,?????????????????????????????????????????????????????????????????????
};
} // namespace udt

/**
 *
 * @brief ??????????????????jsonfunction
 * @tparam BasicJsonType
 * @param j
 * @param f
 */
// template <typename BasicJsonType>
static void to_json(json &j, const udt::testPlugins &f)
{
    j = json{ { "name", f.pluginsName } };
    std::cout << "j = " << j << std::endl;
}

/**
 * @brief ???????????????????????????????????????
 *
 */
void testJson();

#endif