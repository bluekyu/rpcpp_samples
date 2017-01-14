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

#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "sample_stage.h"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
struct PluginInfo
{
	const char* id = PLUGIN_ID_STRING;
	const char* name = "SamplePlugin";
	const char* author = "Name <email@email.com>";
	const char* description =
		"A plugin to use sample.";
	const char* version = "0.1";
};
BOOST_SYMBOL_EXPORT const PluginInfo plugin_info;

}

namespace rpplugins {

class Plugin: public rpcore::BasePlugin
{
public:
    static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline* pipeline)
    {
        return std::shared_ptr<rpplugins::Plugin>(new rpplugins::Plugin(pipeline));
    }

	Plugin(rpcore::RenderPipeline* pipeline): rpcore::BasePlugin(pipeline, plugin_info.id) {}

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
	return plugin_info.name;
}

std::string Plugin::get_author(void) const
{
	return plugin_info.author;
}

std::string Plugin::get_description(void) const
{
	return plugin_info.description;
}

std::string Plugin::get_version(void) const
{
	return plugin_info.version;
}

void Plugin::on_stage_setup(void)
{
	_stage = std::make_shared<SampleStage>(_pipeline);
	add_stage(_stage);
}

// ************************************************************************************************

BOOST_DLL_ALIAS(
    rpplugins::Plugin::create_plugin,
    create_plugin
)

}	// namespace rpplugins
