/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016, Center of human-centered interaction for coexistence.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <boost/version.hpp>
#if BOOST_VERSION >= 106100
#include <boost/dll/alias.hpp>
#endif

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "sample_stage.h"

namespace rpplugins {

class Plugin: public rpcore::BasePlugin
{
public:
    static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline* pipeline)
    {
        return std::shared_ptr<rpplugins::Plugin>(new rpplugins::Plugin(pipeline));
    }

    Plugin(rpcore::RenderPipeline* pipeline): rpcore::BasePlugin(pipeline, PLUGIN_ID_STRING) {}

    virtual std::string get_name(void) const override;
    virtual std::string get_author(void) const override;
    virtual std::string get_description(void) const override;
    virtual std::string get_version(void) const override;

    virtual RequrieType& get_required_plugins(void) const override { return require_plugins; }

	virtual void on_stage_setup(void);

private:
    static RequrieType require_plugins;

	std::shared_ptr<SampleStage> _stage;
};

Plugin::RequrieType Plugin::require_plugins;

std::string Plugin::get_name(void) const
{
	return "SamplePlugin";
}

std::string Plugin::get_author(void) const
{
	return "Name <email@email.com>";
}

std::string Plugin::get_description(void) const
{
	return	"A plugin to use sample.";
}

std::string Plugin::get_version(void) const
{
	return "0.1";
}

void Plugin::on_stage_setup(void)
{
	_stage = std::make_shared<SampleStage>(_pipeline);
	add_stage(_stage);
}

// ************************************************************************************************
#if BOOST_VERSION < 106100

#define BOOST_DLL_SELECTANY

#define BOOST_DLL_SECTION(SectionName, Permissions)                                             \
    BOOST_STATIC_ASSERT_MSG(                                                                    \
        sizeof(#SectionName) < 10,                                                              \
        "Some platforms require section names to be at most 8 bytest"                           \
    );                                                                                          \
    __pragma(section(#SectionName, Permissions)) __declspec(allocate(#SectionName))             \

#define BOOST_DLL_ALIAS(FunctionOrVar, AliasName)                       \
    BOOST_DLL_ALIAS_SECTIONED(FunctionOrVar, AliasName, boostdll)       \

#define BOOST_DLL_ALIAS_SECTIONED(FunctionOrVar, AliasName, SectionName)                        \
    namespace _autoaliases {                                                                    \
        extern "C" BOOST_SYMBOL_EXPORT const void *AliasName;                                   \
        BOOST_DLL_SECTION(SectionName, read) BOOST_DLL_SELECTANY                                \
        const void * AliasName = reinterpret_cast<const void*>(reinterpret_cast<intptr_t>(      \
            &FunctionOrVar                                                                      \
        ));                                                                                     \
    }

#endif

BOOST_DLL_ALIAS(
    rpplugins::Plugin::create_plugin,
    create_plugin
)

}	// namespace rpplugins
